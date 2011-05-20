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

#ifndef _PLAYERBOTAI_H
#define _PLAYERBOTAI_H

#include "Common.h"
#include "../QuestDef.h"
#include "../GameEventMgr.h"
#include "../ObjectGuid.h"

class WorldPacket;
class WorldObject;
class Player;
class Unit;
class Object;
class Item;
class PlayerbotClassAI;
class PlayerbotMgr;

#define BOTLOOT_DISTANCE 25.0f

enum RacialTraits
{
    ARCANE_TORRENT_MANA_CLASSES    = 28730,
    ARCANE_TORRENT_DEATH_KNIGHT    = 50613,
    ARCANE_TORRENT_ROGUE           = 25046,
    BERSERKING_ALL                 = 26297,
    BLOOD_FURY_MELEE_CLASSES       = 20572,
    BLOOD_FURY_WARLOCK             = 33702,
    BLOOD_FURY_SHAMAN              = 33697,
    ESCAPE_ARTIST_ALL              = 20589,
    EVERY_MAN_FOR_HIMSELF_ALL      = 59752,
    GIFT_OF_THE_NAARU_DEATH_KNIGHT = 59545,
    GIFT_OF_THE_NAARU_HUNTER       = 59543,
    GIFT_OF_THE_NAARU_MAGE         = 59548,
    GIFT_OF_THE_NAARU_PALADIN      = 59542,
    GIFT_OF_THE_NAARU_PRIEST       = 59544,
    GIFT_OF_THE_NAARU_SHAMAN       = 59547,
    GIFT_OF_THE_NAARU_WARRIOR      = 28880,
    SHADOWMELD_ALL                 = 58984,
    STONEFORM_ALL                  = 20594,
    WAR_STOMP_ALL                  = 20549,
    WILL_OF_THE_FORSAKEN_ALL       = 7744
};

class MANGOS_DLL_SPEC PlayerbotAI
{
public:
    enum BotState
    {
        BOTSTATE_NORMAL,            // normal AI routines are processed
        BOTSTATE_COMBAT,            // bot is in combat
        BOTSTATE_DEAD,              // we are dead and wait for becoming ghost
        BOTSTATE_DEADRELEASED,      // we released as ghost and wait to revive
        BOTSTATE_LOOTING,           // looting mode, used just after combat
        BOTSTATE_FLYING             // bot is flying
    };

    typedef std::map<uint32, uint32> BotNeedItem;
    typedef std::list<uint64> BotLootCreature;
    typedef std::map<uint32, float> SpellRanges;
    typedef std::vector<uint32> BotTaxiNode;

public:
    PlayerbotAI(PlayerbotMgr * const mgr, Player * const bot);
    virtual ~PlayerbotAI();
    void ReinitAI();
    void InitSpells(PlayerbotAI* const ai);

    // This is called from Unit.cpp and is called every second (I think)
    void UpdateAI(const uint32 p_time);

    // This is called by WorldSession.cpp
    // It provides a view of packets normally sent to the client.
    // Since there is no client at the other end, the packets are dropped of course.
    // For a list of opcodes that can be caught see Opcodes.cpp (SMSG_* opcodes only)
    void HandleBotOutgoingPacket(const WorldPacket& packet);

    // This is called by WorldSession.cpp
    // when it detects that a bot is being teleported. It acknowledges to the server to complete the
    // teleportation
    void HandleTeleportAck();

    PlayerbotClassAI* GetClassAI() { return m_classAI; }
    PlayerbotMgr* const GetManager() { return m_mgr; }

    // finds spell ID for matching substring args
    // in priority of full text match, spells not taking reagents, and highest rank
    uint32 getSpellId(const char* args, bool master = false) const;
    uint32 getPetSpellId(const char* args) const;
    // Initialize spell using rank 1 spell id
    uint32 initSpell(uint32 spellId, bool ignorePossession = false);
    uint32 initPetSpell(uint32 spellIconId);

    // extracts item ids from links
    void extractItemIds(const std::string& text, std::list<uint32>& itemIds) const;

    // extract spellid from links
    bool extractSpellId(const std::string& text, uint32 &spellId) const;

    // extracts currency from a string as #g#s#c and returns the total in copper
    uint32 extractMoney(const std::string& text) const;

    // extracts gameobject info from link
    bool extractGOinfo(const std::string& text, uint32 &guid,  uint32 &entry, int &mapid, float &x, float &y, float &z) const;

