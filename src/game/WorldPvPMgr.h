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

#ifndef WORLD_PVP_MGR_H
#define WORLD_PVP_MGR_H

// why this?
#define WORLD_PVP_OBJECTIVE_UPDATE_INTERVAL 1000

#include "WorldPvP.h"

class Player;
class GameObject;
class Creature;
class ZoneScript;

class WorldPvPMgr
{
    public:
        WorldPvPMgr();
        ~WorldPvPMgr();

        // create world pvp events
        void InitWorldPvP();

        // called when a player enters an world pvp area
        void HandlePlayerEnterZone(Player* pPlayer, uint32 uiZoneId);

        // called when player leaves an world pvp area
        void HandlePlayerLeaveZone(Player* pPlayer, uint32 uiZoneId);

        // return assigned world pvp
        WorldPvP* GetWorldPvPToZoneId(uint32 uiZoneId);

        // return assigned zone script
        ZoneScript* GetZoneScript(uint32 uiZoneId);

        // add zone id to world pvp handler
        void AddZone(uint32 uiZoneId, WorldPvP* pScriptHandler);

        void Update(uint32 diff);

        // Handle cases when a player drops a flag
        void HandleDropFlag(Player* pPlayer, uint32 uiSpellId);

        // Handle cases when a gameobject is used
        bool HandleObjectUse(Player* pPlayer, GameObject* pGo);

        // Handle the complete credit for capture point win events
        void HandleObjectiveComplete(std::set<Player*> m_sObjectivePlayers, uint32 uiEventId);

        typedef std::vector<WorldPvP*> WorldPvPSet;
        typedef std::map<uint32 /* zoneid */, WorldPvP*> WorldPvPMap;
    private:
        // contains all initiated world pvp events
        // used when initing / cleaning up
        WorldPvPSet  m_WorldPvPSet;

        // maps the zone ids to an world pvp event
        // used in player event handling
        WorldPvPMap   m_WorldPvPMap;

        // update interval
        uint32 m_UpdateTimer;
};

#define sWorldPvPMgr MaNGOS::Singleton<WorldPvPMgr>::Instance()

#endif
