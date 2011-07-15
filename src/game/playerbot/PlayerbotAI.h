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
#include "../BattleGround.h"

class WorldPacket;
class WorldObject;
class Player;
class Unit;
class Object;
class Item;
class PlayerbotClassAI;
class PlayerbotMgr;
class BattleGround;

typedef std::map<ObjectGuid, BattleGroundPlayer> BattleGroundPlayerMap;

#define MIN_HP_PERCENT_BEFORE_FLEEING  15.0f
#define MAX_BOTLOOT_DISTANCE           25.0f
#define MAX_DIST_POS_IN_GROUP          30.0f
#define MAX_DIST_COMBAT_TARGET        100.0f
#define MAX_DIST_BETWEEN_BOT_LEADER   MAX_DIST_COMBAT_TARGET + 50.0f
#define MAX_DIST_COMBAT_RANGED_TARGET  25.0f
#define MIN_DIST_COMBAT_CAC_TARGET      2.0f
#define MAX_DIST_COMBAT_CAC_TARGET     20.0f
#define MAX_RANGE_MOVEMENT            250.0f

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

enum BotCombatType
{
    BOTCOMBAT_RANGED,
    BOTCOMBAT_CAC
};

enum CastCondition
{
    NO_CONDITION,

    // Condition on Target
    IF_TARGET_HAS_AURA,
    IF_TARGET_HAS_NOT_AURA,

    IF_TARGET_HAS_AURA_FROM_CURRENT_CASTER,
    IF_TARGET_HAS_NOT_AURA_FROM_CURRENT_CASTER,

    // Condition on Caster
    IF_CASTER_HAS_AURA,
    IF_CASTER_HAS_NOT_AURA,

    IF_CASTER_HAS_AURA_FROM_CURRENT_CASTER,
    IF_CASTER_HAS_NOT_AURA_FROM_CURRENT_CASTER
};

class MANGOS_DLL_SPEC PlayerbotAI
{
public:
    enum BotState
    {
        BOTSTATE_NORMAL,
        BOTSTATE_COMBAT,
        BOTSTATE_DEAD,
        BOTSTATE_DEADRELEASED,
        BOTSTATE_LOOTING,
        BOTSTATE_FLYING
    };

    typedef std::map<uint32, uint32> BotNeedItem;
    typedef std::list<Unit*> BotVictimList;
    typedef std::map<uint32, float> SpellRanges;
    typedef std::vector<uint32> BotTaxiNode;

public:
    PlayerbotAI(PlayerbotMgr * const mgr, Player * const bot);
    virtual ~PlayerbotAI();

    void ReinitAI();
    void InitSpells(PlayerbotAI* const ai);

    void UpdateAI(const uint32 p_time);

    void HandleBotOutgoingPacket(const WorldPacket& packet);
    void HandleTeleportAck();

    PlayerbotClassAI* GetClassAI() { return m_classAI; }

    uint32 getSpellId(const char* args, bool master = false) const;
    uint32 getPetSpellId(const char* args) const;
    uint32 initSpell(uint32 spellId, bool ignorePossession = false);
    uint32 initPetSpell(uint32 spellIconId);

    void extractItemIds(const std::string& text, std::list<uint32>& itemIds) const;
    bool extractSpellId(const std::string& text, uint32 &spellId) const;
    uint32 extractMoney(const std::string& text) const;
    bool extractGOinfo(const std::string& text, uint32 &guid,  uint32 &entry, int &mapid, float &x, float &y, float &z) const;

