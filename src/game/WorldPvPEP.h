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

#ifndef WORLD_PVP_EP
#define WORLD_PVP_EP

enum
{
    MAX_TOWERS                              = 4,

    // store the tower controller team
    TYPE_CROWNGUARD_CONTROLLER              = 1,
    TYPE_EASTWALL_CONTROLLER                = 2,
    TYPE_NORTHPASS_CONTROLLER               = 3,
    TYPE_PLAGUEWOOD_CONTROLLER              = 4,

    // store tower world state to show
    TYPE_CROWNGUARD_STATE                   = 5,
    TYPE_EASTWALL_STATE                     = 6,
    TYPE_NORTHPASS_STATE                    = 7,
    TYPE_PLAGUEWOOD_STATE                   = 8,

    // tower ids
    // they are used in the process event functions
    TOWER_ID_CROWNGUARD                     = 1,
    TOWER_ID_EASTWALL                       = 2,
    TOWER_ID_NORTHPASS                      = 3,
    TOWER_ID_PLAGUEWOOD                     = 4,

    // spells
    // aly
    SPELL_ECOES_OF_LORDAERON_ALY_1          = 11413,
    SPELL_ECOES_OF_LORDAERON_ALY_2          = 11414,
    SPELL_ECOES_OF_LORDAERON_ALY_3          = 11415,
    SPELL_ECOES_OF_LORDAERON_ALY_4          = 1386,

    // horde
    SPELL_ECOES_OF_LORDAERON_HORDE_1        = 30880,
    SPELL_ECOES_OF_LORDAERON_HORDE_2        = 30683,
    SPELL_ECOES_OF_LORDAERON_HORDE_3        = 30682,
    SPELL_ECOES_OF_LORDAERON_HORDE_4        = 29520,

    // zone ids
    ZONE_ID_EASTERN_PLAGUELANDS             = 139,
    ZONE_ID_STRATHOLME                      = 2017,
    ZONE_ID_SCHOLOMANCE                     = 2057,

    // graveyards
    GRAVEYARD_ZONE_EASTERN_PLAGUE           = 139,
    GRAVEYARD_ID_EASTERN_PLAGUE             = 927,
    HONOR_REWARD_PLAGUELANDS                = 18,

    // taxi nodes
    TAXI_NODE_PLAGUEWOOD_TOWER              = 84,
    TAXI_NODE_NORTHPASS_TOWER               = 85,
    TAXI_NODE_EASTWALL_TOWER                = 86,
    TAXI_NODE_CROWNGUARD_TOWER              = 87,

    // npcs
    NPC_SPECTRAL_FLIGHTMASTER               = 17209,

    // quest
    NPC_CROWNGUARD_TOWER_QUEST_DOODAD       = 17689,
    NPC_EASTWALL_TOWER_QUEST_DOODAD         = 17690,
    NPC_NORTHPASS_TOWER_QUEST_DOODAD        = 17696,
    NPC_PLAGUEWOOD_TOWER_QUEST_DOODAD       = 17698,

    // aly
    NPC_LORDAERON_COMMANDER                 = 17635,
    NPC_LORDAERON_SOLDIER                   = 17647,

    // horde
    NPC_LORDAERON_VETERAN                   = 17995,
    NPC_LORDAERON_FIGHTER                   = 17996,

    // gameobjects
    GO_LORDAERON_SHRINE_ALY                 = 181682,
    GO_LORDAERON_SHRINE_HORDE               = 181955,
    GO_TOWER_FLAG                           = 182106,

    // capture points
    GO_BATTLEFIELD_BANNER_PLAGUELANDS_1     = 181899,   // northpass
    GO_BATTLEFIELD_BANNER_PLAGUELANDS_2     = 182096,   // crownguard
    GO_BATTLEFIELD_BANNER_PLAGUELANDS_3     = 182097,   // eastwall
    GO_BATTLEFIELD_BANNER_PLAGUELANDS_4     = 182098,   // plaguewood

    GO_TOWER_BANNER                         = 182106,   // tower banners around

    // events
    // northpass - banner 1
    EVENT_NORTHPASS_WIN_ALLIANCE            = 10568,
    EVENT_NORTHPASS_WIN_HORDE               = 10556,
    EVENT_NORTHPASS_CONTEST_ALLIANCE        = 10697,
    EVENT_NORTHPASS_CONTEST_HORDE           = 10696,
    EVENT_NORTHPASS_PROGRESS_ALLIANCE       = 10699,
    EVENT_NORTHPASS_PROGRESS_HORDE          = 10698,
    EVENT_NORTHPASS_NEUTRAL_ALLIANCE        = 11151,
    EVENT_NORTHPASS_NEUTRAL_HORDE           = 11150,