    // finds items in bots equipment and adds them to foundItemList, removes found items from itemIdSearchList
    void findItemsInEquip(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const;
    // finds items in bots inventory and adds them to foundItemList, removes found items from itemIdSearchList
    void findItemsInInv(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const;

    // currently bots only obey commands from the master
    bool canObeyCommandFrom(const Player& player) const;

    // get current casting spell (will return NULL if no spell!)
    Spell* GetCurrentSpell() const;

    bool HasAura(uint32 spellId, const Unit* unit) const;

    bool CanReceiveSpecificSpell(uint8 spec, Unit* target) const;

    bool PickPocket(Unit* pTarget);
    bool HasPick();
    bool HasSpellReagents(uint32 spellId);

    uint8 GetHealthPercent(const Unit& target) const;
    uint8 GetHealthPercent() const;
    uint8 GetBaseManaPercent(const Unit& target) const;
    uint8 GetBaseManaPercent() const;
    uint8 GetManaPercent(const Unit& target) const;
    uint8 GetManaPercent() const;
    uint8 GetRageAmount(const Unit& target) const;
    uint8 GetRageAmount() const;
    uint8 GetEnergyAmount(const Unit& target) const;
    uint8 GetEnergyAmount() const;
    uint8 GetRunicPower(const Unit& target) const;
    uint8 GetRunicPower() const;

    Item* FindFood() const;
    Item* FindDrink() const;
    Item* FindBandage() const;
    Item* FindPoison() const;
    Item* FindMount(uint32 matchingRidingSkill) const;
    Item* FindItem(uint32 ItemId);
    Item* FindConsumable(uint32 displayId) const;
    void CheckMount();
    void UnMount();
    Player* FindNewGroupLeader();

    // ******* Actions ****************************************
    // Your handlers can call these actions to make the bot do things.
    bool CastSpell(uint32 spellId);
    bool CastSpell(uint32 spellId, Unit* target);
    bool CastAura(uint32 spellId, Unit* target);
    bool CastPetSpell(uint32 spellId, Unit* target = NULL);
    bool Buff(uint32 spellId, Unit* target, void (*beforeCast)(Player *) = NULL);
    bool SelfBuff(uint32 spellId);

    void UseItem(Item *item, uint32 targetFlag, ObjectGuid targetGUID);
    void UseItem(Item *item, uint8 targetInventorySlot);
    void UseItem(Item *item, Unit *target);
    void UseItem(Item *item);

    void EquipItem(Item& item);
    void Feast();
    void InterruptCurrentCastingSpell();
    bool GetCombatTarget(Unit* forcedTarged = NULL);
    Unit *GetCurrentTarget() { return m_targetCombat; };
    void DoNextCombatManeuver();
    void SetIgnoreUpdateTime(uint8 t = 0) { m_ignoreAIUpdatesUntilTime = time(0) + t; };
    void SetEnterBGTime(uint8 t = 0) { m_enterBg = time(0) + t; };

    Player *GetPlayer() const { return m_bot; }
    Player *GetLeader() const;
    void SetLeader(Player* pl);

    uint16 getRole() { return m_role; };
    void setRole(uint16 role) { m_new_role = role; };

    BotState GetState() { return m_botState; };
    void SetState(BotState state);
    void SetQuestNeedItems();
    void SendQuestItemList(Player& player);
    bool FollowCheckTeleport(WorldObject *obj);
    void DoLoot();
    void DoFlight();
    void GetTaxi(ObjectGuid guid, BotTaxiNode& nodes);

    bool CheckTeleport();
    bool CheckMaster();
    void CheckMMaps();
    bool CheckLevel();
    void CheckStuff();
    void CheckRoles();
    void CheckBG();

    uint32 EstRepairAll();
    uint32 EstRepair(uint16 pos);

    void AcceptQuest(Quest const *qInfo, Player *pGiver);
    void TurnInQuests(WorldObject *questgiver);

    bool IsInCombat();
    Player* TargetPlayerFocus();
    void SetMovementTarget(Unit *followTarget = NULL);
    bool isInside(float x1, float y1, float x2, float y2, float x3, float y3, float delta);
    void MovementClear();
    bool IsMoving();
    bool FindPOI();
    Unit* FindEnemy();

    bool SetInFront(const Unit* obj);

    void ItemLocalization(std::string& itemName, const uint32 itemID) const;
    void QuestLocalization(std::string& questTitle, const uint32 questID) const;

    uint8 GetFreeBagSpace() const;

    Unit* GetFollowTarget() { return m_followTarget; }

    void Pull();

private:
    // ****** Closed Actions ********************************
    // it is safe to keep these back reference pointers because m_bot
    // owns the "this" object and m_master owns m_bot. The owner always cleans up.
    PlayerbotMgr* const m_mgr;
    Player* const m_bot;
    PlayerbotClassAI* m_classAI;
    uint16 m_role, m_new_role;

    // ignores AI updates until time specified
    // no need to waste CPU cycles during casting etc
    time_t m_ignoreAIUpdatesUntilTime;
    time_t m_enterBg, m_leaveBg;

    // defines the state of behaviour of the bot
    BotState m_botState;

    // list of items needed to fullfill quests
    BotNeedItem m_needItemList;

    // list of creatures we recently attacked and want to loot
    BotLootCreature m_lootCreature;     // list of creatures
    uint64 m_lootCurrent;               // current remains of interest
    BotTaxiNode m_taxiNodes;            // flight node chain;

    time_t m_TimeDoneEating;
    time_t m_TimeDoneDrinking;
    uint32 m_CurrentlyCastingSpellId;

    // if master commands bot to do something, store here until updateAI
    // can do it
    uint64 m_targetGuidCommand;
    ObjectGuid m_taxiMaster;

    Unit *m_targetCombat;       // current combat target
    Unit *m_followTarget;       // whom to follow in non combat situation?

    float orig_x, orig_y, orig_z;
    uint32 orig_map;

    SpellRanges m_spellRangeMap;
};

#endif
