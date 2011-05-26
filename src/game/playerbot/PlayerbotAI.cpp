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

#include "Config/Config.h"
#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "../ItemPrototype.h"
#include "../World.h"
#include "../SpellMgr.h"
#include "PlayerbotAI.h"
#include "PlayerbotMgr.h"
#include "PlayerbotDeathKnightAI.h"
#include "PlayerbotDruidAI.h"
#include "PlayerbotHunterAI.h"
#include "PlayerbotMageAI.h"
#include "PlayerbotPaladinAI.h"
#include "PlayerbotPriestAI.h"
#include "PlayerbotRogueAI.h"
#include "PlayerbotShamanAI.h"
#include "PlayerbotWarlockAI.h"
#include "PlayerbotWarriorAI.h"
#include "../Player.h"
#include "../ObjectMgr.h"
#include "../Chat.h"
#include "WorldPacket.h"
#include "../Spell.h"
#include "../Unit.h"
#include "../SpellAuras.h"
#include "../SharedDefines.h"
#include "Log.h"
#include "../GossipDef.h"
#include "../ArenaTeam.h"
#include "../MotionMaster.h"
#include "../BattleGroundMgr.h"

// returns a float in range of..
float rand_float(float low, float high)
{
    return (rand() / (static_cast<float> (RAND_MAX) + 1.0)) * (high - low) + low;
}

typedef std::map<ObjectGuid, BattleGroundPlayer> BattleGroundPlayerMap;

// ChatHandler already implements some useful commands the master can call on bots
// These commands are protected inside the ChatHandler class so this class provides access to the commands
// we'd like to call on our bots
class PlayerbotChatHandler : protected ChatHandler
{
public:
    explicit PlayerbotChatHandler(Player* pMasterPlayer) : ChatHandler(pMasterPlayer) {}
    void sysmessage(const char *str) { SendSysMessage(str); }
    bool dropQuest(char *str) { return HandleQuestRemoveCommand(str); }
};

PlayerbotAI::PlayerbotAI(PlayerbotMgr* const mgr, Player* const bot) :
    m_mgr(mgr), m_bot(bot), m_ignoreAIUpdatesUntilTime(0),
    m_TimeDoneEating(0), m_TimeDoneDrinking(0),
    m_CurrentlyCastingSpellId(0), m_enterBg(0), m_leaveBg(0),
    m_targetGuidCommand(0), m_classAI(0),
    m_taxiMaster(ObjectGuid()),
    m_role(0), m_new_role(1)
{
    SetLeader(bot);

    // set bot state and needed item list
    m_botState = BOTSTATE_NORMAL;
    m_targetCombat = NULL;
    m_followTarget = m_bot;

    SetQuestNeedItems();

    m_bot->GetPosition(orig_x, orig_y, orig_z);
    orig_map = m_bot->GetMapId();

    // get class specific ai
    switch (m_bot->getClass())
    {
        case CLASS_WARRIOR:
            m_classAI = (PlayerbotClassAI*) new PlayerbotWarriorAI(m_bot, this);
            break;
        case CLASS_PALADIN:
            m_classAI = (PlayerbotClassAI*) new PlayerbotPaladinAI(m_bot, this);
            break;
        case CLASS_HUNTER:
            m_classAI = (PlayerbotClassAI*) new PlayerbotHunterAI(m_bot, this);
            break;
        case CLASS_ROGUE:
            m_classAI = (PlayerbotClassAI*) new PlayerbotRogueAI(m_bot, this);
            break;
        case CLASS_PRIEST:
            m_classAI = (PlayerbotClassAI*) new PlayerbotPriestAI(m_bot, this);
            break;
        case CLASS_DEATH_KNIGHT:
            m_classAI = (PlayerbotClassAI*) new PlayerbotDeathKnightAI(m_bot, this);
            break;
        case CLASS_SHAMAN:
            m_classAI = (PlayerbotClassAI*) new PlayerbotShamanAI(m_bot, this);
            break;
        case CLASS_MAGE:
            m_classAI = (PlayerbotClassAI*) new PlayerbotMageAI(m_bot, this);
            break;
        case CLASS_WARLOCK:
            m_classAI = (PlayerbotClassAI*) new PlayerbotWarlockAI(m_bot, this);
            break;
        case CLASS_DRUID:
            m_classAI = (PlayerbotClassAI*) new PlayerbotDruidAI(m_bot, this);
            break;
    }
}

PlayerbotAI::~PlayerbotAI()
{
    delete m_classAI;
}

void PlayerbotAI::ReinitAI()
{
    m_botState = BOTSTATE_NORMAL;
    m_targetCombat = NULL;
    m_followTarget = m_bot;

    if (m_bot == GetLeader())
    {
        m_bot->GiveLevel(m_bot->GetLevelAtLoading());
        CheckStuff();
        m_bot->TeleportTo(orig_map, orig_x, orig_y, orig_z, 0.0f);
        for (uint8 i = 0; i < MAX_ARENA_SLOT; ++i)
        {
            uint32 a_id = m_bot->GetArenaTeamId(i);
            if (a_id==0)
                continue;

            ArenaTeam *at = sObjectMgr.GetArenaTeamById(a_id);
            if (!at)
                continue;

            if (at->DisbandNoSave(m_bot))
                delete at;
        }
        if (m_bot->GetGroup())
            m_bot->RemoveFromGroup();
    }
}

Player* PlayerbotAI::GetLeader() const
{
    return m_mgr->GetLeader();
}

void PlayerbotAI::SetLeader(Player* pl)
{
    m_mgr->SetLeader(pl);
}

// finds spell ID for matching substring args
// in priority of full text match, spells not taking reagents, and highest rank
uint32 PlayerbotAI::getSpellId(const char* args, bool master) const
{
    if (!*args)
        return 0;

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return 0;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    int loc = 0;
    if (master)
        loc = GetLeader()->GetSession()->GetSessionDbcLocale();
    else
        loc = m_bot->GetSession()->GetSessionDbcLocale();

    uint32 foundSpellId = 0;
    bool foundExactMatch = false;
    bool foundMatchUsesNoReagents = false;

    for (PlayerSpellMap::iterator itr = m_bot->GetSpellMap().begin(); itr != m_bot->GetSpellMap().end(); ++itr)
    {
        uint32 spellId = itr->first;

        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* pSpellInfo = sSpellStore.LookupEntry(spellId);
        if (!pSpellInfo)
            continue;

        const std::string name = pSpellInfo->SpellName[loc];
        if (name.empty() || !Utf8FitTo(name, wnamepart))
            continue;

        bool isExactMatch = (name.length() == wnamepart.length()) ? true : false;
        bool usesNoReagents = (pSpellInfo->Reagent[0] <= 0) ? true : false;

        // if we already found a spell
        bool useThisSpell = true;
        if (foundSpellId > 0)
        {
            if (isExactMatch && !foundExactMatch) {}
            else if (usesNoReagents && !foundMatchUsesNoReagents) {}
            else if (spellId > foundSpellId) {}
            else
                useThisSpell = false;
        }
        if (useThisSpell)
        {
            foundSpellId = spellId;
            foundExactMatch = isExactMatch;
            foundMatchUsesNoReagents = usesNoReagents;
        }
    }

    return foundSpellId;
}


