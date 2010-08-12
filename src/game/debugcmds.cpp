/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "Vehicle.h"
#include "Player.h"
#include "Opcodes.h"
#include "Chat.h"
#include "Log.h"
#include "Unit.h"
#include "GossipDef.h"
#include "Language.h"
#include "BattleGroundMgr.h"
#include <fstream>
#include "ObjectMgr.h"
#include "ObjectGuid.h"
#include "SpellMgr.h"

#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"

#include "World.h"

#include "pathfinding/Detour/DetourNavMesh.h"
#include "pathfinding/Detour/DetourCommon.h"
#include "PathFinder.h"

bool ChatHandler::HandleDebugSendSpellFailCommand(char* args)
{
    if (!*args)
        return false;

    uint32 failnum;
    if (!ExtractUInt32(&args, failnum) || failnum > 255)
        return false;

    uint32 failarg1;
    if (!ExtractOptUInt32(&args, failarg1, 0))
        return false;

    uint32 failarg2;
    if (!ExtractOptUInt32(&args, failarg2, 0))
        return false;

    WorldPacket data(SMSG_CAST_FAILED, 5);
    data << uint8(0);
    data << uint32(133);
    data << uint8(failnum);
    if (failarg1 || failarg2)
        data << uint32(failarg1);
    if (failarg2)
        data << uint32(failarg2);

    m_session->SendPacket(&data);

    return true;
}

bool ChatHandler::HandleDebugSendPoiCommand(char* args)
{
    Player *pPlayer = m_session->GetPlayer();
    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        return true;
    }

    uint32 icon;
    if (!ExtractUInt32(&args, icon))
        return false;

    uint32 flags;
    if (!ExtractUInt32(&args, flags))
        return false;

    DETAIL_LOG("Command : POI, NPC = %u, icon = %u flags = %u", target->GetGUIDLow(), icon,flags);
    pPlayer->PlayerTalkClass->SendPointOfInterest(target->GetPositionX(), target->GetPositionY(), Poi_Icon(icon), flags, 30, "Test POI");
    return true;
}

bool ChatHandler::HandleDebugSendEquipErrorCommand(char* args)
{
    if (!*args)
        return false;

    uint8 msg = atoi(args);
    m_session->GetPlayer()->SendEquipError(msg, NULL, NULL);
    return true;
}

bool ChatHandler::HandleDebugSendSellErrorCommand(char* args)
{
    if (!*args)
        return false;

    uint8 msg = atoi(args);
    m_session->GetPlayer()->SendSellError(msg, 0, 0, 0);
    return true;
}

bool ChatHandler::HandleDebugSendBuyErrorCommand(char* args)
{
    if (!*args)
        return false;

    uint8 msg = atoi(args);
    m_session->GetPlayer()->SendBuyError(msg, 0, 0, 0);
    return true;
}

bool ChatHandler::HandleDebugSendOpcodeCommand(char* /*args*/)
{
    Unit *unit = getSelectedUnit();
    if (!unit || (unit->GetTypeId() != TYPEID_PLAYER))
        unit = m_session->GetPlayer();

    std::ifstream ifs("opcode.txt");
    if (ifs.bad())
        return false;

    uint32 opcode;
    ifs >> opcode;

    WorldPacket data(opcode, 0);

    while(!ifs.eof())
    {
        std::string type;
        ifs >> type;

        if(type == "")
            break;

        if(type == "uint8")
        {
            uint16 val1;
            ifs >> val1;
            data << uint8(val1);
        }
        else if(type == "uint16")
        {
            uint16 val2;
            ifs >> val2;
            data << val2;
        }
        else if(type == "uint32")
        {
            uint32 val3;
            ifs >> val3;
            data << val3;
        }
        else if(type == "uint64")
        {
            uint64 val4;
            ifs >> val4;
            data << val4;
        }
        else if(type == "float")
        {
            float val5;
            ifs >> val5;
            data << val5;
        }
        else if(type == "string")
        {
            std::string val6;
            ifs >> val6;
            data << val6;
        }
        else if(type == "pguid")
        {
            data << unit->GetPackGUID();
        }
        else
        {
            DEBUG_LOG("Sending opcode: unknown type '%s'", type.c_str());
            break;
        }
    }
    ifs.close();
    DEBUG_LOG("Sending opcode %u", data.GetOpcode());
    data.hexlike();
    ((Player*)unit)->GetSession()->SendPacket(&data);
    PSendSysMessage(LANG_COMMAND_OPCODESENT, data.GetOpcode(), unit->GetName());
    return true;
}

bool ChatHandler::HandleDebugUpdateWorldStateCommand(char* args)
{
    uint32 world;
    if (!ExtractUInt32(&args, world))
        return false;

    uint32 state;
    if (!ExtractUInt32(&args, state))
        return false;

    m_session->GetPlayer()->SendUpdateWorldState(world, state);
    return true;
}

