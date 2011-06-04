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

uint32 PlayerbotAI::getSpellId(const char* args, bool master) const
{
    if (!*args)
        return 0;

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return 0;

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
        if (sSpellStore.LookupEntry(spellId)->SpellIconID != sSpellStore.LookupEntry(itr->second)->SpellIconID)
            continue;

        SpellChainNode const* node = sSpellMgr.GetSpellChainNode(itr->second);
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
        Spell *spell = new Spell(m_bot, pSpellInfo, false);
        SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(pSpellInfo->rangeIndex);
        float range = GetSpellMaxRange(srange, IsPositiveSpell(spellId));
        m_bot->ApplySpellMod(spellId, SPELLMOD_RANGE, range, spell);
        m_spellRangeMap.insert(std::pair<uint32,float>(spellId, range));
        delete spell;
    }
    return (next == 0) ? spellId : next;
}

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
    return 0;
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

Item* PlayerbotAI::FindMount(uint32 matchingRidingSkill) const
{
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
    if (!GetLeader()->IsInWorld() || !m_bot->IsInWorld())
        return;

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
        UnMount();
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
            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                if (pItemProto->Spells[0].SpellCategory == 11)
                    return pItem;
        }
    }

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
                        if (pItemProto->Spells[0].SpellCategory == 11)
                            return pItem;
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

            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_FOOD)
                if (pItemProto->Spells[0].SpellCategory == 59)
                    return pItem;
        }
    }

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
                        if (pItemProto->Spells[0].SpellCategory == 59)
                            return pItem;
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

            if (!pItemProto || m_bot->CanUseItem(pItemProto) != EQUIP_ERR_OK)
                continue;

            if (pItemProto->Class == ITEM_CLASS_CONSUMABLE && pItemProto->SubClass == ITEM_SUBCLASS_BANDAGE)
                return pItem;
        }
    }

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

Item* PlayerbotAI::FindPoison() const
{
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

    if (currentTime > m_TimeDoneEating && currentTime > m_TimeDoneDrinking)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);
}