uint32 PlayerbotAI::getPetSpellId(const char* args) const
{
    if (!*args)
        return 0;

    Pet* pet = m_bot->GetPet();
    if (!pet)
        return 0;

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return 0;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    int loc = GetLeader()->GetSession()->GetSessionDbcLocale();

    uint32 foundSpellId = 0;
    bool foundExactMatch = false;
    bool foundMatchUsesNoReagents = false;

    for (PetSpellMap::iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end(); ++itr)
    {
        uint32 spellId = itr->first;

        if (itr->second.state == PETSPELL_REMOVED || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* pSpellInfo = sSpellStore.LookupEntry(spellId);
        if (!pSpellInfo)
            continue;

        const std::string name = pSpellInfo->SpellName[loc];
        if (name.empty() || !Utf8FitTo(name, wnamepart))
            continue;

        bool isExactMatch = (name.length() == wnamepart.length()) ? true : false;
        bool usesNoReagents = (pSpellInfo->Reagent[0] <= 0) ? true : false;

        // if we already found a spell
        bool useThisSpell = true;
        if (foundSpellId > 0)
        {
            if (isExactMatch && !foundExactMatch) {}
            else if (usesNoReagents && !foundMatchUsesNoReagents) {}
            else if (spellId > foundSpellId) {}
            else
                useThisSpell = false;
        }
        if (useThisSpell)
        {
            foundSpellId = spellId;
            foundExactMatch = isExactMatch;
            foundMatchUsesNoReagents = usesNoReagents;
        }
    }

    return foundSpellId;
}

uint32 PlayerbotAI::initSpell(uint32 spellId, bool ignorePossession)
{
    if (!ignorePossession && !m_bot->HasSpell(spellId))
        return 0;

    uint32 next = 0;
    SpellChainMapNext const& nextMap = sSpellMgr.GetSpellChainNext();
    for (SpellChainMapNext::const_iterator itr = nextMap.lower_bound(spellId); itr != nextMap.upper_bound(spellId); ++itr)
    {
        // Work around buggy chains
        if (sSpellStore.LookupEntry(spellId)->SpellIconID != sSpellStore.LookupEntry(itr->second)->SpellIconID)
            continue;

        SpellChainNode const* node = sSpellMgr.GetSpellChainNode(itr->second);
        // If next spell is a requirement for this one then skip it
        if (node->req == spellId)
            continue;
        if (node->prev == spellId && m_bot->getLevel() >= sSpellStore.LookupEntry(spellId)->spellLevel)
        {
            next = initSpell(itr->second, ignorePossession);
            break;
        }
    }
    if (next == 0)
    {
        const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
        sLog.outDebug("Playerbot spell init: %s is %u", pSpellInfo->SpellName[0], spellId);

        // Add spell to spellrange map
        Spell *spell = new Spell(m_bot, pSpellInfo, false);
        SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(pSpellInfo->rangeIndex);
        float range = GetSpellMaxRange(srange, IsPositiveSpell(spellId));
        m_bot->ApplySpellMod(spellId, SPELLMOD_RANGE, range, spell);
        m_spellRangeMap.insert(std::pair<uint32,float>(spellId, range));
        delete spell;
    }
    return (next == 0) ? spellId : next;
}


// Pet spells do not form chains like player spells.
// One of the options to initialize a spell is to use spell icon id
uint32 PlayerbotAI::initPetSpell(uint32 spellIconId)
{
    Pet * pet = m_bot->GetPet();

    if (!pet)
        return 0;

    for (PetSpellMap::iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end(); ++itr)
    {
        const uint32 spellId = itr->first;

        if (itr->second.state == PETSPELL_REMOVED || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
        if (!pSpellInfo)
            continue;

        if (pSpellInfo->SpellIconID == spellIconId)
            return spellId;
    }

    // Nothing found
    return 0;
}

/*
 * Send a list of equipment that is in bot's inventor that is currently unequipped.
 * This is called when the master is inspecting the bot.
 */

// handle outgoing packets the server would send to the client
void PlayerbotAI::HandleBotOutgoingPacket(const WorldPacket& packet)
{
    switch (packet.GetOpcode())
    {
        case SMSG_DUEL_WINNER:
        {
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_APPLAUD);
            return;
        }
        case SMSG_DUEL_COMPLETE:
        {
            m_bot->GetMotionMaster()->Clear(true);
            return;
        }
        case SMSG_DUEL_OUTOFBOUNDS:
        {
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_CHICKEN);
            return;
        }
        case SMSG_DUEL_REQUESTED:
        {
            WorldPacket p(packet);
            ObjectGuid flagGuid;
            p >> flagGuid;
            ObjectGuid playerGuid;
            p >> playerGuid;

            WorldPacket* const packet = new WorldPacket(CMSG_DUEL_ACCEPTED, 8);
            *packet << flagGuid;
            m_bot->GetSession()->QueuePacket(packet); // queue the packet to get around race condition

            m_ignoreAIUpdatesUntilTime = time(0) + 3;
            return;
        }

        case SMSG_SPELL_FAILURE:
        {
            WorldPacket p(packet);
            uint8 castCount;
            uint32 spellId;

            ObjectGuid casterGuid = p.readPackGUID();
            if (casterGuid != m_bot->GetObjectGuid())
                return;

            p >> castCount >> spellId;
            if (m_CurrentlyCastingSpellId == spellId)
            {
                m_ignoreAIUpdatesUntilTime = time(0);
                m_CurrentlyCastingSpellId = 0;
            }
            return;
        }

        case SMSG_FORCE_RUN_SPEED_CHANGE:
        {
            CheckMount();
            return;
        }

        case SMSG_MOVE_SET_CAN_FLY:
        {
            WorldPacket p(packet);
            ObjectGuid guid = p.readPackGUID();
            if (guid != m_bot->GetObjectGuid())
                return;
            m_bot->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
            return;
        }

        case SMSG_MOVE_UNSET_CAN_FLY:
        {
            WorldPacket p(packet);
            ObjectGuid guid = p.readPackGUID();
            if (guid != m_bot->GetObjectGuid())
                return;
            m_bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
            return;
        }

        // If the master role was given to the bot automatically give it to the master
        // if the master is in the group, otherwise leave group
        case SMSG_GROUP_SET_LEADER:
        {
            WorldPacket p(packet);
            std::string name;
            p >> name;
            if (m_bot->GetGroup() && name == m_bot->GetName())
            {
                Player* newLeader = FindNewGroupLeader();
                if (newLeader)
                {
                    SetLeader(newLeader);
                    m_bot->GetGroup()->ChangeLeader(newLeader->GetObjectGuid());
                }
                else if (m_bot->GetGroup()->IsMember(GetLeader()->GetObjectGuid()))
                {
                    m_bot->GetGroup()->ChangeLeader(newLeader->GetObjectGuid());
                }
            }
            return;
        }

        // Handle Group invites (auto accept if master is in group, otherwise decline & send message
        case SMSG_GROUP_INVITE:
        {
            if (m_bot->GetGroupInvite())
            {
                const Group* const grp = m_bot->GetGroupInvite();
                if (!grp)
                    return;

                Player* const inviter = sObjectMgr.GetPlayer(grp->GetLeaderGuid());
                if (!inviter)
                {
                    m_bot->SetGroupInvite(NULL);
                    return;
                }
                WorldPacket p;

                if (inviter->GetBattleGround())
                {
                    ChatHandler ch(inviter);
                    ch.PSendSysMessage("Impossible d'inviter un bot depuis un champ de bataille");
                    m_bot->GetSession()->HandleGroupDeclineOpcode(p);
                    m_bot->SetGroupInvite(NULL);
                    return;
                }

                if (inviter->getLevel() < 55 && m_bot->getClass() == CLASS_DEATH_KNIGHT)
                {
                    ChatHandler ch(inviter);
                    ch.PSendSysMessage("Impossible d'inviter un Chevalier de la Mort avant le niveau 55");
                    m_bot->GetSession()->HandleGroupDeclineOpcode(p);
                    m_bot->SetGroupInvite(NULL);
                    return;
                }

                m_bot->GetSession()->HandleGroupAcceptOpcode(p);
                MovementClear();
                if (!inviter->IsBot())
                {
                    m_bot->SetDungeonDifficulty(inviter->GetDungeonDifficulty());
                    SetMovementTarget(inviter);
                }
                m_ignoreAIUpdatesUntilTime = time(0);
            }
            return;
        }

        case SMSG_PETITION_SHOW_SIGNATURES:
        {
            WorldPacket p(packet);
            ObjectGuid petitionguid;
            p >> petitionguid;

            WorldPacket packet = WorldPacket(CMSG_PETITION_SIGN, 8+1);
            packet << petitionguid;
            packet << uint8(1);
            m_bot->GetSession()->HandlePetitionSignOpcode(packet);
            return;
        }

        case SMSG_ARENA_TEAM_INVITE:
        {
            WorldPacket pk;
            m_bot->GetSession()->HandleArenaTeamAcceptOpcode(pk);
            break;
        }

        case SMSG_BATTLEFIELD_STATUS:
        {
            WorldPacket p(packet);
            if (p.size() == 4+8)
                return;

            uint32 QueueSlot;
            uint8 type;
            uint8 unk;
            uint32 bgTypeId_;
            uint16 unk0;
            uint8 unk1;
            uint8 unk2;
            uint32 ClientInstanceID;
            uint8 Rated;
            uint32 StatusID;

            p >> QueueSlot >> type >> unk >> bgTypeId_ >> unk0 >> unk1 >> unk2 >> ClientInstanceID >> Rated >> StatusID;

            if (StatusID == STATUS_WAIT_JOIN)
            {
                BattleGroundTypeId bgTypeId = BattleGroundTypeId(bgTypeId_);
                BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(bgTypeId, ArenaType(type));
                WorldPacket packet = WorldPacket(CMSG_BATTLEFIELD_PORT,1+1+4+2+1);
                packet << uint8(type);                           // arenatype if arena
                packet << uint8(unk);                            // unk, can be 0x0 (may be if was invited?) and 0x1
                packet << uint32(bgTypeId_);                     // type id from dbc
                packet << uint16(unk0);                          // 0x1F90 constant?
                packet << uint8(m_bot->IsInvitedForBattleGroundQueueType(bgQueueTypeId) ? 1 : 0);   // enter battle 0x1, leave queue 0x0
                m_bot->GetSession()->HandleBattleFieldPortOpcode(packet);
                m_bot->GetPlayerbotAI()->SetEnterBGTime(60);
            }
            return;
        }
        case MSG_PVP_LOG_DATA:
        {
            BattleGround *bg = m_bot->GetBattleGround();
            if (bg && bg->GetStatus() == STATUS_WAIT_LEAVE)
            {
                m_leaveBg = time(0) + 10;
                m_ignoreAIUpdatesUntilTime = time(0) + 10;
            }
            return;
        }
        // Handle when another player opens the trade window with the bot
        // also sends list of tradable items bot can trade if bot is allowed to obey commands from
        /*case SMSG_TRADE_STATUS:
        {
            if (m_bot->GetTrader() == NULL)
                break;

            WorldPacket p(packet);
            uint32 status;
            p >> status;
            p.resize(4);

            //4 == TRADE_STATUS_TRADE_ACCEPT
            if (status == 4)
                m_bot->GetSession()->HandleAcceptTradeOpcode(p);  // packet not used

            //1 == TRADE_STATUS_BEGIN_TRADE
            else if (status == 1)
            {
                m_bot->GetSession()->HandleBeginTradeOpcode(p); // packet not used

                if (!canObeyCommandFrom(*(m_bot->GetTrader())))
                    return;

                // list out items available for trade


                // list out items in main backpack
                for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
                {
                    const Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    if (pItem && pItem->CanBeTraded())
                    {
                        const ItemPrototype* const pItemProto = pItem->GetProto();

                        std::string itemName = pItemProto->Name1;
                        ItemLocalization(itemName, pItemProto->ItemId);


                            << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
                        if (pItem->GetCount() > 1)

                    }
                }
                // list out items in other removable backpacks
                for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
                {
                    const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                    if (pBag)
                        for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                        {
                            const Item* const pItem = m_bot->GetItemByPos(bag, slot);
                            if (pItem && pItem->CanBeTraded())
                            {
                                const ItemPrototype* const pItemProto = pItem->GetProto();

                                std::string itemName = pItemProto->Name1;
                                ItemLocalization(itemName, pItemProto->ItemId);

                                // item link format: http://www.wowwiki.com/ItemString
                                // itemId, enchantId, jewelId1, jewelId2, jewelId3, jewelId4, suffixId, uniqueId

                                    << ":0:0:0:0:0:0:0" << "|h[" << itemName
                                    << "]|h|r";
                                if (pItem->GetCount() > 1)

                            }
                        }
                }

                // calculate how much money bot has
                uint32 copper = m_bot->GetMoney();
                uint32 gold = uint32(copper / 10000);
                copper -= (gold * 10000);
                uint32 silver = uint32(copper / 100);
                copper -= (silver * 100);

                // send bot the message

                whisper << "Je possède actuellement |cff00ff00" << gold
                        << "|r|cfffffc00g|r|cff00ff00" << silver
                        << "|r|cffcdcdcds|r|cff00ff00" << copper
                        << "|r|cffffd333c|r" << " et je peux échanger les objets suivants :";

                ChatHandler ch(m_bot->GetTrader());
                ch.SendSysMessage(out.str().c_str());
            }
            return;
        }*/

        case SMSG_SPELL_GO:
        {
            WorldPacket p(packet);
            ObjectGuid castItemGuid = p.readPackGUID();
            ObjectGuid casterGuid = p.readPackGUID();
            if (casterGuid != m_bot->GetObjectGuid())
                return;

            uint32 spellId;
            p >> spellId;
            uint16 castFlags;
            p >> castFlags;
            uint32 msTime;
            p >> msTime;
            uint8 numHit;
            p >> numHit;

            if (m_CurrentlyCastingSpellId == spellId)
            {
                Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(spellId);
                if (!pSpell)
                    return;

                if (pSpell->IsChannelActive() || pSpell->IsAutoRepeat())
                    m_ignoreAIUpdatesUntilTime = time(0) + (GetSpellDuration(pSpell->m_spellInfo) / 1000) + 1;
                else if (pSpell->IsAutoRepeat())
                    m_ignoreAIUpdatesUntilTime = time(0) + 6;
                else
                {
                    m_ignoreAIUpdatesUntilTime = time(0) + 1;
                    m_CurrentlyCastingSpellId = 0;
                }
            }
            return;
        }

        // if someone tries to resurrect, then accept
        case SMSG_RESURRECT_REQUEST:
        {
            if (!m_bot->isAlive())
            {
                WorldPacket p(packet);
                ObjectGuid guid;
                p >> guid;

                WorldPacket* const packet = new WorldPacket(CMSG_RESURRECT_RESPONSE, 8+1);
                *packet << guid;
                *packet << uint8(1);                        // accept
                m_bot->GetSession()->QueuePacket(packet);   // queue the packet to get around race condition

                // set back to normal
                SetState(BOTSTATE_NORMAL);
                SetIgnoreUpdateTime(0);
            }
            return;
        }
        /*case SMSG_MONSTER_MOVE:
        case SMSG_UPDATE_WORLD_STATE:
        case SMSG_COMPRESSED_UPDATE_OBJECT:
        case MSG_MOVE_SET_FACING:
        case MSG_MOVE_STOP:
        case MSG_MOVE_HEARTBEAT:
        case MSG_MOVE_STOP_STRAFE:
        case MSG_MOVE_START_STRAFE_LEFT:
        case SMSG_UPDATE_OBJECT:
        case MSG_MOVE_START_FORWARD:
        case MSG_MOVE_START_STRAFE_RIGHT:
        case SMSG_DESTROY_OBJECT:
        case MSG_MOVE_START_BACKWARD:
        case SMSG_AURA_UPDATE_ALL:
        case MSG_MOVE_FALL_LAND:
        case MSG_MOVE_JUMP:
        case CMSG_NAME_QUERY:
        case CMSG_STANDSTATECHANGE:
        case CMSG_QUERY_TIME:
        case CMSG_CREATURE_QUERY:
        case CMSG_GAMEOBJECT_QUERY:
        case SMSG_TIME_SYNC_REQ:
        case CMSG_TIME_SYNC_RESP:
        case CMSG_WORLD_STATE_UI_TIMER_UPDATE:
        case SMSG_WORLD_STATE_UI_TIMER_UPDATE:
        case SMSG_EMOTE:
        case SMSG_POWER_UPDATE:
        case SMSG_AURA_UPDATE:
        case SMSG_PERIODICAURALOG:
            break;
        default:
        {
            const char* oc = LookupOpcodeName(packet.GetOpcode());
            sLog.outString(oc);
        }*/
    }
}

uint8 PlayerbotAI::GetHealthPercent(const Unit& target) const
{
    return (static_cast<float> (target.GetHealth()) / target.GetMaxHealth()) * 100;
}

uint8 PlayerbotAI::GetHealthPercent() const
{
    return GetHealthPercent(*m_bot);
}

uint8 PlayerbotAI::GetManaPercent(const Unit& target) const
{
    return (static_cast<float> (target.GetPower(POWER_MANA)) / target.GetMaxPower(POWER_MANA)) * 100;
}

uint8 PlayerbotAI::GetManaPercent() const
{
    return GetManaPercent(*m_bot);
}

uint8 PlayerbotAI::GetBaseManaPercent(const Unit& target) const
{
    if (target.GetPower(POWER_MANA) >= target.GetCreateMana())
        return (100);
    else
        return (static_cast<float> (target.GetPower(POWER_MANA)) / target.GetCreateMana()) * 100;
}