bool ChatHandler::HandleDebugMoveMapCommand(char* args)
{
    if(!args)
        return false;

    if(!m_session->GetPlayer()->GetMap() || !m_session->GetPlayer()->GetMap()->GetNavMesh())
    {
        PSendSysMessage("NavMesh not loaded for current map.");
        return true;
    }

    char* w = strtok((char*)args, " ");

    if (w && strcmp(w, "path") == 0)
    {
        PSendSysMessage("mmap path:");
        float extents[3] = {2.f, 4.f, 2.f};
        dtQueryFilter filter = dtQueryFilter();

        // units
        Player* player = m_session->GetPlayer();
        Unit* target = getSelectedUnit();

        // unit locations
        float x, y, z;
        target->GetPosition(x, y, z);
        float start[3] = {y, z, x};
        player->GetPosition(x, y, z);
        float end[3] = {y, z, x};
        float pathPos[MAX_PATH_LENGTH];

        // path
        PathInfo path = PathInfo(target, x, y, z);
        PSendSysMessage("%s's path to %s:", target->GetName(), player->GetName());
        PSendSysMessage("length %i", path.m_length);
        PSendSysMessage("start  (%f,%f,%f)", path.m_startPosition[0],path.m_startPosition[1],path.m_startPosition[2]);
        PSendSysMessage("next   (%f,%f,%f)", path.m_nextPosition[0],path.m_nextPosition[1],path.m_nextPosition[2]);
        PSendSysMessage("end    (%f,%f,%f)", path.m_endPosition[0],path.m_endPosition[1],path.m_endPosition[2]);

        if(path.m_length > 0)
        {
            PSendSysMessage("path");
            for(int i = 0; i < path.m_length; ++i)
                PSendSysMessage("       %i", path.m_pathPolyRefs[i]);

            // unit polyrefs
            dtPolyRef startPoly = path.m_navMesh->findNearestPoly(start, extents, &filter, 0);
            dtPolyRef endPoly = path.m_navMesh->findNearestPoly(end, extents, &filter, 0);

            // straithPath
            int length = path.m_navMesh->findStraightPath(start, end, path.m_pathPolyRefs, path.m_length, pathPos, 0, 0, MAX_PATH_LENGTH);
            PSendSysMessage("Path positions:");
            for(int i = 0; i < length; ++i)
                PSendSysMessage("(%.2f,%.2f,%.2f)", pathPos[i*3], pathPos[i*3+1], pathPos[i*3+2]);
        }
        else
            PSendSysMessage("Path is 0 length");

        return true;
    }
    else if(w && strcmp(w, "verts") == 0)
    {
        PSendSysMessage("mmap verts:");
        float extents[3] = {2.f, 4.f, 2.f};
        dtQueryFilter filter = dtQueryFilter();

        // units
        Player* player = m_session->GetPlayer();
        Unit* target = getSelectedUnit();

        // unit locations
        float x, y, z;
        target->GetPosition(x, y, z);
        float start[3] = {y, z, x};
        player->GetPosition(x, y, z);
        float end[3] = {y, z, x};

        PathInfo path = PathInfo(target, x, y, z);

        if(path.m_length <= 0)
        {
            PSendSysMessage("Path is 0 length");
            return true;
        }

        dtPolyRef startPoly = path.m_navMesh->findNearestPoly(start, extents, &filter, 0);
        dtPolyRef endPoly = path.m_navMesh->findNearestPoly(end, extents, &filter, 0);

        // vertices stuff
        int polyindex;
        const dtMeshTile* tile = path.m_navMesh->getTileByPolyRef(startPoly, &polyindex);
        dtPoly poly = tile->polys[polyindex];
        float vertices[DT_VERTS_PER_POLYGON*3];

        // startpoly vertices
        int nv = 0;
        for (int i = 0; i < (int)poly.vertCount; ++i)
        {
            dtVcopy(&vertices[nv*3], &tile->verts[poly.verts[i]*3]);
            nv++;
        }

        PSendSysMessage("Poly vertices for %i:", startPoly);
        for(int i = 0; i < (int)poly.vertCount; ++i)
            PSendSysMessage("(%.2f,%.2f,%.2f)", vertices[i*3], vertices[i*3+1], vertices[i*3+2]);

        // endpoly vertices
        tile = path.m_navMesh->getTileByPolyRef(endPoly, &polyindex);
        poly = tile->polys[polyindex];
        nv = 0;
        for (int i = 0; i < (int)poly.vertCount; ++i)
        {
            dtVcopy(&vertices[nv*3], &tile->verts[poly.verts[i]*3]);
            nv++;
        }

        PSendSysMessage("Poly vertices for %i:", endPoly);
        for(int i = 0; i < (int)poly.vertCount; ++i)
            PSendSysMessage("(%.2f,%.2f,%.2f)", vertices[i*3], vertices[i*3+1], vertices[i*3+2]);

        return true;
    }
    else if(w && strcmp(w, "tileloc") == 0)
    {
        PSendSysMessage("mmap tileloc:");

        // grid tile location
        Player* player = m_session->GetPlayer();

        int gx = 32 - player->GetPositionX() / 533.33333f;
        int gy = 32 - player->GetPositionY() / 533.33333f;

        PSendSysMessage("gridloc [%i,%i]", gx, gy);

        // calculate navmesh tile location
        dtNavMesh* navmesh = player->GetMap()->GetNavMesh();
        const float* min = navmesh->getParams()->orig;

        float x, y, z;
        player->GetPosition(x, y, z);
        float location[3] = {y, z, x};
        float extents[3] = {2.f,4.f,2.f};

        int tilex = (int) (y - min[0]) / 533.33333;
        int tiley = (int) (x - min[2]) / 533.33333;

        PSendSysMessage("Calc   [%02i,%02i]", tilex, tiley);

        // navmesh poly -> navmesh tile location
        dtPolyRef poly = navmesh->findNearestPoly(location, extents, &(dtQueryFilter()), 0);
        if(!poly)
            PSendSysMessage("Dt     [??,??] (invalid poly, probably no tile loaded)");
        else
        {
            const dtMeshTile* tile = navmesh->getTileByPolyRef(poly, 0);
            if(tile)
                PSendSysMessage("Dt     [%02i,%02i]", tile->header->x, tile->header->y);
            else
                PSendSysMessage("Dt     [??,??] (no tile loaded)");
        }

        // mmtile file header -> navmesh tile location
        char path[512];
        sprintf(path, "%smmaps/%03u%02i%02i.mmtile", sWorld.GetDataPath().c_str(), player->GetMapId(), gx, gy);
        FILE* file = fopen(path, "rb");
        if(!file)
            PSendSysMessage("mmtile [??,??] (file %03u%02i%02i.mmtile not found)", player->GetMapId(), gx, gy);
        else
        {
            fseek(file, 0, SEEK_END);
            int length = ftell(file);
            fseek(file, 0, SEEK_SET);

            unsigned char* data = new unsigned char[length];
            fread(data, length, 1, file);
            fclose(file);

            dtMeshHeader* header = (dtMeshHeader*)data;

            PSendSysMessage("mmtile [%02i,%02i]", header->x, header->y);

            delete [] data;
        }

        return true;
    }
    else if(w && strcmp(w, "polytest") == 0)
    {
        PSendSysMessage("mmap polytest:");
        Player* player = m_session->GetPlayer();
        dtNavMesh* navmesh = player->GetMap()->GetNavMesh();

        float x, y, z;
        player->GetPosition(x, y, z);
        float location[3] = {y, z, x};
        float extents[3] = {2.f,4.f,2.f};
        dtQueryFilter filter = dtQueryFilter();
        filter.includeFlags = 0xFFFF;

        dtPolyRef nearestPoly;

        PSendSysMessage("Nearest poly is:");

        nearestPoly = navmesh->findNearestPoly(location, extents, &filter, 0);
        PSendSysMessage("(y,z,x)");
        PSendSysMessage("(%.2f,%.2f,%.2f)", y, z, x);
        PSendSysMessage("%u", nearestPoly);

        return true;
    }
    else if(w && strcmp(w, "loadedtiles") == 0)
    {
        PSendSysMessage("mmap loadedtiles:");
        dtNavMesh* navmesh = m_session->GetPlayer()->GetMap()->GetNavMesh();

        for(int i = 0; i < navmesh->getMaxTiles(); ++i)
        {
            const dtMeshTile* tile = navmesh->getTile(i);
            if(!tile || !tile->header)
                continue;

            PSendSysMessage("[%02i,%02i]", tile->header->x, tile->header->y);
        }

        return true;
    }
    else if(w && strcmp(w, "stats") == 0)
    {
        PSendSysMessage("mmap stats:");
        dtNavMesh* navmesh = m_session->GetPlayer()->GetMap()->GetNavMesh();

        uint32 tileCount = 0;
        uint32 nodeCount = 0;
        uint32 polyCount = 0;
        uint32 vertCount = 0;
        uint32 triCount = 0;
        uint32 triVertCount = 0;
        uint32 dataSize = 0;
        for(int i = 0; i < navmesh->getMaxTiles(); ++i)
        {
            const dtMeshTile* tile = navmesh->getTile(i);
            if(!tile || !tile->header)
                continue;

            tileCount ++;
            nodeCount += tile->header->bvNodeCount;
            polyCount += tile->header->polyCount;
            vertCount += tile->header->vertCount;
            triCount += tile->header->detailTriCount;
            triVertCount += tile->header->detailVertCount;
            dataSize += tile->dataSize;
        }

        PSendSysMessage("Navmesh stats:");
        PSendSysMessage(" %u tiles loaded", tileCount);
        PSendSysMessage(" %u BVTree nodes", nodeCount);
        PSendSysMessage(" %u polygons (%u vertices)", polyCount, vertCount);
        PSendSysMessage(" %u triangles (%u vertices)", triCount, triVertCount);
        PSendSysMessage(" %.2f MB of data (not including pointers)", ((float)dataSize / sizeof(unsigned char)) / 1048576);

        return true;
    }
    else
    {
        // usage
        PSendSysMessage("mmap usage:");
        PSendSysMessage("  path        print path info from target (or self) to self");
        PSendSysMessage("  verts       print vertices of current start and end poly");
        PSendSysMessage("  tileloc     print the current tile's navmesh index");
        PSendSysMessage("  polytest    print the current polygon's ref");
        PSendSysMessage("  loadedtiles print tile info for loaded tiles");
        PSendSysMessage("  stats       print stats about the current navmesh");

        return true;
    }

    return false;
}

