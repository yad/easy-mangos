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
#include "QuestDef.h"
#include "GameEventMgr.h"

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

        enum CombatStyle
        {
            COMBAT_MELEE = 0x01,
            COMBAT_RANGED = 0x02
        };

        enum CombatOrderType
        {
            ORDERS_NONE = 0x00,
            ORDERS_TANK = 0x01,
            ORDERS_ASSIST = 0x02,
            ORDERS_HEAL = 0x04,
            ORDERS_PROTECT = 0x10,
            ORDERS_PRIMARY = 0x0F,
            ORDERS_SECONDARY = 0xF0,
            ORDERS_RESET = 0xFF
        };

        enum CombatTargetType
        {
            TARGET_NORMAL = 0x00,
            TARGET_THREATEN = 0x01
        };

        enum BotState
        {
            BOTSTATE_NORMAL,
            BOTSTATE_COMBAT,
            BOTSTATE_DEAD,
            BOTSTATE_DEADRELEASED,
            BOTSTATE_LOOTING
        };

        enum MovementOrderType
        {
            MOVEMENT_NONE = 0x00,
            MOVEMENT_FOLLOW = 0x01,
            MOVEMENT_STAY = 0x02
        };

        typedef std::map<uint32, uint32> BotNeedItem;
        typedef std::list<uint64> BotLootCreature;

        enum ATTACKERINFOTYPE
        {
            AIT_NONE = 0x00,
            AIT_LOWESTTHREAT = 0x01,
            AIT_HIGHESTTHREAT = 0x02,
            AIT_VICTIMSELF = 0x04,
            AIT_VICTIMNOTSELF = 0x08
        };

        struct AttackerInfo
        {
            Unit*    attacker;
            Unit*    victim;
            float    threat;
            float    threat2;
            uint32   count;
            uint32   source;
        };

        typedef std::map<uint64, AttackerInfo> AttackerInfoList;

    public:

        PlayerbotAI(PlayerbotMgr* const mgr, Player* const bot);
        virtual ~PlayerbotAI();
        void InitSpells(PlayerbotAI* const ai);

        void UpdateAI(const uint32 p_time);

        void HandleCommand(const std::string& text, Player& fromPlayer);

        void HandleBotOutgoingPacket(const WorldPacket& packet);

        void HandleTeleportAck();

        PlayerbotClassAI* GetClassAI() { return m_classAI; }
        PlayerbotMgr* const GetManager() { return m_mgr; }

        uint32 initSpell(uint32 spellId);

        void extractItemIds(const std::string& text, std::list<uint32> & itemIds) const;

        uint32 extractMoney(const std::string& text) const;

        bool extractGOinfo(const std::string& text, uint32 &guid, uint32 &entry, int &mapid, float &x, float &y, float &z) const;

        void findItemsInEquip(std::list<uint32> & itemIdSearchList, std::list<Item*> & foundItemList) const;
        void findItemsInInv(std::list<uint32> & itemIdSearchList, std::list<Item*> & foundItemList) const;

        bool canObeyCommandFrom(const Player& player) const;

        Spell* GetCurrentSpell() const;

        bool HasAura(uint32 spellId, const Unit& player) const;

        bool HasPick();

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
        void CheckMount();

        void TellMaster(const std::string& text) const;
        void TellMaster(const char *fmt, ...) const;
        void SendWhisper(const std::string& text, Player& player) const;
        bool CastSpell(uint32 spellId);
        bool CastSpell(uint32 spellId, Unit& target);
        void UseItem(Item& item);
        void EquipItem(Item& item);
        void Stay();
        bool Follow(Player& player);
        void SendNotEquipList(Player& player);
        void Feast();
        void InterruptCurrentCastingSpell();
        void GetCombatTarget(Unit* forcedTarged = 0);
        Unit *GetCurrentTarget() { return m_targetCombat; };
        bool DoNextCombatManeuver();
        void DoCombatMovement();
        void SetIgnoreUpdateTime(uint8 t) {m_ignoreAIUpdatesUntilTime =time(0) + t; };
        void SetIgnoreTeleport(uint8 t) {m_ignoreTeleport = time(0) + t; };
        void SetIgnoreSpell(uint8 t) { m_ignoreSpell = time(0) + t; };
        void SetIgnoreUpdateSpellsAndItems() { m_ignoreSpellsAndItems = time(0) + 30; };
        time_t GetIgnoreSpell() { return m_ignoreSpell; };

        Player *GetPlayerBot() const { return m_bot; }
        Player *GetPlayer() const { return m_bot; }
        Player *GetMaster() const;
        void SetMaster(Player* pl);

        uint16 GetSpe() { return m_spe; };
        void SetSpe(uint16 spe) { m_spe = spe; };

        BotState GetState() { return m_botState; };
        void SetState(BotState state);
        void SetQuestNeedItems();
        void SendQuestItemList(Player& player);
        void SendOrders(Player& player);
        bool FollowCheckTeleport(WorldObject *obj);
        void DoLoot();

        bool CheckTeleport();
        bool CheckMaster();
        void CheckStuff();

        uint32 EstRepairAll();
        uint32 EstRepair(uint16 pos);

        void AcceptQuest(Quest const *qInfo, Player *pGiver);
        void TurnInQuests(WorldObject *questgiver);

        bool IsInCombat();
        void UpdateAttackerInfo();
        Unit* FindAttacker(ATTACKERINFOTYPE ait =AIT_NONE, Unit *victim = 0);
        uint32 GetAttackerCount() { return m_attackerInfo.size(); };
        void SetCombatOrderByStr(std::string str, Unit *target = 0);
        void SetCombatOrder(CombatOrderType co, Unit *target = 0);
        CombatOrderType GetCombatOrder() { return this->m_combatOrder; }
        void SetMovementOrder(MovementOrderType mo, Unit *followTarget = 0);
        MovementOrderType GetMovementOrder() { return this->m_movementOrder; }
        void MovementReset();
        void MovementUpdate();
        void MovementClear();
        bool IsMoving();
        void FindPOI(float &x, float &y, float &z, uint32 &mapId);

        void SetInFront(const Unit* obj);

        void ItemLocalization(std::string& itemName, const uint32 itemID) const;
        void QuestLocalization(std::string& questTitle, const uint32 questID) const;

    private:

        bool TradeItem(const Item& item, int8 slot =-1);
        bool TradeCopper(uint32 copper);

        PlayerbotMgr* const m_mgr;
        Player* const m_bot;
        PlayerbotClassAI* m_classAI;
        uint16 m_spe;

        time_t m_ignoreAIUpdatesUntilTime;
        time_t m_ignoreTeleport;
        time_t m_ignoreSpell;
        time_t m_ignoreSpellsAndItems;
        CombatStyle m_combatStyle;
        CombatOrderType m_combatOrder;
        MovementOrderType m_movementOrder;

        BotState m_botState;

        BotNeedItem m_needItemList;

        BotLootCreature m_lootCreature;
        uint64 m_lootCurrent;

        time_t m_TimeDoneEating;
        time_t m_TimeDoneDrinking;
        uint32 m_CurrentlyCastingSpellId;
        bool m_IsFollowingMaster;

        uint64 m_targetGuidCommand;

        AttackerInfoList m_attackerInfo;

        bool m_targetChanged;
        CombatTargetType m_targetType;

        Unit *m_targetCombat;
        Unit *m_targetAssist;
        Unit *m_targetProtect;

        Unit *m_followTarget;

        float m_position_fin_x;
        float m_position_fin_y;
        float m_position_fin_z;
        uint32 m_mapId_fin;
};

#endif