uint8 PlayerbotAI::GetBaseManaPercent() const
{
    return GetBaseManaPercent(*m_bot);
}

uint8 PlayerbotAI::GetRageAmount(const Unit& target) const
{
    return (static_cast<float> (target.GetPower(POWER_RAGE)));
}

uint8 PlayerbotAI::GetRageAmount() const
{
    return GetRageAmount(*m_bot);
}

uint8 PlayerbotAI::GetEnergyAmount(const Unit& target) const
{
    return (static_cast<float> (target.GetPower(POWER_ENERGY)));
}

uint8 PlayerbotAI::GetEnergyAmount() const
{
    return GetEnergyAmount(*m_bot);
}

uint8 PlayerbotAI::GetRunicPower(const Unit& target) const
{
    return (static_cast<float>(target.GetPower(POWER_RUNIC_POWER)));
}

uint8 PlayerbotAI::GetRunicPower() const
{
    return GetRunicPower(*m_bot);
}

// looks through all items / spells that bot could have to get a mount
Item* PlayerbotAI::FindMount(uint32 matchingRidingSkill) const
{
    // list out items in main backpack

    Item* partialMatch = NULL;

    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK || pItemProto->RequiredSkill != SKILL_RIDING)
                continue;

            if (pItemProto->RequiredSkillRank == matchingRidingSkill)
                return pItem;

            else if (!partialMatch || (partialMatch && partialMatch->GetProto()->RequiredSkillRank < pItemProto->RequiredSkillRank))
                partialMatch = pItem;
        }
    }

    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK || pItemProto->RequiredSkill != SKILL_RIDING)
                        continue;

                    if (pItemProto->RequiredSkillRank == matchingRidingSkill)
                        return pItem;

                    else if (!partialMatch || (partialMatch && partialMatch->GetProto()->RequiredSkillRank < pItemProto->RequiredSkillRank))
                        partialMatch = pItem;
                }
            }
    }
    return partialMatch;
}

Player* PlayerbotAI::FindNewGroupLeader()
{
    Player* pl = NULL;
    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (!ref->getSource()->IsBot())
            {
                pl = ref->getSource();
                break;
            }
            ref = ref->next();
        }
    }
    return pl;
}

void PlayerbotAI::CheckBG()
{
    BattleGround *bg = m_bot->GetBattleGround();
    if (bg)
    {
        bool existsHuman = false;
        BattleGroundPlayerMap players = bg->GetPlayers();
        for(BattleGroundPlayerMap::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player *plr = ObjectAccessor::FindPlayer(itr->first);
            if (!plr || !plr->IsInWorld() || plr->IsBot())
                continue;

            existsHuman = true;
            break;
        }
        if ( (!existsHuman && m_enterBg < time(0)) || (bg->GetStatus() == STATUS_WAIT_LEAVE && m_leaveBg < time(0)) )
        {
            m_bot->LeaveBattleground();
            if (GetLeader()->IsBot())
            {
                SetLeader(m_bot);
                ReinitAI();
            }
        }
    }
}

void PlayerbotAI::UnMount()
{
    WorldPacket emptyPacket;
    m_bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
    m_bot->Unmount();
    m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
    m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED);
    m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED);
}

void PlayerbotAI::CheckMount()
{
    if ((GetLeader()->IsMounted()) && (!m_bot->IsMounted()))
    {
        if (!GetLeader()->GetAurasByType(SPELL_AURA_MOUNTED).empty())
        {
            int32 speed1 = GetLeader()->GetAurasByType(SPELL_AURA_MOUNTED).front()->GetSpellProto()->EffectBasePoints[1];
            int32 speed2 = GetLeader()->GetAurasByType(SPELL_AURA_MOUNTED).front()->GetSpellProto()->EffectBasePoints[2];

            Item* pItem225 = m_bot->GetPlayerbotAI()->FindMount(225);
            Item* pItem300 = m_bot->GetPlayerbotAI()->FindMount(300);

            if (pItem300 && (speed1 > 150 || speed2 > 150))
            {
                m_bot->GetPlayerbotAI()->UseItem(pItem300);
                return;
            }
            else if (pItem225)
            {
                m_bot->GetPlayerbotAI()->UseItem(pItem225);
                return;
            }
            if (pItem300)
            {
                m_bot->GetPlayerbotAI()->UseItem(pItem300);
                return;
            }

            for (PlayerSpellMap::iterator itr = m_bot->GetSpellMap().begin(); itr != m_bot->GetSpellMap().end(); ++itr)
            {
                uint32 spellId = itr->first;
                if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
                    continue;

                const SpellEntry* pSpellInfo = sSpellStore.LookupEntry(spellId);
                if (!pSpellInfo)
                    continue;

                if (pSpellInfo->EffectApplyAuraName[0] == SPELL_AURA_MOUNTED)
                {
                    if (pSpellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED
                        && pSpellInfo->EffectBasePoints[1] == speed1)
                    {
                        CastSpell(spellId, m_bot);
                        break;
                    }
                    else if ((pSpellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED)
                        && (pSpellInfo->EffectApplyAuraName[2] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)
                        && (pSpellInfo->EffectBasePoints[1] == speed1)
                        && (pSpellInfo->EffectBasePoints[2] == speed2))
                    {
                        CastSpell(spellId, m_bot);
                        break;
                    }
                    else if ((pSpellInfo->EffectApplyAuraName[2] == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED)
                        && (pSpellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)
                        && (pSpellInfo->EffectBasePoints[2] == speed2)
                        && (pSpellInfo->EffectBasePoints[1] == speed1))
                    {
                        CastSpell(spellId, m_bot);
                        break;
                    }
                }
            }
        }
    }
    else if (!GetLeader()->IsMounted() && m_bot->IsMounted())
    {
        WorldPacket emptyPacket;
        m_bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        m_bot->Unmount();
        m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
        m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED);
        m_bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED);
    }
}

Item* PlayerbotAI::FindFood() const
{
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                // if is FOOD
                // this enum is no longer defined in mangos. Is it no longer valid?
                // according to google it was 11
                if (pItemProto->Spells[0].SpellCategory == 11)
                    return pItem;
        }
    }
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                        continue;

                    // this enum is no longer defined in mangos. Is it no longer valid?
                    // according to google it was 11
                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                        // if is FOOD
                        // this enum is no longer defined in mangos. Is it no longer valid?
                        // according to google it was 11
                        // if (pItemProto->Spells[0].SpellCategory == SPELL_CATEGORY_FOOD)
                        if (pItemProto->Spells[0].SpellCategory == 11)
                            return pItem;
                }
            }
    }
    return NULL;
}

Item* PlayerbotAI::FindDrink() const
{
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                // if (pItemProto->Spells[0].SpellCategory == SPELL_CATEGORY_DRINK)

                // this enum is no longer defined in mangos. Is it no longer valid?
                // according to google it was 59
                // if (pItemProto->Spells[0].SpellCategory == 59)
                if (pItemProto->Spells[0].SpellCategory == 59)
                    return pItem;
        }
    }
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                        // if is WATER
                        // SPELL_CATEGORY_DRINK is no longer defined in an enum in mangos
                        // google says the valus is 59. Is this still valid?
                        // if (pItemProto->Spells[0].SpellCategory == SPELL_CATEGORY_DRINK)
                        if (pItemProto->Spells[0].SpellCategory == 59)
                            return pItem;
                }
            }
    }
    return NULL;
}

Item* PlayerbotAI::FindBandage() const
{
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_BANDAGE)
                return pItem;
        }
    }
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_BANDAGE)
                        return pItem;
                }
            }
    }
    return NULL;
}
//Find Poison ...Natsukawa
Item* PlayerbotAI::FindPoison() const
{
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == 6)
                return pItem;
        }
    }
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == 6)
                        return pItem;
                }
            }
    }
    return NULL;
}

Item* PlayerbotAI::FindConsumable(uint32 displayId) const
{
    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->DisplayInfoID == displayId)
                return pItem;
        }
    }
    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->DisplayInfoID == displayId)
                        return pItem;
                }
            }
    }
    return NULL;
}

void PlayerbotAI::InterruptCurrentCastingSpell()
{
    WorldPacket* const packet = new WorldPacket(CMSG_CANCEL_CAST, 5);  //changed from thetourist suggestion
    *packet << m_CurrentlyCastingSpellId;
    *packet << m_targetGuidCommand;   //changed from thetourist suggestion
    m_CurrentlyCastingSpellId = 0;
    m_bot->GetSession()->QueuePacket(packet);
}

void PlayerbotAI::Feast()
{
    // stand up if we are done feasting
    if (!(m_bot->GetHealth() < m_bot->GetMaxHealth() || (m_bot->getPowerType() == POWER_MANA && m_bot->GetPower(POWER_MANA) < m_bot->GetMaxPower(POWER_MANA))))
    {
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
        return;
    }

    // wait 3 seconds before checking if we need to drink more or eat more
    time_t currentTime = time(0);
    m_ignoreAIUpdatesUntilTime = currentTime + 3;

    // should we drink another
    if (m_bot->getPowerType() == POWER_MANA && currentTime > m_TimeDoneDrinking
        && ((static_cast<float> (m_bot->GetPower(POWER_MANA)) / m_bot->GetMaxPower(POWER_MANA)) < 0.8))
    {
        Item* pItem = FindDrink();
        if (pItem != NULL)
        {
            UseItem(pItem);
            m_TimeDoneDrinking = currentTime + 30;
            return;
        }
    }

    // should we eat another
    if (currentTime > m_TimeDoneEating && ((static_cast<float> (m_bot->GetHealth()) / m_bot->GetMaxHealth()) < 0.8))
    {
        Item* pItem = FindFood();
        if (pItem != NULL)
        {
            UseItem(pItem);
            m_TimeDoneEating = currentTime + 30;
            return;
        }
    }

    // if we are no longer eating or drinking
    // because we are out of items or we are above 80% in both stats
    if (currentTime > m_TimeDoneEating && currentTime > m_TimeDoneDrinking)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
}

// intelligently sets a reasonable combat order for this bot
// based on its class / level / etc
bool PlayerbotAI::GetCombatTarget(Unit* forcedTarget)
{
    if (m_botState != BOTSTATE_COMBAT)
    {
        SetState(BOTSTATE_COMBAT);
        SetQuestNeedItems();
        m_lootCreature.clear();
        m_lootCurrent = 0;
        m_targetCombat = NULL;
        if (m_bot->HasAura(25990))
            m_bot->RemoveAurasDueToSpell(25990);
    }

    if (forcedTarget)
    {
        m_targetCombat = forcedTarget;
        return true;
    }

    /*TODO BG WP*/
    if (m_bot->GetBattleGround())
    {
        Unit* target = FindEnemy();
        if (target && target != m_targetCombat)
        {
            m_targetCombat = target;
            return true;
        }
    }

    m_targetCombat = NULL;

    if (!m_bot->getAttackers().empty())
    {
        for(Unit::AttackerSet::const_iterator itr = m_bot->getAttackers().begin(); itr != m_bot->getAttackers().end(); ++itr)
        {
            m_targetCombat = (*itr)->GetOwner();
            if (!m_targetCombat || !m_targetCombat->isAlive())
                m_targetCombat = (*itr);
            if (!m_targetCombat)
            {
                m_targetCombat = FindEnemy();
                if (!m_targetCombat)
                    return false;
                return true;
            }
            else
                return true;
        }
    }
    if (!GetLeader()->getAttackers().empty())
    {
        for(Unit::AttackerSet::const_iterator itr = GetLeader()->getAttackers().begin(); itr != GetLeader()->getAttackers().end(); ++itr)
        {
            m_targetCombat = (*itr)->GetOwner();
            if (!m_targetCombat || !m_targetCombat->isAlive())
                m_targetCombat = (*itr);
            if (!m_targetCombat)
            {
                m_targetCombat = FindEnemy();
                if (!m_targetCombat)
                    return false;
                return true;
            }
            else
                return true;
        }
    }
    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (ref->getSource()->GetPet() && !ref->getSource()->GetPet()->getAttackers().empty())
            {
                for(Unit::AttackerSet::const_iterator itr = ref->getSource()->GetPet()->getAttackers().begin(); itr != ref->getSource()->GetPet()->getAttackers().end(); ++itr)
                {
                    m_targetCombat = (*itr)->GetOwner();
                    if (!m_targetCombat || !m_targetCombat->isAlive())
                        m_targetCombat = (*itr);
                    if (!m_targetCombat)
                    {
                        m_targetCombat = FindEnemy();
                        if (!m_targetCombat)
                            return false;
                        return true;
                    }
                    else
                        return true;
                }
            }
            ref = ref->next();
        }
    }

    if (m_bot->GetPet() && !m_bot->GetPet()->getAttackers().empty())
    {
        for(Unit::AttackerSet::const_iterator itr = m_bot->GetPet()->getAttackers().begin(); itr != m_bot->GetPet()->getAttackers().end(); ++itr)
        {
            m_targetCombat = (*itr)->GetOwner();
            if (!m_targetCombat || !m_targetCombat->isAlive())
                m_targetCombat = (*itr);
            if (!m_targetCombat)
            {
                m_targetCombat = FindEnemy();
                if (!m_targetCombat)
                    return false;
                return true;
            }
            else
                return true;
        }
    }
    if (GetLeader()->GetPet() && !GetLeader()->GetPet()->getAttackers().empty())
    {
        for(Unit::AttackerSet::const_iterator itr = GetLeader()->GetPet()->getAttackers().begin(); itr != GetLeader()->GetPet()->getAttackers().end(); ++itr)
        {
            m_targetCombat = (*itr)->GetOwner();
            if (!m_targetCombat || !m_targetCombat->isAlive())
                m_targetCombat = (*itr);
            if (!m_targetCombat)
            {
                m_targetCombat = FindEnemy();
                if (!m_targetCombat)
                    return false;
                return true;
            }
            else
                return true;
        }
    }
    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (!ref->getSource()->getAttackers().empty())
            {
                for(Unit::AttackerSet::const_iterator itr = ref->getSource()->getAttackers().begin(); itr != ref->getSource()->getAttackers().end(); ++itr)
                {
                    m_targetCombat = (*itr)->GetOwner();
                    if (!m_targetCombat || !m_targetCombat->isAlive())
                        m_targetCombat = (*itr);
                    if (!m_targetCombat)
                    {
                        m_targetCombat = FindEnemy();
                        if (!m_targetCombat)
                            return false;
                        return true;
                    }
                    else
                        return true;
                }
            }
            ref = ref->next();
        }
    }

    return false;
}

