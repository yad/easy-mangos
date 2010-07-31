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
#include "ItemPrototype.h"
#include "World.h"
#include "SpellMgr.h"
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
#include "Player.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "Spell.h"
#include "Unit.h"
#include "SpellAuras.h"
#include "SharedDefines.h"
#include "Log.h"
#include "GossipDef.h"

float rand_float(float low, float high)
{
    return (rand() / (static_cast<float>(RAND_MAX) + 1.0)) * (high - low) + low;
}

uint64 extractGuid(WorldPacket& packet)
{
    uint8 mask;
    packet >> mask;
    uint64 guid = 0;
    uint8 bit = 0;
    uint8 testMask = 1;
    while (true)
    {
        if (mask & testMask)
        {
            uint8 word;
            packet >> word;
            guid += (word << bit);
        }
        if (bit == 7)
            break;
        ++bit;
        testMask <<= 1;
    }
    return guid;
}

class PlayerbotChatHandler: protected ChatHandler
{
    public:
        explicit PlayerbotChatHandler(Player* pMasterPlayer): ChatHandler(pMasterPlayer) {}
        bool revive(const Player& botPlayer) { return HandleReviveCommand(botPlayer.GetName()); }
        bool teleport(const Player& botPlayer) { return HandleNamegoCommand(botPlayer.GetName()); }
        void sysmessage(const char *str) { SendSysMessage(str); }
        bool dropQuest(const char *str) { return HandleQuestRemove(str); }
        bool gmstartup(const char *str) { return HandleGMStartUpCommand(str); }
};

PlayerbotAI::PlayerbotAI(PlayerbotMgr* const mgr, Player* const bot):
    m_mgr(mgr), m_bot(bot), m_ignoreAIUpdatesUntilTime(0),
    m_ignoreTeleport(0), m_ignoreSpellsAndItems(0),
    m_combatOrder(ORDERS_NONE),
    m_TimeDoneEating(0), m_TimeDoneDrinking(0),
    m_CurrentlyCastingSpellId(0),
    m_targetGuidCommand(0), m_classAI(0)
{
    SetMaster(bot);
    m_botState = BOTSTATE_NORMAL;
    SetQuestNeedItems();

    m_targetChanged = false;
    m_targetType = TARGET_NORMAL;
    m_targetCombat = 0;
    m_targetAssist = 0;
    m_targetProtect = 0;

    switch (m_bot->getClass())
    {
        case CLASS_WARRIOR:
            m_combatStyle = COMBAT_MELEE;
            m_spe = (uint8)WarriorProtection;
            m_classAI = (PlayerbotClassAI*) new PlayerbotWarriorAI(m_bot, this);
            break;
        case CLASS_PALADIN:
            m_combatStyle = COMBAT_MELEE;
            m_spe = (uint8)PaladinHoly;
            m_classAI = (PlayerbotClassAI*) new PlayerbotPaladinAI(m_bot, this);
            break;
        case CLASS_HUNTER:
            m_combatStyle = COMBAT_RANGED;
            m_spe = (uint8)HunterBeastMastery;
            m_classAI = (PlayerbotClassAI*) new PlayerbotHunterAI(m_bot, this);
            break;
        case CLASS_ROGUE:
            m_combatStyle = COMBAT_MELEE;
            m_spe = (uint8)RogueSubtlety;
            m_classAI = (PlayerbotClassAI*) new PlayerbotRogueAI(m_bot, this);
            break;
        case CLASS_PRIEST:
            m_combatStyle = COMBAT_RANGED;
            m_spe = (uint8)PriestHoly;
            m_classAI = (PlayerbotClassAI*) new PlayerbotPriestAI(m_bot, this);
            break;
        case CLASS_DEATH_KNIGHT:
            m_combatStyle = COMBAT_MELEE;
            m_spe = (uint8)DeathKnightUnholy;
            m_classAI = (PlayerbotClassAI*) new PlayerbotDeathKnightAI(m_bot, this);
            break;
        case CLASS_SHAMAN:
            m_combatStyle = COMBAT_MELEE;
            m_spe = (uint8)ShamanElementalCombat;
            m_classAI = (PlayerbotClassAI*) new PlayerbotShamanAI(m_bot, this);
            break;
        case CLASS_MAGE:
            m_combatStyle = COMBAT_RANGED;
            m_spe = (uint8)MageFrost;
            m_classAI = (PlayerbotClassAI*) new PlayerbotMageAI(m_bot, this);
            break;
        case CLASS_WARLOCK:
            m_combatStyle = COMBAT_RANGED;
            m_spe = (uint8)WarlockDestruction;
            m_classAI = (PlayerbotClassAI*) new PlayerbotWarlockAI(m_bot, this);
            break;
        case CLASS_DRUID:
            m_combatStyle = COMBAT_MELEE;
            m_spe = (uint8)DruidFeralCombat;
            m_classAI = (PlayerbotClassAI*) new PlayerbotDruidAI(m_bot, this);
            break;
    }
    bot->GetPosition(m_position_fin_x, m_position_fin_y, m_position_fin_z);
    uint32 m_mapId_fin = bot->GetMapId();
}

PlayerbotAI::~PlayerbotAI()
{
    delete m_classAI;
}

Player* PlayerbotAI::GetMaster() const
{
    return m_mgr->GetMaster();
}

void PlayerbotAI::SetMaster(Player* pl)
{
    m_mgr->SetMaster(pl);
}

uint32 PlayerbotAI::getSpellId(const char* args) const
{
    if (!*args)
        return 0;

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return 0;

    wstrToLower(wnamepart);

    uint32 foundSpellId = 0;
    bool foundExactMatch = false;
    bool foundMatchUsesNoReagents = false;

    for (PlayerSpellMap::iterator itr = m_bot->GetSpellMap().begin(); itr != m_bot->GetSpellMap().end(); ++itr)
    {
        uint32 spellId = itr->first;

        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
            continue;

        const std::string name = sObjectMgr.GetSpellName(spellId);
        if (name.empty() || !Utf8FitTo(name, wnamepart))
            continue;

        const SpellEntry* pSpellInfo = sSpellStore.LookupEntry(spellId);
        if (!pSpellInfo)
            continue;

        bool isExactMatch = (name.length() == wnamepart.length()) ? true : false;
        bool usesNoReagents = (pSpellInfo->Reagent[0] <= 0) ? true : false;

        bool useThisSpell = true;

        if (foundSpellId > 0)
        {
            if (isExactMatch && !foundExactMatch) {}
            else if (usesNoReagents && !foundMatchUsesNoReagents) {}
            else if (spellId > foundSpellId) {}
            else useThisSpell = false;
        }

        if (useThisSpell)
        {
            foundSpellId = spellId;
            foundExactMatch = isExactMatch;
            foundMatchUsesNoReagents = usesNoReagents;
        }
    }

    if (m_mgr->m_confDebugWhisper)
    {
        if (foundSpellId == 0)
            sLog.outDebug("Spell %s : not found...", args);
        else
            sLog.outDebug("Spell %s : found ID = %d", args, foundSpellId);
    }

    return foundSpellId;
}

void PlayerbotAI::SendNotEquipList(Player& player)
{
    std::list<Item*>* equip[19];
    for (uint8 i = 0; i < 19; ++i)
        equip[i] = NULL;

    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        uint16 dest;
        uint8 msg = m_bot->CanEquipItem(NULL_SLOT, dest, pItem, !pItem->IsBag());
        if (msg != EQUIP_ERR_OK)
            continue;

        uint8 equipSlot = uint8(dest);
        if (!((equipSlot >= 0) && (equipSlot < 19)))
            continue;

        if (equip[equipSlot] == NULL)
            equip[equipSlot] = new std::list<Item*>;

        std::list<Item*>* itemListForEqSlot = equip[equipSlot];
        itemListForEqSlot->push_back(pItem);
    }

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (!pItem)
                    continue;

                uint16 equipSlot;
                uint8 msg = m_bot->CanEquipItem(NULL_SLOT, equipSlot, pItem, !pItem->IsBag());
                if (msg != EQUIP_ERR_OK)
                    continue;
                if (!((equipSlot >= 0) && (equipSlot < 19)))
                    continue;

                if (equip[equipSlot] == NULL)
                    equip[equipSlot] = new std::list<Item*>;

                std::list<Item*>* itemListForEqSlot = equip[equipSlot];
                itemListForEqSlot->push_back(pItem);
            }
        }
    }

    TellMaster("Voici tous les objets de mon inventaire que je peux porter.");
    ChatHandler ch(GetMaster());

    const std::string descr[] = { "head", "neck", "shoulders", "body", "chest",
            "waist", "legs", "feet", "wrists", "hands", "finger1", "finger2",
            "trinket1", "trinket2", "back", "mainhand", "offhand", "ranged",
            "tabard" };

    for (uint8 equipSlot = 0; equipSlot < 19; ++equipSlot)
    {
        if (equip[equipSlot] == NULL)
            continue;
        std::list<Item*>* itemListForEqSlot = equip[equipSlot];
        std::ostringstream out;
        out << descr[equipSlot] << ": ";
        for (std::list<Item*> ::iterator it = itemListForEqSlot->begin(); it != itemListForEqSlot->end(); ++it)
        {
            const ItemPrototype* const pItemProto = (*it)->GetProto();

            std::string itemName = pItemProto->Name1;
            ItemLocalization(itemName, pItemProto->ItemId);

            out << " |cffffffff|Hitem:" << pItemProto->ItemId
               << ":0:0:0:0:0:0:0" << "|h[" << itemName
               << "]|h|r";
        }
        ch.SendSysMessage(out.str().c_str());

        delete itemListForEqSlot;
    }
}

void PlayerbotAI::SendQuestItemList(Player& player)
{
    std::ostringstream out;

    for (BotNeedItem::iterator itr =m_needItemList.begin(); itr != m_needItemList.end(); ++itr)
    {
        const ItemPrototype * pItemProto = sObjectMgr.GetItemPrototype(itr->first);

        std::string itemName = pItemProto->Name1;
        ItemLocalization(itemName, pItemProto->ItemId);

        out << " " << itr->second << "x|cffffffff|Hitem:" << pItemProto->ItemId
           << ":0:0:0:0:0:0:0" << "|h[" << itemName
           << "]|h|r";
    }

    TellMaster("Voici la liste des objets dont j'ai besoin pour valider mes quêtes :");
    TellMaster(out.str().c_str());
}

