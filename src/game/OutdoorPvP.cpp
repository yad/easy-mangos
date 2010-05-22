/*
 * Copyright (C) 2005-2008 MaNGOS <http://getmangos.com/>
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

#include "OutdoorPvP.h"
#include "OutdoorPvPMgr.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Map.h"
#include "MapManager.h"
#include "OutdoorPvPObjectiveAI.h"
#include "Group.h"
#include "WorldPacket.h"

OutdoorPvPObjective::OutdoorPvPObjective(OutdoorPvP * pvp)
: m_PvP(pvp), m_AllianceActivePlayerCount(0), m_HordeActivePlayerCount(0),
    m_ShiftTimer(0), m_ShiftPhase(0), m_ShiftMaxPhase(0), m_OldPhase(0),
    m_State(0), m_OldState(0), m_CapturePoint(0), m_NeutralValue(0), m_ShiftMaxCaptureSpeed(0), m_CapturePointCreature(0),
    m_spawned(false)
{
}

void OutdoorPvPObjective::HandlePlayerEnter(Player * plr)
{
    // only called if really entered
    // player distance and activity state was checked already in the AI
    std::set<uint64>::iterator pitr = m_ActivePlayerGuids.find(plr->GetGUID());
    // if not already counted as active, add player
    if(pitr == m_ActivePlayerGuids.end())
    {
        if(plr->GetTeam() == ALLIANCE)
            ++m_AllianceActivePlayerCount;
        else
            ++m_HordeActivePlayerCount;
        m_ActivePlayerGuids.insert(plr->GetGUID());
        sLog.outDebug("OutdoorPvPObjective: player %u entered an outdoorpvpobjective", plr->GetGUIDLow());
    }
}

void OutdoorPvPObjective::HandlePlayerLeave(Player * plr)
{
    // only decrease the count if the player is in the active list
    if(m_ActivePlayerGuids.find(plr->GetGUID())!=m_ActivePlayerGuids.end())
    {
        if(plr->GetTeam() == ALLIANCE)
            --m_AllianceActivePlayerCount;
        else
            --m_HordeActivePlayerCount;
        m_ActivePlayerGuids.erase(plr->GetGUID());
    }
}

void OutdoorPvPObjective::HandlePlayerActivityChanged(Player *plr)
{
    Map *map = m_PvP->GetMap();
    Creature *c = NULL;
    if (!IsSpawned())
    {
        if (c = map->GetSingleCreatureGuid(m_CapturePointCreature, 0))
            InitSpawn();
        else
            return;
    }

    if (c = map->GetSingleCreature(m_CapturePointCreature, 0))
        if(c->AI())
            c->AI()->MoveInLineOfSight(plr);
}

bool OutdoorPvPObjective::AddObject(uint32 type, uint32 entry, float x, float y, float z, float o, float rotation0, float rotation1, float rotation2, float rotation3)
{
    Map* map = m_PvP->GetMap();
    if (!map)
        return false;

    GameObject* go = new GameObject;
    if (!go->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), entry, map,
        PHASEMASK_NORMAL, x, y, z, o, rotation0, rotation1, rotation2, rotation3, 100, GO_STATE_READY))
    {
        sLog.outError("OutdoorPvP: Gameobject template %u not found in database.", entry);
        delete go;
        return false;
     }
    m_Objects[type] = MAKE_NEW_GUID(go->GetGUID(), entry, HIGHGUID_GAMEOBJECT);
    m_ObjectTypes[m_Objects[type]] = type;
    go->AddToWorld();

    return true;
}

bool OutdoorPvPObjective::AddCreature(uint32 type, uint32 entry, uint32 teamval, float x, float y, float z, float o, uint32 spawntimedelay)
{
    Map* map = m_PvP->GetMap();
    if (!map)
        return false;

    Creature* pCreature = new Creature;
    if (!pCreature->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT), map, PHASEMASK_NORMAL, entry, teamval))
    {
        sLog.outError("OutdoorPvP: Can't create creature entry: %u",entry);
        delete pCreature;
        return false;
    }

    pCreature->Relocate(x, y, z, o);
    if (!pCreature->IsPositionValid())
    {
        sLog.outError("OutdoorPvP: Creature (guidlow %d, entry %d) not added. Suggested coordinates isn't valid (X: %f Y: %f)",
            pCreature->GetGUIDLow(), pCreature->GetEntry(), pCreature->GetPositionX(), pCreature->GetPositionY());
        delete pCreature;
        return false;
    }

    pCreature->AIM_Initialize();

    if (spawntimedelay)
        pCreature->SetRespawnDelay(spawntimedelay);

    m_Creatures[type] = MAKE_NEW_GUID(pCreature->GetGUID(), entry, HIGHGUID_UNIT);
    m_CreatureTypes[m_Creatures[type]] = type;

    map->Add(pCreature);

    return true;
}

bool OutdoorPvPObjective::AddCapturePoint(uint32 entry, float x, float y, float z, float o, float rotation0, float rotation1, float rotation2, float rotation3)
{
    Map* map = m_PvP->GetMap();
    if (!map)
        return false;

    GameObject* go = new GameObject;
    if (!go->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), entry, map,
        PHASEMASK_NORMAL, x, y, z, o, rotation0, rotation1, rotation2, rotation3, 100, GO_STATE_READY))
    {
        sLog.outError("OutdoorPvP: Gameobject template %u not found in database.", entry);
        delete go;
        // TODO: Should we return here?
        //return false;
    }
    else
        go->AddToWorld();

    Creature* pCreature = new Creature;
    if (!pCreature->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT), map,PHASEMASK_NORMAL, OPVP_TRIGGER_CREATURE_ENTRY, 0))
    {
        sLog.outError("OutdoorPvP: Can't create creature entry: %u",entry);
        delete pCreature;
        return false;
    }

    pCreature->Relocate(x, y, z, o);
    if (!pCreature->IsPositionValid())
    {
        sLog.outError("OutdoorPvP: Creature (guidlow %d, entry %d) not added to opvp. Suggested coordinates isn't valid (X: %f Y: %f)",pCreature->GetGUIDLow(),pCreature->GetEntry(),pCreature->GetPositionX(),pCreature->GetPositionY());
        delete pCreature;
        return false;
    }

    pCreature->AIM_Initialize();

    map->Add(pCreature);

    m_CapturePointCreature = MAKE_NEW_GUID(pCreature->GetGUID(), OPVP_TRIGGER_CREATURE_ENTRY, HIGHGUID_UNIT);
    m_CapturePoint = MAKE_NEW_GUID(go->GetGUID(), entry, HIGHGUID_GAMEOBJECT);

    // TODO: Use proper outdoor PvP GO type.
    GameObjectInfo const* goinfo = go->GetGOInfo();
    m_ShiftMaxPhase = goinfo->raw.data[17];
    m_ShiftMaxCaptureSpeed = m_ShiftMaxPhase / float(goinfo->raw.data[16]);
    m_NeutralValue = goinfo->raw.data[12];

    return true;
}

bool OutdoorPvPObjective::DelCreature(uint32 type)
{
    if (!m_Creatures[type])
    {
        sLog.outDebug("OutdoorPvP creature type %u was already deleted",type);
        return false;
    }

    Map* map = m_PvP->GetMap();
    if (!map)
        return false;
    Creature *cr = map->GetCreature(m_Creatures[type]);
    if (!cr)
    {
        sLog.outError("OutdoorPvPObjective: Can't find creature guid: %u", GUID_LOPART(m_Creatures[type]));
        return false;
    }
    cr->AddObjectToRemoveList();
    m_CreatureTypes[m_Creatures[type]] = 0;
    m_Creatures[type] = 0;
    return true;
}

bool OutdoorPvPObjective::DelObject(uint32 type)
{
    if (!m_Objects[type])
        return false;

    Map* map = m_PvP->GetMap();
    GameObject *obj = map->GetGameObject(m_Objects[type]);
    if (!obj)
    {
        sLog.outError("OutdoorPvPObjective: Can't find gobject guid: %u", GUID_LOPART(m_Objects[type]));
        return false;
    }

    obj->SetRespawnTime(0);                                 // not save respawn time
    obj->Delete();
    m_ObjectTypes[m_Objects[type]] = 0;
    m_Objects[type] = 0;
    return true;
}

bool OutdoorPvPObjective::DelCapturePoint()
{
    Map* map = m_PvP->GetMap();

    if (m_CapturePoint)
    {
        GameObject *obj = map->GetGameObject(m_CapturePoint);
        if (obj)
        {
            obj->SetRespawnTime(0);                                 // not save respawn time
            obj->Delete();
        }

        m_CapturePoint = 0;
    }
    if (m_CapturePointCreature)
    {
        Creature *cr = map->GetCreature(m_CapturePointCreature);
        if (cr)
            cr->AddObjectToRemoveList();

        m_CapturePointCreature = 0;
    }
    return true;
}

void OutdoorPvPObjective::DeleteSpawns()
{
    for(std::map<uint32,uint64>::iterator i = m_Objects.begin(); i != m_Objects.end(); ++i)
        DelObject(i->first);
    for(std::map<uint32,uint64>::iterator i = m_Creatures.begin(); i != m_Creatures.end(); ++i)
        DelCreature(i->first);
    DelCapturePoint();
}

void OutdoorPvP::DeleteSpawns()
{
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        (*itr)->DeleteSpawns();
}

OutdoorPvP::OutdoorPvP()
: m_Map(NULL)
{
    sOutdoorPvPMgr.AddOPvP(this);
}

OutdoorPvP::~OutdoorPvP()
{
    DeleteSpawns();
    GetMap()->DelOutdoorPvP(GetTypeId());
    sOutdoorPvPMgr.DelOPvP(this);
}

void OutdoorPvP::HandlePlayerEnterZone(Player * plr, uint32 zone)
{
    if (plr->GetTeam()==ALLIANCE)
        m_PlayerGuids[0].insert(plr->GetGUID());
    else
        m_PlayerGuids[1].insert(plr->GetGUID());
}

void OutdoorPvP::HandlePlayerLeaveZone(Player * plr, uint32 zone)
{
    // inform the objectives of the leaving
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        (*itr)->HandlePlayerLeave(plr);
    // remove the world state information from the player (we can't keep everyone up to date, so leave out those who are not in the concerning zones)
    // outcommenting as test, i think worldstates don't must be removed,
    // if player changes zone i think client handles this already
    /* if(zone != plr->GetZoneId())
        SendRemoveWorldStates(plr);
    */
    if (plr->GetTeam()==ALLIANCE)
        m_PlayerGuids[0].erase(plr->GetGUID());
    else
        m_PlayerGuids[1].erase(plr->GetGUID());
    sLog.outDebug("OutdoorPvP: player left an outdoorpvp zone");
}