void PlayerbotAI::DoNextCombatManeuver()
{
    if (m_bot->GetGroup())
    {
        if ((!GetLeader()->IsMounted()) && (m_bot->IsMounted()))
            UnMount();
    }

    bool targetChanged = false;
    if (m_bot->GetBattleGround())
    {
        if (!m_targetCombat || m_targetCombat->isDead() || !m_targetCombat->IsInWorld()
            || !m_bot->IsInMap(m_targetCombat) || !((Player*)m_targetCombat)->GetBattleGround())
        {
            targetChanged = GetCombatTarget();
        }

        if (!m_targetCombat || m_targetCombat->isDead() || !m_targetCombat->IsInWorld()
            || !m_bot->IsInMap(m_targetCombat) || !((Player*)m_targetCombat)->GetBattleGround())
        {
            m_bot->AttackStop();
            m_bot->SetSelectionGuid(ObjectGuid());
            m_bot->InterruptNonMeleeSpells(true);
            m_targetCombat = NULL;
            SetMovementTarget(GetLeader());
            return;
        }
        else
        {
            SetMovementTarget(m_targetCombat);
            if (targetChanged)
            {
                m_bot->SetSelectionGuid((m_targetCombat->GetObjectGuid()));
                if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
                    m_bot->SetStandState(UNIT_STAND_STATE_STAND);
                m_bot->Attack(m_targetCombat, true);
                m_lootCreature.push_back(m_targetCombat->GetObjectGuid());
                GetClassAI()->DoFirstCombatManeuver(m_targetCombat);
            }
            else
                GetClassAI()->DoNextCombatManeuver(m_targetCombat);
        }
    }
    else
    {
        if (!m_targetCombat || m_targetCombat->isDead() || !m_targetCombat->IsInWorld()
            || !m_bot->IsWithinDistInMap(m_targetCombat, 50) || !m_bot->IsHostileTo(m_targetCombat)
            || !m_bot->IsInMap(m_targetCombat))
        {
            targetChanged = GetCombatTarget();
        }

        if (!m_targetCombat || m_targetCombat->isDead() || !m_targetCombat->IsInWorld()
            || !m_bot->IsWithinDistInMap(m_targetCombat, 50) || !m_bot->IsHostileTo(m_targetCombat)
            || !m_bot->IsInMap(m_targetCombat))
        {
            m_bot->AttackStop();
            m_bot->SetSelectionGuid(ObjectGuid());
            m_bot->InterruptNonMeleeSpells(true);
            m_targetCombat = NULL;
            SetMovementTarget(GetLeader());
            return;
        }
        else
        {
            if (m_bot->getClass() != CLASS_HUNTER && m_bot->getClass() != CLASS_PRIEST/* && m_bot->getClass() != CLASS_MAGE && m_bot->getClass() != CLASS_WARLOCK*/)
                SetMovementTarget(m_targetCombat);

            if (targetChanged)
            {
                m_bot->SetSelectionGuid((m_targetCombat->GetObjectGuid()));
                if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
                    m_bot->SetStandState(UNIT_STAND_STATE_STAND);
                m_bot->Attack(m_targetCombat, true);
                m_lootCreature.push_back(m_targetCombat->GetObjectGuid());
                GetClassAI()->DoFirstCombatManeuver(m_targetCombat);
            }
            else
                GetClassAI()->DoNextCombatManeuver(m_targetCombat);
        }
    }
}

void PlayerbotAI::SetQuestNeedItems()
{
    // reset values first
    m_needItemList.clear();
    m_lootCreature.clear();
    m_lootCurrent = 0;

    // run through accepted quests, get quest infoand data
    for (QuestStatusMap::iterator iter = m_bot->getQuestStatusMap().begin(); iter != m_bot->getQuestStatusMap().end(); ++iter)
    {
        const Quest *qInfo = sObjectMgr.GetQuestTemplate(iter->first);
        if (!qInfo)
            continue;

        QuestStatusData *qData = &iter->second;
        // only check quest if it is incomplete
        if (qData->m_status != QUEST_STATUS_INCOMPLETE)
            continue;

        // check for items we not have enough of
        for (int i = 0; i < QUEST_OBJECTIVES_COUNT; i++)
        {
            if (!qInfo->ReqItemCount[i] || (qInfo->ReqItemCount[i] - qData->m_itemcount[i]) <= 0)
                continue;
            m_needItemList[qInfo->ReqItemId[i]] = (qInfo->ReqItemCount[i] - qData->m_itemcount[i]);
        }
    }
}

void PlayerbotAI::SetState(BotState state)
{
    //sLog.outDebug( "[PlayerbotAI]: %s switch state %d to %d", m_bot->GetName(), m_botState, state );
    m_botState = state;
}

uint8 PlayerbotAI::GetFreeBagSpace() const
{
    uint8 space = 0;
    for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        Item *pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (!pItem)
            ++space;
    }
    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
    {
        Bag* pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (pBag && pBag->GetProto()->BagFamily == BAG_FAMILY_MASK_NONE)
            space += pBag->GetFreeSlots();
    }
    return space;
}

void PlayerbotAI::DoFlight()
{
    DEBUG_LOG("DoFlight %s : %s", m_bot->GetName(), m_taxiMaster.GetString().c_str());

    Creature *npc = m_bot->GetNPCIfCanInteractWith(m_taxiMaster, UNIT_NPC_FLAG_FLIGHTMASTER);
    if (!npc)
    {
        DEBUG_LOG("PlayerbotAI: DoFlight - %s not found or you can't interact with it.", m_taxiMaster.GetString().c_str());
        return;
    }

    m_bot->ActivateTaxiPathTo(m_taxiNodes, npc);
}

void PlayerbotAI::DoLoot()
{
    if (!m_lootCurrent && m_lootCreature.empty())
    {
        //sLog.outDebug( "[PlayerbotAI]: %s reset loot list / go back to idle", m_bot->GetName() );
        m_botState = BOTSTATE_NORMAL;
        SetQuestNeedItems();
        return;
    }

    if (!m_lootCurrent)
    {
        m_lootCurrent = m_lootCreature.front();
        m_lootCreature.pop_front();
        Player *m_master = GetLeader();
        Creature *c = m_bot->GetMap()->GetCreature(m_lootCurrent);
        // check if we got a creature and if it is still a corpse, otherwise bot runs to spawn point
        if (!c || c->getDeathState() != CORPSE || GetLeader()->GetDistance(c) > BOTLOOT_DISTANCE)
        {
            m_lootCurrent = 0;
            return;
        }
        SetMovementTarget(m_master);
        //sLog.outDebug( "[PlayerbotAI]: %s is going to loot '%s' deathState=%d", m_bot->GetName(), c->GetName(), c->getDeathState() );
    }
    else
    {
        Player *m_master = GetLeader();
        Creature *c = m_bot->GetMap()->GetCreature(m_lootCurrent);
        if (!c || c->getDeathState() != CORPSE || GetLeader()->GetDistance(c) > BOTLOOT_DISTANCE)
        {
            m_lootCurrent = 0;
            return;
        }
        if (m_master->IsWithinDistInMap(m_bot, 100.0f))
        {
            // check for needed items
            m_bot->SendLoot(m_lootCurrent, LOOT_CORPSE);
            Loot *loot = &c->loot;
            uint32 lootNum = loot->GetMaxSlotInLootFor(m_bot);
            //sLog.outDebug( "[PlayerbotAI]: %s looting: '%s' got %d items", m_bot->GetName(), c->GetName(), loot->GetMaxSlotInLootFor( m_bot ) );
            for (uint32 l = 0; l < lootNum; l++)
            {
                QuestItem *qitem = 0, *ffaitem = 0, *conditem = 0;
                LootItem *item = loot->LootItemInSlot(l, m_bot, &qitem, &ffaitem, &conditem);
                if (!item)
                    continue;

                if (!qitem && item->is_blocked)
                {
                    m_bot->SendLootRelease(m_bot->GetLootGuid());
                    continue;
                }

                if (m_needItemList[item->itemid] > 0)
                {
                    //sLog.outDebug( "[PlayerbotAI]: %s looting: needed item '%s'", m_bot->GetName(), sObjectMgr.GetItemLocale(item->itemid)->Name );
                    ItemPosCountVec dest;
                    if (m_bot->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item->itemid, item->count) == EQUIP_ERR_OK)
                    {
                        Item * newitem = m_bot->StoreNewItem(dest, item->itemid, true, item->randomPropertyId);

                        if (qitem)
                        {
                            qitem->is_looted = true;
                            if (item->freeforall || loot->GetPlayerQuestItems().size() == 1)
                                m_bot->SendNotifyLootItemRemoved(l);
                            else
                                loot->NotifyQuestItemRemoved(qitem->index);
                        }
                        else
                        {
                            if (ffaitem)
                            {
                                ffaitem->is_looted = true;
                                m_bot->SendNotifyLootItemRemoved(l);
                            }
                            else
                            {
                                if (conditem)
                                    conditem->is_looted = true;
                                loot->NotifyItemRemoved(l);
                            }
                        }
                        if (!item->freeforall)
                            item->is_looted = true;
                        --loot->unlootedCount;
                        m_bot->SendNewItem(newitem, uint32(item->count), false, false, true);
                        m_bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM, item->itemid, item->count);
                    }
                }
            }
            // release loot
            // if( uint64 lguid = m_bot->GetLootGUID() && m_bot->GetSession() )
            m_bot->GetSession()->DoLootRelease(m_lootCurrent);

            // clear movement target, take next target on next update
            m_bot->GetMotionMaster()->Clear();
            m_bot->GetMotionMaster()->MoveIdle();
            SetQuestNeedItems();
        }
    }
}