void PlayerbotAI::SendOrders(Player& player)
{
    std::ostringstream out;

    if (!m_combatOrder)
        out << "Je n'ai pas d'ordre de combat";
    else if (m_combatOrder&ORDERS_TANK)
        out << "Je tank ";
    else if (m_combatOrder&ORDERS_ASSIST)
        out << "Je passe en assist " << (m_targetAssist ?m_targetAssist->GetName():"unknown");
    else if (m_combatOrder&ORDERS_HEAL)
        out << "Je prends en charge les soins ";
    if ((m_combatOrder&ORDERS_PRIMARY) && (m_combatOrder&ORDERS_SECONDARY))
        out << " et ";
    if (m_combatOrder&ORDERS_PROTECT)
        out << "Je protège " << (m_targetProtect ?m_targetProtect->GetName():"unknown");
    out << ".";

    TellMaster(out.str().c_str());
}

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
            m_ignoreAIUpdatesUntilTime = time(0) + 4;
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
            m_ignoreAIUpdatesUntilTime = 0;
            WorldPacket p(packet);
            uint64 flagGuid;
            p >> flagGuid;
            uint64 playerGuid;
            p >> playerGuid;
            Player* const pPlayer = ObjectAccessor::FindPlayer(playerGuid);
            if (canObeyCommandFrom(*pPlayer))
            {
                WorldPacket* const packet = new WorldPacket(CMSG_DUEL_ACCEPTED, 8);
                *packet << flagGuid;
                m_bot->GetSession()->QueuePacket(packet);

                float angle = rand_float(0, M_PI_F);
                float dist = rand_float(4, 10);
                float distX, distY;
                m_bot->GetMotionMaster()->Clear(true);
                distX = pPlayer->GetPositionX() + (dist * cos(angle));
                distY = pPlayer->GetPositionY() + (dist * sin(angle));
                m_bot->GetMotionMaster()->MovePoint(pPlayer->GetMapId(), distX, distY, pPlayer->GetPositionZ());
                m_bot->SetInFront(pPlayer);
                pPlayer->SendCreateUpdateToPlayer(m_bot);

                m_bot->SetSelection(playerGuid);
                m_ignoreAIUpdatesUntilTime = time(0) + 4;
            }
            return;
        }
        case SMSG_INVENTORY_CHANGE_FAILURE:
            return;
        case SMSG_SPELL_FAILURE:
        {
            WorldPacket p(packet);
            uint64 casterGuid = extractGuid(p);
            if (casterGuid != m_bot->GetGUID())
                return;
            uint32 spellId;
            p >> spellId;
            if (m_CurrentlyCastingSpellId == spellId)
            {
                m_ignoreAIUpdatesUntilTime = time(0) + 1;
                m_CurrentlyCastingSpellId = 0;
            }
            return;
        }
        case SMSG_FORCE_RUN_SPEED_CHANGE:
        {
            WorldPacket p(packet);
            uint64 guid = extractGuid(p);
            if (guid != GetMaster()->GetGUID())
                return;
            CheckMount();
            return;
        }
        case SMSG_MOVE_SET_CAN_FLY:
        {
            WorldPacket p(packet);
            uint64 guid = extractGuid(p);
            if (guid != m_bot->GetGUID())
                return;
            m_bot->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
            return;
        }
        case SMSG_MOVE_UNSET_CAN_FLY:
        {
            WorldPacket p(packet);
            uint64 guid = extractGuid(p);
            if (guid != m_bot->GetGUID())
                return;
            m_bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
            return;
        }
        case SMSG_GROUP_SET_LEADER:
        {
            WorldPacket p(packet);
            std::string name;
            p >> name;
            if ((m_bot->GetGroup()) && (name == m_bot->GetName()))
            {
                if (m_bot->GetGroup()->IsMember(GetMaster()->GetGUID()))
                {
                    p.resize(8);
                    p << GetMaster()->GetGUID();
                    m_bot->GetSession()->HandleGroupSetLeaderOpcode(p);
                }
                else
                {
                    p.clear();
                    m_bot->GetSession()->HandleGroupDisbandOpcode(p);
                }
            }
            return;
        }
        case SMSG_PARTY_COMMAND_RESULT:
        {
            WorldPacket p(packet);
            uint32 operation;
            p >> operation;
            std::string member;
            p >> member;
            uint32 result;
            p >> result;
            p.clear();
            if (operation == PARTY_OP_LEAVE)
            {
                if (member == GetMaster()->GetName())
                    m_bot->GetSession()->HandleGroupDisbandOpcode(p);
            }
            return;
        }
        case SMSG_GROUP_INVITE:
        {
            if (m_bot->GetGroupInvite())
            {
                const Group* const grp = m_bot->GetGroupInvite();
                if (!grp)
                    return;

                Player* const inviter = sObjectMgr.GetPlayer(grp->GetLeaderGUID());
                if (!inviter)
                    return;

                WorldPacket p;
                m_bot->GetSession()->HandleGroupAcceptOpcode(p);
            }
            return;
        }
        case SMSG_TRADE_STATUS:
        {
            if (m_bot->GetTrader() == NULL)
                break;

            WorldPacket p(packet);
            uint32 status;
            p >> status;
            p.resize(4);

            if (status == 4)
                m_bot->GetSession()->HandleAcceptTradeOpcode(p);
            else if (status == 1)
            {
                m_bot->GetSession()->HandleBeginTradeOpcode(p);

                if (!canObeyCommandFrom(* (m_bot->GetTrader())))
                {
                    SendWhisper("Je n'ai pas le droit de vous donner mes objets. Mais je peux accepter les votres ainsi que votre or.", * (m_bot->GetTrader()));
                    return;
                }

                std::ostringstream out;

                for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
                {
                    const Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    if (pItem && (pItem->CanBeTraded()))
                    {
                        const ItemPrototype* const pItemProto = pItem->GetProto();

                        std::string itemName = pItemProto->Name1;
                        ItemLocalization(itemName, pItemProto->ItemId);

                        out << " |cffffffff|Hitem:" << pItemProto->ItemId
                           << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
                        if (pItem->GetCount() > 1)
                            out << "x" << pItem->GetCount() << ' ';
                    }
                }

                for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
                {
                    const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                    if (pBag)
                    {
                        for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                        {
                            const Item* const pItem = m_bot->GetItemByPos(bag, slot);
                            if (pItem && (pItem->CanBeTraded()))
                            {
                                const ItemPrototype* const pItemProto = pItem->GetProto();

                                std::string itemName = pItemProto->Name1;
                                ItemLocalization(itemName, pItemProto->ItemId);

                                out << " |cffffffff|Hitem:" << pItemProto->ItemId
                                   << ":0:0:0:0:0:0:0" << "|h[" << itemName
                                   << "]|h|r";
                                if (pItem->GetCount() > 1)
                                    out << "x" << pItem->GetCount() << ' ';
                            }
                        }
                    }
                }

                uint32 copper = m_bot->GetMoney();
                uint32 gold = uint32(copper / 10000);
                copper -= (gold * 10000);
                uint32 silver = uint32(copper / 100);
                copper -= (silver * 100);

                std::ostringstream whisper;
                whisper << "Je possède actuellement |cff00ff00" << gold
                       << "|r|cfffffc00g|r|cff00ff00" << silver
                       << "|r|cffcdcdcds|r|cff00ff00" << copper
                       << "|r|cffffd333c|r" << " et je peux échanger les objets suivants :";
                SendWhisper(whisper.str().c_str(), * (m_bot->GetTrader()));
                ChatHandler ch(m_bot->GetTrader());
                ch.SendSysMessage(out.str().c_str());
            }
            return;
        }

        case SMSG_SPELL_GO:
        {
            WorldPacket p(packet);
            uint64 castItemGuid = extractGuid(p);
            uint64 casterGuid = extractGuid(p);
            if (casterGuid != m_bot->GetGUID())
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
    }
}

uint8 PlayerbotAI::GetHealthPercent(const Unit& target) const
{
    return (static_cast<float>(target.GetHealth()) / target.GetMaxHealth()) * 100;
}

uint8 PlayerbotAI::GetHealthPercent() const
{
    return GetHealthPercent(*m_bot);
}

uint8 PlayerbotAI::GetManaPercent(const Unit& target) const
{
    return (static_cast<float>(target.GetPower(POWER_MANA)) / target.GetMaxPower(POWER_MANA)) * 100;
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
        return (static_cast<float>(target.GetPower(POWER_MANA)) / target.GetMaxPower(POWER_MANA)) * 100;
}

uint8 PlayerbotAI::GetBaseManaPercent() const
{
    return GetBaseManaPercent(*m_bot);
}

uint8 PlayerbotAI::GetRageAmount(const Unit& target) const
{
    return (static_cast<float>(target.GetPower(POWER_RAGE)));
}

uint8 PlayerbotAI::GetRageAmount() const
{
    return GetRageAmount(*m_bot);
}

uint8 PlayerbotAI::GetEnergyAmount(const Unit& target) const
{
    return (static_cast<float>(target.GetPower(POWER_ENERGY)));
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

bool PlayerbotAI::HasAura(uint32 spellId, const Unit& player) const
{
    if (spellId <= 0)
        return false;

    if (!m_bot->IsWithinDistInMap(&player, 50, true))
        return true;

    for (Unit::SpellAuraHolderMap::const_iterator iter = player.GetSpellAuraHolderMap().begin(); iter != player.GetSpellAuraHolderMap().end(); ++iter)
    {
        if (iter->second->GetId() == spellId)
            return true;
    }
    return false;
}

bool PlayerbotAI::HasAura(const char* spellName) const
{
    return HasAura(spellName, *m_bot);
}

bool PlayerbotAI::HasAura(const char* spellName, const Unit& player) const
{
    uint32 spellId = getSpellId(spellName);
    return (spellId) ? HasAura(spellId, player): false;
}

Item* PlayerbotAI::FindMount(uint32 matchingRidingSkill) const
{
    Item* partialMatch = NULL;

    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto || !m_bot->CanUseItem(pItemProto) || pItemProto->RequiredSkill != SKILL_RIDING)
                continue;

            if (pItemProto->RequiredSkillRank == matchingRidingSkill)
                return pItem;

            else if (!partialMatch  || (partialMatch && (partialMatch->GetProto()->RequiredSkillRank < pItemProto->RequiredSkillRank)))
                partialMatch = pItem;
        }
    }

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto || !m_bot->CanUseItem(pItemProto) || pItemProto->RequiredSkill != SKILL_RIDING)
                        continue;

                    if (pItemProto->RequiredSkillRank == matchingRidingSkill)
                        return pItem;

                    else if (!partialMatch  || (partialMatch && (partialMatch->GetProto()->RequiredSkillRank < pItemProto->RequiredSkillRank)))
                        partialMatch = pItem;
                }
            }
        }
    }
    return partialMatch;
}