bool ChatHandler::HandleDebugPlayCinematicCommand(char* args)
{
    // USAGE: .debug play cinematic #cinematicid
    // #cinematicid - ID decimal number from CinemaicSequences.dbc (1st column)
    uint32 dwId;
    if (!ExtractUInt32(&args, dwId))
        return false;

    if (!sCinematicSequencesStore.LookupEntry(dwId))
    {
        PSendSysMessage(LANG_CINEMATIC_NOT_EXIST, dwId);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->SendCinematicStart(dwId);
    return true;
}

bool ChatHandler::HandleDebugPlayMovieCommand(char* args)
{
    // USAGE: .debug play movie #movieid
    // #movieid - ID decimal number from Movie.dbc (1st column)
    uint32 dwId;
    if (!ExtractUInt32(&args, dwId))
        return false;

    if (!sMovieStore.LookupEntry(dwId))
    {
        PSendSysMessage(LANG_MOVIE_NOT_EXIST, dwId);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->SendMovieStart(dwId);
    return true;
}

//Play sound
bool ChatHandler::HandleDebugPlaySoundCommand(char* args)
{
    // USAGE: .debug playsound #soundid
    // #soundid - ID decimal number from SoundEntries.dbc (1st column)
    uint32 dwSoundId;
    if (!ExtractUInt32(&args, dwSoundId))
        return false;

    if (!sSoundEntriesStore.LookupEntry(dwSoundId))
    {
        PSendSysMessage(LANG_SOUND_NOT_EXIST, dwSoundId);
        SetSentErrorMessage(true);
        return false;
    }

    Unit* unit = getSelectedUnit();
    if (!unit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (m_session->GetPlayer()->GetSelection())
        unit->PlayDistanceSound(dwSoundId,m_session->GetPlayer());
    else
        unit->PlayDirectSound(dwSoundId,m_session->GetPlayer());

    PSendSysMessage(LANG_YOU_HEAR_SOUND, dwSoundId);
    return true;
}

//Send notification in channel
bool ChatHandler::HandleDebugSendChannelNotifyCommand(char* args)
{
    const char *name = "test";

    uint32 code;
    if (!ExtractUInt32(&args, code) || code > 255)
        return false;

    WorldPacket data(SMSG_CHANNEL_NOTIFY, (1+10));
    data << uint8(code);                                    // notify type
    data << name;                                           // channel name
    data << uint32(0);
    data << uint32(0);
    m_session->SendPacket(&data);
    return true;
}

//Send notification in chat
bool ChatHandler::HandleDebugSendChatMsgCommand(char* args)
{
    const char *msg = "testtest";

    uint32 type;
    if (!ExtractUInt32(&args, type) || type > 255)
        return false;

    WorldPacket data;
    ChatHandler::FillMessageData(&data, m_session, type, 0, "chan", m_session->GetPlayer()->GetGUID(), msg, m_session->GetPlayer());
    m_session->SendPacket(&data);
    return true;
}

bool ChatHandler::HandleDebugSendQuestPartyMsgCommand(char* args)
{
    uint32 msg;
    if (!ExtractUInt32(&args, msg))
        return false;

    m_session->GetPlayer()->SendPushToPartyResponse(m_session->GetPlayer(), msg);
    return true;
}

bool ChatHandler::HandleDebugGetLootRecipientCommand(char* /*args*/)
{
    Creature* target = getSelectedCreature();
    if (!target)
        return false;

    if (!target->HasLootRecipient())
        SendSysMessage("loot recipient: no loot recipient");
    else if(Player* recipient = target->GetLootRecipient())
        PSendSysMessage("loot recipient: %s with raw data %s from group %u",
            recipient->GetObjectGuid().GetString().c_str(),
            target->GetLootRecipientGuid().GetString().c_str(),
            target->GetLootGroupRecipientId());
    else
        SendSysMessage("loot recipient: offline ");

    return true;
}

bool ChatHandler::HandleDebugSendQuestInvalidMsgCommand(char* args)
{
    uint32 msg = atol(args);
    m_session->GetPlayer()->SendCanTakeQuestResponse(msg);
    return true;
}

bool ChatHandler::HandleDebugGetItemStateCommand(char* args)
{
    if (!*args)
        return false;

    std::string state_str = args;

    ItemUpdateState state = ITEM_UNCHANGED;
    bool list_queue = false, check_all = false;
    if (state_str == "unchanged") state = ITEM_UNCHANGED;
    else if (state_str == "changed") state = ITEM_CHANGED;
    else if (state_str == "new") state = ITEM_NEW;
    else if (state_str == "removed") state = ITEM_REMOVED;
    else if (state_str == "queue") list_queue = true;
    else if (state_str == "check_all") check_all = true;
    else return false;

    Player* player = getSelectedPlayer();
    if (!player) player = m_session->GetPlayer();

    if (!list_queue && !check_all)
    {
        state_str = "The player has the following " + state_str + " items: ";
        SendSysMessage(state_str.c_str());
        for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; ++i)
        {
            if(i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                continue;

            Item *item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
            if (!item) continue;
            if (!item->IsBag())
            {
                if (item->GetState() == state)
                    PSendSysMessage("bag: 255 slot: %d guid: %d owner: %d", item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()));
            }
            else
            {
                Bag *bag = (Bag*)item;
                for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                {
                    Item* item2 = bag->GetItemByPos(j);
                    if (item2 && item2->GetState() == state)
                        PSendSysMessage("bag: 255 slot: %d guid: %d owner: %d", item2->GetSlot(), item2->GetGUIDLow(), GUID_LOPART(item2->GetOwnerGUID()));
                }
            }
        }
    }

    if (list_queue)
    {
        std::vector<Item *> &updateQueue = player->GetItemUpdateQueue();
        for(size_t i = 0; i < updateQueue.size(); ++i)
        {
            Item *item = updateQueue[i];
            if(!item) continue;

            Bag *container = item->GetContainer();
            uint8 bag_slot = container ? container->GetSlot() : uint8(INVENTORY_SLOT_BAG_0);

            std::string st;
            switch(item->GetState())
            {
                case ITEM_UNCHANGED: st = "unchanged"; break;
                case ITEM_CHANGED: st = "changed"; break;
                case ITEM_NEW: st = "new"; break;
                case ITEM_REMOVED: st = "removed"; break;
            }

            PSendSysMessage("bag: %d slot: %d guid: %d - state: %s", bag_slot, item->GetSlot(), item->GetGUIDLow(), st.c_str());
        }
        if (updateQueue.empty())
            PSendSysMessage("updatequeue empty");
    }

    if (check_all)
    {
        bool error = false;
        std::vector<Item *> &updateQueue = player->GetItemUpdateQueue();
        for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; ++i)
        {
            if(i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                continue;

            Item *item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
            if (!item) continue;

            if (item->GetSlot() != i)
            {
                PSendSysMessage("item at slot %d, guid %d has an incorrect slot value: %d", i, item->GetGUIDLow(), item->GetSlot());
                error = true; continue;
            }

            if (item->GetOwnerGUID() != player->GetGUID())
            {
                PSendSysMessage("for the item at slot %d and itemguid %d, owner's guid (%d) and player's guid (%d) don't match!", item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()), player->GetGUIDLow());
                error = true; continue;
            }

            if (Bag *container = item->GetContainer())
            {
                PSendSysMessage("item at slot: %d guid: %d has a container (slot: %d, guid: %d) but shouldnt!", item->GetSlot(), item->GetGUIDLow(), container->GetSlot(), container->GetGUIDLow());
                error = true; continue;
            }

            if (item->IsInUpdateQueue())
            {
                uint16 qp = item->GetQueuePos();
                if (qp > updateQueue.size())
                {
                    PSendSysMessage("item at slot: %d guid: %d has a queuepos (%d) larger than the update queue size! ", item->GetSlot(), item->GetGUIDLow(), qp);
                    error = true; continue;
                }

                if (updateQueue[qp] == NULL)
                {
                    PSendSysMessage("item at slot: %d guid: %d has a queuepos (%d) that points to NULL in the queue!", item->GetSlot(), item->GetGUIDLow(), qp);
                    error = true; continue;
                }

                if (updateQueue[qp] != item)
                {
                    PSendSysMessage("item at slot: %d guid: %d has has a queuepos (%d) that points to another item in the queue (bag: %d, slot: %d, guid: %d)", item->GetSlot(), item->GetGUIDLow(), qp, updateQueue[qp]->GetBagSlot(), updateQueue[qp]->GetSlot(), updateQueue[qp]->GetGUIDLow());
                    error = true; continue;
                }
            }
            else if (item->GetState() != ITEM_UNCHANGED)
            {
                PSendSysMessage("item at slot: %d guid: %d is not in queue but should be (state: %d)!", item->GetSlot(), item->GetGUIDLow(), item->GetState());
                error = true; continue;
            }

            if(item->IsBag())
            {
                Bag *bag = (Bag*)item;
                for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                {
                    Item* item2 = bag->GetItemByPos(j);
                    if (!item2) continue;

                    if (item2->GetSlot() != j)
                    {
                        PSendSysMessage("the item in bag %d slot %d, guid %d has an incorrect slot value: %d", bag->GetSlot(), j, item2->GetGUIDLow(), item2->GetSlot());
                        error = true; continue;
                    }

                    if (item2->GetOwnerGUID() != player->GetGUID())
                    {
                        PSendSysMessage("for the item in bag %d at slot %d and itemguid %d, owner's guid (%d) and player's guid (%d) don't match!", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), GUID_LOPART(item2->GetOwnerGUID()), player->GetGUIDLow());
                        error = true; continue;
                    }

                    Bag *container = item2->GetContainer();
                    if (!container)
                    {
                        PSendSysMessage("the item in bag %d at slot %d with guid %d has no container!", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow());
                        error = true; continue;
                    }

                    if (container != bag)
                    {
                        PSendSysMessage("the item in bag %d at slot %d with guid %d has a different container(slot %d guid %d)!", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), container->GetSlot(), container->GetGUIDLow());
                        error = true; continue;
                    }

                    if (item2->IsInUpdateQueue())
                    {
                        uint16 qp = item2->GetQueuePos();
                        if (qp > updateQueue.size())
                        {
                            PSendSysMessage("item in bag: %d at slot: %d guid: %d has a queuepos (%d) larger than the update queue size! ", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), qp);
                            error = true; continue;
                        }

                        if (updateQueue[qp] == NULL)
                        {
                            PSendSysMessage("item in bag: %d at slot: %d guid: %d has a queuepos (%d) that points to NULL in the queue!", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), qp);
                            error = true; continue;
                        }

                        if (updateQueue[qp] != item2)
                        {
                            PSendSysMessage("item in bag: %d at slot: %d guid: %d has has a queuepos (%d) that points to another item in the queue (bag: %d, slot: %d, guid: %d)", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), qp, updateQueue[qp]->GetBagSlot(), updateQueue[qp]->GetSlot(), updateQueue[qp]->GetGUIDLow());
                            error = true; continue;
                        }
                    }
                    else if (item2->GetState() != ITEM_UNCHANGED)
                    {
                        PSendSysMessage("item in bag: %d at slot: %d guid: %d is not in queue but should be (state: %d)!", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), item2->GetState());
                        error = true; continue;
                    }
                }
            }
        }

        for(size_t i = 0; i < updateQueue.size(); ++i)
        {
            Item *item = updateQueue[i];
            if(!item) continue;

            if (item->GetOwnerGUID() != player->GetGUID())
            {
                PSendSysMessage("queue(" SIZEFMTD "): for the an item (guid %d), the owner's guid (%d) and player's guid (%d) don't match!", i, item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()), player->GetGUIDLow());
                error = true; continue;
            }

            if (item->GetQueuePos() != i)
            {
                PSendSysMessage("queue(" SIZEFMTD "): for the an item (guid %d), the queuepos doesn't match it's position in the queue!", i, item->GetGUIDLow());
                error = true; continue;
            }

            if (item->GetState() == ITEM_REMOVED) continue;
            Item *test = player->GetItemByPos( item->GetBagSlot(), item->GetSlot());

            if (test == NULL)
            {
                PSendSysMessage("queue(" SIZEFMTD "): the bag(%d) and slot(%d) values for the item with guid %d are incorrect, the player doesn't have an item at that position!", i, item->GetBagSlot(), item->GetSlot(), item->GetGUIDLow());
                error = true; continue;
            }

            if (test != item)
            {
                PSendSysMessage("queue(" SIZEFMTD "): the bag(%d) and slot(%d) values for the item with guid %d are incorrect, the item with guid %d is there instead!", i, item->GetBagSlot(), item->GetSlot(), item->GetGUIDLow(), test->GetGUIDLow());
                error = true; continue;
            }
        }
        if (!error)
            SendSysMessage("All OK!");
    }

    return true;
}