void PlayerbotAI::AcceptQuest(Quest const *qInfo, Player *pGiver)
{
    if (!qInfo || !pGiver)
        return;

    uint32 quest = qInfo->GetQuestId();

    if (!pGiver->CanShareQuest(qInfo->GetQuestId()))
    {
        // giver can't share quest
        m_bot->ClearDividerGuid();
        return;
    }

    if (!m_bot->CanTakeQuest(qInfo, false))
    {
        // can't take quest
        m_bot->ClearDividerGuid();
        return;
    }

    if (!m_bot->GetDividerGuid().IsEmpty())
    {
        // send msg to quest giving player
        pGiver->SendPushToPartyResponse(m_bot, QUEST_PARTY_MSG_ACCEPT_QUEST);
        m_bot->ClearDividerGuid();
    }

    if (m_bot->CanAddQuest(qInfo, false))
    {
        m_bot->AddQuest(qInfo, pGiver);

        if (m_bot->CanCompleteQuest(quest))
            m_bot->CompleteQuest(quest);

        // Runsttren: did not add typeid switch from WorldSession::HandleQuestgiverAcceptQuestOpcode!
        // I think it's not needed, cause typeid should be TYPEID_PLAYER - and this one is not handled
        // there and there is no default case also.

        if (qInfo->GetSrcSpell() > 0)
            m_bot->CastSpell(m_bot, qInfo->GetSrcSpell(), true);
    }
}

void PlayerbotAI::TurnInQuests(WorldObject *questgiver)
{
    ObjectGuid giverGUID = questgiver->GetObjectGuid();

    if (m_bot->IsInMap(questgiver))
    {
        m_bot->SetSelectionGuid(giverGUID);

        // auto complete every completed quest this NPC has
        m_bot->PrepareQuestMenu(giverGUID);
        QuestMenu& questMenu = m_bot->PlayerTalkClass->GetQuestMenu();
        for (uint32 iI = 0; iI < questMenu.MenuItemCount(); ++iI)
        {
            QuestMenuItem const& qItem = questMenu.GetItem(iI);
            uint32 questID = qItem.m_qId;
            Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);

            std::string questTitle  = pQuest->GetTitle();
            QuestLocalization(questTitle, questID);

            QuestStatus status = m_bot->GetQuestStatus(questID);

            // if quest is complete, turn it in
            if (status == QUEST_STATUS_COMPLETE)
            {
                // if bot hasn't already turned quest in
                if (!m_bot->GetQuestRewardStatus(questID))
                {
                    // auto reward quest if no choice in reward
                    if (pQuest->GetRewChoiceItemsCount() == 0)
                    {
                        if (m_bot->CanRewardQuest(pQuest, false))
                        {
                            m_bot->RewardQuest(pQuest, 0, questgiver, false);
                        }
                    }

                    // auto reward quest if one item as reward
                    else if (pQuest->GetRewChoiceItemsCount() == 1)
                    {
                        int rewardIdx = 0;
                        ItemPrototype const *pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[rewardIdx]);
                        std::string itemName = pRewardItem->Name1;
                        ItemLocalization(itemName, pRewardItem->ItemId);
                        if (m_bot->CanRewardQuest(pQuest, rewardIdx, false))
                            m_bot->RewardQuest(pQuest, rewardIdx, questgiver, true);
                    }
                }
            }
        }
    }
}

Player* PlayerbotAI::TargetPlayerFocus()
{
    if (GetCurrentTarget() && GetCurrentTarget()->GetTypeId()==TYPEID_PLAYER)
    {
        return (Player*)GetCurrentTarget();
    }

    if (GetLeader()->GetPlayerbotAI()
        && GetLeader()->GetPlayerbotAI()->GetCurrentTarget()
        && GetLeader()->GetPlayerbotAI()->GetCurrentTarget()->GetTypeId()==TYPEID_PLAYER)
    {
        return (Player*)GetLeader()->GetPlayerbotAI()->GetCurrentTarget();
    }

    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (ref->getSource()->GetPlayerbotAI()
                && ref->getSource()->GetPlayerbotAI()->GetCurrentTarget()
                && ref->getSource()->GetPlayerbotAI()->GetCurrentTarget()->GetTypeId()==TYPEID_PLAYER)
            return (Player*)ref->getSource()->GetPlayerbotAI()->GetCurrentTarget();
            ref = ref->next();
        }
    }
    return NULL;
}

bool PlayerbotAI::IsInCombat()
{
    if (m_bot->duel)
        return true;

    if (m_bot->GetBattleGround())
        return true;

    Player* plTarget = TargetPlayerFocus();
    if (plTarget && plTarget->isAlive() && plTarget->IsInWorld() && m_bot->IsHostileTo(plTarget) && m_bot->IsInMap(plTarget))
    {
        int difflvl = ((int)plTarget->getLevel()) - ((int)m_bot->getLevel());
        if (difflvl > 4)
        {
            if(m_bot->GetGroup() && !plTarget->GetGroup())
                return true;
            else
                return false;
        }
        return true;
    }

    if (!m_bot->getAttackers().empty()
        || (m_bot->GetPet() && !m_bot->GetPet()->getAttackers().empty()))
        return true;

    if (!GetLeader()->getAttackers().empty()
        || (GetLeader()->GetPet() && !GetLeader()->GetPet()->getAttackers().empty()))
        return true;

    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (!ref->getSource()->getAttackers().empty()
                || (ref->getSource()->GetPet() && !ref->getSource()->GetPet()->getAttackers().empty()))
                return true;
            ref = ref->next();
        }
    }
    return false;
}

uint32 PlayerbotAI::EstRepairAll()
{
    uint32 TotalCost = 0;
    // equipped, backpack, bags itself
    for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        TotalCost += EstRepair(((INVENTORY_SLOT_BAG_0 << 8) | i));

    // bank, buyback and keys not repaired

    // items in inventory bags
    for (int j = INVENTORY_SLOT_BAG_START; j < INVENTORY_SLOT_BAG_END; ++j)
        for (int i = 0; i < MAX_BAG_SIZE; ++i)
            TotalCost += EstRepair(((j << 8) | i));
    return TotalCost;
}

uint32 PlayerbotAI::EstRepair(uint16 pos)
{
    Item* item = m_bot->GetItemByPos(pos);

    uint32 TotalCost = 0;
    if (!item)
        return TotalCost;

    uint32 maxDurability = item->GetUInt32Value(ITEM_FIELD_MAXDURABILITY);
    if (!maxDurability)
        return TotalCost;

    uint32 curDurability = item->GetUInt32Value(ITEM_FIELD_DURABILITY);

    uint32 LostDurability = maxDurability - curDurability;
    if (LostDurability > 0)
    {
        ItemPrototype const *ditemProto = item->GetProto();

        DurabilityCostsEntry const *dcost = sDurabilityCostsStore.LookupEntry(ditemProto->ItemLevel);
        if (!dcost)
        {
            sLog.outError("RepairDurability: Wrong item lvl %u", ditemProto->ItemLevel);
            return TotalCost;
        }

        uint32 dQualitymodEntryId = (ditemProto->Quality + 1) * 2;
        DurabilityQualityEntry const *dQualitymodEntry = sDurabilityQualityStore.LookupEntry(dQualitymodEntryId);
        if (!dQualitymodEntry)
        {
            sLog.outError("RepairDurability: Wrong dQualityModEntry %u", dQualitymodEntryId);
            return TotalCost;
        }

        uint32 dmultiplier = dcost->multiplier[ItemSubClassToDurabilityMultiplierId(ditemProto->Class, ditemProto->SubClass)];
        uint32 costs = uint32(LostDurability * dmultiplier * double(dQualitymodEntry->quality_mod));

        if (costs == 0)                                 //fix for ITEM_QUALITY_ARTIFACT
            costs = 1;

        TotalCost = costs;
    }
    return TotalCost;
}

void PlayerbotAI::SetMovementTarget(Unit *followTarget)
{
    if (!followTarget)
    {
        MovementClear();
        return;
    }

    m_followTarget = followTarget;

    Player* target = NULL;
    if (m_followTarget->GetTypeId() == TYPEID_PLAYER)
        target = ((Player*)m_followTarget);

    if (target && target->IsBeingTeleported())
        return;

    if (target && target->GetCorpse())
    {
        if (!FollowCheckTeleport(target->GetCorpse()))
            return;
    }
    else if (!FollowCheckTeleport(m_followTarget))
        return;

    if (m_bot == GetLeader())
    {
        if (!m_bot->GetBattleGround())
        {
            if (m_bot->IsWithinDistInMap(m_followTarget, orig_x, orig_y, orig_z, 5.0f))
            {
                if (FindPOI())
                {
                    m_ignoreAIUpdatesUntilTime = time(0) + urand(5, 30);
                    m_bot->GetMotionMaster()->MoveFollow(m_followTarget, 1.0f, rand_float(0, M_PI_F));
                }
                else
                    m_bot->GetMotionMaster()->MovePoint(0, orig_x, orig_y, orig_z);
            }
            else if (m_bot->IsWithinDistInMap(m_followTarget, orig_x, orig_y, orig_z, 500.0f))
            {
                float xcb, ycb, zcb;
                float xdb, ydb, zdb;
                float xt, yt, zt;
                m_bot->GetMotionMaster()->GetDestination(xdb, ydb, zdb);
                m_bot->GetPosition(xcb, ycb, zcb);
                m_followTarget->GetPosition(xt, yt, zt);
                if (!isInside(xcb, ycb, xdb, ydb, xt, yt, 3.0f))
                {
                    SetInFront(m_followTarget);
                    m_bot->GetMotionMaster()->MoveFollow(m_followTarget, 1.0f, rand_float(0, M_PI_F));
                }
            }
            else
            {
                if (FindPOI())
                    m_bot->GetMotionMaster()->MoveFollow(m_followTarget, 1.0f, rand_float(0, M_PI_F));
                else
                    m_bot->GetMotionMaster()->MovePoint(0, orig_x, orig_y, orig_z);
            }
        }
        else
        {
            if (m_bot->IsWithinDistInMap(m_followTarget, orig_x, orig_y, orig_z, 3.0f))
            {
                if (SetInFront(m_followTarget))
                    MovementClear();
            }
            else
            {
                float xcb, ycb, zcb;
                float xdb, ydb, zdb;
                float xt, yt, zt;
                m_bot->GetMotionMaster()->GetDestination(xdb, ydb, zdb);
                m_bot->GetPosition(xcb, ycb, zcb);
                m_followTarget->GetPosition(xt, yt, zt);
                if (!isInside(xcb, ycb, xdb, ydb, xt, yt, 3.0f))
                {
                    m_bot->GetMotionMaster()->MoveFollow(m_followTarget, 1.0f, rand_float(0, M_PI_F));
                }
            }
        }
    }
    else
    {
        if (!m_bot->GetBattleGround())
        {
            if (m_bot->IsWithinDistInMap(m_followTarget, 100.0f))
            {
                Group *gr = m_bot->GetGroup();
                if (gr->GetMembersCount() >= 6 && !m_bot->IsWithinDistInMap(m_followTarget, 5.0f))
                {
                    float xcb, ycb, zcb;
                    float xdb, ydb, zdb;
                    float xt, yt, zt;
                    m_bot->GetMotionMaster()->GetDestination(xdb, ydb, zdb);
                    m_bot->GetPosition(xcb, ycb, zcb);
                    m_followTarget->GetPosition(xt, yt, zt);
                    if (!isInside(xcb, ycb, xdb, ydb, xt, yt, 5.0f))
                    {
                        m_bot->GetMotionMaster()->MoveFollow(m_followTarget, rand_float(1.0f, 5.0f), rand_float(0, M_PI_F));
                    }
                }
                if (!m_bot->IsWithinDistInMap(m_followTarget, 3.0f))
                {
                    float xcb, ycb, zcb;
                    float xdb, ydb, zdb;
                    float xt, yt, zt;
                    m_bot->GetMotionMaster()->GetDestination(xdb, ydb, zdb);
                    m_bot->GetPosition(xcb, ycb, zcb);
                    m_followTarget->GetPosition(xt, yt, zt);
                    if (!isInside(xcb, ycb, xdb, ydb, xt, yt, 3.0f))
                    {
                        float angle = rand_float(0, M_PI_F);
                        float dist = rand_float(1.0f, 3.0f);
                        m_bot->GetMotionMaster()->MoveFollow(m_followTarget, dist, angle);
                    }
                }
            }
        }
        else
        {
            if (m_bot->IsWithinDistInMap(m_followTarget, 3.0f))
            {
                if (SetInFront(m_followTarget))
                    MovementClear();
            }
            else if (m_bot->IsInMap(GetLeader()))
            {
                float xcb, ycb, zcb;
                float xdb, ydb, zdb;
                float xt, yt, zt;
                m_bot->GetMotionMaster()->GetDestination(xdb, ydb, zdb);
                m_bot->GetPosition(xcb, ycb, zcb);
                m_followTarget->GetPosition(xt, yt, zt);
                if (!isInside(xcb, ycb, xdb, ydb, xt, yt, 3.0f))
                {
                    SetInFront(m_followTarget);
                    float angle = rand_float(0, M_PI_F);
                    float dist = rand_float(1.0f, 3.0f);
                    m_bot->GetMotionMaster()->MoveFollow(m_followTarget, dist, angle);
                }
            }
        }
    }
}