void PlayerbotAI::CheckMount()
{
    time_t currentTime = time(0);
    if (currentTime < m_ignoreTeleport)
        return;

    if ((GetMaster()->IsMounted()) && (!m_bot->IsMounted()))
    {
        if (!GetMaster()->GetAurasByType(SPELL_AURA_MOUNTED).empty())
        {
            int32 master_speed1 = 0;
            int32 master_speed2 = 0;
            master_speed1 = GetMaster()->GetAurasByType(SPELL_AURA_MOUNTED).front()->GetSpellProto()->EffectBasePoints[1];
            master_speed2 = GetMaster()->GetAurasByType(SPELL_AURA_MOUNTED).front()->GetSpellProto()->EffectBasePoints[2];

            Item* pItem225 = NULL;
            Item* pItem300 = NULL;
            pItem225 = m_bot->GetPlayerbotAI()->FindMount(225);
            pItem300 = m_bot->GetPlayerbotAI()->FindMount(300);
            if (pItem300 && (master_speed1 > 150 || master_speed2 > 150))
            {
                m_bot->GetPlayerbotAI()->UseItem(*pItem300);
                return;
            }
            else if (pItem225)
            {
                m_bot->GetPlayerbotAI()->UseItem(*pItem225);
                return;
            }
            if (pItem300)
            {
                m_bot->GetPlayerbotAI()->UseItem(*pItem300);
                return;
            }
            uint32 spellMount = 0;
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
                    if (pSpellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED)
                    {
                        if (pSpellInfo->EffectBasePoints[1] == master_speed1)
                        {
                            spellMount = spellId;
                            break;
                        }
                    }
                    else if ((pSpellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED)
                        && (pSpellInfo->EffectApplyAuraName[2] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED))
                    {
                        if ((pSpellInfo->EffectBasePoints[1] == master_speed1)
                            && (pSpellInfo->EffectBasePoints[2] == master_speed2))
                        {
                            spellMount = spellId;
                            break;
                        }
                    }
                    else if ((pSpellInfo->EffectApplyAuraName[2] == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED)
                        && (pSpellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED))
                    {
                        if ((pSpellInfo->EffectBasePoints[2] == master_speed2)
                            && (pSpellInfo->EffectBasePoints[1] == master_speed1))
                        {
                            spellMount = spellId;
                            break;
                        }
                    }
                }
            }
            if (spellMount > 0)m_bot->CastSpell(m_bot, spellMount, false);
        }
    }
    else if (!GetMaster()->IsMounted() && m_bot->IsMounted())
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
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto || !m_bot->CanUseItem(pItemProto))
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
            {
                if (pItemProto->Spells[0].SpellCategory == 11)
                    return pItem;
            }
        }
    }

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || !m_bot->CanUseItem(pItemProto))
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                    {
                        if (pItemProto->Spells[0].SpellCategory == 11)
                            return pItem;
                    }
                }
            }
        }
    }
    return NULL;
}

Item* PlayerbotAI::FindDrink() const
{
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || !m_bot->CanUseItem(pItemProto))
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
            {
                if (pItemProto->Spells[0].SpellCategory == 59)
                    return pItem;
            }
        }
    }

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || !m_bot->CanUseItem(pItemProto))
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                    {
                        if (pItemProto->Spells[0].SpellCategory == 59)
                            return pItem;
                    }
                }
            }
        }
    }
    return NULL;
}

Item* PlayerbotAI::FindBandage() const
{
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || !m_bot->CanUseItem(pItemProto))
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_BANDAGE)
                return pItem;
        }
    }

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || !m_bot->CanUseItem(pItemProto))
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_BANDAGE)
                        return pItem;
                }
            }
        }
    }
    return NULL;
}

Item* PlayerbotAI::FindPoison() const
{
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();

            if (!pItemProto || !m_bot->CanUseItem(pItemProto))
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == 6)
                return pItem;
        }
    }

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();

                    if (!pItemProto || !m_bot->CanUseItem(pItemProto))
                        continue;

                    if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == 6)
                        return pItem;
                }
            }
        }
    }
    return NULL;
}

void PlayerbotAI::InterruptCurrentCastingSpell()
{
    WorldPacket* const packet = new WorldPacket(CMSG_CANCEL_CAST, 5);
    *packet << m_CurrentlyCastingSpellId;
    *packet << m_targetGuidCommand;
    m_CurrentlyCastingSpellId = 0;
    m_bot->GetSession()->QueuePacket(packet);
}

void PlayerbotAI::Feast()
{
    if (!(m_bot->GetHealth() < m_bot->GetMaxHealth() || (m_bot->getPowerType() == POWER_MANA && m_bot->GetPower(POWER_MANA) < m_bot->GetMaxPower(POWER_MANA))))
    {
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
        return;
    }

    time_t currentTime = time(0);
    m_ignoreAIUpdatesUntilTime = currentTime + 3;

    if (m_bot->getPowerType() == POWER_MANA && currentTime > m_TimeDoneDrinking
        && ((static_cast<float>(m_bot->GetPower(POWER_MANA)) / m_bot->GetMaxPower(POWER_MANA)) < 0.8))
    {
        Item* pItem = FindDrink();
        if (pItem)
        {
            UseItem(*pItem);
            m_TimeDoneDrinking = currentTime + 30;
            return;
        }
        TellMaster("J'ai besoin d'eau...");
    }

    if (currentTime > m_TimeDoneEating && ((static_cast<float>(m_bot->GetHealth()) / m_bot->GetMaxHealth()) < 0.8))
    {
        Item* pItem = FindFood();
        if (pItem)
        {
            UseItem(*pItem);
            m_TimeDoneEating = currentTime + 30;
            return;
        }
        TellMaster("J'ai besoin de nourriture.");
    }

    if (currentTime > m_TimeDoneEating && currentTime > m_TimeDoneDrinking)
    {
        TellMaster("Festin fini !");
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
    }
}

void PlayerbotAI::GetCombatTarget(Unit* forcedTarget)
{
    if (m_botState != BOTSTATE_COMBAT)
    {
        SetState(BOTSTATE_COMBAT);
        SetQuestNeedItems();
        m_lootCreature.clear();
        m_lootCurrent = 0;
        m_targetCombat = 0;
    }

    UpdateAttackerInfo();

    if (!forcedTarget && (m_combatOrder&ORDERS_PROTECT) && m_targetProtect != 0)
    {
        Unit * newTarget = FindAttacker((ATTACKERINFOTYPE)(AIT_VICTIMNOTSELF|AIT_HIGHESTTHREAT), m_targetProtect);
        if (newTarget && newTarget != m_targetCombat)
        {
            forcedTarget = newTarget;
            m_targetType = TARGET_THREATEN;
        }
    }
    else if (forcedTarget)
    {
        m_targetType = (m_combatOrder ==ORDERS_TANK ? TARGET_THREATEN : TARGET_NORMAL);
    }

    if (m_targetCombat && !forcedTarget)
        return;

    if (forcedTarget)
    {
        m_targetCombat = forcedTarget;
        m_targetChanged = true;
    }

    if (!m_targetCombat && (m_combatOrder&ORDERS_ASSIST) && m_targetAssist != 0)
    {
        m_targetCombat = FindAttacker((ATTACKERINFOTYPE)(AIT_VICTIMNOTSELF|AIT_LOWESTTHREAT), m_targetAssist);
        m_targetType = (m_combatOrder ==ORDERS_TANK ? TARGET_THREATEN : TARGET_NORMAL);
        m_targetChanged = true;
    }

    if (!m_targetCombat)
    {
        m_targetCombat = FindAttacker();
        m_targetType = (m_combatOrder ==ORDERS_TANK ? TARGET_THREATEN : TARGET_NORMAL);
        m_targetChanged = true;
    }

    if (!m_targetCombat)
    {
        m_targetType = TARGET_NORMAL;
        m_targetChanged = false;
        return;
    }

    if (m_targetCombat->GetTypeId() == TYPEID_PLAYER && dynamic_cast<Player*>(m_targetCombat)->duel)
    {
        m_ignoreAIUpdatesUntilTime = time(0) + 6;
        return;
    }

    m_bot->SetSelection(m_targetCombat->GetGUID());
    m_ignoreAIUpdatesUntilTime = time(0) + 1;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    m_bot->Attack(m_targetCombat, true);

    m_lootCreature.push_back(m_targetCombat->GetGUID());

    MovementClear();
    return;
}

bool PlayerbotAI::DoNextCombatManeuver()
{
    GetCombatTarget();

    if (!m_targetCombat || m_targetCombat->isDead() || !m_targetCombat->IsInWorld() || !m_bot->IsHostileTo(m_targetCombat))
    {
        m_bot->AttackStop();
        m_bot->SetSelection(0);
        MovementReset();
        m_bot->InterruptNonMeleeSpells(true);
        m_targetCombat = 0;
        m_targetChanged = false;
        m_targetType = TARGET_NORMAL;
        return false;
    }

    if (m_targetChanged)
    {
        if (GetClassAI())
            m_targetChanged = GetClassAI()->DoFirstCombatManeuver(m_targetCombat);
        else
            m_targetChanged = false;
    }

    DoCombatMovement();

    if (GetClassAI() && !m_targetChanged)
       (GetClassAI())->DoNextCombatManeuver(m_targetCombat);

    return true;
}

void PlayerbotAI::DoCombatMovement()
{
    if (!m_targetCombat)return;

    float targetDist = m_bot->GetDistance(m_targetCombat);

    if (m_combatStyle ==COMBAT_MELEE && m_movementOrder != MOVEMENT_STAY)
    {
        float angle = rand_float(0, M_PI_F);
        float dist = rand_float(1.0f, 3.0f);
        float distX, distY;
        m_bot->GetMotionMaster()->Clear(true);
        distX = m_targetCombat->GetPositionX() + (dist * cos(angle));
        distY = m_targetCombat->GetPositionY() + (dist * sin(angle));
        m_bot->GetMotionMaster()->MovePoint(m_targetCombat->GetMapId(), distX, distY, m_targetCombat->GetPositionZ());
        m_bot->SetInFront(m_targetCombat);
        m_targetCombat->SendCreateUpdateToPlayer(m_bot);
    }
    else if (m_combatStyle ==COMBAT_RANGED && m_movementOrder != MOVEMENT_STAY)
    {
        if (targetDist > 50.0f)
        {
            float angle = rand_float(0, M_PI_F);
            float dist = rand_float(10.0f, 20.0f);
            float distX, distY;
            m_bot->GetMotionMaster()->Clear(true);
            distX = m_targetCombat->GetPositionX() + (dist * cos(angle));
            distY = m_targetCombat->GetPositionY() + (dist * sin(angle));
            m_bot->GetMotionMaster()->MovePoint(m_targetCombat->GetMapId(), distX, distY, m_targetCombat->GetPositionZ());
            m_bot->SetInFront(m_targetCombat);
            m_targetCombat->SendCreateUpdateToPlayer(m_bot);
        }
    }
}

void PlayerbotAI::SetQuestNeedItems()
{
    m_needItemList.clear();
    m_lootCreature.clear();
    m_lootCurrent = 0;

    for (QuestStatusMap::iterator iter =m_bot->getQuestStatusMap().begin(); iter != m_bot->getQuestStatusMap().end(); ++iter)
    {
        const Quest *qInfo = sObjectMgr.GetQuestTemplate(iter->first);
        if (!qInfo)
            continue;

        QuestStatusData *qData = &iter->second;
        if (qData->m_status != QUEST_STATUS_INCOMPLETE)
            continue;

        for (int i =0; i < QUEST_OBJECTIVES_COUNT; i++)
        {
            if (!qInfo->ReqItemCount[i]  || (qInfo->ReqItemCount[i]-qData->m_itemcount[i]) <= 0)
                continue;
            m_needItemList[qInfo->ReqItemId[i]] = (qInfo->ReqItemCount[i]-qData->m_itemcount[i]);
        }
    }
}