bool ChatHandler::HandleDebugBattlegroundCommand(char* /*args*/)
{
    sBattleGroundMgr.ToggleTesting();
    return true;
}

bool ChatHandler::HandleDebugArenaCommand(char* /*args*/)
{
    sBattleGroundMgr.ToggleArenaTesting();
    return true;
}

bool ChatHandler::HandleDebugSpawnVehicleCommand(char* args)
{
    uint32 entry;
    if (!ExtractUInt32(&args, entry))
        return false;

    uint32 id;
    if (!ExtractUInt32(&args, id))
        return false;

    CreatureInfo const *ci = ObjectMgr::GetCreatureTemplate(entry);
    if (!ci)
        return false;

    VehicleEntry const *ve = sVehicleStore.LookupEntry(id);
    if (!ve)
        return false;

    Vehicle *v = new Vehicle;
    Map *map = m_session->GetPlayer()->GetMap();

    if (!v->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_VEHICLE), map, m_session->GetPlayer()->GetPhaseMaskForSpawn(), entry, id, m_session->GetPlayer()->GetTeam()))
    {
        delete v;
        return false;
    }

    float px, py, pz;
    m_session->GetPlayer()->GetClosePoint(px, py, pz, m_session->GetPlayer()->GetObjectBoundingRadius());

    v->Relocate(px, py, pz, m_session->GetPlayer()->GetOrientation());

    if (!v->IsPositionValid())
    {
        sLog.outError("Vehicle (guidlow %d, entry %d) not created. Suggested coordinates isn't valid (X: %f Y: %f)",
            v->GetGUIDLow(), v->GetEntry(), v->GetPositionX(), v->GetPositionY());
        delete v;
        return false;
    }

    map->Add((Creature*)v);
    v->AIM_Initialize();

    return true;
}