bool PlayerbotAI::isInside(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float delta)
{
    float distAC = sqrt(pow(Cx-Ax, 2)+pow(Cy-Ay, 2));
    float distABC = sqrt(pow(Bx-Ax, 2)+pow(By-Ay, 2)) + sqrt(pow(Cx-Bx, 2)+pow(Cy-By, 2));

    if ((distAC * (1+delta/100)) < distABC)
        return false;

    return true;
}

bool PlayerbotAI::FindPOI()
{
    Unit* target = m_bot->SelectRandomFriendlyTargetBetween(0, 10.0f, 500.0f);

    if (!m_followTarget)
    {
        if (target)
        {
            m_followTarget = target;
            return true;
        }
        else
            return false;
    }
    else
    {
        if (target)
        {
            if (target == m_followTarget)
            {
                m_followTarget = target;
                return false;
            }
            else
            {
                m_followTarget = target;
                return true;
            }
        }
        else
        {
            m_followTarget = NULL;
            return false;
        }
    }
    return false;
}

Unit* PlayerbotAI::FindEnemy()
{
    if (m_bot->duel)
        return m_bot->duel->opponent;
    else if (m_bot->GetBattleGround())
    {
        BattleGroundPlayerMap players = m_bot->GetBattleGround()->GetPlayers();

        for(BattleGroundPlayerMap::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player *plr = ObjectAccessor::FindPlayer(itr->first);
            if (!plr || !plr->IsInWorld() || !plr->isAlive())
                continue;

            if (plr->HasAura(32727) || plr->HasAura(32728) || plr->HasAura(44521))
                return NULL;
        }

        for(BattleGroundPlayerMap::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player *plr = ObjectAccessor::FindPlayer(itr->first);
            if (!plr || !plr->IsInWorld() || !plr->isAlive())
                continue;

            if (plr->GetBGTeam() == m_bot->GetBGTeam())
                continue;

            return plr;
        }
    }
    else
    {
        Unit* target = m_bot->SelectRandomUnfriendlyTargetWithBgCheck(0, 500.0f);
        if (target)
        {
            int difflvl = ((int)target->getLevel()) - ((int)m_bot->getLevel());
            if (3 >= difflvl && difflvl >= 0)
                return target;
        }
    }
    return NULL;
}

void PlayerbotAI::MovementClear()
{
    m_bot->GetMotionMaster()->Clear(true);
    m_bot->clearUnitState(UNIT_STAT_CHASE);
    m_bot->clearUnitState(UNIT_STAT_FOLLOW);
}

bool PlayerbotAI::IsMoving()
{
    return (m_bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE ? false : true);
}

bool PlayerbotAI::SetInFront(const Unit* obj)
{
    if (m_bot->HasInArc((float)M_PI/16.0f, obj))
        return false;

    m_bot->SetInFront(obj);
    float ori = m_bot->GetAngle(obj);
    float x, y, z;
    x = m_bot->m_movementInfo.GetPos()->x;
    y = m_bot->m_movementInfo.GetPos()->y;
    z = m_bot->m_movementInfo.GetPos()->z;
    m_bot->m_movementInfo.ChangePosition(x,y,z,ori);
    m_bot->SendHeartBeat(false);
    return true;
}

void PlayerbotAI::UpdateAI(const uint32 p_time)
{
    time_t currentTime = time(0);
    if (currentTime < m_ignoreAIUpdatesUntilTime)
        return;
    m_ignoreAIUpdatesUntilTime = currentTime + 1;

    if (m_bot->GetTrader())
        return;

    if (!CheckTeleport())
        return;

    if (!CheckMaster())
        return;

    if (m_bot==GetLeader() && m_bot->GetZoneId()==876)
        return;

    CheckBG();
    CheckMMaps();

    if (!m_bot->isAlive())
    {
        if (m_botState != BOTSTATE_DEAD && m_botState != BOTSTATE_DEADRELEASED)
        {
            m_lootCreature.clear();
            m_lootCurrent = 0;
            m_bot->SetSelectionGuid(ObjectGuid());
            m_bot->GetMotionMaster()->Clear(true);
            SetState(BOTSTATE_DEAD);
        }
        else if (m_botState == BOTSTATE_DEAD)
        {
            if (m_bot != GetLeader() && !GetLeader()->GetMap()->IsBattleGroundOrArena())
            {
                if (m_bot->GetCorpse())
                {
                    SetState(BOTSTATE_DEADRELEASED);
                    return;
                }
                m_bot->SetBotDeathTimer();
                m_bot->BuildPlayerRepop();
                SetState(BOTSTATE_DEADRELEASED);

                WorldLocation loc;
                Corpse *corpse = m_bot->GetCorpse();
                corpse->GetPosition(loc);
                m_bot->TeleportTo(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, m_bot->GetOrientation());
                return;
            }
            else
            {
                if (m_bot->GetMap()->IsBattleArena())
                    return;

                if (m_bot->GetCorpse())
                {
                    SetState(BOTSTATE_DEADRELEASED);
                    return;
                }
                m_bot->SetBotDeathTimer();
                m_bot->BuildPlayerRepop();
                SetState(BOTSTATE_DEADRELEASED);

                if (m_bot->GetMap()->IsBattleGround())
                    return;

                WorldLocation loc;
                Corpse *corpse = m_bot->GetCorpse();
                corpse->GetPosition(loc);
                m_bot->TeleportTo(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, m_bot->GetOrientation());
            }
        }
        else if (m_botState == BOTSTATE_DEADRELEASED)
        {
            Corpse *corpse = m_bot->GetCorpse();
            if (!corpse)
                return;

            FollowCheckTeleport(corpse);

            if (corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP) > time(0))
            {
                m_ignoreAIUpdatesUntilTime = corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP);
                return;
            }

            m_ignoreAIUpdatesUntilTime = time(0) + 6;
            PlayerbotChatHandler ch(GetLeader());

            m_bot->ResurrectPlayer(0.5f);
            m_bot->SpawnCorpseBones();

            if (m_bot->getAttackers().empty())
            {
                uint32 lvl = m_bot->getLevel() +1;
                if (lvl < 81 && m_bot == GetLeader())
                {
                    m_bot->GiveLevel(lvl);
                    GetClassAI()->InitSpells(m_bot->GetPlayerbotAI());
                }
                m_bot->GMStartup();
                m_bot->SetHealth(m_bot->GetMaxHealth());
                m_bot->SetPower(m_bot->getPowerType(), m_bot->GetMaxPower(m_bot->getPowerType()));
            }

            SetState(BOTSTATE_NORMAL);
        }
    }
    else
    {
        Spell* const pSpell = GetCurrentSpell();
        if (pSpell && !(pSpell->IsChannelActive() || pSpell->IsAutoRepeat()))
            InterruptCurrentCastingSpell();

        else if (!IsInCombat() && m_bot->IsInWorld())
        {
            if (m_bot->getClass() == CLASS_DEATH_KNIGHT && GetLeader() != m_bot && GetLeader()->getLevel() < 55)
            {
                SetLeader(m_bot);
                ReinitAI();
                return;
            }
            CheckRoles();
            if(CheckLevel())
                CheckStuff();
        }

        if (IsInCombat() || m_targetCombat)
        {
            if (!pSpell || !pSpell->IsChannelActive())
                DoNextCombatManeuver();
            else
                SetIgnoreUpdateTime(1);
        }

        else if (m_botState == BOTSTATE_COMBAT)
        {
            SetState(BOTSTATE_LOOTING);
            SetIgnoreUpdateTime();
        }

        else if (m_botState == BOTSTATE_LOOTING)
        {
            if (m_bot->getClass() == CLASS_HUNTER)
                   m_bot->HandleEmote(1);

            CastAura(25990, m_bot); // Regen hp + mana : Spell Aura Gruccu Food
            DoLoot();
        }

        else if (!IsInCombat())
        {
            SetMovementTarget(GetLeader());
            GetClassAI()->DoNonCombatActions();
            CheckMount();
        }
    }
}

bool PlayerbotAI::CheckTeleport()
{
    if (GetLeader()->IsBeingTeleported())
        return false;

    if (m_bot->IsBeingTeleported())
        return false;

    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (ref->getSource()->IsBeingTeleported())
                return false;
            ref = ref->next();
        }
    }
    return true;
}

bool PlayerbotAI::CheckMaster()
{
    if (m_bot != GetLeader())
    {
        if (!GetLeader() || !GetLeader()->IsInWorld())
        {
            SetLeader(m_bot);
            ReinitAI();
            return false;
        }

        if (!m_bot->GetGroup())
        {
            SetLeader(m_bot);
            ReinitAI();
            return false;
        }

        if (!GetLeader()->GetGroup() && !GetLeader()->GetOriginalGroup())
        {
            SetLeader(m_bot);
            ReinitAI();
            return false;
        }
    }
    return true;
}

void PlayerbotAI::CheckMMaps()
{
    /*Player* leader = GetLeader();
    if (leader->GetTransport() || (m_bot->GetBattleGround() && m_bot->GetBattleGround()->isArena()))
    {
        if(!m_bot->hasUnitState(UNIT_STAT_IGNORE_PATHFINDING))
            m_bot->addUnitState(UNIT_STAT_IGNORE_PATHFINDING);
    }
    else
    {
        if(m_bot->hasUnitState(UNIT_STAT_IGNORE_PATHFINDING))
            m_bot->clearUnitState(UNIT_STAT_IGNORE_PATHFINDING);
    }*/
}

void PlayerbotAI::CheckRoles()
{
    switch (m_bot->getClass())
    {
        case CLASS_WARRIOR:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = WarriorArms;              break;
                        case 1:        m_new_role = WarriorProtection;        break;
                        default:       m_new_role = WarriorFury;              break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotWarriorAI(m_bot, this);
            }
            break;
        case CLASS_PALADIN:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = PaladinCombat;            break;
                        case 1:        m_new_role = PaladinHoly;              break;
                        default:       m_new_role = PaladinProtection;        break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotPaladinAI(m_bot, this);
            }
            break;
        case CLASS_HUNTER:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = HunterBeastMastery;       break;
                        case 1:        m_new_role = HunterSurvival;           break;
                        default:       m_new_role = HunterMarksmanship;       break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotHunterAI(m_bot, this);
            }
            break;
        case CLASS_ROGUE:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = RogueCombat;              break;
                        case 1:        m_new_role = RogueAssassination;       break;
                        default:       m_new_role = RogueSubtlety;            break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotRogueAI(m_bot, this);
            }
            break;
        case CLASS_PRIEST:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = PriestDiscipline;         break;
                        case 1:        m_new_role = PriestHoly;               break;
                        default:       m_new_role = PriestShadow;             break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotPriestAI(m_bot, this);
            }
            break;
        case CLASS_DEATH_KNIGHT:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = DeathKnightBlood;         break;
                        case 1:        m_new_role = DeathKnightFrost;         break;
                        default:       m_new_role = DeathKnightUnholy;        break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotDeathKnightAI(m_bot, this);
            }
            break;
        case CLASS_SHAMAN:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = ShamanElementalCombat;    break;
                        case 1:        m_new_role = ShamanRestoration;        break;
                        default:       m_new_role = ShamanEnhancement;        break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotShamanAI(m_bot, this);
            }
            break;
        case CLASS_MAGE:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = MageFire;                 break;
                        case 1:        m_new_role = MageArcane;               break;
                        default:       m_new_role = MageFrost;                break;
                    }
                }
                if (m_new_role!=MageFire && m_bot->getLevel() < 10)
                {
                    m_role = MageFire;
                    m_new_role = MageFire;
                }
                else if((m_new_role==MageArcane) && m_bot->getLevel() < 60)
                {
                    if (urand(0,1)==0)
                    {
                        m_role = MageFire;
                        m_new_role = MageFire;
                    }
                    else
                    {
                        m_role = MageFrost;
                        m_new_role = MageFrost;
                    }
                }
                else
                {
                    m_role = m_new_role;
                }
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotMageAI(m_bot, this);
            }
            break;
        case CLASS_WARLOCK:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = WarlockDestruction;       break;
                        case 1:        m_new_role = WarlockCurses;            break;
                        default:       m_new_role = WarlockSummoning;         break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotWarlockAI(m_bot, this);
            }
            break;
        case CLASS_DRUID:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = DruidFeralCombat;         break;
                        case 1:        m_new_role = DruidRestoration;         break;
                        default:       m_new_role = DruidBalance;             break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotDruidAI(m_bot, this);
            }
            break;
    }
}