void PlayerbotAI::SetState(BotState state)
{
    m_botState = state;
}

void PlayerbotAI::DoLoot()
{
    if (!m_lootCurrent && m_lootCreature.empty())
    {
        m_botState = BOTSTATE_NORMAL;
        SetQuestNeedItems();
        return;
    }

    if (!m_lootCurrent)
    {
        m_lootCurrent = m_lootCreature.front();
        m_lootCreature.pop_front();
        Creature *c = m_bot->GetMap()->GetCreature(m_lootCurrent);
        if (!c || c->getDeathState() != CORPSE || GetMaster()->GetDistance(c) > BOTLOOT_DISTANCE)
        {
            m_lootCurrent = 0;
            return;
        }
        m_bot->GetMotionMaster()->MovePoint(c->GetMapId(), c->GetPositionX(), c->GetPositionY(), c->GetPositionZ());
    }
    else
    {
        Creature *c = m_bot->GetMap()->GetCreature(m_lootCurrent);
        if (!c || c->getDeathState() != CORPSE || GetMaster()->GetDistance(c) > BOTLOOT_DISTANCE)
        {
            m_lootCurrent = 0;
            return;
        }
        if (m_bot->IsWithinDistInMap(c, INTERACTION_DISTANCE))
        {
            m_bot->SendLoot(m_lootCurrent, LOOT_CORPSE);
            Loot *loot = &c->loot;
            uint32 lootNum = loot->GetMaxSlotInLootFor(m_bot);
            for (uint32 l =0; l < lootNum; l++)
            {
                QuestItem *qitem =0, *ffaitem =0, *conditem =0;
                LootItem *item = loot->LootItemInSlot(l, m_bot, &qitem, &ffaitem, &conditem);
                if (!item)
                    continue;

                if (!qitem && item->is_blocked)
                {
                    m_bot->SendLootRelease(m_bot->GetLootGUID());
                    continue;
                }

                if (m_needItemList[item->itemid] > 0)
                {
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
                                ffaitem->is_looted =true;
                                m_bot->SendNotifyLootItemRemoved(l);
                            }
                            else
                            {
                                if (conditem)
                                    conditem->is_looted =true;
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

            m_bot->GetSession()->DoLootRelease(m_lootCurrent);
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
        m_bot->SetDivider(0);
        return;
    }

    if (!m_bot->CanTakeQuest(qInfo, false))
    {
        m_bot->SetDivider(0);
        return;
    }

    if (m_bot->GetDivider() != 0)
    {
        pGiver->SendPushToPartyResponse(m_bot, QUEST_PARTY_MSG_ACCEPT_QUEST);
        m_bot->SetDivider(0);
    }

    if (m_bot->CanAddQuest(qInfo, false))
    {
        m_bot->AddQuest(qInfo, pGiver);

        if (m_bot->CanCompleteQuest(quest))
            m_bot->CompleteQuest(quest);

        if (qInfo->GetSrcSpell() > 0)
            m_bot->CastSpell(m_bot, qInfo->GetSrcSpell(), true);
    }
}

void PlayerbotAI::TurnInQuests(WorldObject *questgiver)
{
    uint64 giverGUID = questgiver->GetGUID();

    if (!m_bot->IsInMap(questgiver))
        TellMaster("Hey, tu prends des quetes sans moi !");
    else
    {
        m_bot->SetSelection(giverGUID);
        m_bot->PrepareQuestMenu(giverGUID);
        QuestMenu& questMenu = m_bot->PlayerTalkClass->GetQuestMenu();

        for (uint32 iI = 0; iI < questMenu.MenuItemCount(); ++iI)
        {
            QuestMenuItem const& qItem = questMenu.GetItem(iI);
            uint32 questID = qItem.m_qId;
            Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);

            std::ostringstream out;
            std::string questTitle = pQuest->GetTitle();
            QuestLocalization(questTitle, questID);

            QuestStatus status = m_bot->GetQuestStatus(questID);

            if (status == QUEST_STATUS_COMPLETE)
            {
                if (!m_bot->GetQuestRewardStatus(questID))
                {
                    if (pQuest->GetRewChoiceItemsCount() == 0)
                    {
                        if (m_bot->CanRewardQuest(pQuest, false))
                        {
                            m_bot->RewardQuest(pQuest, 0, questgiver, false);
                            out << "Quête finie : |cff808080|Hquetes:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
                        }
                        else
                            out << "|cffff0000Impossible de prendre la quête:|r |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
                    }

                    else if (pQuest->GetRewChoiceItemsCount() == 1)
                    {
                        int rewardIdx = 0;
                        ItemPrototype const *pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[rewardIdx]);
                        std::string itemName = pRewardItem->Name1;
                        ItemLocalization(itemName, pRewardItem->ItemId);
                        if (m_bot->CanRewardQuest(pQuest, rewardIdx, false))
                        {
                            m_bot->RewardQuest(pQuest, rewardIdx, questgiver, true);

                            std::string itemName = pRewardItem->Name1;
                            ItemLocalization(itemName, pRewardItem->ItemId);

                            out << "Quête(s)finie(s): "
                               << " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel()
                               << "|h[" << questTitle << "]|h|r reward: |cffffffff|Hitem:"
                               << pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
                        }
                        else
                        {
                            out << "|cffff0000Impossible de prendre la quête :|r "
                               << "|cff808080|Hquest:" << questID << ':'
                               << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r"
                               << " reward: |cffffffff|Hitem:"
                               << pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
                        }
                    }
                    else
                    {
                        out << "Quelle récompense dois-je prendre|cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel()
                           << "|h[" << questTitle << "]|h|r ? ";
                        for (uint8 i =0; i < pQuest->GetRewChoiceItemsCount(); ++i)
                        {
                            ItemPrototype const * const pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[i]);
                            std::string itemName = pRewardItem->Name1;
                            ItemLocalization(itemName, pRewardItem->ItemId);
                            out << "|cffffffff|Hitem:" << pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
                        }
                    }
                }
            }

            else if (status == QUEST_STATUS_INCOMPLETE)
            {
                out << "|cffff0000Quête(s)incomplète(s):|r "
                   << " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
            }

            else if (status == QUEST_STATUS_AVAILABLE)
            {
                out << "|cff00ff00Quête(s)active(s):|r "
                   << " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
            }

            if (!out.str().empty())
                TellMaster(out.str());
        }
    }
}

bool PlayerbotAI::IsInCombat()
{
    if (!m_bot->getAttackers().empty())
        return true;
    if (!GetMaster()->getAttackers().empty())
        return true;

    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (!ref->getSource()->getAttackers().empty())
                return true;
            ref = ref->next();
        }
    }
    return false;
}

void PlayerbotAI::UpdateAttackerInfo()
{
    m_attackerInfo.clear();

    HostileReference *ref = m_bot->getHostileRefManager().getFirst();
    while (ref)
    {
        ThreatManager *target = ref->getSource();
        uint64 guid = target->getOwner()->GetGUID();
        m_attackerInfo[guid].attacker = target->getOwner();
        m_attackerInfo[guid].victim = target->getOwner()->getVictim();
        m_attackerInfo[guid].threat = target->getThreat(m_bot);
        m_attackerInfo[guid].count = 1;
        m_attackerInfo[guid].source = 1;
        ref = ref->next();
    }

    ref = NULL;
    if ((m_bot != GetMaster()) && m_bot->IsWithinDistInMap(GetMaster(), 100, true))
        ref = GetMaster()->getHostileRefManager().getFirst();

    while (ref)
    {
        ThreatManager *target = ref->getSource();
        uint64 guid = target->getOwner()->GetGUID();
        if (m_attackerInfo.find(guid) == m_attackerInfo.end())
        {
            m_attackerInfo[guid].attacker = target->getOwner();
            m_attackerInfo[guid].victim = target->getOwner()->getVictim();
            m_attackerInfo[guid].count = 0;
            m_attackerInfo[guid].source = 2;
        }
        m_attackerInfo[guid].threat = target->getThreat(m_bot);
        m_attackerInfo[guid].count++;
        ref = ref->next();
    }

    if (m_bot->GetGroup())
    {
        GroupReference *gref = m_bot->GetGroup()->GetFirstMember();
        while (gref)
        {
            if (gref->getSource() == m_bot || gref->getSource() == GetMaster())
            {
                gref = gref->next();
                continue;
            }
            ref = NULL;
            if (m_bot->IsWithinDistInMap(gref->getSource(), 100, true))
                ref = gref->getSource()->getHostileRefManager().getFirst();
            while (ref)
            {
                ThreatManager *target = ref->getSource();
                uint64 guid = target->getOwner()->GetGUID();
                if (m_attackerInfo.find(guid) == m_attackerInfo.end())
                {
                    m_attackerInfo[guid].attacker = target->getOwner();
                    m_attackerInfo[guid].victim = target->getOwner()->getVictim();
                    m_attackerInfo[guid].count = 0;
                    m_attackerInfo[guid].source = 3;
                }
                m_attackerInfo[guid].threat = target->getThreat(m_bot);
                m_attackerInfo[guid].count++;
                ref = ref->next();
            }
            gref = gref->next();
        }
    }

    for (AttackerInfoList::iterator itr =m_attackerInfo.begin(); itr != m_attackerInfo.end(); ++itr)
    {
        if (!itr->second.attacker)
            continue;
        Unit *a = itr->second.attacker;
        float t = 0.00;
        std::list< HostileReference* > ::const_iterator i =a->getThreatManager().getThreatList().begin();
        for (; i != a->getThreatManager().getThreatList().end(); ++i)
        {
            if ((*i)->getThreat() > t && (*i)->getTarget() != m_bot)
                t = (*i)->getThreat();
        }
        m_attackerInfo[itr->first].threat2 = t;
    }
}

uint32 PlayerbotAI::EstRepairAll()
{
    uint32 TotalCost = 0;
    for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        TotalCost += EstRepair(((INVENTORY_SLOT_BAG_0 << 8)| i));

    for (int j = INVENTORY_SLOT_BAG_START; j < INVENTORY_SLOT_BAG_END; ++j)
        for (int i = 0; i < MAX_BAG_SIZE; ++i)
            TotalCost += EstRepair(((j << 8)| i));
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
            return TotalCost;

        uint32 dQualitymodEntryId = (ditemProto->Quality + 1) *2;
        DurabilityQualityEntry const *dQualitymodEntry = sDurabilityQualityStore.LookupEntry(dQualitymodEntryId);
        if (!dQualitymodEntry)
            return TotalCost;

        uint32 dmultiplier = dcost->multiplier[ItemSubClassToDurabilityMultiplierId(ditemProto->Class, ditemProto->SubClass)];
        uint32 costs = uint32(LostDurability*dmultiplier*double(dQualitymodEntry->quality_mod));

        if (costs ==0)
            costs = 1;

        TotalCost = costs;
    }
    return TotalCost;
}