    // crownguard - banner 2
    EVENT_CROWNGUARD_WIN_ALLIANCE           = 10570,
    EVENT_CROWNGUARD_WIN_HORDE              = 10566,
    EVENT_CROWNGUARD_CONTEST_ALLIANCE       = 10703,
    EVENT_CROWNGUARD_CONTEST_HORDE          = 10702,
    EVENT_CROWNGUARD_PROGRESS_ALLIANCE      = 10705,
    EVENT_CROWNGUARD_PROGRESS_HORDE         = 10704,
    EVENT_CROWNGUARD_NEUTRAL_ALLIANCE       = 11155,
    EVENT_CROWNGUARD_NEUTRAL_HORDE          = 11154,

    // eastwall - banner 3
    EVENT_EASTWALL_WIN_ALLIANCE             = 10569,
    EVENT_EASTWALL_WIN_HORDE                = 10565,
    EVENT_EASTWALL_CONTEST_ALLIANCE         = 10689,
    EVENT_EASTWALL_CONTEST_HORDE            = 10690,
    EVENT_EASTWALL_PROGRESS_ALLIANCE        = 10691,
    EVENT_EASTWALL_PROGRESS_HORDE           = 10692,
    EVENT_EASTWALL_NEUTRAL_ALLIANCE         = 11149,
    EVENT_EASTWALL_NEUTRAL_HORDE            = 11148,

    // plaguewood - banner 4
    EVENT_PLAGUEWOOD_WIN_ALLIANCE           = 10567,
    EVENT_PLAGUEWOOD_WIN_HORDE              = 10564,
    EVENT_PLAGUEWOOD_CONTEST_ALLIANCE       = 10687,
    EVENT_PLAGUEWOOD_CONTEST_HORDE          = 10688,
    EVENT_PLAGUEWOOD_PROGRESS_ALLIANCE      = 10701,
    EVENT_PLAGUEWOOD_PROGRESS_HORDE         = 10700,
    EVENT_PLAGUEWOOD_NEUTRAL_ALLIANCE       = 11153,
    EVENT_PLAGUEWOOD_NEUTRAL_HORDE          = 11152,

    // world states
    WORLD_STATE_TOWER_COUNT_ALY             = 2327,
    WORLD_STATE_TOWER_COUNT_HORDE           = 2328,

    // plaguewood tower
    WORLD_STATE_PLAGUEWOOD_CONT_ALY         = 2366,
    WORLD_STATE_PLAGUEWOOD_CONT_HORDE       = 2367,
    WORLD_STATE_PLAGUEWOOD_PROG_ALY         = 2368,
    WORLD_STATE_PLAGUEWOOD_PROG_HORDE       = 2369,
    WORLD_STATE_PLAGUEWOOD_ALY              = 2370,
    WORLD_STATE_PLAGUEWOOD_HORDE            = 2371,
    WORLD_STATE_PLAGUEWOOD_NEUTRAL          = 2353,

    // northpass tower
    WORLD_STATE_NORTHPASS_CONT_ALY          = 2362,
    WORLD_STATE_NORTHPASS_CONT_HORDE        = 2363,
    WORLD_STATE_NORTHPASS_PROG_ALY          = 2364,
    WORLD_STATE_NORTHPASS_PROG_HORDE        = 2365,
    WORLD_STATE_NORTHPASS_ALY               = 2372,
    WORLD_STATE_NORTHPASS_HORDE             = 2373,
    WORLD_STATE_NORTHPASS_NEUTRAL           = 2352,

    // eastwall tower
    WORLD_STATE_EASTWALL_CONT_ALY           = 2359,
    WORLD_STATE_EASTWALL_CONT_HORDE         = 2360,
    WORLD_STATE_EASTWALL_PROG_ALY           = 2357,
    WORLD_STATE_EASTWALL_PROG_HORDE         = 2358,
    WORLD_STATE_EASTWALL_ALY                = 2354,
    WORLD_STATE_EASTWALL_HORDE              = 2356,
    WORLD_STATE_EASTWALL_NEUTRAL            = 2361,

    // crownguard tower
    WORLD_STATE_CROWNGUARD_CONT_ALY         = 2374,
    WORLD_STATE_CROWNGUARD_CONT_HORDE       = 2375,
    WORLD_STATE_CROWNGUARD_PROG_ALY         = 2376,
    WORLD_STATE_CROWNGUARD_PROG_HORDE       = 2377,
    WORLD_STATE_CROWNGUARD_ALY              = 2378,
    WORLD_STATE_CROWNGUARD_HORDE            = 2379,
    WORLD_STATE_CROWNGUARD_NEUTRAL          = 2355,
};