bool PlayerbotAI::CheckLevel()
{
    if (GetLeader()->getLevel() == m_bot->getLevel())
        return false;

    m_bot->GiveLevel(GetLeader()->getLevel());
    return true;
}

void PlayerbotAI::CheckStuff()
{
    m_bot->GMStartup();
    m_bot->SetHealth(m_bot->GetMaxHealth());
    m_bot->SetPower(m_bot->getPowerType(), m_bot->GetMaxPower(m_bot->getPowerType()));
    GetClassAI()->InitSpells(m_bot->GetPlayerbotAI());
}

Spell* PlayerbotAI::GetCurrentSpell() const
{
    if (m_CurrentlyCastingSpellId == 0)
        return NULL;

    Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(m_CurrentlyCastingSpellId);
    return pSpell;
}

bool PlayerbotAI::canObeyCommandFrom(const Player& player) const
{
    return player.GetSession()->GetAccountId() == GetLeader()->GetSession()->GetAccountId();
}

bool PlayerbotAI::CastSpell(uint32 spellId, Unit* target)
{
    if (spellId <= 0)
        return false;

    ObjectGuid oldSel = m_bot->GetSelectionGuid();
    m_bot->SetSelectionGuid(target->GetObjectGuid());
    bool rv = CastSpell(spellId);
    m_bot->SetSelectionGuid(oldSel);
    return rv;
}

bool PlayerbotAI::HasAura(uint32 spellId, const Unit* unit) const
{
    if (spellId <= 0)
        return false;

    for (Unit::SpellAuraHolderMap::const_iterator iter = unit->GetSpellAuraHolderMap().begin(); iter != unit->GetSpellAuraHolderMap().end(); ++iter)
    {
        if (iter->second->GetId() == spellId)
            return true;
    }
    return false;
}

bool PlayerbotAI::CastAura(uint32 spellId, Unit* target)
{
    if (HasAura(spellId, target))
        return true;

    return CastSpell(spellId, target);
}

bool PlayerbotAI::CastSpell(uint32 spellId)
{
    // some AIs don't check if the bot doesn't have spell before using it
    // so just return false when this happens
    if (spellId <= 0)
        return false;

    // check spell cooldown
    if (m_bot->HasSpellCooldown(spellId))
        return false;

    // see Creature.cpp 1738 for reference
    // don't allow bot to cast damage spells on friends
    const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
    if (!pSpellInfo)
        return false;

    // set target
    ObjectGuid targetGUID = m_bot->GetSelectionGuid();
    Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, targetGUID);

    if (!pTarget)
        pTarget = m_bot;

    // Check spell range
    SpellRanges::iterator it = m_spellRangeMap.find(spellId);
    if (it != m_spellRangeMap.end() && (int)it->second != 0)
    {
        float dist = m_bot->GetCombatDistance(pTarget);
        if (dist > it->second + 1.25) // See Spell::CheckRange for modifier value
            return false;
    }

    // Check line of sight
    if (!m_bot->IsWithinLOSInMap(pTarget))
        return false;

    if (IsPositiveSpell(spellId))
    {
        if (pTarget && !m_bot->IsFriendlyTo(pTarget))
            pTarget = m_bot;
    }
    else
    {
        if (pTarget && m_bot->IsFriendlyTo(pTarget))
            return false;

        SetInFront(pTarget);
    }

    // stop movement to prevent cancel spell casting
    SpellCastTimesEntry const * castTimeEntry = sSpellCastTimesStore.LookupEntry(pSpellInfo->CastingTimeIndex);
    if (castTimeEntry && castTimeEntry->CastTime)
        MovementClear();

    // actually cast spell
    m_bot->CastSpell(pTarget, pSpellInfo, false);

    Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(spellId);
    if (!pSpell)
        return false;

    m_CurrentlyCastingSpellId = spellId;
    m_ignoreAIUpdatesUntilTime = time(0) + (int32) ((float) pSpell->GetCastTime() / 1000.0f) + 1;

    return true;
}

bool PlayerbotAI::CastPetSpell(uint32 spellId, Unit* target)
{
    if (spellId == 0)
        return false;

    Pet* pet = m_bot->GetPet();
    if (!pet)
        return false;

    if (pet->HasSpellCooldown(spellId))
        return false;

    const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
    if (!pSpellInfo)
        return false;

    // set target
    Unit* pTarget;
    if (!target)
    {
        ObjectGuid targetGUID = m_bot->GetSelectionGuid();
        pTarget = ObjectAccessor::GetUnit(*m_bot, targetGUID);
    }
    else
        pTarget = target;

    if (IsPositiveSpell(spellId))
    {
        if (pTarget && !m_bot->IsFriendlyTo(pTarget))
            pTarget = m_bot;
    }
    else
    {
        if (pTarget && m_bot->IsFriendlyTo(pTarget))
            return false;

        if (!pet->isInFrontInMap(pTarget, 10)) // distance probably should be calculated
            pet->SetInFront(pTarget);
    }

    pet->CastSpell(pTarget, pSpellInfo, false);

    Spell* const pSpell = pet->FindCurrentSpellBySpellId(spellId);
    if (!pSpell)
        return false;

    return true;
}

// Perform sanity checks and cast spell
bool PlayerbotAI::Buff(uint32 spellId, Unit* target, void (*beforeCast)(Player *))
{
    if (spellId == 0)
        return false;

    SpellEntry const * spellProto = sSpellStore.LookupEntry(spellId);

    if (!spellProto)
        return false;

    if (!target)
        return false;

    // Select appropriate spell rank for target's level
    spellProto = sSpellMgr.SelectAuraRankForLevel(spellProto, target->getLevel());
    if (!spellProto)
        return false;

    // Check if spell will boost one of already existent auras
    bool willBenefitFromSpell = false;
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (spellProto->EffectApplyAuraName[i] == SPELL_AURA_NONE)
            break;

        bool sameOrBetterAuraFound = false;
        int32 bonus = m_bot->CalculateSpellDamage(target, spellProto, SpellEffectIndex(i));
        Unit::AuraList const& auras = target->GetAurasByType(AuraType(spellProto->EffectApplyAuraName[i]));
        for (Unit::AuraList::const_iterator it = auras.begin(); it != auras.end(); ++it)
            if ((*it)->GetModifier()->m_miscvalue == spellProto->EffectMiscValue[i] && (*it)->GetModifier()->m_amount >= bonus)
            {
                sameOrBetterAuraFound = true;
                break;
            }
        willBenefitFromSpell = willBenefitFromSpell || !sameOrBetterAuraFound;
    }

    if (!willBenefitFromSpell)
        return false;

    // Druids may need to shapeshift before casting
    if (beforeCast)
        (*beforeCast)(m_bot);

    return CastSpell(spellProto->Id, target);
}

// Can be used for personal buffs like Mage Armor and Inner Fire
bool PlayerbotAI::SelfBuff(uint32 spellId)
{
    if (spellId == 0)
        return false;

    if (m_bot->HasAura(spellId))
        return false;

    return CastSpell(spellId, m_bot);
}

// Checks if spell is single per target per caster and will make any effect on target
bool PlayerbotAI::CanReceiveSpecificSpell(uint8 spec, Unit* target) const
{
    if (IsSingleFromSpellSpecificPerTargetPerCaster(SpellSpecific(spec), SpellSpecific(spec)))
    {
        Unit::SpellAuraHolderMap holders = target->GetSpellAuraHolderMap();
        Unit::SpellAuraHolderMap::iterator it;
        for (it = holders.begin(); it != holders.end(); ++it)
            if ((*it).second->GetCasterGuid() == m_bot->GetObjectGuid() && GetSpellSpecific((*it).second->GetId()) == SpellSpecific(spec))
                return false;
    }
    return true;
}

Item* PlayerbotAI::FindItem(uint32 ItemId)
{
    // list out items in main backpack
    //INVENTORY_SLOT_ITEM_START = 23
    //INVENTORY_SLOT_ITEM_END = 39

    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        // sLog.outDebug("[%s's]backpack slot = %u",m_bot->GetName(),slot); // 23 to 38 = 16
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);  // 255, 23 to 38
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto)
                continue;

            if (pItemProto->ItemId == ItemId)   // have required item
                return pItem;
        }
    }
    // list out items in other removable backpacks
    //INVENTORY_SLOT_BAG_START = 19
    //INVENTORY_SLOT_BAG_END = 23

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)  // 20 to 23 = 4
    {
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);   // 255, 20 to 23
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                sLog.outDebug("[%s's]bag[%u] slot = %u", m_bot->GetName(), bag, slot);  // 1 to bagsize = ?
                Item* const pItem = m_bot->GetItemByPos(bag, slot); // 20 to 23, 1 to bagsize
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    if (pItemProto->ItemId == ItemId)        // have required item
                        return pItem;
                }
            }
    }
    return NULL;
}

bool PlayerbotAI::HasPick()
{
    if (m_bot->HasItemTotemCategory(TC_MINING_PICK))
        return true;

    return false;
}

bool PlayerbotAI::HasSpellReagents(uint32 spellId)
{
    const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
    if (!pSpellInfo)
        return false;

    if (m_bot->CanNoReagentCast(pSpellInfo))
        return true;

    for (uint32 i = 0; i < MAX_SPELL_REAGENTS; ++i)
    {
        if(pSpellInfo->Reagent[i] <= 0)
            continue;

        uint32 itemid = pSpellInfo->Reagent[i];
        uint32 count = pSpellInfo->ReagentCount[i];

        if (!m_bot->HasItemCount(itemid, count))
            return false;
    }

    return true;
}

// extracts all item ids in format below
// I decided to roll my own extractor rather then use the one in ChatHandler
// because this one works on a const string, and it handles multiple links
// |color|linkType:key:something1:...:somethingN|h[name]|h|r
void PlayerbotAI::extractItemIds(const std::string& text, std::list<uint32>& itemIds) const
{
    uint8 pos = 0;
    while (true)
    {
        int i = text.find("Hitem:", pos);
        if (i == -1)
            break;
        pos = i + 6;
        int endPos = text.find(':', pos);
        if (endPos == -1)
            break;
        std::string idC = text.substr(pos, endPos - pos);
        uint32 id = atol(idC.c_str());
        pos = endPos;
        if (id)
            itemIds.push_back(id);
    }
}

bool PlayerbotAI::extractSpellId(const std::string& text, uint32 &spellId) const
{

    //   Link format
    //   |cffffffff|Hspell:" << spellId << ":" << "|h[" << pSpellInfo->SpellName[loc] << "]|h|r";
    //   cast |cff71d5ff|Hspell:686|h[Shadow Bolt]|h|r";
    //   012345678901234567890123456
    //        base = 16 >|  +7 >|

    uint8 pos = 0;

    int i = text.find("Hspell:", pos);
    if (i == -1)
        return false;

    // DEBUG_LOG("extractSpellId first pos %u i %u",pos,i);
    pos = i + 7;     // start of window in text 16 + 7 = 23
    int endPos = text.find('|', pos);
    if (endPos == -1)
        return false;

    // DEBUG_LOG("extractSpellId second endpos : %u pos : %u",endPos,pos);
    std::string idC = text.substr(pos, endPos - pos);     // 26 - 23
    spellId = atol(idC.c_str());
    pos = endPos;     // end
    return true;
}