bool OutdoorPvP::Update(uint32 diff)
{
    bool objective_changed = false;
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        objective_changed |= (*itr)->Update(diff);
    return objective_changed;
}

bool OutdoorPvPObjective::Update(uint32 diff)
{
    uint32 Challenger = 0;
    if (m_ShiftTimer < diff)
    {
        m_ShiftTimer = OUTDOORPVP_OBJECTIVE_UPDATE_INTERVAL;

        // get the difference of numbers
        float fact_diff = (m_AllianceActivePlayerCount - m_HordeActivePlayerCount);

        if (fact_diff < 0)
        {
            if(fact_diff < - m_ShiftMaxCaptureSpeed)
                fact_diff = - m_ShiftMaxCaptureSpeed;
            Challenger = HORDE;
            // horde is in majority, but it's already horde-controlled -> no change
            if(m_State == OBJECTIVESTATE_HORDE && m_ShiftPhase == - m_ShiftMaxPhase)
                return false;
        }
        else if(fact_diff>0)
        {
            if(fact_diff > m_ShiftMaxCaptureSpeed)
                fact_diff = m_ShiftMaxCaptureSpeed;
            Challenger = ALLIANCE;
            // ally is in majority, but it's already ally-controlled -> no change
            if(m_State == OBJECTIVESTATE_ALLIANCE && m_ShiftPhase == m_ShiftMaxPhase)
                return false;
        }
        else /*if(fact_diff==0)*/ // no change
            return false;

        m_OldPhase = m_ShiftPhase;

        m_OldState = m_State;

        m_ShiftPhase += fact_diff;

        // check limits, these are over the grey part
        if(m_ShiftPhase <= - m_ShiftMaxPhase * (float)(m_NeutralValue) / 100.0f)
        {
            if(m_ShiftPhase <= - m_ShiftMaxPhase)
                m_ShiftPhase = - m_ShiftMaxPhase;
            m_State = OBJECTIVESTATE_HORDE;
            return true;
        }
        else if(m_ShiftPhase >= m_ShiftMaxPhase * (float)(m_NeutralValue) / 100.0f)
        {
            if(m_ShiftPhase >= m_ShiftMaxPhase)
                m_ShiftPhase = m_ShiftMaxPhase;
            m_State = OBJECTIVESTATE_ALLIANCE;
            return true;
        }

        if(m_OldPhase*m_ShiftPhase <=0)
        {
            // gone through neutral
            // if challenger is ally, then n->a challenge
            if(Challenger == ALLIANCE)
                m_State = OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE;
            // if challenger is horde, then n->h challenge
            else if(Challenger == HORDE)
                m_State = OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE;
        }
        else
        {
            // old phase and current are on the same side, so one team challenges the other
            if(Challenger == ALLIANCE && (m_OldState == OBJECTIVESTATE_HORDE || m_OldState == OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE))
                m_State = OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE;
            else if(Challenger == HORDE && (m_OldState == OBJECTIVESTATE_ALLIANCE || m_OldState == OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE))
                m_State = OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE;
        }

        return true;
    } else m_ShiftTimer-=diff;

    return false;
}