struct sTowerBuffs
{
    uint32 uiTowers, uiSpellIdAlly, uiSpellIdHorde;
};

static sTowerBuffs m_aPlaguelandsTowerBuffs[MAX_TOWERS] =
{
    {1, SPELL_ECOES_OF_LORDAERON_ALY_1, SPELL_ECOES_OF_LORDAERON_HORDE_1},
    {2, SPELL_ECOES_OF_LORDAERON_ALY_2, SPELL_ECOES_OF_LORDAERON_HORDE_2},
    {3, SPELL_ECOES_OF_LORDAERON_ALY_3, SPELL_ECOES_OF_LORDAERON_HORDE_3},
    {4, SPELL_ECOES_OF_LORDAERON_ALY_4, SPELL_ECOES_OF_LORDAERON_HORDE_4}
};

// summon position at the eastwall tower
static sSpawnLocations m_aPlaguelandSoldiersSpawnLocs[] =
{
    {2526.220f, -4758.520f, 101.056f},
    {2532.452f, -4760.138f, 102.408f},
    {2535.058f, -4757.152f, 102.219f},
    {2526.297f, -4764.442f, 102.360f},
    {2522.425f, -4767.049f, 102.552f}
};

static sSpawnLocations m_aPlaguelandFlightmasterSpawnLocs[] =
{
    {2987.5f, -3049.11f, 120.126f, 5.75959f}
};

// Capture points coords to sort the banners
static sSpawnLocations m_aTowersSpawnLocs[MAX_TOWERS] =
{
    {3181.08f, -4379.36f, 174.123f},       // northpass
    {1860.85f, -3731.23f, 196.716f},       // crownguard
    {2574.51f, -4794.89f, 144.704f},       // eastwall
    {2962.71f, -3042.31f, 154.789f}        // plaguewood
};

class WorldPvPEP : public WorldPvP
{
    public:
        WorldPvPEP();

        bool InitOutdoorPvPArea();

        //void OnCreatureCreate(Creature* pCreature);
        void OnGameObjectCreate(GameObject* pGo);
        void ProcessEvent(GameObject* pGo, Player* pPlayer, uint32 uiEventId);

        void HandlePlayerEnterZone(Player* pPlayer);
        void HandlePlayerLeaveZone(Player* pPlayer);
        void HandleObjectiveComplete(PlayerSet m_sPlayersSet, uint32 uiEventId);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        void FillInitialWorldStates(WorldPacket& data, uint32& count);
        void SendRemoveWorldStates(Player* pPlayer);
        void UpdateWorldState();

    private:
        // process capture events
        void ProcessCaptureEvent(uint32 uiCaptureType, uint32 uiTeam, uint32 uiTower);

        // set banners artkit
        void SetBannersArtKit(std::list<ObjectGuid> lBannersGuids, uint32 uiArtKit);

        // plaguewood bonus - flightmaster
        void DoSummonFlightMasterIfCan(uint32 uiFaction);
        void DoUnsummonFlightMaster();

        // eastwall bonus - soldiers
        void DoSummonSoldiersIfCan(uint32 uiFaction);
        void DoUnsummonSoldiers();

        // northpass bonus - shrine
        void DoUpdateShrine(ObjectGuid uiShrineGuid, bool bRemove = false);

        // crownguard bonus - graveyard
        void DoSetGraveyard(uint32 uiFaction, bool bRemove = false) { /* ToDo */ }

        uint32 m_uiPlaguewoodController;
        uint32 m_uiEastwallController;
        uint32 m_uiNorthpassController;
        uint32 m_uiCrownguardController;
        uint32 m_uiPlaguewoodState;
        uint32 m_uiNorthpassState;
        uint32 m_uiEastwallState;
        uint32 m_uiCrownguardState;
        uint32 m_uiPlaguewoodWorldState;
        uint32 m_uiNorthpassWorldState;
        uint32 m_uiEastwallWorldState;
        uint32 m_uiCrownguardWorldState;
        uint32 m_uiTowersAlly;
        uint32 m_uiTowersHorde;

        ObjectGuid m_uiFlightMasterGUID;
        ObjectGuid m_uiLordaeronShrineAlyGUID;
        ObjectGuid m_uiLordaeronShrineHordeGUID;

        std::list<ObjectGuid> m_lSoldiersGuids;

        std::list<ObjectGuid> m_lPlaguewoodTowerBanners;
        std::list<ObjectGuid> m_lNorthpassTowerBanners;
        std::list<ObjectGuid> m_lEastwallTowerBanners;
        std::list<ObjectGuid> m_lCrownguardTowerBanners;
};

#endif