Unit *PlayerbotAI::FindAttacker(ATTACKERINFOTYPE ait, Unit *victim)
{
    if (m_attackerInfo.empty())
        return 0;

    if (!ait)
        return (m_attackerInfo.begin())->second.attacker;

    float t = ((ait & AIT_HIGHESTTHREAT) ? 0.00 : 9999.00);
    Unit *a = 0;
    AttackerInfoList::iterator itr = m_attackerInfo.begin();
    for (; itr != m_attackerInfo.end(); ++itr)
    {
        if ((ait & AIT_VICTIMSELF) && !(ait & AIT_VICTIMNOTSELF) && itr->second.victim != m_bot)
            continue;

        if (!(ait & AIT_VICTIMSELF) && (ait & AIT_VICTIMNOTSELF) && itr->second.victim == m_bot)
            continue;

        if ((ait & AIT_VICTIMNOTSELF) && victim && itr->second.victim != victim)
            continue;

        if (!(ait &(AIT_LOWESTTHREAT|AIT_HIGHESTTHREAT)))
        {
            a = itr->second.attacker;
            itr = m_attackerInfo.end();
        }
        else
        {
            if ((ait & AIT_HIGHESTTHREAT) && itr->second.threat >= t)
            {
                t = itr->second.threat;
                a = itr->second.attacker;
            }
            else if ((ait & AIT_LOWESTTHREAT) && itr->second.threat <= t)
            {
                t = itr->second.threat;
                a = itr->second.attacker;
            }
        }
    }
    return a;
}

void PlayerbotAI::SetCombatOrderByStr(std::string str, Unit *target)
{
    CombatOrderType co;
    if (str == "tank")co = ORDERS_TANK;
    else if (str == "assist")co = ORDERS_ASSIST;
    else if (str == "heal")co = ORDERS_HEAL;
    else if (str == "protect")co = ORDERS_PROTECT;
    else co = ORDERS_RESET;
    SetCombatOrder(co, target);
}

void PlayerbotAI::SetCombatOrder(CombatOrderType co, Unit *target)
{
    if ((co == ORDERS_ASSIST || co == ORDERS_PROTECT) && !target)
        return;
    if (co == ORDERS_RESET)
    {
        m_combatOrder = ORDERS_NONE;
        m_targetAssist = 0;
        m_targetProtect = 0;
        return;
    }
    if (co == ORDERS_PROTECT)
        m_targetProtect = target;
    else if (co == ORDERS_ASSIST)
        m_targetAssist = target;
    if ((co&ORDERS_PRIMARY))
        m_combatOrder = (CombatOrderType)(((uint32)m_combatOrder&(uint32)ORDERS_SECONDARY)|(uint32)co);
    else
        m_combatOrder = (CombatOrderType)(((uint32)m_combatOrder&(uint32)ORDERS_PRIMARY)|(uint32)co);
}

void PlayerbotAI::SetMovementOrder(MovementOrderType mo, Unit *followTarget)
{
    m_movementOrder = mo;
    m_followTarget = followTarget;
    MovementReset();
}

void PlayerbotAI::MovementReset()
{
    MovementClear();

    if (m_movementOrder == MOVEMENT_FOLLOW)
    {
        if (!m_followTarget)
            return;

        if (m_followTarget->GetTypeId() ==TYPEID_PLAYER && ((Player*)m_followTarget)->IsBeingTeleported())
            return;

        if (!IsInCombat())
        {
            if (m_followTarget->GetTypeId() ==TYPEID_PLAYER && ((Player*)m_followTarget)->GetCorpse())
            {
                if (!FollowCheckTeleport(((Player*)m_followTarget)->GetCorpse()))
                    return;
            }
            else
            {
                if (!FollowCheckTeleport(m_followTarget))
                    return;
            }
        }

        if (m_bot->isAlive())
        {
            if (m_bot == GetMaster())
            {
                if (m_bot->IsInRange3d(m_position_fin_x, m_position_fin_y, m_position_fin_z, 0.0f, 10.0f))
                {
                    FindPOI(m_position_fin_x, m_position_fin_y, m_position_fin_z, m_mapId_fin);
                    m_bot->GetMotionMaster()->Clear(true);
                    m_bot->GetMotionMaster()->MovePoint(m_mapId_fin, m_position_fin_x, m_position_fin_y, m_position_fin_z);
                }
                else
                {
                    m_bot->GetMotionMaster()->Clear(true);
                    m_bot->GetMotionMaster()->MovePoint(m_mapId_fin, m_position_fin_x, m_position_fin_y, m_position_fin_z);
                }
            }
            else
            {
                float angle = rand_float(0, M_PI_F);
                float dist = rand_float(5.0f, 10.0f);

                if (m_bot->IsWithinDist(GetMaster(), 10.0f))
                    return;

                float distX, distY;
                distX = m_followTarget->GetPositionX() + (dist * cos(angle));
                distY = m_followTarget->GetPositionY() + (dist * sin(angle));
                m_bot->GetMotionMaster()->MovePoint(m_followTarget->GetMapId(), distX, distY, m_followTarget->GetPositionZ());
                m_bot->SetInFront(m_followTarget);
                m_followTarget->SendCreateUpdateToPlayer(m_bot);
            }
        }
    }
}

void PlayerbotAI::MovementUpdate()
{
    WorldPacket data;
    m_bot->BuildHeartBeatMsg(&data);
    m_bot->SendMessageToSet(&data, false);

    float x = m_bot->GetPositionX();
    float y = m_bot->GetPositionY();
    float z = m_bot->GetPositionZ();
    float o = m_bot->GetOrientation();

    m_bot->UpdateGroundPositionZ(x, y, z);
    m_bot->SetPosition(x, y, z, o, false);
}

void PlayerbotAI::FindPOI(float &x, float &y, float &z, uint32 &mapId)
{
    Unit* target = m_bot->SelectRandomFriendlyTarget(0, 500.0f);
    if (target)
    {
        target->GetPosition(x, y, z);
        mapId = target->GetMapId();
    }
    else
    {
        m_bot->GetPosition(x, y, z);
        mapId = m_bot->GetMapId();
    }
}

void PlayerbotAI::MovementClear()
{
    m_bot->GetMotionMaster()->Clear(true);
    m_bot->clearUnitState(UNIT_STAT_CHASE);
    m_bot->clearUnitState(UNIT_STAT_FOLLOW);

    if (!m_bot->IsStandState())
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
}

bool PlayerbotAI::IsMoving()
{
    return (m_bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE ? false : true);
}

void PlayerbotAI::SetInFront(const Unit* obj)
{
    m_bot->SetInFront(obj);
}

void PlayerbotAI::UpdateAI(const uint32 p_time)
{
    time_t currentTime = time(0);
    if (currentTime < m_ignoreAIUpdatesUntilTime)
        return;

    m_ignoreAIUpdatesUntilTime = time(0) + 2;

    if (!GetMaster())
    {
        m_bot->GetPlayerbotMgr()->LogoutPlayerBot(m_bot->GetGUID());
        m_bot->GetPlayerbotMgr()->AddAllBots(sConfig.GetIntDefault( "PlayerbotAI.MaxBots", 100 ));
        return;
    }

    if (!GetMaster()->IsInWorld())
    {
        if (m_bot->GetGroup())
            m_bot->RemoveFromGroup();
        m_bot->GetPlayerbotMgr()->LogoutPlayerBot(m_bot->GetGUID());
        m_bot->GetPlayerbotMgr()->AddAllBots(sConfig.GetIntDefault( "PlayerbotAI.MaxBots", 100 ));
        return;
    }

    if ((GetMaster() != m_bot) && !m_bot->GetGroup())
    {
        m_bot->GetPlayerbotMgr()->LogoutPlayerBot(m_bot->GetGUID());
        m_bot->GetPlayerbotMgr()->AddAllBots(sConfig.GetIntDefault( "PlayerbotAI.MaxBots", 100 ));
        return;
    }

    if (GetMaster()->getLevel() != m_bot->getLevel())
    {
        ChatHandler ch(m_bot);
        m_bot->PurgeMyBags();
        m_bot->GiveLevel(GetMaster()->getLevel());
        ch.HandleGMStartUpCommand("");
        m_bot->SetHealth(m_bot->GetMaxHealth());
        m_bot->SetPower(m_bot->getPowerType(), m_bot->GetMaxPower(m_bot->getPowerType()));
        (GetClassAI())->InitSpells(m_bot->GetPlayerbotAI());
    }

    if (m_bot->IsBeingTeleported())
        return;

    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (ref->getSource()->IsBeingTeleported())
                return;
            ref = ref->next();
        }
    }

    MovementUpdate();

    if (!m_bot->isAlive())
    {
        if (m_botState != BOTSTATE_DEAD && m_botState != BOTSTATE_DEADRELEASED)
        {
            m_lootCreature.clear();
            m_lootCurrent = 0;
            m_bot->SetSelection(0);
            m_bot->GetMotionMaster()->Clear(true);
            SetState(BOTSTATE_DEAD);
            m_ignoreAIUpdatesUntilTime = time(0) + 30;
        }
        else if (m_botState == BOTSTATE_DEAD)
        {
            if (m_bot->GetCorpse())
            {
                SetState(BOTSTATE_DEADRELEASED);
                return;
            }
            m_bot->SetBotDeathTimer();
            m_bot->BuildPlayerRepop();
            WorldLocation loc;
            Corpse *corpse = m_bot->GetCorpse();
            corpse->GetPosition(loc);
            m_bot->TeleportTo(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, m_bot->GetOrientation());
            SetState(BOTSTATE_DEADRELEASED);
        }
        else if (m_botState == BOTSTATE_DEADRELEASED)
        {
            Corpse *corpse = m_bot->GetCorpse();
            if (!corpse)
                return;

            FollowCheckTeleport(corpse);

            if (corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() ==CORPSE_RESURRECTABLE_PVP) > time(0))
            {
                m_ignoreAIUpdatesUntilTime = corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() ==CORPSE_RESURRECTABLE_PVP);
                return;
            }

            m_ignoreAIUpdatesUntilTime = time(0) + 6;

            PlayerbotChatHandler ch(GetMaster());
            if (!ch.revive(*m_bot))
            {
                ch.sysmessage(".. ne peut pas ressuciter ..");
                return;
            }

            SetState(BOTSTATE_NORMAL);
        }
    }
    else
    {
        Spell* const pSpell = GetCurrentSpell();

        if (pSpell && !(pSpell->IsChannelActive() || pSpell->IsAutoRepeat()))
            InterruptCurrentCastingSpell();

        else if (IsInCombat())
        {
            if (m_bot->getAttackers().empty() && !m_bot->IsWithinDistInMap(GetMaster(), 100, true))
            {
                PlayerbotChatHandler ch(GetMaster());
                if (ch.teleport(*m_bot))
                    SetIgnoreTeleport(5);
            }
            DoNextCombatManeuver();
        }

        else if (m_botState == BOTSTATE_COMBAT)
        {
            SetState(BOTSTATE_LOOTING);
            m_attackerInfo.clear();
        }

        else if (m_botState == BOTSTATE_LOOTING)
            DoLoot();

        else if (!IsInCombat() && !IsMoving())
        {
            if (currentTime > m_ignoreSpellsAndItems)
            {
               (GetClassAI())->InitSpells(m_bot->GetPlayerbotAI());
                m_bot->AutoEquipItem();
                SetIgnoreUpdateSpellsAndItems();
            }
            MovementReset();
            SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());
            CheckMount();
        }

        else if (GetClassAI())
        {
            if (currentTime > m_ignoreSpellsAndItems)
            {
               (GetClassAI())->InitSpells(m_bot->GetPlayerbotAI());
                m_bot->AutoEquipItem();
                SetIgnoreUpdateSpellsAndItems();
            }
            if (!(GetClassAI())->DoNonCombatActions())
                SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());
            CheckMount();
        }
    }
}

