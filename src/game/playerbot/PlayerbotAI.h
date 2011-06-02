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

enum BotCombatType
{
    BOTCOMBAT_RANGED,
    BOTCOMBAT_CAC
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
    typedef std::list<Creature*> BotLootCreature;
    typedef std::map<uint32, float> SpellRanges;
    typedef std::vector<uint32> BotTaxiNode;
    typedef std::map<ObjectGuid, BattleGroundPlayer> BattleGroundPlayerMap;

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

    bool CastSpell(uint32 spellId);
    bool CastSpell(uint32 spellId, Unit* target);
    bool CastAura(uint32 spellId, Unit* target);
    bool CastPetSpell(uint32 spellId, Unit* target = NULL);
    bool Buff(uint32 spellId, Unit* target, void (*beforeCast)(Player *) = NULL);
    bool SelfBuff(uint32 spellId);
    Spell* GetCurrentSpell() const;
    bool HasAura(uint32 spellId, const Unit* unit) const;

    void UseItem(Item *item, uint32 targetFlag, ObjectGuid targetGUID);
    void UseItem(Item *item, uint8 targetInventorySlot);
    void UseItem(Item *item, Unit *target);
    void UseItem(Item *item);

    void EquipItem(Item& item);
    void Feast();
    void InterruptCurrentCastingSpell();
    void SetCombatTarget(Unit* targetCombat);
    Unit *GetCurrentTarget() { return m_targetCombat; };
    Unit *GetNewCombatTarget();
    void DoCombatManeuver();
    BotCombatType GetCombatType();
    void MoveTo(float angle, float minDist = 1.0f, float maxDist = 3.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f);
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
    bool CheckMaster();
    bool CheckLevel();
    void CheckStuff();
    void CheckRoles();
    void CheckBG();

    uint32 EstRepairAll();
    uint32 EstRepair(uint16 pos);

    void AcceptQuest(Quest const *qInfo, Player *pGiver);
    void TurnInQuests(WorldObject *questgiver);

    bool IsInCombat();
    void MovementClear();
    bool FindPOI();
    Unit* FindEnemy();

    void CheckMount();
    void UnMount();
    Player* FindNewGroupLeader();

    bool SetInFront(const Unit* obj);

    void ItemLocalization(std::string& itemName, const uint32 itemID) const;
    void QuestLocalization(std::string& questTitle, const uint32 questID) const;

    uint8 GetFreeBagSpace() const;

    void SetFollowTarget(Unit *followTarget);
    Unit* GetFollowTarget() { return m_followTarget; }

    void Pull();

    void MoveInLineOfSight(Unit *);               //Usefull call in mangos update position system
    void AttackStart(Unit *) {};                  //Useless by we can rewrite code to use it
    void EnterEvadeMode() {};                     //Implemented later
    void AttackedBy(Unit*) {};                    //Not used for now
    bool IsVisible(Unit *) const {return true;};  //Fake but need to be implemented later
    bool IsInEvadeMode() const {return false;};   //Fake but need to be implemented later

private:
    PlayerbotMgr* const m_mgr;
    Player* const m_bot;
    PlayerbotClassAI* m_classAI;
    uint16 m_role, m_new_role;

    time_t m_ignoreAIUpdatesUntilTime;
    time_t m_enterBg, m_leaveBg;

    BotState m_botState;

    BotNeedItem m_needItemList;

    BotLootCreature m_lootCreature;
    Creature* m_lootCurrent;
    BotTaxiNode m_taxiNodes;

    time_t m_TimeDoneEating;
    time_t m_TimeDoneDrinking;
    uint32 m_CurrentlyCastingSpellId;

    uint64 m_targetGuidCommand;
    ObjectGuid m_taxiMaster;

    Unit *m_targetCombat;
    Unit *m_followTarget;

    float orig_x, orig_y, orig_z;
    uint32 orig_map;

    SpellRanges m_spellRangeMap;
};

#endif