    void findItemsInEquip(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const;
    void findItemsInInv(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const;

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

    bool Cast(uint32 spellId, Unit *target = NULL, CastCondition condition = IF_TARGET_HAS_NOT_AURA, uint32 spellIdForSecondCondition = 0, CastCondition SecondCondition = NO_CONDITION);
    bool CastAura(uint32 spellId, Unit* target);
    bool CastSpell(uint32 spellId, Unit* target = NULL);
    bool CastPetAura(uint32 spellId, Unit* target);
    bool CastPetSpell(uint32 spellId, Unit* target = NULL);
    bool Buff(uint32 spellId, Unit* target);
    bool SelfBuff(uint32 spellId);
    Spell* GetCurrentSpell() const;
    bool HasAura(uint32 spellId, const Unit* unit) const;
    bool HasAuraFromUnit(uint32 spellId, Unit *target, Unit *caster);
    bool RespectCondition(uint32 spellId, Unit *target, CastCondition condition);

    void UseItem(Item *item, uint32 targetFlag, ObjectGuid targetGUID);
    void UseItem(Item *item, uint8 targetInventorySlot);
    void UseItem(Item *item, Unit *target);
    void UseItem(Item *item);

    void EquipItem(Item& item);
    void Feast();
    void InterruptCurrentCastingSpell();
    void DoCombatManeuver(Unit* = NULL);
    BotCombatType GetCombatType();
    void SetIgnoreUpdateTime(uint8 t = 0) { m_ignoreAIUpdatesUntilTime = time(0) + t; };
    void SetEnterBGTime(uint8 t = 0) { m_enterBg = time(0) + t; };

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
    bool CheckLeader();
    bool CheckGroup();
    void InitBotStatsForLevel(uint32, bool = false);
    void CheckRoles();
    void CheckBG();

    uint32 EstRepairAll();
    uint32 EstRepair(uint16 pos);

    void AcceptQuest(Quest const *qInfo, Player *pGiver);
    void TurnInQuests(WorldObject *questgiver);

    bool IsInCombat();
    Unit* FindPOI();
    Unit* FindEnemy();

    void CheckMount();
    void UnMount();
    Player* FindGroupRandomRealPlayer();
    Player* FindGroupRandomPlayer(Classes _class);
    Player* FindGroupMainTank();
    bool MasterIsMainTank() { return m_bot!=GetLeader() && GetLeader()==FindGroupMainTank(); };

    bool SetInFront(const Unit* obj);

    void ItemLocalization(std::string& itemName, const uint32 itemID) const;
    void QuestLocalization(std::string& questTitle, const uint32 questID) const;

    uint8 GetFreeBagSpace() const;

    void SetFollowTarget(Unit *followTarget, bool forced = false);
    float GetDist() { return GetCombatType()==BOTCOMBAT_CAC ? MAX_DIST_COMBAT_CAC_TARGET : MAX_DIST_COMBAT_RANGED_TARGET; };
    void MoveTo(Unit*, float = 0.0f, float = 1.0f, float = 3.0f);
    void MoveTo(float, float, float);
    bool HasArrived();

    void Pull();

    void MoveInLineOfSight(Unit *);               //Not used
    void AttackStart(Unit *);                     //Always attack new target with this
    void EnterEvadeMode() {};                     //Not implemented
    void AttackedBy(Unit*) {};                    //Not used
    bool IsVisible(Unit *) const {return true;};  //Not used
    bool IsInEvadeMode() const {return false;};   //Not used

    bool IsValidEnemy(Unit *, bool = false);
    Unit *GetNewCombatTarget(bool = false);

private:
    PlayerbotMgr* const m_mgr;
    Player* const m_bot;
    PlayerbotClassAI* m_classAI;
    uint16 m_role, m_new_role;

    time_t m_ignoreAIUpdatesUntilTime;
    uint32 m_CurrentlyCastingSpellId;
    time_t m_enterBg, m_leaveBg;

    BotState m_botState;

    BotNeedItem m_needItemList;

    BotTaxiNode m_taxiNodes;

    time_t m_TimeDoneEating;
    time_t m_TimeDoneDrinking;

    uint64 m_targetGuidCommand;
    ObjectGuid m_taxiMaster;

    Creature* m_lootCreature;
    BotVictimList m_victimList;

    Unit* m_latestVictim;

    float orig_x, orig_y, orig_z;
    uint32 orig_map;

    SpellRanges m_spellRangeMap;
};

#endif