Spell* PlayerbotAI::GetCurrentSpell() const
{
    if (m_CurrentlyCastingSpellId == 0)
        return NULL;

    Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(m_CurrentlyCastingSpellId);
    return pSpell;
}

void PlayerbotAI::TellMaster(const std::string& text) const
{
    SendWhisper(text, *GetMaster());
}

void PlayerbotAI::TellMaster(const char *fmt, ...) const
{
    char temp_buf[1024];
    va_list ap;
    va_start(ap, fmt);
    size_t temp_len = vsnprintf(temp_buf, 1024, fmt, ap);
    va_end(ap);
    std::string str = temp_buf;
    TellMaster(str);
}

void PlayerbotAI::SendWhisper(const std::string& text, Player& player) const
{
    WorldPacket data(SMSG_MESSAGECHAT, 200);
    m_bot->BuildPlayerChat(&data, CHAT_MSG_WHISPER, text, LANG_UNIVERSAL);
    player.GetSession()->SendPacket(&data);
}

bool PlayerbotAI::canObeyCommandFrom(const Player& player) const
{
    return player.GetSession()->GetAccountId() == GetMaster()->GetSession()->GetAccountId();
}

bool PlayerbotAI::CastSpell(const char* args)
{
    uint32 spellId = getSpellId(args);
    return (spellId) ? CastSpell(spellId): false;
}

bool PlayerbotAI::CastSpell(uint32 spellId, Unit& target)
{
    uint64 oldSel = m_bot->GetSelection();
    m_bot->SetSelection(target.GetGUID());
    bool rv = CastSpell(spellId);
    m_bot->SetSelection(oldSel);
    return rv;
}

bool PlayerbotAI::CastSpell(uint32 spellId)
{
    time_t currentTime = time(0);
    if (currentTime < m_bot->GetPlayerbotAI()->GetIgnoreSpell())
        return false;

    if (spellId == 0)
        return false;

    if (m_bot->HasSpellCooldown(spellId))
        return false;

    const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
    if (!pSpellInfo)
        return false;

    uint64 targetGUID = m_bot->GetSelection();
    Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, m_bot->GetSelection());

    if (!pTarget)
        return false;

    if (pTarget->GetTypeId() == TYPEID_PLAYER)
    {
        Player *plTarget = (Player*)pTarget;
        if (plTarget && plTarget->GetPlayerbotAI())
            if (currentTime < plTarget->GetPlayerbotAI()->GetIgnoreSpell())
                return false;
    }

    if (!m_bot->IsWithinDistInMap(pTarget, 50, true))
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

        if (!m_bot->isInFrontInMap(pTarget, 10))
        {
            m_bot->SetInFront(pTarget);
            MovementUpdate();
        }
    }

    if (HasAura(spellId, *pTarget))
        return false;

    MovementClear();

    m_bot->CastSpell(pTarget, pSpellInfo, false);

    Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(spellId);
    if (!pSpell)
        return false;

    m_CurrentlyCastingSpellId = spellId;
    m_ignoreAIUpdatesUntilTime = time(0) + (int32)((float)pSpell->GetCastTime() / 1000.0f) + 1;

    if (m_mgr->m_confDebugWhisper)
    {
        std::ostringstream out;
        out << "Je lance : " << pSpellInfo->SpellName[2];
        m_bot->GetPlayerbotAI()->TellMaster(out.str().c_str());
    }
    return true;
}

Item* PlayerbotAI::FindItem(uint32 ItemId)
{
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
             const ItemPrototype* const pItemProto = pItem->GetProto();
             if (!pItemProto)
                continue;

            if (pItemProto->ItemId == ItemId)
                return pItem;
        }
    }

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    if (pItemProto->ItemId == ItemId)
                        return pItem;
                }
            }
        }
    }

    return NULL;
}

bool PlayerbotAI::HasPick()
{
    QueryResult *result;

    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
    {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto)
                continue;

            result = WorldDatabase.PQuery("SELECT TotemCategory FROM item_template WHERE entry = '%i'", pItemProto->ItemId);
            if (result)
            {
                Field *fields = result->Fetch();
                uint32 tc = fields[0].GetUInt32();
                if (tc == 165 || tc == 167)
                    return true;
            }
        }
    }

    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto)
                continue;

            result = WorldDatabase.PQuery("SELECT TotemCategory FROM item_template WHERE entry = '%i'", pItemProto->ItemId);
            if (result)
            {
                Field *fields = result->Fetch();
                uint32 tc = fields[0].GetUInt32();
                if (tc == 165 || tc == 167)
                    return true;
            }
        }
    }

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = m_bot->GetItemByPos(bag, slot);
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    result = WorldDatabase.PQuery("SELECT TotemCategory FROM item_template WHERE entry = '%i'", pItemProto->ItemId);
                    if (result)
                    {
                        Field *fields = result->Fetch();
                        uint32 tc = fields[0].GetUInt32();
                        if (tc == 165 || tc == 167)
                            return true;
                    }
                }
            }
        }
    }
    std::ostringstream out;
    out << "|cffffffffI do not have a pick!";
    TellMaster(out.str().c_str());
    return false;
}

void PlayerbotAI::extractItemIds(const std::string& text, std::list<uint32> & itemIds) const
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

bool PlayerbotAI::extractGOinfo(const std::string& text, uint32 &guid, uint32 &entry, int &mapid, float &x, float &y, float &z) const
{
    uint8 pos = 0;

    int i = text.find("Hfound:", pos);
    if (i == -1)
        return false;

    pos = i + 7;
    int endPos = text.find(':', pos);
    if (endPos == -1)
        return false;
    std::string guidC = text.substr(pos, endPos - pos);
    guid = atol(guidC.c_str());

    pos = endPos + 1;
    endPos = text.find(':', pos);
    if (endPos == -1)
        return false;

    std::string entryC = text.substr(pos, endPos - pos);
    entry = atol(entryC.c_str());

    pos = endPos + 1;
    endPos = text.find(':', pos);
    if (endPos == -1)
        return false;

    std::string xC = text.substr(pos, endPos - pos);
    x = atof(xC.c_str());

    pos = endPos + 1;
    endPos = text.find(':', pos);
    if (endPos == -1)
        return false;

    std::string yC = text.substr(pos, endPos - pos);
    y = atof(yC.c_str());

    pos = endPos + 1;
    endPos = text.find(':', pos);
    if (endPos == -1)
        return false;

    std::string zC = text.substr(pos, endPos - pos);
    z = atof(zC.c_str());

    pos = endPos + 1;
    endPos = text.find(':', pos);
    if (endPos == -1)
        return false;

    std::string mapidC = text.substr(pos, endPos - pos);
    mapid = atoi(mapidC.c_str());
    pos = endPos;

    return true;
}