bool OutdoorPvPObjective::HandleCaptureCreaturePlayerMoveInLos(Player* plr, Creature* c)
{
    uint64 guid = 0;
    if (!IsSpawned())
    {
        if (guid = map->GetSingleCreatureGuid(m_CaptureEvent, 0))
            InitSpawn();
        else
            return false;
    }
    else
        guid = map->GetSingleCreatureGuid(m_CaptureEvent, 0);

    // check if guid matches
    if(c->GetGUID() != guid)
        return false;

    GameObject* cp = map->GetSingleGameObject(m_CaptureEvent);
    if (!cp)
        return false;

    // check range and activity
    if (cp->IsWithinDistInMap(p,cp->GetGOInfo()->raw.data[0]) && p->IsOutdoorPvPActive())
        // data[8] will be used for player enter
        return HandleCapturePointEvent(plr, cp->GetGOInfo()->raw.data[8]); //i_objective->HandlePlayerEnter((Player*)u);
    else
        // data[9] will be used for player leave
        return HandleCapturePointEvent(plr, cp->GetGOInfo()->raw.data[9]); //i_objective->HandlePlayerLeave((Player*)u);
}

void OutdoorPvP::SendUpdateWorldState(uint32 field, uint32 value)
{
    // send to both factions
    for(int i = 0; i < 2; ++i)
        // send to all players present in the area
        for(std::set<uint64>::iterator itr = m_PlayerGuids[i].begin(); itr != m_PlayerGuids[i].end(); ++itr)
            if (Player* plr = sObjectMgr.GetPlayer(*itr))
                plr->SendUpdateWorldState(field,value);
}