bool ChatHandler::HandleDebugSpellCheckCommand(char* /*args*/)
{
    sLog.outString( "Check expected in code spell properties base at table 'spell_check' content...");
    sSpellMgr.CheckUsedSpells("spell_check");
    return true;
}

bool ChatHandler::HandleDebugSendLargePacketCommand(char* /*args*/)
{
    const char* stuffingString = "This is a dummy string to push the packet's size beyond 128000 bytes. ";
    std::ostringstream ss;
    while(ss.str().size() < 128000)
        ss << stuffingString;
    SendSysMessage(ss.str().c_str());
    return true;
}

bool ChatHandler::HandleDebugSendSetPhaseShiftCommand(char* args)
{
    if (!*args)
        return false;

    uint32 PhaseShift = atoi(args);
    m_session->SendSetPhaseShift(PhaseShift);
    return true;
}

//show animation
bool ChatHandler::HandleDebugAnimCommand(char* args)
{
    uint32 emote_id;
    if (!ExtractUInt32(&args, emote_id))
        return false;

    m_session->GetPlayer()->HandleEmoteCommand(emote_id);
    return true;
}

bool ChatHandler::HandleDebugSetAuraStateCommand(char* args)
{
    int32 state;
    if (!ExtractInt32(&args, state))
        return false;

    Unit* unit = getSelectedUnit();
    if (!unit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (!state)
    {
        // reset all states
        for(int i = 1; i <= 32; ++i)
            unit->ModifyAuraState(AuraState(i),false);
        return true;
    }

    unit->ModifyAuraState(AuraState(abs(state)),state > 0);
    return true;
}

bool ChatHandler::HandleSetValueHelper(Object* target, uint32 field, char* typeStr, char* valStr)
{
    ObjectGuid guid = target->GetObjectGuid();

    // not allow access to not existed or critical for work field
    if (field >= target->GetValuesCount() || field <= OBJECT_FIELD_ENTRY)
    {
        PSendSysMessage(LANG_TOO_BIG_INDEX, field, guid.GetString().c_str(), target->GetValuesCount());
        return false;
    }

    uint32 base;                                            // 0 -> float
    if (!typeStr)
        base = 10;
    else if (strncmp(typeStr, "int", strlen(typeStr)) == 0)
        base = 10;
    else if (strncmp(typeStr, "hex", strlen(typeStr)) == 0)
        base = 16;
    else if (strncmp(typeStr, "bit", strlen(typeStr)) == 0)
        base = 2;
    else if (strncmp(typeStr, "float", strlen(typeStr)) == 0)
        base = 0;
    else
        return false;

    if(base)
    {
        uint32 iValue;
        if (!ExtractUInt32Base(&valStr, iValue, base))
            return false;

        DEBUG_LOG(GetMangosString(LANG_SET_UINT), guid.GetString().c_str(), field, iValue);
        target->SetUInt32Value(field , iValue);
        PSendSysMessage(LANG_SET_UINT_FIELD, guid.GetString().c_str(), field, iValue);
    }
    else
    {
        float fValue;
        if (!ExtractFloat(&valStr, fValue))
            return false;

        DEBUG_LOG(GetMangosString(LANG_SET_FLOAT), guid.GetString().c_str(), field, fValue);
        target->SetFloatValue(field , fValue );
        PSendSysMessage(LANG_SET_FLOAT_FIELD, guid.GetString().c_str(), field, fValue);
    }

    return true;
}

bool ChatHandler::HandleDebugSetItemValueCommand(char* args)
{
    uint32 guid;
    if (!ExtractUInt32(&args, guid))
        return false;

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractOptNotLastArg(&args);
    if (!typeStr)
        return false;

    char* valStr = ExtractLiteralArg(&args);
    if (!valStr)
        return false;

    Item *item = m_session->GetPlayer()->GetItemByGuid(ObjectGuid(HIGHGUID_ITEM, guid));
    if (!item)
        return false;

    return HandleSetValueHelper(item, field, typeStr, valStr);
}

bool ChatHandler::HandleDebugSetValueCommand(char* args)
{
    Unit* target = getSelectedUnit();
    if(!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractOptNotLastArg(&args);
    if (!typeStr)
        return false;

    char* valStr = ExtractLiteralArg(&args);
    if (!valStr)
        return false;

    return HandleSetValueHelper(target, field, typeStr, valStr);
}

bool ChatHandler::HandleGetValueHelper(Object* target, uint32 field, char* typeStr)
{
    ObjectGuid guid = target->GetObjectGuid();

    if (field >= target->GetValuesCount())
    {
        PSendSysMessage(LANG_TOO_BIG_INDEX, field, guid.GetString().c_str(), target->GetValuesCount());
        return false;
    }

    uint32 base;                                            // 0 -> float
    if (!typeStr)
        base = 10;
    else if (strncmp(typeStr, "int", strlen(typeStr)) == 0)
        base = 10;
    else if (strncmp(typeStr, "hex", strlen(typeStr)) == 0)
        base = 16;
    else if (strncmp(typeStr, "bit", strlen(typeStr)) == 0)
        base = 2;
    else if (strncmp(typeStr, "float", strlen(typeStr)) == 0)
        base = 0;
    else
        return false;

    if (base)
    {
        uint32 iValue = target->GetUInt32Value(field);

        switch(base)
        {
            case 2:
            {
                // starting 0 if need as required bitstring format
                std::string res;
                res.reserve(1+32+1);
                res = iValue & (1 << (32-1)) ? "0" : " ";
                for (int i = 32; i > 0; --i)
                    res += iValue & (1 << (i-1)) ? "1" : "0";
                DEBUG_LOG(GetMangosString(LANG_GET_BITSTR), guid.GetString().c_str(), field, res.c_str());
                PSendSysMessage(LANG_GET_BITSTR_FIELD, guid.GetString().c_str(), field, res.c_str());
                break;
            }
            case 16:
                DEBUG_LOG(GetMangosString(LANG_GET_HEX), guid.GetString().c_str(), field, iValue);
                PSendSysMessage(LANG_GET_HEX_FIELD, guid.GetString().c_str(), field, iValue);
                break;
            case 10:
            default:
                DEBUG_LOG(GetMangosString(LANG_GET_UINT), guid.GetString().c_str(), field, iValue);
                PSendSysMessage(LANG_GET_UINT_FIELD, guid.GetString().c_str(), field, iValue);
        }
    }
    else
    {
        float fValue = target->GetFloatValue(field);
        DEBUG_LOG(GetMangosString(LANG_GET_FLOAT), guid.GetString().c_str(), field, fValue);
        PSendSysMessage(LANG_GET_FLOAT_FIELD, guid.GetString().c_str(), field, fValue);
    }

    return true;
}

bool ChatHandler::HandleDebugGetItemValueCommand(char* args)
{
    uint32 guid;
    if (!ExtractUInt32(&args, guid))
        return false;

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractLiteralArg(&args);
    if (!typeStr && *args)                                  // optional arg but check format fail case
        return false;

    Item* item = m_session->GetPlayer()->GetItemByGuid(ObjectGuid(HIGHGUID_ITEM, guid));
    if (!item)
        return false;

    return HandleGetValueHelper(item, field, typeStr);
}

bool ChatHandler::HandleDebugGetValueCommand(char* args)
{
    Unit* target = getSelectedUnit();
    if(!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractLiteralArg(&args);
    if (!typeStr && *args)                                  // optional arg but check format fail case
        return false;

    return HandleGetValueHelper(target, field, typeStr);
}

bool ChatHandler::HandlerDebugModValueHelper( Object* target, uint32 field, char* typeStr, char* valStr )
{
    ObjectGuid guid = target->GetObjectGuid();

    // not allow access to not existed or critical for work field
    if (field >= target->GetValuesCount() || field <= OBJECT_FIELD_ENTRY)
    {
        PSendSysMessage(LANG_TOO_BIG_INDEX, field, guid.GetString().c_str(), target->GetValuesCount());
        return false;
    }

    uint32 type;                                            // 0 -> float 1 -> int add 2-> bit or 3 -> bit and  4 -> bit and not
    if (strncmp(typeStr, "int", strlen(typeStr)) == 0)
        type = 1;
    else if (strncmp(typeStr, "float", strlen(typeStr)) == 0)
        type = 0;
    else if (strncmp(typeStr, "|=", strlen("|=")+1) == 0)   // exactly copy
        type = 2;
    else if (strncmp(typeStr, "&=", strlen("&=")+1) == 0)   // exactly copy
        type = 3;
    else if (strncmp(typeStr, "&=~", strlen("&=~")+1) == 0) // exactly copy
        type = 4;
    else
        return false;

    if (type)
    {
        uint32 iValue;
        if (!ExtractUInt32Base(&valStr, iValue, type == 1 ? 10 : 16))
            return false;

        uint32 value = target->GetUInt32Value(field);

        switch(type)
        {
            default:
            case 1:                                         // int +
                value = uint32(int32(value) + int32(iValue));
                DEBUG_LOG(GetMangosString(LANG_CHANGE_INT32), guid.GetString().c_str(), field, iValue, value, value);
                PSendSysMessage(LANG_CHANGE_INT32_FIELD, guid.GetString().c_str(), field, iValue, value, value);
                break;
            case 2:                                         // |= bit or
                value |= iValue;
                DEBUG_LOG(GetMangosString(LANG_CHANGE_HEX), guid.GetString().c_str(), field, typeStr, iValue, value);
                PSendSysMessage(LANG_CHANGE_HEX_FIELD, guid.GetString().c_str(), field, typeStr, iValue, value);
                break;
            case 3:                                         // &= bit and
                value &= iValue;
                DEBUG_LOG(GetMangosString(LANG_CHANGE_HEX), guid.GetString().c_str(), field, typeStr, iValue, value);
                PSendSysMessage(LANG_CHANGE_HEX_FIELD, guid.GetString().c_str(), field, typeStr, iValue, value);
                break;
            case 4:                                         // &=~ bit and not
                value &= ~iValue;
                DEBUG_LOG(GetMangosString(LANG_CHANGE_HEX), guid.GetString().c_str(), field, typeStr, iValue, value);
                PSendSysMessage(LANG_CHANGE_HEX_FIELD, guid.GetString().c_str(), field, typeStr, iValue, value);
                break;
        }

        target->SetUInt32Value(field, value);
    }
    else
    {
        float fValue;
        if (!ExtractFloat(&valStr, fValue))
            return false;

        float value = target->GetFloatValue(field);

        value += fValue;

        DEBUG_LOG(GetMangosString(LANG_CHANGE_FLOAT), guid.GetString().c_str(), field, fValue, value);
        PSendSysMessage(LANG_CHANGE_FLOAT_FIELD, guid.GetString().c_str(), field, fValue, value);

        target->SetFloatValue(field, value);
    }

    return true;
}

bool ChatHandler::HandleDebugModItemValueCommand(char* args)
{
    uint32 guid;
    if (!ExtractUInt32(&args, guid))
        return false;

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractLiteralArg(&args);
    if (!typeStr)
        return false;

    char* valStr = ExtractLiteralArg(&args);
    if (!valStr)
        return false;

    Item *item = m_session->GetPlayer()->GetItemByGuid(ObjectGuid(HIGHGUID_ITEM, guid));
    if (!item)
        return false;

    return HandlerDebugModValueHelper(item, field, typeStr, valStr);
}

bool ChatHandler::HandleDebugModValueCommand(char* args)
{
    Unit* target = getSelectedUnit();
    if(!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 field;
    if (!ExtractUInt32(&args, field))
        return false;

    char* typeStr = ExtractLiteralArg(&args);
    if (!typeStr && *args)                                  // optional arg but check format fail case
        return false;

    char* valStr = ExtractLiteralArg(&args);
    if (!valStr)
        return false;

    return HandlerDebugModValueHelper(target, field, typeStr, valStr);
}

bool ChatHandler::HandleDebugSpellModsCommand(char* args)
{
    char* typeStr = ExtractLiteralArg(&args);
    if (!typeStr)
        return false;

    uint16 opcode;
    if (strncmp(typeStr, "flat", strlen(typeStr)) == 0)
        opcode = SMSG_SET_FLAT_SPELL_MODIFIER;
    else if (strncmp(typeStr, "pct", strlen(typeStr)) == 0)
        opcode = SMSG_SET_PCT_SPELL_MODIFIER;
    else
        return false;

    uint32 effidx;
    if (!ExtractUInt32(&args, effidx) || effidx >= 64+32)
        return false;

    uint32 spellmodop;
    if (!ExtractUInt32(&args, spellmodop) || spellmodop >= MAX_SPELLMOD)
        return false;

    int32 value;
    if (!ExtractInt32(&args, value))
        return false;

    Player *chr = getSelectedPlayer();
    if (chr == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(chr, 0))
        return false;

    PSendSysMessage(LANG_YOU_CHANGE_SPELLMODS, opcode == SMSG_SET_FLAT_SPELL_MODIFIER ? "flat" : "pct",
        spellmodop, value, effidx, GetNameLink(chr).c_str());
    if (needReportToTarget(chr))
        ChatHandler(chr).PSendSysMessage(LANG_YOURS_SPELLMODS_CHANGED, GetNameLink().c_str(),
            opcode == SMSG_SET_FLAT_SPELL_MODIFIER ? "flat" : "pct", spellmodop, value, effidx);

    WorldPacket data(opcode, (1+1+2+2));
    data << uint8(effidx);
    data << uint8(spellmodop);
    data << int32(value);
    chr->GetSession()->SendPacket(&data);

    return true;
}