uint32 PlayerbotAI::extractMoney(const std::string& text) const
{
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

void PlayerbotAI::findItemsInEquip(std::list<uint32> & itemIdSearchList, std::list<Item*> & foundItemList) const
{
    for (uint8 slot =EQUIPMENT_SLOT_START; itemIdSearchList.size() > 0 && slot < EQUIPMENT_SLOT_END; slot++)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        for (std::list<uint32> ::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
        {
            if (pItem->GetProto()->ItemId != *it)
                continue;

            foundItemList.push_back(pItem);
            itemIdSearchList.erase(it);
            break;
        }
    }
}

void PlayerbotAI::findItemsInInv(std::list<uint32> & itemIdSearchList, std::list<Item*> & foundItemList) const
{
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; itemIdSearchList.size() > 0 && slot < INVENTORY_SLOT_ITEM_END; ++slot)
    {
        Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        for (std::list<uint32> ::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
        {
            if (pItem->GetProto()->ItemId != *it)
                continue;

            foundItemList.push_back(pItem);
            itemIdSearchList.erase(it);
            break;
        }
    }

    for (uint8 bag = INVENTORY_SLOT_BAG_START; itemIdSearchList.size() > 0 && bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (!pBag)
            continue;

        for (uint8 slot = 0; itemIdSearchList.size() > 0 && slot < pBag->GetBagSize(); ++slot)
        {
            Item* const pItem = m_bot->GetItemByPos(bag, slot);
            if (!pItem)
                continue;

            for (std::list<uint32> ::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
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

void PlayerbotAI::UseItem(Item& item)
{
    uint8 bagIndex = item.GetBagSlot();
    uint8 slot = item.GetSlot();
    uint8 cast_count = 1;
    uint32 spellid = 0;
    uint64 item_guid = item.GetGUID();
    uint32 glyphIndex = 0;
    uint8 unk_flags = 0;

    uint32 target = TARGET_FLAG_SELF;

    WorldPacket* const packet = new WorldPacket(CMSG_USE_ITEM, 1 + 1 + 1 + 4 + 8 + 4 + 1);
    *packet << bagIndex << slot << cast_count << spellid << item_guid
            << glyphIndex << unk_flags << target;
    m_bot->GetSession()->QueuePacket(packet);
}

void PlayerbotAI::EquipItem(Item& item)
{
    uint8 bagIndex = item.GetBagSlot();
    uint8 slot = item.GetSlot();

    WorldPacket* const packet = new WorldPacket(CMSG_AUTOEQUIP_ITEM, 2);
    *packet << bagIndex << slot;
    m_bot->GetSession()->QueuePacket(packet);
}

bool PlayerbotAI::TradeItem(const Item& item, int8 slot)
{
    if (!m_bot->GetTrader() || item.IsInTrade() || (!item.CanBeTraded() && slot != TRADE_SLOT_NONTRADED))
        return false;

    int8 tradeSlot = -1;

    TradeData* pTrade = m_bot->GetTradeData();
    if ((slot >= 0 && slot < TRADE_SLOT_COUNT) && pTrade->GetTraderData()->GetItem(TradeSlots(slot)) == NULL)
        tradeSlot = slot;
    else
    {
        for (uint8 i =0; i < TRADE_SLOT_TRADED_COUNT && tradeSlot ==-1; i++)
        {
            if (pTrade->GetTraderData()->GetItem(TradeSlots(i)) == NULL)
                tradeSlot = i;
        }
    }

    if (tradeSlot == -1)return false;

    WorldPacket* const packet = new WorldPacket(CMSG_SET_TRADE_ITEM, 3);
    *packet << (uint8)tradeSlot << (uint8)item.GetBagSlot()
           << (uint8)item.GetSlot();
    m_bot->GetSession()->QueuePacket(packet);
    return true;
}

bool PlayerbotAI::TradeCopper(uint32 copper)
{
    if (copper > 0)
    {
        WorldPacket* const packet = new WorldPacket(CMSG_SET_TRADE_GOLD, 4);
        *packet << copper;
        m_bot->GetSession()->QueuePacket(packet);
        return true;
    }
    return false;
}

bool PlayerbotAI::FollowCheckTeleport(WorldObject *obj)
{
    time_t currentTime = time(0);
    if (currentTime < m_ignoreTeleport)
        return false;

    if (!m_bot->IsWithinDistInMap(obj, 100, true) && GetMaster()->isAlive())
    {
        m_ignoreAIUpdatesUntilTime = time(0) + 6;
        PlayerbotChatHandler ch(GetMaster());
        if (!ch.teleport(*m_bot))
        {
            ch.sysmessage("Téléportation impossible...");
            return false;
        }
        else
            SetIgnoreTeleport(5);
    }
    return true;
}

void PlayerbotAI::HandleTeleportAck()
{
    m_ignoreAIUpdatesUntilTime = time(0) + 6;
    m_bot->GetMotionMaster()->Clear(true);
    if (m_bot->IsBeingTeleportedNear())
    {
        WorldPacket p = WorldPacket(MSG_MOVE_TELEPORT_ACK, 8 + 4 + 4);
        p.appendPackGUID(m_bot->GetGUID());
        p << (uint32)0;
        p << (uint32)time(0);
        m_bot->GetSession()->HandleMoveTeleportAck(p);
    }
    else if (m_bot->IsBeingTeleportedFar())
        m_bot->GetSession()->HandleMoveWorldportAckOpcode();
}

void PlayerbotAI::ItemLocalization(std::string& itemName, const uint32 itemID) const
{
    uint32 loc = GetMaster()->GetSession()->GetSessionDbLocaleIndex();
    std::wstring wnamepart;

    ItemLocale const *pItemInfo = sObjectMgr.GetItemLocale(itemID);
    if (pItemInfo)
    {
        if (pItemInfo->Name.size() > loc && !pItemInfo->Name[loc].empty())
        {
            const std::string name = pItemInfo->Name[loc];
            if (Utf8FitTo(name, wnamepart))
                itemName = name.c_str();
        }
    }
}

void PlayerbotAI::QuestLocalization(std::string& questTitle, const uint32 questID) const
{
    uint32 loc = GetMaster()->GetSession()->GetSessionDbLocaleIndex();
    std::wstring wnamepart;

    QuestLocale const *pQuestInfo = sObjectMgr.GetQuestLocale(questID);
    if (pQuestInfo)
    {
        if (pQuestInfo->Title.size() > loc && !pQuestInfo->Title[loc].empty())
        {
            const std::string title = pQuestInfo->Title[loc];
            if (Utf8FitTo(title, wnamepart))
                questTitle = title.c_str();
        }
    }
}

void PlayerbotAI::HandleCommand(const std::string& text, Player& fromPlayer)
{
    if (text.empty() ||
        text.find("X-Perl") != std::wstring::npos  ||
        text.find("HealBot") != std::wstring::npos  ||
        text.find("LOOT_OPENED") != std::wstring::npos  ||
        text.find("CTRA") != std::wstring::npos)
        return;

    if (!canObeyCommandFrom(fromPlayer))
    {
        std::string msg = "Je ne peux pas vous parler. Contactez plutot mon maitre ";
        msg += GetMaster()->GetName();
        SendWhisper(msg, fromPlayer);
        m_bot->HandleEmoteCommand(EMOTE_ONESHOT_NO);
    }

    else if (m_bot->GetTrader() && m_bot->GetTrader()->GetGUID() == fromPlayer.GetGUID())
    {
        uint32 copper = extractMoney(text);
        if (copper > 0)
            TradeCopper(copper);

        std::list<uint32> itemIds;
        extractItemIds(text, itemIds);
        if (itemIds.size() == 0)
            SendWhisper("Indiquez-moi dans le Tchat ce que vous voulez(MAJ + Clic sur l'objet).", fromPlayer);
        else if (!strncmp(text.c_str(), "nt ", 3))
        {
            if (itemIds.size() > 1)
                SendWhisper("Il n'y a qu'un seul emplacement 'Ne sera pas echange'. MAJ + Clic sur un seul objet, s'il vous plait !", fromPlayer);
            else
            {
                std::list<Item*> itemList;
                findItemsInEquip(itemIds, itemList);
                findItemsInInv(itemIds, itemList);
                if (itemList.size() > 0)
                    TradeItem((**itemList.begin()), TRADE_SLOT_NONTRADED);
                else
                    SendWhisper("Je n'ai pas cet objet dans mon inventaire ou dans mon equipement !", fromPlayer);
            }
        }
        else
        {
            std::list<Item*> itemList;
            findItemsInInv(itemIds, itemList);
            for (std::list<Item*> ::iterator it = itemList.begin(); it != itemList.end(); ++it)
                TradeItem(**it);
        }
    }
    else if (text == "reset")
    {
        SetState(BOTSTATE_NORMAL);
        MovementReset();
        SetQuestNeedItems();
        UpdateAttackerInfo();
        m_lootCreature.clear();
        m_lootCurrent = 0;
        m_targetCombat = 0;
    }
    else if (text == "report")
        SendQuestItemList(*GetMaster());
    else if (text == "orders")
        SendOrders(*GetMaster());
    else if (text == "follow" || text == "come")
        SetMovementOrder(MOVEMENT_FOLLOW, GetMaster());
    else if (text == "stay" || text == "stop")
        SetMovementOrder(MOVEMENT_STAY);
    else if (text == "attack")
    {
        uint64 attackOnGuid = fromPlayer.GetSelection();
        if (attackOnGuid)
        {
            Unit* thingToAttack = ObjectAccessor::GetUnit(*m_bot, attackOnGuid);
            if (!m_bot->IsFriendlyTo(thingToAttack) && m_bot->IsWithinLOSInMap(thingToAttack))
                GetCombatTarget(thingToAttack);
        }
        else
        {
            TellMaster("Pas de cible.");
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
        }
    }
    else if (text.size() > 2 && text.substr(0, 2) == "u " || text.size() > 4 && text.substr(0, 4) == "use ")
    {
        std::list<uint32> itemIds;
        std::list<Item*> itemList;
        extractItemIds(text, itemIds);
        findItemsInInv(itemIds, itemList);
        for (std::list<Item*> ::iterator it = itemList.begin(); it != itemList.end(); ++it)
            UseItem(**it);
    }
    else if (text.size() > 2 && text.substr(0, 2) == "e " || text.size() > 6 && text.substr(0, 6) == "equip ")
    {
        std::list<uint32> itemIds;
        std::list<Item*> itemList;
        extractItemIds(text, itemIds);
        findItemsInInv(itemIds, itemList);
        for (std::list<Item*> ::iterator it = itemList.begin(); it != itemList.end(); ++it)
            EquipItem(**it);
    }
    else if (text.size() > 2 && text.substr(0, 2) == "f " || text.size() > 5 && text.substr(0, 5) == "find ")
    {
        uint32 guid;
        float x, y, z;
        uint32 entry;
        int mapid;
        if (extractGOinfo(text, guid, entry, mapid, x, y, z))
        {
             m_bot->UpdateGroundPositionZ(x, y, z);
             SetMovementOrder(MOVEMENT_STAY);
             m_bot->GetMotionMaster()->MovePoint(mapid, x, y, z);
        }
        else
             SendWhisper("I have no info on that object", fromPlayer);
    }
    else if (text.size() > 2 && text.substr(0, 2) == "g " || text.size() > 4 && text.substr(0, 4) == "get ")
    {
        uint32 guid;
        float x, y, z;
        uint32 entry;
        int mapid;
        bool looted = false;
        if (extractGOinfo(text, guid, entry, mapid, x, y, z))
        {
            m_lootCurrent = MAKE_NEW_GUID(guid, entry, HIGHGUID_GAMEOBJECT);
            GameObject *go = m_bot->GetMap()->GetGameObject(m_lootCurrent);
            if (!go)
            {
                m_lootCurrent = 0;
                return;
            }

            if (!go->isSpawned())
                return;

            m_bot->UpdateGroundPositionZ(x, y, z);
            m_bot->GetMotionMaster()->MovePoint(mapid, x, y, z);
            m_bot->SetPosition(x, y, z, m_bot->GetOrientation());
            m_bot->SendLoot(m_lootCurrent, LOOT_CORPSE);
            Loot *loot = &go->loot;
            uint32 lootNum = loot->GetMaxSlotInLootFor(m_bot);
            if (lootNum == 0)
            {
                uint32 lockId = go->GetGOInfo()->GetLockId();
                LockEntry const *lockInfo = sLockStore.LookupEntry(lockId);
                if (lockInfo)
                {
                    for (int i = 0; i < 8; ++i)
                    {
                        uint32 skillId = SkillByLockType(LockType(lockInfo->Index[i]));
                        if (skillId > 0)
                        {
                            if (m_bot->HasSkill(skillId))
                            {
                                uint32 reqSkillValue = lockInfo->Skill[i];
                                uint32 SkillValue = m_bot->GetPureSkillValue(skillId);
                                if (SkillValue >= reqSkillValue)
                                {
                                    m_bot->UpdateGatherSkill(skillId, SkillValue, reqSkillValue);
                                    looted = true;
                                }
                            }
                            break;
                        }
                    }
                }
            }
            for (uint32 l =0; l < lootNum; l++)
            {
                QuestItem *qitem =0, *ffaitem =0, *conditem =0;
                LootItem *item = loot->LootItemInSlot(l, m_bot, &qitem, &ffaitem, &conditem);
                if (!item)
                    continue;

                if (!qitem && item->is_blocked)
                {
                    m_bot->SendLootRelease(m_lootCurrent);
                    continue;
                }

                if (m_needItemList[item->itemid] > 0)
                {
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
                                ffaitem->is_looted =true;
                                m_bot->SendNotifyLootItemRemoved(l);
                            }
                            else
                            {
                                if (conditem)
                                    conditem->is_looted =true;
                                loot->NotifyItemRemoved(l);
                            }
                        }
                        if (!item->freeforall)
                            item->is_looted = true;
                        --loot->unlootedCount;
                        m_bot->SendNewItem(newitem, uint32(item->count), false, false, true);
                        m_bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM, item->itemid, item->count);
                        looted = true;
                    }
                    continue;
                }

                uint32 lockId = go->GetGOInfo()->GetLockId();
                LockEntry const *lockInfo = sLockStore.LookupEntry(lockId);
                if (lockInfo)
                {
                    uint32 skillId = 0;
                    uint32 reqSkillValue = 0;
                    for (int i = 0; i < 8; ++i)
                    {
                        skillId = SkillByLockType(LockType(lockInfo->Index[i]));
                        if (skillId > 0)
                        {
                            reqSkillValue = lockInfo->Skill[i];
                            break;
                        }
                    }

                    if (m_bot->HasSkill(skillId) || skillId == SKILL_NONE)
                    {
                        if ((skillId == SKILL_MINING) && !HasPick())
                            continue;

                        ItemPosCountVec dest;
                        if (m_bot->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item->itemid, item->count) == EQUIP_ERR_OK)
                        {
                            Item* pItem = m_bot->StoreNewItem(dest, item->itemid, true, item->randomPropertyId);
                            uint32 SkillValue = m_bot->GetPureSkillValue(skillId);
                            if (SkillValue >= reqSkillValue)
                            {
                                m_bot->SendNewItem(pItem, uint32(item->count), false, false, true);
                                m_bot->UpdateGatherSkill(skillId, SkillValue, reqSkillValue);
                                --loot->unlootedCount;
                                looted = true;
                            }
                        }
                    }
                }
            }

            if (looted)
                m_bot->GetSession()->DoLootRelease(m_lootCurrent);
            else
                m_bot->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOOTING);
            SetQuestNeedItems();
        }
        else
            SendWhisper("I have no info on that object", fromPlayer);
    }
    else if (text == "quests")
    {
        bool hasIncompleteQuests = false;
        std::ostringstream incomout;
        incomout << "Mes quetes non finies:";
        bool hasCompleteQuests = false;
        std::ostringstream comout;
        comout << "Mes quetes finies:";
        for (uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
        {
            if (uint32 questId = m_bot->GetQuestSlotQuestId(slot))
            {
                  Quest const* pQuest = sObjectMgr.GetQuestTemplate(questId);

                  std::string questTitle = pQuest->GetTitle();
                  m_bot->GetPlayerbotAI()->QuestLocalization(questTitle, questId);

                  if (m_bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE)
                  {
                      hasCompleteQuests = true;
                      comout << " |cFFFFFF00|Hquest:" << questId << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
                  }
                  else
                  {
                      hasIncompleteQuests = true;
                      incomout << " |cFFFFFF00|Hquest:" << questId << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
                  }
             }
        }
        if (hasCompleteQuests)
            SendWhisper(comout.str(), fromPlayer);
        if (hasIncompleteQuests)
            SendWhisper(incomout.str(), fromPlayer);
        if (!hasCompleteQuests && !hasIncompleteQuests)
            SendWhisper("Je n'ai pas de quete !", fromPlayer);
    }
    else if (text.size() > 5 && text.substr(0, 5) == "drop ")
    {
        uint64 oldSelectionGUID = 0;
        if (fromPlayer.GetSelection() != m_bot->GetGUID())
        {
            oldSelectionGUID = m_bot->GetGUID();
            fromPlayer.SetSelection(m_bot->GetGUID());
        }
        PlayerbotChatHandler ch(GetMaster());
        if (!ch.dropQuest(text.substr(5).c_str()))
            ch.sysmessage("ERROR: could not drop quest");
        if (oldSelectionGUID)
            fromPlayer.SetSelection(oldSelectionGUID);
    }
    else if (text == "spells")
    {
        int loc = GetMaster()->GetSession()->GetSessionDbcLocale();

        std::ostringstream posOut;
        std::ostringstream negOut;

        const std::string ignoreList = ", Opening, Closing, Stuck, Remove Insignia, Opening - No Text, Grovel, Duel, Honorless Target, ";
        std::string alreadySeenList = ", ";

        for (PlayerSpellMap::iterator itr = m_bot->GetSpellMap().begin(); itr != m_bot->GetSpellMap().end(); ++itr)
        {
            const uint32 spellId = itr->first;

            if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
                continue;

            const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
            if (!pSpellInfo)
                continue;

            std::string comp = ", ";
            comp.append(pSpellInfo->SpellName[loc]);
            comp.append(", ");

            if (!(ignoreList.find(comp) == std::string::npos && alreadySeenList.find(comp) == std::string::npos))
                continue;

            alreadySeenList += pSpellInfo->SpellName[loc];
            alreadySeenList += ", ";

            if (IsPositiveSpell(spellId))
                posOut << " |cffffffff|Hspell:" << spellId << "|h["
                      << pSpellInfo->SpellName[loc] << "]|h|r";
            else
                negOut << " |cffffffff|Hspell:" << spellId << "|h["
                      << pSpellInfo->SpellName[loc] << "]|h|r";
        }

        ChatHandler ch(&fromPlayer);
        SendWhisper("Voici mes sorts non offensifs:", fromPlayer);
        ch.SendSysMessage(posOut.str().c_str());
        SendWhisper("et voici mes sorts offensifs:", fromPlayer);
        ch.SendSysMessage(negOut.str().c_str());
    }
    else if (text == "survey")
    {
        uint32 count = 0;
        std::ostringstream detectout;
        QueryResult *result;
        GameEventMgr::ActiveEvents const& activeEventsList = sGameEventMgr.GetActiveEventList();
        std::ostringstream eventFilter;
        eventFilter << " AND(event IS NULL ";
        bool initString = true;

        for (GameEventMgr::ActiveEvents::const_iterator itr = activeEventsList.begin(); itr != activeEventsList.end(); ++itr)
        {
            if (initString)
            {
                eventFilter << "OR event IN(" << *itr;
                initString =false;
            }
            else
                eventFilter << ", " << *itr;
        }

        if (!initString)
            eventFilter << "))";
        else
            eventFilter << ")";

        result = WorldDatabase.PQuery("SELECT gameobject.guid, id, position_x, position_y, position_z, map, "
            "(POW(position_x - %f, 2) + POW(position_y - %f, 2) + POW(position_z - %f, 2))AS order_ FROM gameobject "
            "LEFT OUTER JOIN game_event_gameobject on gameobject.guid =game_event_gameobject.guid WHERE map = '%i' %s ORDER BY order_ ASC LIMIT 10",
            m_bot->GetPositionX(), m_bot->GetPositionY(), m_bot->GetPositionZ(), m_bot->GetMapId(), eventFilter.str().c_str());

        if (result)
        {
            do
            {
                Field *fields = result->Fetch();
                uint32 guid = fields[0].GetUInt32();
                uint32 entry = fields[1].GetUInt32();
                float x = fields[2].GetFloat();
                float y = fields[3].GetFloat();
                float z = fields[4].GetFloat();
                int mapid = fields[5].GetUInt16();

                GameObject *go = m_bot->GetMap()->GetGameObject(MAKE_NEW_GUID(guid, entry, HIGHGUID_GAMEOBJECT));
                if (!go)
                    continue;

                if (!go->isSpawned())
                    continue;

                detectout << "|cFFFFFF00|Hfound:" << guid << ":" << entry << ":" << x << ":" << y << ":" << z << ":" << mapid << ":" << "|h[" << go->GetGOInfo()->name << "]|h|r";
                ++count;

            } while (result->NextRow());

            delete result;
        }
        SendWhisper(detectout.str().c_str(), fromPlayer);
    }
    else if (text == "stats")
    {
        std::ostringstream out;

        uint32 totalused = 0;
        for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
        {
            const Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            if (pItem)
                totalused++;
        }
        uint32 totalfree = 16 - totalused;
        for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
        {
            const Bag* const pBag = (Bag*)m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
            if (pBag)
            {
                ItemPrototype const* pBagProto = pBag->GetProto();
                if (pBagProto->Class == ITEM_CLASS_CONTAINER && pBagProto->SubClass == ITEM_SUBCLASS_CONTAINER)
                    totalfree = totalfree + pBag->GetFreeSlots();
            }
        }

        uint32 copper = m_bot->GetMoney();
        uint32 gold = uint32(copper / 10000);
        copper -= (gold * 10000);
        uint32 silver = uint32(copper / 100);
        copper -= (silver * 100);

        out << "|cffffffff[|h|cff00ffff" << m_bot->GetName() << "|h|cffffffff]" << " possède |r|cff00ff00" << gold
                << "|r|cfffffc00g|r|cff00ff00" << silver
                << "|r|cffcdcdcds|r|cff00ff00" << copper
                << "|r|cffffd333c" << "|h|cffffffff bag slots |h|cff00ff00" << totalfree;

        copper = EstRepairAll();
        gold = uint32(copper / 10000);
        copper -= (gold * 10000);
        silver = uint32(copper / 100);
        copper -= (silver * 100);

        out << "|h|cffffffff & item damage cost " << "|r|cff00ff00" << gold
                << "|r|cfffffc00g|r|cff00ff00" << silver
                << "|r|cffcdcdcds|r|cff00ff00" << copper
                << "|r|cffffd333c";
        ChatHandler ch(&fromPlayer);
        ch.SendSysMessage(out.str().c_str());
    }
    else
    {
        std::list<uint32> itemIds;
        extractItemIds(text, itemIds);
        if (!itemIds.empty())
        {
            uint32 itemId = itemIds.front();
            bool wasRewarded = false;
            uint64 questRewarderGUID = m_bot->GetSelection();
            Object* const pNpc = (WorldObject*)m_bot->GetObjectByTypeMask(questRewarderGUID, TYPEMASK_CREATURE_OR_GAMEOBJECT);
            if (!pNpc)
                return;

            QuestMenu& questMenu = m_bot->PlayerTalkClass->GetQuestMenu();
            for (uint32 iI = 0; !wasRewarded && iI < questMenu.MenuItemCount(); ++iI)
            {
                QuestMenuItem const& qItem = questMenu.GetItem(iI);

                uint32 questID = qItem.m_qId;
                Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);
                QuestStatus status = m_bot->GetQuestStatus(questID);

                if (status == QUEST_STATUS_COMPLETE &&
                    !m_bot->GetQuestRewardStatus(questID) &&
                    pQuest->GetRewChoiceItemsCount() > 1 &&
                    m_bot->CanRewardQuest(pQuest, false))
                {
                    for (uint8 rewardIdx =0; !wasRewarded && rewardIdx < pQuest->GetRewChoiceItemsCount(); ++rewardIdx)
                    {
                        ItemPrototype const * const pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[rewardIdx]);
                        if (itemId == pRewardItem->ItemId)
                        {
                            m_bot->RewardQuest(pQuest, rewardIdx, pNpc, false);

                            std::string questTitle = pQuest->GetTitle();
                            m_bot->GetPlayerbotAI()->QuestLocalization(questTitle, questID);
                            std::string itemName = pRewardItem->Name1;
                            m_bot->GetPlayerbotAI()->ItemLocalization(itemName, pRewardItem->ItemId);

                            std::ostringstream out;
                            out << "J'ai pris |cffffffff|Hitem:" << pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r comme récompense de quête.";
                            SendWhisper(out.str(), fromPlayer);
                            wasRewarded = true;
                        }
                    }
                }
            }
        }
        else
        {
            std::string msg = "Comment ? follow, stay, spells, (e)quip < itemlink > , (u)se < itemlink > , drop < questlink > , report, quests, stats(Pour obtenir un < itemlink > ou < questlink > faites MAJ + Clic sur l'objet ou sur la quete pour le linker dans le canal de tchat)";
            SendWhisper(msg, fromPlayer);
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
        }
    }
}