void PlayerbotAI::SetQuestNeedItems()
{
    m_needItemList.clear();
    m_lootCreature.clear();
    m_lootCurrent = 0;

    for (QuestStatusMap::iterator iter = m_bot->getQuestStatusMap().begin(); iter != m_bot->getQuestStatusMap().end(); ++iter)
    {
        const Quest *qInfo = sObjectMgr.GetQuestTemplate(iter->first);
        if (!qInfo)
            continue;

        QuestStatusData *qData = &iter->second;
        if (qData->m_status != QUEST_STATUS_INCOMPLETE)
            continue;

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
        m_botState = BOTSTATE_NORMAL;
        SetQuestNeedItems();
        return;
    }

    if (!m_lootCurrent)
    {
        m_lootCurrent = m_lootCreature.front();
        m_lootCreature.pop_front();
        if (!m_lootCurrent || !m_lootCurrent->IsInWorld() || !m_lootCurrent->IsInMap(m_bot) || m_lootCurrent->getDeathState() != CORPSE
            || GetLeader()->GetDistance(m_lootCurrent) > BOTLOOT_DISTANCE)
        {
            m_lootCurrent = NULL;
            return;
        }
        SetFollowTarget(GetLeader());
    }
    else
    {
        if (!m_lootCurrent || !m_lootCurrent->IsInWorld() || !m_lootCurrent->IsInMap(m_bot) || m_lootCurrent->getDeathState() != CORPSE
            || GetLeader()->GetDistance(m_lootCurrent) > BOTLOOT_DISTANCE)
        {
            m_lootCurrent = NULL;
            return;
        }
        if (m_bot->IsWithinDistInMap(GetLeader(), 100.0f))
        {
            m_bot->SendLoot(m_lootCurrent->GetObjectGuid(), LOOT_CORPSE);
            Loot *loot = &m_lootCurrent->loot;
            uint32 lootNum = loot->GetMaxSlotInLootFor(m_bot);
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
            m_bot->GetSession()->DoLootRelease(m_lootCurrent->GetObjectGuid());
            SetFollowTarget(GetLeader());
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
        m_bot->ClearDividerGuid();
        return;
    }

    if (!m_bot->CanTakeQuest(qInfo, false))
    {
        m_bot->ClearDividerGuid();
        return;
    }

    if (!m_bot->GetDividerGuid().IsEmpty())
    {
        pGiver->SendPushToPartyResponse(m_bot, QUEST_PARTY_MSG_ACCEPT_QUEST);
        m_bot->ClearDividerGuid();
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
    ObjectGuid giverGUID = questgiver->GetObjectGuid();

    if (m_bot->IsInMap(questgiver))
    {
        m_bot->SetSelectionGuid(giverGUID);
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

            if (status == QUEST_STATUS_COMPLETE)
            {
                if (!m_bot->GetQuestRewardStatus(questID))
                {
                    if (pQuest->GetRewChoiceItemsCount() == 0)
                    {
                        if (m_bot->CanRewardQuest(pQuest, false))
                        {
                            m_bot->RewardQuest(pQuest, 0, questgiver, false);
                        }
                    }

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

uint32 PlayerbotAI::EstRepairAll()
{
    uint32 TotalCost = 0;
    for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        TotalCost += EstRepair(((INVENTORY_SLOT_BAG_0 << 8) | i));

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
            return TotalCost;

        uint32 dQualitymodEntryId = (ditemProto->Quality + 1) * 2;
        DurabilityQualityEntry const *dQualitymodEntry = sDurabilityQualityStore.LookupEntry(dQualitymodEntryId);
        if (!dQualitymodEntry)
            return TotalCost;

        uint32 dmultiplier = dcost->multiplier[ItemSubClassToDurabilityMultiplierId(ditemProto->Class, ditemProto->SubClass)];
        uint32 costs = uint32(LostDurability * dmultiplier * double(dQualitymodEntry->quality_mod));

        if (costs == 0)
            costs = 1;

        TotalCost = costs;
    }
    return TotalCost;
}

bool PlayerbotAI::CheckTeleport()
{
    if (!m_bot->GetMap() || !m_bot->GetTerrain())
        return false;

    if (m_bot->IsBeingTeleported())
    {
        m_bot->GetMotionMaster()->Clear(true);
        return false;
    }

    //Don't remove this check !
    if (GetLeader() && m_bot!=GetLeader())
    {
        if (!GetLeader()->GetMap() || !GetLeader()->GetTerrain())
        {
            m_bot->GetMotionMaster()->Clear(true);
            return false;
        }

        if (!m_bot->IsInMap(GetLeader()))
        {
            m_bot->GetMotionMaster()->Clear(true);
            this->SetFollowTarget(GetLeader(), true);
            return false;
        }

        if (GetLeader()->IsBeingTeleported())
        {
            m_bot->GetMotionMaster()->Clear(true);
            return false;
        }
    }

    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (!ref->getSource()->GetMap() || !ref->getSource()->GetTerrain())
            {
                m_bot->GetMotionMaster()->Clear(true);
                return false;
            }

            if (ref->getSource()->IsBeingTeleported())
            {
                m_bot->GetMotionMaster()->Clear(true);
                return false;
            }
            ref = ref->next();
        }
    }
    return true;
}

bool PlayerbotAI::CheckLeader()
{
    if (!GetLeader() || (!GetLeader()->IsInWorld() && !GetLeader()->IsBeingTeleported()))
    {
        SetLeader(m_bot);
        ReinitAI();
        return false;
    }
    return true;
}

bool PlayerbotAI::CheckGroup()
{
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
    return true;
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
    if (spellId <= 0)
        return false;

    if (m_bot->HasSpellCooldown(spellId))
        return false;

    const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
    if (!pSpellInfo)
        return false;

    ObjectGuid targetGUID = m_bot->GetSelectionGuid();
    Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, targetGUID);

    if (!pTarget)
        pTarget = m_bot;

    SpellRanges::iterator it = m_spellRangeMap.find(spellId);
    if (it != m_spellRangeMap.end() && (int)it->second != 0)
    {
        float dist = m_bot->GetCombatDistance(pTarget);
        if (dist > it->second + 1.25)
            return false;
    }

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

    /*SpellCastTimesEntry const * castTimeEntry = sSpellCastTimesStore.LookupEntry(pSpellInfo->CastingTimeIndex);
    if (castTimeEntry && castTimeEntry->CastTime)
        m_bot->GetMotionMaster()->Clear(true);*/

    m_bot->CastSpell(pTarget, pSpellInfo, false);

    Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(spellId);
    if (!pSpell)
        return false;

    m_CurrentlyCastingSpellId = spellId;
    m_ignoreAIUpdatesUntilTime = time(0) + (int32) ((float) pSpell->GetCastTime() / 1000.0f) + 1;

    return true;
}

bool PlayerbotAI::CastPetAura(uint32 spellId, Unit* target)
{
    if (HasAura(spellId, target))
        return true;

    return CastPetSpell(spellId, target);
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

        if (!pet->isInFrontInMap(pTarget, 10))
            pet->SetInFront(pTarget);
    }

    pet->CastSpell(pTarget, pSpellInfo, false);

    Spell* const pSpell = pet->FindCurrentSpellBySpellId(spellId);
    if (!pSpell)
        return false;

    return true;
}

bool PlayerbotAI::Buff(uint32 spellId, Unit* target, void (*beforeCast)(Player *))
{
    if (spellId == 0)
        return false;

    SpellEntry const * spellProto = sSpellStore.LookupEntry(spellId);

    if (!spellProto)
        return false;

    if (!target)
        return false;

    spellProto = sSpellMgr.SelectAuraRankForLevel(spellProto, target->getLevel());
    if (!spellProto)
        return false;

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

    if (beforeCast)
        (*beforeCast)(m_bot);

    return CastSpell(spellProto->Id, target);
}

bool PlayerbotAI::SelfBuff(uint32 spellId)
{
    if (spellId == 0)
        return false;

    if (m_bot->HasAura(spellId))
        return false;

    return CastSpell(spellId, m_bot);
}

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
        const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
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
    uint8 pos = 0;

    int i = text.find("Hspell:", pos);
    if (i == -1)
        return false;

    pos = i + 7;
    int endPos = text.find('|', pos);
    if (endPos == -1)
        return false;

    std::string idC = text.substr(pos, endPos - pos);
    spellId = atol(idC.c_str());
    pos = endPos;
    return true;
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

void PlayerbotAI::findItemsInInv(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const
{
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

void PlayerbotAI::UseItem(Item *item)
{
    UseItem(item, TARGET_FLAG_SELF, ObjectGuid());
}

void PlayerbotAI::UseItem(Item *item, uint8 targetInventorySlot)
{
    if (targetInventorySlot >= EQUIPMENT_SLOT_END)
        return;

    Item* const targetItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, targetInventorySlot);
    if (!targetItem)
        return;

    UseItem(item, TARGET_FLAG_ITEM, targetItem->GetObjectGuid());
}

void PlayerbotAI::UseItem(Item *item, Unit *target)
{
    if (!target)
        return;

    UseItem(item, TARGET_FLAG_UNIT, target->GetObjectGuid());
}

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

void PlayerbotAI::EquipItem(Item& item)
{
    uint8 bagIndex = item.GetBagSlot();
    uint8 slot = item.GetSlot();

    WorldPacket* const packet = new WorldPacket(CMSG_AUTOEQUIP_ITEM, 2);
    *packet << bagIndex << slot;
    m_bot->GetSession()->QueuePacket(packet);
}


void PlayerbotAI::HandleTeleportAck()
{
    m_bot->GetMotionMaster()->Clear(true);
    if (m_bot->IsBeingTeleportedNear())
    {
        WorldPacket p = WorldPacket(MSG_MOVE_TELEPORT_ACK, 8 + 4 + 4);
        p.appendPackGUID(m_bot->GetObjectGuid());
        p << (uint32) 0;
        p << (uint32) time(0);
        m_bot->GetSession()->HandleMoveTeleportAckOpcode(p);
    }
    else if (m_bot->IsBeingTeleportedFar())
        m_bot->GetSession()->HandleMoveWorldportAckOpcode();
}

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

        ChangeCombatTarget(m_targetCombat);
        SetInFront(m_targetCombat);
        SetFollowTarget(m_targetCombat);
        m_bot->Attack(m_targetCombat, true);
        GetClassAI()->DoCombatManeuver(m_targetCombat, true);
    }
}