void OutdoorPvPObjective::SendUpdateWorldState(uint32 field, uint32 value)
{
    // send to all players present in the area
    for(std::set<uint64>::iterator itr = m_ActivePlayerGuids.begin(); itr != m_ActivePlayerGuids.end(); ++itr)
        if (Player* plr = sObjectMgr.GetPlayer(*itr))
            plr->SendUpdateWorldState(field,value);
}

void OutdoorPvPObjective::SendObjectiveComplete(uint32 id,uint64 guid)
{
    uint32 controlling_faction;
    switch(m_State)
    {
        case OBJECTIVESTATE_ALLIANCE:
            controlling_faction = ALLIANCE;
            break;
        case OBJECTIVESTATE_HORDE:
            controlling_faction = HORDE;
            break;
        default:
            return;
            break;
    }

    // send to all players present in the area
    for(std::set<uint64>::iterator itr = m_ActivePlayerGuids.begin(); itr != m_ActivePlayerGuids.end(); ++itr)
    {
        Player * plr = sObjectMgr.GetPlayer(*itr);
        if(plr && plr->GetTeam() == controlling_faction)
            plr->KilledMonsterCredit(id,guid);
    }
}

void OutdoorPvP::HandlePlayerActivityChanged(Player * plr)
{
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        (*itr)->HandlePlayerActivityChanged(plr);
}

