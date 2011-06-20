/*
 * Copyright (C) 2008-2011 MaNGOS <http://www.MaNGOScore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "WorldPvP.h"
#include "WorldPvPEP.h"


WorldPvPEP::WorldPvPEP() : WorldPvP(),
    m_uiPlaguewoodController(0),
    m_uiEastwallController(0),
    m_uiNorthpassController(0),
    m_uiCrownguardController(0),

    m_uiPlaguewoodState(NEUTRAL),
    m_uiNorthpassState(NEUTRAL),
    m_uiEastwallState(NEUTRAL),
    m_uiCrownguardState(NEUTRAL),

    m_uiPlaguewoodWorldState(WORLD_STATE_PLAGUEWOOD_NEUTRAL),
    m_uiNorthpassWorldState(WORLD_STATE_NORTHPASS_NEUTRAL),
    m_uiEastwallWorldState(WORLD_STATE_EASTWALL_NEUTRAL),
    m_uiCrownguardWorldState(WORLD_STATE_CROWNGUARD_NEUTRAL),

    m_uiTowersAlly(0),
    m_uiTowersHorde(0)
{
    m_uiTypeId = WORLD_PVP_TYPE_EP;

    m_uiFlightMasterGUID.Clear();
    m_uiLordaeronShrineAlyGUID.Clear();
    m_uiLordaeronShrineHordeGUID.Clear();
}

bool WorldPvPEP::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_EASTERN_PLAGUELANDS);
    RegisterZone(ZONE_ID_STRATHOLME);
    RegisterZone(ZONE_ID_SCHOLOMANCE);

    return true;
}

void WorldPvPEP::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_ALY,   m_uiTowersAlly);
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_HORDE, m_uiTowersHorde);

    FillInitialWorldState(data, count, m_uiPlaguewoodWorldState,    1);
    FillInitialWorldState(data, count, m_uiNorthpassWorldState,     1);
    FillInitialWorldState(data, count, m_uiEastwallWorldState,      1);
    FillInitialWorldState(data, count, m_uiCrownguardWorldState,    1);
}

void WorldPvPEP::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_ALY,      0);
    pPlayer->SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HORDE,    0);

    pPlayer->SendUpdateWorldState(m_uiPlaguewoodWorldState,         0);
    pPlayer->SendUpdateWorldState(m_uiNorthpassWorldState,          0);
    pPlayer->SendUpdateWorldState(m_uiEastwallWorldState,           0);
    pPlayer->SendUpdateWorldState(m_uiCrownguardWorldState,         0);
}

void WorldPvPEP::UpdateWorldState()
{
    // update only tower count; tower states is updated in the process event
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_ALY,   m_uiTowersAlly);
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HORDE, m_uiTowersHorde);
}

void WorldPvPEP::HandlePlayerEnterZone(Player* pPlayer)
{
    // cast buff the the player which enters the zone
    switch(pPlayer->GetTeam())
    {
        case ALLIANCE:
            for (uint8 i = 0; i < MAX_TOWERS; i++)
            {
                if (m_uiTowersAlly == m_aPlaguelandsTowerBuffs[i].uiTowers)
                    pPlayer->CastSpell(pPlayer, m_aPlaguelandsTowerBuffs[i].uiSpellIdAlly, true);
            }
            break;
        case HORDE:
            for (uint8 i = 0; i < MAX_TOWERS; i++)
            {
                if (m_uiTowersHorde == m_aPlaguelandsTowerBuffs[i].uiTowers)
                    pPlayer->CastSpell(pPlayer, m_aPlaguelandsTowerBuffs[i].uiSpellIdHorde, true);
            }
            break;
    }

    WorldPvP::HandlePlayerEnterZone(pPlayer);
}

void WorldPvPEP::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the buff from the player
    for (uint8 i = 0; i < MAX_TOWERS; i++)
    {
        if (pPlayer->HasAura(pPlayer->GetTeam() == ALLIANCE ? m_aPlaguelandsTowerBuffs[i].uiSpellIdAlly : m_aPlaguelandsTowerBuffs[i].uiSpellIdHorde))
            pPlayer->RemoveAurasDueToSpell(pPlayer->GetTeam() == ALLIANCE ? m_aPlaguelandsTowerBuffs[i].uiSpellIdAlly : m_aPlaguelandsTowerBuffs[i].uiSpellIdHorde);
    }

    WorldPvP::HandlePlayerLeaveZone(pPlayer);
}

void WorldPvPEP::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_1:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_2:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_3:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_4:
        case GO_TOWER_BANNER:
            // sort banners
            if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[0].m_fX, m_aTowersSpawnLocs[0].m_fY, 50.0f))
                m_lNorthpassTowerBanners.push_back(pGo->GetObjectGuid());
            else if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[1].m_fX, m_aTowersSpawnLocs[1].m_fY, 50.0f))
                m_lCrownguardTowerBanners.push_back(pGo->GetObjectGuid());
            else if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[2].m_fX, m_aTowersSpawnLocs[2].m_fY, 50.0f))
                m_lEastwallTowerBanners.push_back(pGo->GetObjectGuid());
            else if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[3].m_fX, m_aTowersSpawnLocs[3].m_fY, 50.0f))
                m_lPlaguewoodTowerBanners.push_back(pGo->GetObjectGuid());
            // set artkit to neutral
            pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
            break;
        case GO_LORDAERON_SHRINE_ALY:
            m_uiLordaeronShrineAlyGUID = pGo->GetObjectGuid();
            break;
        case GO_LORDAERON_SHRINE_HORDE:
            m_uiLordaeronShrineHordeGUID = pGo->GetObjectGuid();
            break;
    }
}

void WorldPvPEP::HandleObjectiveComplete(PlayerSet m_sPlayersSet, uint32 uiEventId)
{
    uint32 uiCredit = 0;

    switch(uiEventId)
    {
        case EVENT_CROWNGUARD_PROGRESS_ALLIANCE:
        case EVENT_CROWNGUARD_PROGRESS_HORDE:
            uiCredit = NPC_CROWNGUARD_TOWER_QUEST_DOODAD;
            break;
        case EVENT_EASTWALL_PROGRESS_ALLIANCE:
        case EVENT_EASTWALL_PROGRESS_HORDE:
            uiCredit = NPC_EASTWALL_TOWER_QUEST_DOODAD;
            break;
        case EVENT_NORTHPASS_PROGRESS_ALLIANCE:
        case EVENT_NORTHPASS_PROGRESS_HORDE:
            uiCredit = NPC_NORTHPASS_TOWER_QUEST_DOODAD;
            break;
        case EVENT_PLAGUEWOOD_PROGRESS_ALLIANCE:
        case EVENT_PLAGUEWOOD_PROGRESS_HORDE:
            uiCredit = NPC_PLAGUEWOOD_TOWER_QUEST_DOODAD;
            break;
    }

    if (!uiCredit)
        return;

    for(PlayerSet::iterator itr = m_sPlayersSet.begin(); itr != m_sPlayersSet.end(); ++itr)
    {
        if (!(*itr))
            continue;

        (*itr)->KilledMonsterCredit(uiCredit);
        (*itr)->RewardHonor(NULL, 1, HONOR_REWARD_PLAGUELANDS);
    }
}

// process the capture events
void WorldPvPEP::ProcessEvent(GameObject* pGo, Player* pPlayer, uint32 uiEventId)
{
    switch(pGo->GetEntry())
    {
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_1:
            switch(uiEventId)
            {
                case EVENT_NORTHPASS_WIN_ALLIANCE:
                case EVENT_NORTHPASS_WIN_HORDE:
                    ProcessCaptureEvent(WIN, pPlayer->GetTeam(), TOWER_ID_NORTHPASS);
                    break;
                case EVENT_NORTHPASS_CONTEST_ALLIANCE:
                case EVENT_NORTHPASS_CONTEST_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(CONTESTED, pPlayer->GetTeam(), TOWER_ID_NORTHPASS);
                    break;
                case EVENT_NORTHPASS_PROGRESS_ALLIANCE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_NORTHPASS);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_NPT_A));
                    break;
                case EVENT_NORTHPASS_PROGRESS_HORDE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_NORTHPASS);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_NPT_H));
                    break;
                case EVENT_NORTHPASS_NEUTRAL_ALLIANCE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_NORTHPASS);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_NPT_A));
                    break;
                case EVENT_NORTHPASS_NEUTRAL_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_NORTHPASS);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_NPT_H));
                    break;
            }
            break;
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_2:
            switch(uiEventId)
            {
                case EVENT_CROWNGUARD_WIN_ALLIANCE:
                case EVENT_CROWNGUARD_WIN_HORDE:
                    ProcessCaptureEvent(WIN, pPlayer->GetTeam(), TOWER_ID_CROWNGUARD);
                    break;
                case EVENT_CROWNGUARD_CONTEST_ALLIANCE:
                case EVENT_CROWNGUARD_CONTEST_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(CONTESTED, pPlayer->GetTeam(), TOWER_ID_CROWNGUARD);
                    break;
                case EVENT_CROWNGUARD_PROGRESS_ALLIANCE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_CROWNGUARD);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_CGT_A));
                    break;
                case EVENT_CROWNGUARD_PROGRESS_HORDE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_CROWNGUARD);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_CGT_H));
                    break;
                case EVENT_CROWNGUARD_NEUTRAL_ALLIANCE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_CROWNGUARD);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_CGT_A));
                    break;
                case EVENT_CROWNGUARD_NEUTRAL_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_CROWNGUARD);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_CGT_H));
                    break;
            }
            break;
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_3:
            switch(uiEventId)
            {
                case EVENT_EASTWALL_WIN_ALLIANCE:
                case EVENT_EASTWALL_WIN_HORDE:
                    ProcessCaptureEvent(WIN, pPlayer->GetTeam(), TOWER_ID_EASTWALL);
                    break;
                case EVENT_EASTWALL_CONTEST_ALLIANCE:
                case EVENT_EASTWALL_CONTEST_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(CONTESTED, pPlayer->GetTeam(), TOWER_ID_EASTWALL);
                    break;
                case EVENT_EASTWALL_PROGRESS_ALLIANCE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_EASTWALL);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_EWT_A));
                    break;
                case EVENT_EASTWALL_PROGRESS_HORDE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_EASTWALL);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_EWT_H));
                    break;
                case EVENT_EASTWALL_NEUTRAL_ALLIANCE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_EASTWALL);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_EWT_A));
                    break;
                case EVENT_EASTWALL_NEUTRAL_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_EASTWALL);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_EWT_H));
                    break;
            }
            break;
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_4:
            switch(uiEventId)
            {
                case EVENT_PLAGUEWOOD_WIN_ALLIANCE:
                case EVENT_PLAGUEWOOD_WIN_HORDE:
                    ProcessCaptureEvent(WIN, pPlayer->GetTeam(), TOWER_ID_PLAGUEWOOD);
                    break;
                case EVENT_PLAGUEWOOD_CONTEST_ALLIANCE:
                case EVENT_PLAGUEWOOD_CONTEST_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(CONTESTED, pPlayer->GetTeam(), TOWER_ID_PLAGUEWOOD);
                    break;
                case EVENT_PLAGUEWOOD_PROGRESS_ALLIANCE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_PLAGUEWOOD);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_PWT_A));
                    break;
                case EVENT_PLAGUEWOOD_PROGRESS_HORDE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_PLAGUEWOOD);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_CAPTURE_PWT_H));
                    break;
                case EVENT_PLAGUEWOOD_NEUTRAL_ALLIANCE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_PLAGUEWOOD);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_PWT_A));
                    break;
                case EVENT_PLAGUEWOOD_NEUTRAL_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_PLAGUEWOOD);
                    sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_EP_LOOSE_PWT_H));
                    break;
            }
            break;
    }
}

void WorldPvPEP::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_CROWNGUARD_CONTROLLER:
            if (uiData == ALLIANCE)
            {
                if (GetData(TYPE_CROWNGUARD_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersHorde;
                    m_uiCrownguardWorldState = WORLD_STATE_CROWNGUARD_NEUTRAL;
                    // unsummon soldiers
                    DoSetGraveyard(ALLIANCE, true);
                    // set artkit
                    SetBannersArtKit(m_lCrownguardTowerBanners, GO_ARTKIT_BANNER_NEUTRAL);
                }
                else if (GetData(TYPE_CROWNGUARD_STATE) == PROGRESS)
                {
                    m_uiCrownguardWorldState = WORLD_STATE_CROWNGUARD_PROG_ALY;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_CROWNGUARD_CONTROLLER))
                    {
                        // summon alliance soldiers
                        DoSetGraveyard(ALLIANCE);
                        ++m_uiTowersAlly;

                        // set artkit
                        SetBannersArtKit(m_lCrownguardTowerBanners, GO_ARTKIT_BANNER_ALLIANCE);
                    }
                }
                // contested states are also sent by the opposite team
                // alliance contests horde tower
                else if (GetData(TYPE_CROWNGUARD_STATE) == CONTESTED)
                    m_uiCrownguardWorldState = WORLD_STATE_CROWNGUARD_CONT_HORDE;
                else if (GetData(TYPE_CROWNGUARD_STATE) == WIN)
                    m_uiCrownguardWorldState = WORLD_STATE_CROWNGUARD_ALY;
            }
            else if (uiData == HORDE)
            {
                if (GetData(TYPE_CROWNGUARD_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersAlly;
                    m_uiCrownguardWorldState = WORLD_STATE_CROWNGUARD_NEUTRAL;
                    // unsummon soldiers
                    DoSetGraveyard(HORDE, true);
                    // set artkit
                    SetBannersArtKit(m_lCrownguardTowerBanners, GO_ARTKIT_BANNER_NEUTRAL);
                }
                if (GetData(TYPE_CROWNGUARD_STATE) == PROGRESS)
                {
                    m_uiCrownguardWorldState = WORLD_STATE_CROWNGUARD_PROG_HORDE;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_CROWNGUARD_CONTROLLER))
                    {
                        // summon horde soldiers
                        DoSetGraveyard(HORDE);
                        ++m_uiTowersHorde;

                        // set artkit
                        SetBannersArtKit(m_lCrownguardTowerBanners, GO_ARTKIT_BANNER_HORDE);
                    }
                }
                // contested states are also sent by the opposite team
                // horde contests alliance tower
                else if (GetData(TYPE_CROWNGUARD_STATE) == CONTESTED)
                    m_uiCrownguardWorldState = WORLD_STATE_CROWNGUARD_CONT_ALY;
                else if (GetData(TYPE_CROWNGUARD_STATE) == WIN)
                    m_uiCrownguardWorldState = WORLD_STATE_CROWNGUARD_HORDE;
            }
            // set controller only for progress and neutral
            if (GetData(TYPE_CROWNGUARD_STATE) == PROGRESS)
                m_uiCrownguardController = uiData;
            else if (GetData(TYPE_CROWNGUARD_STATE) == NEUTRAL)
                m_uiCrownguardController = 0;
            break;
        case TYPE_EASTWALL_CONTROLLER:
            if (uiData == ALLIANCE)
            {
                if (GetData(TYPE_EASTWALL_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersHorde;
                    m_uiEastwallWorldState = WORLD_STATE_EASTWALL_NEUTRAL;
                    // unsummon soldiers
                    DoUnsummonSoldiers();
                    // set artkit
                    SetBannersArtKit(m_lEastwallTowerBanners, GO_ARTKIT_BANNER_NEUTRAL);
                }
                else if (GetData(TYPE_EASTWALL_STATE) == PROGRESS)
                {
                    m_uiEastwallWorldState = WORLD_STATE_EASTWALL_PROG_ALY;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_EASTWALL_CONTROLLER))
                    {
                        // summon alliance soldiers
                        DoSummonSoldiersIfCan(ALLIANCE);
                        ++m_uiTowersAlly;

                        // set artkit
                        SetBannersArtKit(m_lEastwallTowerBanners, GO_ARTKIT_BANNER_ALLIANCE);
                    }
                }
                // contested states are also sent by the opposite team
                // alliance contests horde tower
                else if (GetData(TYPE_EASTWALL_STATE) == CONTESTED)
                    m_uiEastwallWorldState = WORLD_STATE_EASTWALL_CONT_HORDE;
                else if (GetData(TYPE_EASTWALL_STATE) == WIN)
                    m_uiEastwallWorldState = WORLD_STATE_EASTWALL_ALY;
            }
            else if (uiData == HORDE)
            {
                if (GetData(TYPE_EASTWALL_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersAlly;
                    m_uiEastwallWorldState = WORLD_STATE_EASTWALL_NEUTRAL;
                    // unsummon soldiers
                    DoUnsummonSoldiers();
                    // set artkit
                    SetBannersArtKit(m_lEastwallTowerBanners, GO_ARTKIT_BANNER_NEUTRAL);
                }
                if (GetData(TYPE_EASTWALL_STATE) == PROGRESS)
                {
                    m_uiEastwallWorldState = WORLD_STATE_EASTWALL_PROG_HORDE;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_EASTWALL_CONTROLLER))
                    {
                        // summon horde soldiers
                        DoSummonSoldiersIfCan(HORDE);
                        ++m_uiTowersHorde;

                        // set artkit
                        SetBannersArtKit(m_lEastwallTowerBanners, GO_ARTKIT_BANNER_HORDE);
                    }
                }
                // contested states are also sent by the opposite team
                // horde contests alliance tower
                else if (GetData(TYPE_EASTWALL_STATE) == CONTESTED)
                    m_uiEastwallWorldState = WORLD_STATE_EASTWALL_CONT_ALY;
                else if (GetData(TYPE_EASTWALL_STATE) == WIN)
                    m_uiEastwallWorldState = WORLD_STATE_EASTWALL_HORDE;
            }
            // set controller only for progress and neutral
            if (GetData(TYPE_EASTWALL_STATE) == PROGRESS)
                m_uiEastwallController = uiData;
            else if (GetData(TYPE_EASTWALL_STATE) == NEUTRAL)
                m_uiEastwallController = 0;
            break;
        case TYPE_NORTHPASS_CONTROLLER:
            if (uiData == ALLIANCE)
            {
                if (GetData(TYPE_NORTHPASS_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersHorde;
                    m_uiNorthpassWorldState = WORLD_STATE_NORTHPASS_NEUTRAL;
                    // delete aly shrine
                    DoUpdateShrine(m_uiLordaeronShrineHordeGUID, true);
                    // set artkit
                    SetBannersArtKit(m_lNorthpassTowerBanners, GO_ARTKIT_BANNER_NEUTRAL);
                }
                else if (GetData(TYPE_NORTHPASS_STATE) == PROGRESS)
                {
                    m_uiNorthpassWorldState = WORLD_STATE_NORTHPASS_PROG_ALY;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_NORTHPASS_CONTROLLER))
                    {
                        DoUpdateShrine(m_uiLordaeronShrineAlyGUID);
                        ++m_uiTowersAlly;

                        // set artkit
                        SetBannersArtKit(m_lNorthpassTowerBanners, GO_ARTKIT_BANNER_ALLIANCE);
                    }
                }
                // contested states are also sent by the opposite team
                // alliance contests horde tower
                else if (GetData(TYPE_NORTHPASS_STATE) == CONTESTED)
                    m_uiNorthpassWorldState = WORLD_STATE_NORTHPASS_CONT_HORDE;
                else if (GetData(TYPE_NORTHPASS_STATE) == WIN)
                    m_uiNorthpassWorldState = WORLD_STATE_NORTHPASS_ALY;
            }
            else if (uiData == HORDE)
            {
                if (GetData(TYPE_NORTHPASS_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersAlly;
                    m_uiNorthpassWorldState = WORLD_STATE_NORTHPASS_NEUTRAL;
                    // delete aly shrine
                    DoUpdateShrine(m_uiLordaeronShrineAlyGUID, true);
                    // set artkit
                    SetBannersArtKit(m_lNorthpassTowerBanners, GO_ARTKIT_BANNER_NEUTRAL);
                }
                if (GetData(TYPE_NORTHPASS_STATE) == PROGRESS)
                {
                    m_uiNorthpassWorldState = WORLD_STATE_NORTHPASS_PROG_HORDE;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_NORTHPASS_CONTROLLER))
                    {
                        // respawn horde shrine
                        DoUpdateShrine(m_uiLordaeronShrineHordeGUID);
                        ++m_uiTowersHorde;

                        // set artkit
                        SetBannersArtKit(m_lNorthpassTowerBanners, GO_ARTKIT_BANNER_HORDE);
                    }
                }
                // contested states are also sent by the opposite team
                // horde contests alliance tower
                else if (GetData(TYPE_NORTHPASS_STATE) == CONTESTED)
                    m_uiNorthpassWorldState = WORLD_STATE_NORTHPASS_CONT_ALY;
                else if (GetData(TYPE_NORTHPASS_STATE) == WIN)
                    m_uiNorthpassWorldState = WORLD_STATE_NORTHPASS_HORDE;
            }
            // set controller only for progress and neutral
            if (GetData(TYPE_NORTHPASS_STATE) == PROGRESS)
                m_uiNorthpassController = uiData;
            else if (GetData(TYPE_NORTHPASS_STATE) == NEUTRAL)
                m_uiNorthpassController = 0;
            break;
        case TYPE_PLAGUEWOOD_CONTROLLER:
            if (uiData == ALLIANCE)
            {
                if (GetData(TYPE_PLAGUEWOOD_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersHorde;
                    m_uiPlaguewoodWorldState = WORLD_STATE_PLAGUEWOOD_NEUTRAL;
                    // unsummon flightmaster
                    DoUnsummonFlightMaster();
                    // set artkit
                    SetBannersArtKit(m_lPlaguewoodTowerBanners, GO_ARTKIT_BANNER_NEUTRAL);
                }
                else if (GetData(TYPE_PLAGUEWOOD_STATE) == PROGRESS)
                {
                    m_uiPlaguewoodWorldState = WORLD_STATE_PLAGUEWOOD_PROG_ALY;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_PLAGUEWOOD_CONTROLLER))
                    {
                        DoSummonFlightMasterIfCan(ALLIANCE);
                        ++m_uiTowersAlly;

                        // set artkit
                        SetBannersArtKit(m_lPlaguewoodTowerBanners, GO_ARTKIT_BANNER_ALLIANCE);
                    }
                }
                // contested states are also sent by the opposite team
                // alliance contests horde tower
                else if (GetData(TYPE_PLAGUEWOOD_STATE) == CONTESTED)
                    m_uiPlaguewoodWorldState = WORLD_STATE_PLAGUEWOOD_CONT_HORDE;
                else if (GetData(TYPE_PLAGUEWOOD_STATE) == WIN)
                    m_uiPlaguewoodWorldState = WORLD_STATE_PLAGUEWOOD_ALY;
            }
            else if (uiData == HORDE)
            {
                if (GetData(TYPE_PLAGUEWOOD_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersAlly;
                    m_uiPlaguewoodWorldState = WORLD_STATE_PLAGUEWOOD_NEUTRAL;
                    // unsummon flightmaster
                    DoUnsummonFlightMaster();
                    // set artkit
                    SetBannersArtKit(m_lPlaguewoodTowerBanners, GO_ARTKIT_BANNER_NEUTRAL);
                }
                if (GetData(TYPE_PLAGUEWOOD_STATE) == PROGRESS)
                {
                    m_uiPlaguewoodWorldState = WORLD_STATE_PLAGUEWOOD_PROG_HORDE;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_PLAGUEWOOD_CONTROLLER))
                    {
                        DoSummonFlightMasterIfCan(HORDE);
                        ++m_uiTowersHorde;

                        // set artkit
                        SetBannersArtKit(m_lPlaguewoodTowerBanners, GO_ARTKIT_BANNER_HORDE);
                    }
                }
                // contested states are also sent by the opposite team
                // horde contests alliance tower
                else if (GetData(TYPE_PLAGUEWOOD_STATE) == CONTESTED)
                    m_uiPlaguewoodWorldState = WORLD_STATE_PLAGUEWOOD_CONT_ALY;
                else if (GetData(TYPE_PLAGUEWOOD_STATE) == WIN)
                    m_uiPlaguewoodWorldState = WORLD_STATE_PLAGUEWOOD_HORDE;
            }
            // set controller only for progress and neutral
            if (GetData(TYPE_PLAGUEWOOD_STATE) == PROGRESS)
                m_uiPlaguewoodController = uiData;
            else if (GetData(TYPE_PLAGUEWOOD_STATE) == NEUTRAL)
                m_uiPlaguewoodController = 0;
            break;
        case TYPE_CROWNGUARD_STATE:
            m_uiCrownguardState = uiData;
            return;
        case TYPE_EASTWALL_STATE:
            m_uiEastwallState = uiData;
            return;
        case TYPE_NORTHPASS_STATE:
            m_uiNorthpassState = uiData;
            return;
        case TYPE_PLAGUEWOOD_STATE:
            m_uiPlaguewoodState = uiData;
            return;
    }

    // update buffs
    for (uint8 i = 0; i < MAX_TOWERS; i++)
    {
        // buff alliance
        if (m_uiTowersAlly == m_aPlaguelandsTowerBuffs[i].uiTowers)
            DoProcessTeamBuff(ALLIANCE, m_aPlaguelandsTowerBuffs[i].uiSpellIdAlly);
        // buff horde
        if (m_uiTowersHorde == m_aPlaguelandsTowerBuffs[i].uiTowers)
            DoProcessTeamBuff(HORDE, m_aPlaguelandsTowerBuffs[i].uiSpellIdHorde);
    }

    // debuff players if towers == 0; spell to remove will be always the first
    if (m_uiTowersHorde == 0)
        DoProcessTeamBuff(HORDE, m_aPlaguelandsTowerBuffs[0].uiSpellIdHorde, true);
    if (m_uiTowersAlly == 0)
        DoProcessTeamBuff(ALLIANCE, m_aPlaguelandsTowerBuffs[0].uiSpellIdAlly, true);

    // update states counters
    // the map tower states are updated in the ProcessCaptureEvent function
    UpdateWorldState();
}

uint32 WorldPvPEP::GetData(uint32 uiType)
{
    switch(uiType)
    {
        case TYPE_CROWNGUARD_CONTROLLER:
            return m_uiCrownguardController;
        case TYPE_EASTWALL_CONTROLLER:
            return m_uiEastwallController;
        case TYPE_NORTHPASS_CONTROLLER:
            return m_uiNorthpassController;
        case TYPE_PLAGUEWOOD_CONTROLLER:
            return m_uiPlaguewoodController;
        case TYPE_CROWNGUARD_STATE:
            return m_uiCrownguardState;
        case TYPE_EASTWALL_STATE:
            return m_uiEastwallState;
        case TYPE_NORTHPASS_STATE:
            return m_uiNorthpassState;
        case TYPE_PLAGUEWOOD_STATE:
            return m_uiPlaguewoodState;
    }
    return 0;
}

void WorldPvPEP::ProcessCaptureEvent(uint32 uiCaptureType, uint32 uiTeam, uint32 uiTower)
{
    switch(uiTower)
    {
        case TOWER_ID_CROWNGUARD:
             // remove old tower state
            SendUpdateWorldState(m_uiCrownguardWorldState, 0);
            // update data
            SetData(TYPE_CROWNGUARD_STATE, uiCaptureType);
            SetData(TYPE_CROWNGUARD_CONTROLLER, uiTeam);
            // send new tower state
            SendUpdateWorldState(m_uiCrownguardWorldState, 1);
            break;
        case TOWER_ID_EASTWALL:
            // remove old tower state
            SendUpdateWorldState(m_uiEastwallWorldState, 0);
            // update data
            SetData(TYPE_EASTWALL_STATE, uiCaptureType);
            SetData(TYPE_EASTWALL_CONTROLLER, uiTeam);
            // send new tower state
            SendUpdateWorldState(m_uiEastwallWorldState, 1);
            break;
        case TOWER_ID_NORTHPASS:
            // remove old tower state
            SendUpdateWorldState(m_uiNorthpassWorldState, 0);
            // update data
            SetData(TYPE_NORTHPASS_STATE, uiCaptureType);
            SetData(TYPE_NORTHPASS_CONTROLLER, uiTeam);
            // send new tower state
            SendUpdateWorldState(m_uiNorthpassWorldState, 1);
            break;
        case TOWER_ID_PLAGUEWOOD:
            // remove old tower state
            SendUpdateWorldState(m_uiPlaguewoodWorldState, 0);
            // update data
            SetData(TYPE_PLAGUEWOOD_STATE, uiCaptureType);
            SetData(TYPE_PLAGUEWOOD_CONTROLLER, uiTeam);
            // send new tower state
            SendUpdateWorldState(m_uiPlaguewoodWorldState, 1);
            break;
    }
}

void WorldPvPEP::DoSummonFlightMasterIfCan(uint32 uiFaction)
{
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    // return if already summoned
    if (m_uiFlightMasterGUID)
        return;

    // summon the flightmaster
    if (Creature* pFlightMaster = pPlayer->SummonCreature(NPC_SPECTRAL_FLIGHTMASTER, m_aPlaguelandFlightmasterSpawnLocs[0].m_fX, m_aPlaguelandFlightmasterSpawnLocs[0].m_fY, m_aPlaguelandFlightmasterSpawnLocs[0].m_fZ, m_aPlaguelandFlightmasterSpawnLocs[0].m_fO, TEMPSUMMON_MANUAL_DESPAWN, 0))
    {
        //pFlightMaster->setFaction(uiFaction);
        pFlightMaster->SetRespawnDelay(7*DAY);
        m_uiFlightMasterGUID = pFlightMaster->GetObjectGuid();
    }
}

void WorldPvPEP::DoUnsummonFlightMaster()
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (!m_uiFlightMasterGUID)
        return;

    if (Creature* pFlightMaster = pPlayer->GetMap()->GetCreature(m_uiFlightMasterGUID))
        pFlightMaster->ForcedDespawn();
}

void WorldPvPEP::DoSummonSoldiersIfCan(uint32 uiFaction)
{
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    uint32 uiEntry = 0;

    if (uiFaction == ALLIANCE)
    {
        for (uint8 i = 0; i < MAX_TOWERS + 1; i++)
        {
            if (i == 0)
                uiEntry = NPC_LORDAERON_COMMANDER;
            else
                uiEntry = NPC_LORDAERON_SOLDIER;

            if (Creature* pSoldier = pPlayer->SummonCreature(uiEntry, m_aPlaguelandSoldiersSpawnLocs[i].m_fX, m_aPlaguelandSoldiersSpawnLocs[i].m_fY, m_aPlaguelandSoldiersSpawnLocs[i].m_fZ, m_aPlaguelandSoldiersSpawnLocs[i].m_fO, TEMPSUMMON_DEAD_DESPAWN, 0))
            {
                pSoldier->SetRespawnDelay(7*DAY);
                m_lSoldiersGuids.push_back(pSoldier->GetObjectGuid());
            }
        }
    }
    else
    {
        for (uint8 i = 0; i < MAX_TOWERS + 1; i++)
        {
            if (i == 0)
                uiEntry = NPC_LORDAERON_VETERAN;
            else
                uiEntry = NPC_LORDAERON_FIGHTER;

            if (Creature* pSoldier = pPlayer->SummonCreature(uiEntry, m_aPlaguelandSoldiersSpawnLocs[i].m_fX, m_aPlaguelandSoldiersSpawnLocs[i].m_fY, m_aPlaguelandSoldiersSpawnLocs[i].m_fZ, m_aPlaguelandSoldiersSpawnLocs[i].m_fO, TEMPSUMMON_DEAD_DESPAWN, 0))
            {
                pSoldier->SetRespawnDelay(7*DAY);
                m_lSoldiersGuids.push_back(pSoldier->GetObjectGuid());
            }
        }
    }
}

void WorldPvPEP::DoUnsummonSoldiers()
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    for (std::list<ObjectGuid>::iterator itr = m_lSoldiersGuids.begin(); itr != m_lSoldiersGuids.end(); ++itr)
    {
        if (Creature* pSoldier = pPlayer->GetMap()->GetCreature(*itr))
            pSoldier->ForcedDespawn();
    }
}

void WorldPvPEP::DoUpdateShrine(ObjectGuid uiShrineGuid, bool bRemove)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    // process zones shrine
    if (GameObject* pShrine = pPlayer->GetMap()->GetGameObject(uiShrineGuid))
    {
        if (!bRemove)
        {
            pShrine->SetRespawnTime(DAY);
            pShrine->Respawn();
        }
        else
            pShrine->Delete();
    }
}

void WorldPvPEP::SetBannersArtKit(std::list<ObjectGuid> lBannersGuids, uint32 uiArtkit)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    for (std::list<ObjectGuid>::iterator itr = lBannersGuids.begin(); itr != lBannersGuids.end(); ++itr)
    {
        if (GameObject* pBanner = pPlayer->GetMap()->GetGameObject(*itr))
            pBanner->SetGoArtKit(uiArtkit);
    }
}