bool PlayerbotAI::extractGOinfo(const std::string& text, uint32 &guid, uint32 &entry, int &mapid, float &x, float &y, float &z) const
{

    //    Link format
    //    |cFFFFFF00|Hfound:" << guid << ':'  << entry << ':' << x << ':' << y << ':' << z  << ':' << mapid << ':' <<  "|h[" << gInfo->name << "]|h|r";
    //    |cFFFFFF00|Hfound:5093:1731:-9295:-270:81.874:0:|h[Copper Vein]|h|r

    uint8 pos = 0;

    // extract GO guid
    int i = text.find("Hfound:", pos);     // base H = 11
    if (i == -1)     // break if error
        return false;

    pos = i + 7;     //start of window in text 11 + 7 = 18
    int endPos = text.find(':', pos);     // end of window in text 22
    if (endPos == -1)     //break if error
        return false;
    std::string guidC = text.substr(pos, endPos - pos);     // get string within window i.e guid 22 - 18 =  4
    guid = atol(guidC.c_str());     // convert ascii to long int

    // extract GO entry
    pos = endPos + 1;
    endPos = text.find(':', pos);     // end of window in text
    if (endPos == -1)     //break if error
        return false;

    std::string entryC = text.substr(pos, endPos - pos);     // get string within window i.e entry
    entry = atol(entryC.c_str());     // convert ascii to float

    // extract GO x
    pos = endPos + 1;
    endPos = text.find(':', pos);     // end of window in text
    if (endPos == -1)     //break if error
        return false;

    std::string xC = text.substr(pos, endPos - pos);     // get string within window i.e x
    x = atof(xC.c_str());     // convert ascii to float

    // extract GO y
    pos = endPos + 1;
    endPos = text.find(':', pos);     // end of window in text
    if (endPos == -1)     //break if error
        return false;

    std::string yC = text.substr(pos, endPos - pos);     // get string within window i.e y
    y = atof(yC.c_str());     // convert ascii to float

    // extract GO z
    pos = endPos + 1;
    endPos = text.find(':', pos);     // end of window in text
    if (endPos == -1)     //break if error
        return false;

    std::string zC = text.substr(pos, endPos - pos);     // get string within window i.e z
    z = atof(zC.c_str());     // convert ascii to float

    //extract GO mapid
    pos = endPos + 1;
    endPos = text.find(':', pos);     // end of window in text
    if (endPos == -1)     //break if error
        return false;

    std::string mapidC = text.substr(pos, endPos - pos);     // get string within window i.e mapid
    mapid = atoi(mapidC.c_str());     // convert ascii to int
    pos = endPos;     // end
    return true;
}

// extracts currency in #g#s#c format
uint32 PlayerbotAI::extractMoney(const std::string& text) const
{
    // if user specified money in ##g##s##c format
    std::string acum = "";
    uint32 copper = 0;
    for (uint8 i = 0; i < text.length(); i++)
    {
        if (text[i] == 'g')
        {
            copper += (atol(acum.c_str()) * 100 * 100);
            acum = "";
        }
        else if (text[i] == 'c')
        {
            copper += atol(acum.c_str());
            acum = "";
        }
        else if (text[i] == 's')
        {
            copper += (atol(acum.c_str()) * 100);
            acum = "";
        }
        else if (text[i] == ' ')
            break;
        else if (text[i] >= 48 && text[i] <= 57)
            acum += text[i];
        else
        {
            copper = 0;
            break;
        }
    }
    return copper;
}

// finds items in equipment and adds Item* to foundItemList
// also removes found item IDs from itemIdSearchList when found
void PlayerbotAI::findItemsInEquip(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const
{
    for (uint8 slot = EQUIPMENT_SLOT_START; itemIdSearchList.size() > 0 && slot < EQUIPMENT_SLOT_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        for (std::list<uint32>::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
        {
            if (pItem->GetProto()->ItemId != *it)
                continue;

            foundItemList.push_back(pItem);
            itemIdSearchList.erase(it);
            break;
        }
    }
}

// finds items in inventory and adds Item* to foundItemList
// also removes found item IDs from itemIdSearchList when found
void PlayerbotAI::findItemsInInv(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const
{

    // look for items in main bag
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; itemIdSearchList.size() > 0 && slot < INVENTORY_SLOT_ITEM_END; ++slot)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        for (std::list<uint32>::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
        {
            if (pItem->GetProto()->ItemId != *it)
                continue;

            foundItemList.push_back(pItem);
            itemIdSearchList.erase(it);
            break;
        }
    }

    // for all for items in other bags
    for (uint8 bag = INVENTORY_SLOT_BAG_START; itemIdSearchList.size() > 0 && bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (!pBag)
            continue;

        for (uint8 slot = 0; itemIdSearchList.size() > 0 && slot < pBag->GetBagSize(); ++slot)
        {
            Item* const pItem = m_bot->GetItemByPos(bag, slot);
            if (!pItem)
                continue;

            for (std::list<uint32>::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
            {
                if (pItem->GetProto()->ItemId != *it)
                    continue;

                foundItemList.push_back(pItem);
                itemIdSearchList.erase(it);
                break;
            }
        }
    }
}

// use item on self
void PlayerbotAI::UseItem(Item *item)
{
    UseItem(item, TARGET_FLAG_SELF, ObjectGuid());
}

// use item on equipped item
void PlayerbotAI::UseItem(Item *item, uint8 targetInventorySlot)
{
    if (targetInventorySlot >= EQUIPMENT_SLOT_END)
        return;

    Item* const targetItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, targetInventorySlot);
    if (!targetItem)
        return;

    UseItem(item, TARGET_FLAG_ITEM, targetItem->GetObjectGuid());
}

// use item on unit
void PlayerbotAI::UseItem(Item *item, Unit *target)
{
    if (!target)
        return;

    UseItem(item, TARGET_FLAG_UNIT, target->GetObjectGuid());
}

// generic item use method
void PlayerbotAI::UseItem(Item *item, uint32 targetFlag, ObjectGuid targetGUID)
{
    if (!item)
        return;

    uint8 bagIndex = item->GetBagSlot();
    uint8 slot = item->GetSlot();
    uint8 cast_count = 1;
    ObjectGuid item_guid = item->GetObjectGuid();
    uint32 glyphIndex = 0;
    uint8 unk_flags = 0;

    uint32 spellId = 0;
    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        if (item->GetProto()->Spells[i].SpellId > 0)
        {
            spellId = item->GetProto()->Spells[i].SpellId;
            break;
        }
    }

    WorldPacket *packet = new WorldPacket(CMSG_USE_ITEM, 28);
    *packet << bagIndex << slot << cast_count << spellId << item_guid
           << glyphIndex << unk_flags << targetFlag;

    if (targetFlag & (TARGET_FLAG_UNIT | TARGET_FLAG_ITEM))
        *packet << targetGUID.WriteAsPacked();

    m_bot->GetSession()->QueuePacket(packet);

    SpellEntry const * spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return;

    SpellCastTimesEntry const * castingTimeEntry = sSpellCastTimesStore.LookupEntry(spellInfo->CastingTimeIndex);
    if (!castingTimeEntry)
        return;

    uint8 duration, castTime;
    castTime = (uint8)((float)castingTimeEntry->CastTime / 1000.0f);

    if (item->GetProto()->Class == ITEM_CLASS_CONSUMABLE && item->GetProto()->SubClass == ITEM_SUBCLASS_FOOD)
    {
        duration = (uint8)((float)GetSpellDuration(spellInfo) / 1000.0f);
        SetIgnoreUpdateTime(castTime + duration);
    }
    else
        SetIgnoreUpdateTime(castTime);
}

// submits packet to use an item
void PlayerbotAI::EquipItem(Item& item)
{
    uint8 bagIndex = item.GetBagSlot();
    uint8 slot = item.GetSlot();

    WorldPacket* const packet = new WorldPacket(CMSG_AUTOEQUIP_ITEM, 2);
    *packet << bagIndex << slot;
    m_bot->GetSession()->QueuePacket(packet);
}

bool PlayerbotAI::FollowCheckTeleport(WorldObject *obj)
{
    if (m_bot->GetMap() && m_bot->GetMap()->IsBattleGroundOrArena())
        return true;

    if (!m_bot->IsWithinDistInMap(obj, 100, true) && GetLeader()->isAlive() && !GetLeader()->IsTaxiFlying())
    {
        m_targetCombat = NULL;
        m_followTarget = NULL;

        if (m_bot == GetLeader())
            return false;

        if (m_bot->IsBeingTeleported())
            return false;

        Map* pMap = GetLeader()->GetMap();

        if (pMap->IsBattleGroundOrArena())
        {
            if (m_bot->GetBattleGroundId() && GetLeader()->GetBattleGroundId() != m_bot->GetBattleGroundId())
                return false;

            m_bot->SetBattleGroundId(GetLeader()->GetBattleGroundId(), GetLeader()->GetBattleGroundTypeId());
            if (!m_bot->GetMap()->IsBattleGroundOrArena())
                m_bot->SetBattleGroundEntryPoint();
        }
        else if (pMap->IsDungeon())
        {
            Map* cMap = m_bot->GetMap();
            if (cMap->Instanceable() && cMap->GetInstanceId() != pMap->GetInstanceId())
                return false;

            if (!GetLeader()->GetGroup() || !m_bot->GetGroup() ||
                (m_bot->GetGroup()->GetLeaderGuid() != GetLeader()->GetObjectGuid()) ||
                (GetLeader()->GetGroup()->GetLeaderGuid() != GetLeader()->GetObjectGuid()))
                return false;
        }

        if (m_bot->IsTaxiFlying())
        {
            m_bot->GetMotionMaster()->MovementExpired();
            m_bot->m_taxi.ClearTaxiDestinations();
        }
        else
            m_bot->SaveRecallPosition();

        float x,y,z;
        GetLeader()->GetClosePoint(x, y, z, m_bot->GetObjectBoundingRadius());
        m_bot->TeleportTo(GetLeader()->GetMapId(),x,y,z,m_bot->GetOrientation());
    }
    return true;
}

void PlayerbotAI::HandleTeleportAck()
{
    m_bot->GetMotionMaster()->Clear(true);
    if (m_bot->IsBeingTeleportedNear())
    {
        WorldPacket p = WorldPacket(MSG_MOVE_TELEPORT_ACK, 8 + 4 + 4);
        p.appendPackGUID(m_bot->GetObjectGuid());
        p << (uint32) 0; // supposed to be flags? not used currently
        p << (uint32) time(0); // time - not currently used
        m_bot->GetSession()->HandleMoveTeleportAckOpcode(p);
    }
    else if (m_bot->IsBeingTeleportedFar())
        m_bot->GetSession()->HandleMoveWorldportAckOpcode();
}

// Localization support
void PlayerbotAI::ItemLocalization(std::string& itemName, const uint32 itemID) const
{
    uint32 loc = GetLeader()->GetSession()->GetSessionDbLocaleIndex();
    std::wstring wnamepart;

    ItemLocale const *pItemInfo = sObjectMgr.GetItemLocale(itemID);
    if (pItemInfo)
        if (pItemInfo->Name.size() > loc && !pItemInfo->Name[loc].empty())
        {
            const std::string name = pItemInfo->Name[loc];
            if (Utf8FitTo(name, wnamepart))
                itemName = name.c_str();
        }
}

void PlayerbotAI::QuestLocalization(std::string& questTitle, const uint32 questID) const
{
    uint32 loc = GetLeader()->GetSession()->GetSessionDbLocaleIndex();
    std::wstring wnamepart;

    QuestLocale const *pQuestInfo = sObjectMgr.GetQuestLocale(questID);
    if (pQuestInfo)
        if (pQuestInfo->Title.size() > loc && !pQuestInfo->Title[loc].empty())
        {
            const std::string title = pQuestInfo->Title[loc];
            if (Utf8FitTo(title, wnamepart))
                questTitle = title.c_str();
        }
}

void PlayerbotAI::GetTaxi(ObjectGuid guid, BotTaxiNode& nodes)
{
    DEBUG_LOG("PlayerbotAI: GetTaxi %s node[0] %d node[1] %d", m_bot->GetName(), nodes[0], nodes[1]);

    Creature *unit = m_bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_FLIGHTMASTER);
    if (!unit)
    {
        DEBUG_LOG("PlayerbotAI: GetTaxi - %s not found or you can't interact with it.", guid.GetString().c_str());
        return;
    }

    if (m_bot->m_taxi.IsTaximaskNodeKnown(nodes[0]) ? 0 : 1)
        return;

    if (m_bot->m_taxi.IsTaximaskNodeKnown(nodes[nodes.size() - 1]) ? 0 : 1)
        return;

    m_taxiNodes = nodes;
    m_taxiMaster = guid;
    SetState(BOTSTATE_FLYING);
}

void PlayerbotAI::Pull()
{
    Group *gr = m_bot->GetGroup();

    if (gr && !IsInCombat())
    {
        if (gr->IsAssistant(m_bot->GetObjectGuid()))
            m_targetCombat = gr->GetAssistTarget();
        else
            m_targetCombat = gr->GetTankTarget();

        m_followTarget = m_targetCombat;

        GetCombatTarget(m_targetCombat);
        SetInFront(m_targetCombat);
        SetMovementTarget(m_targetCombat);
        m_bot->Attack(m_targetCombat, true);
        GetClassAI()->DoFirstCombatManeuver(m_targetCombat);
    }
}