void OutdoorPvP::HandleKill(Player *killer, Unit * killed)
{
    if(Group * pGroup = killer->GetGroup())
    {
        for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player *pGroupGuy = itr->getSource();

            if(!pGroupGuy)
                continue;

            // skip if too far away
            if(!pGroupGuy->IsAtGroupRewardDistance(killed))
                continue;

            // creature kills must be notified, even if not inside objective / not outdoor pvp active
            // player kills only count if active and inside objective
            if(( pGroupGuy->IsOutdoorPvPActive() && IsInsideObjective(pGroupGuy) ) || killed->GetTypeId() == TYPEID_UNIT)
                HandleKillImpl(pGroupGuy, killed);
        }
    }
    else
    {
        // creature kills must be notified, even if not inside objective / not outdoor pvp active
        if(killer && (( killer->IsOutdoorPvPActive() && IsInsideObjective(killer) ) || killed->GetTypeId() == TYPEID_UNIT))
            HandleKillImpl(killer, killed);
    }
}

bool OutdoorPvP::IsInsideObjective(Player *plr)
{
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        if((*itr)->IsInsideObjective(plr))
            return true;

    return false;
}

bool OutdoorPvP::HandleCustomSpell(Player *plr, uint32 spellId, GameObject * go)
{
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        if((*itr)->HandleCustomSpell(plr,spellId,go))
            return true;

    return false;
}

bool OutdoorPvP::HandleOpenGo(Player *plr, uint64 guid)
{
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        if((*itr)->HandleOpenGo(plr,guid) >= 0)
            return true;

    return false;
}

bool OutdoorPvP::HandleCaptureCreaturePlayerMoveInLos(Player * p, Creature * c)
{
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        if((*itr)->HandleCaptureCreaturePlayerMoveInLos(p, c))
            return true;

    return false;
}

bool OutdoorPvP::HandleGossipOption(Player * plr, uint64 guid, uint32 id)
{
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        if((*itr)->HandleGossipOption(plr, guid, id))
            return true;

    return false;
}

bool OutdoorPvP::CanTalkTo(Player * plr, Creature * c, GossipOption &gso)
{
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        if((*itr)->CanTalkTo(plr, c, gso))
            return true;

    return false;
}

bool OutdoorPvP::HandleDropFlag(Player * plr, uint32 id)
{
    for(OutdoorPvPObjectiveSet::iterator itr = m_OutdoorPvPObjectives.begin(); itr != m_OutdoorPvPObjectives.end(); ++itr)
        if((*itr)->HandleDropFlag(plr, id))
            return true;

    return false;
}

bool OutdoorPvPObjective::IsInsideObjective(Player * plr)
{
    return m_ActivePlayerGuids.find(plr->GetGUID()) != m_ActivePlayerGuids.end();
}
