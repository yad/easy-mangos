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

#include "PlayerbotPaladinAI.h"

class PlayerbotAI;

PlayerbotPaladinAI::PlayerbotPaladinAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

PlayerbotPaladinAI::~PlayerbotPaladinAI() {}

void PlayerbotPaladinAI::InitSpells(PlayerbotAI* const ai)
{
    RETRIBUTION_AURA              = ai->initSpell(RETRIBUTION_AURA_1);
    CRUSADER_AURA                 = ai->initSpell(CRUSADER_AURA_1);
    CRUSADER_STRIKE               = ai->initSpell(CRUSADER_STRIKE_1);
    SEAL_OF_COMMAND               = ai->initSpell(SEAL_OF_COMMAND_1);
    JUDGEMENT_OF_LIGHT            = ai->initSpell(JUDGEMENT_OF_LIGHT_1);
    JUDGEMENT_OF_WISDOM           = ai->initSpell(JUDGEMENT_OF_WISDOM_1);
    JUDGEMENT_OF_JUSTICE          = ai->initSpell(JUDGEMENT_OF_JUSTICE_1);
    DIVINE_STORM                  = ai->initSpell(DIVINE_STORM_1);
    BLESSING_OF_MIGHT             = ai->initSpell(BLESSING_OF_MIGHT_1);
    GREATER_BLESSING_OF_MIGHT     = ai->initSpell(GREATER_BLESSING_OF_MIGHT_1);
    HAMMER_OF_WRATH               = ai->initSpell(HAMMER_OF_WRATH_1);
    FLASH_OF_LIGHT                = ai->initSpell(FLASH_OF_LIGHT_1); // Holy
    HOLY_LIGHT                    = ai->initSpell(HOLY_LIGHT_1);
    HOLY_SHOCK                    = ai->initSpell(HOLY_SHOCK_1);
    HOLY_WRATH                    = ai->initSpell(HOLY_WRATH_1);
    DIVINE_FAVOR                  = ai->initSpell(DIVINE_FAVOR_1);
    CONCENTRATION_AURA            = ai->initSpell(CONCENTRATION_AURA_1);
    BLESSING_OF_WISDOM            = ai->initSpell(BLESSING_OF_WISDOM_1);
    GREATER_BLESSING_OF_WISDOM    = ai->initSpell(GREATER_BLESSING_OF_WISDOM_1);
    CONSECRATION                  = ai->initSpell(CONSECRATION_1);
    AVENGING_WRATH                = ai->initSpell(AVENGING_WRATH_1);
    LAY_ON_HANDS                  = ai->initSpell(LAY_ON_HANDS_1);
    EXORCISM                      = ai->initSpell(EXORCISM_1);
    SACRED_SHIELD                 = ai->initSpell(SACRED_SHIELD_1);
    DIVINE_PLEA                   = ai->initSpell(DIVINE_PLEA_1);
    BLESSING_OF_KINGS             = ai->initSpell(BLESSING_OF_KINGS_1); // Protection
    GREATER_BLESSING_OF_KINGS     = ai->initSpell(GREATER_BLESSING_OF_KINGS_1);
    GREATER_BLESSING_OF_SANCTUARY = ai->initSpell(GREATER_BLESSING_OF_SANCTUARY_1);
    HAMMER_OF_JUSTICE             = ai->initSpell(HAMMER_OF_JUSTICE_1);
    RIGHTEOUS_FURY                = ai->initSpell(RIGHTEOUS_FURY_1);
    RIGHTEOUS_DEFENSE             = ai->initSpell(RIGHTEOUS_DEFENSE_1);
    SHADOW_RESISTANCE_AURA        = ai->initSpell(SHADOW_RESISTANCE_AURA_1);
    DEVOTION_AURA                 = ai->initSpell(DEVOTION_AURA_1);
    FIRE_RESISTANCE_AURA          = ai->initSpell(FIRE_RESISTANCE_AURA_1);
    FROST_RESISTANCE_AURA         = ai->initSpell(FROST_RESISTANCE_AURA_1);
    HAND_OF_PROTECTION            = ai->initSpell(HAND_OF_PROTECTION_1);
    DIVINE_PROTECTION             = ai->initSpell(DIVINE_PROTECTION_1);
    DIVINE_INTERVENTION           = ai->initSpell(DIVINE_INTERVENTION_1);
    DIVINE_SACRIFICE              = ai->initSpell(DIVINE_SACRIFICE_1);
    DIVINE_SHIELD                 = ai->initSpell(DIVINE_SHIELD_1);
    HOLY_SHIELD                   = ai->initSpell(HOLY_SHIELD_1);
    AVENGERS_SHIELD               = ai->initSpell(AVENGERS_SHIELD_1);
    HAND_OF_SACRIFICE             = ai->initSpell(HAND_OF_SACRIFICE_1);
    SHIELD_OF_RIGHTEOUSNESS       = ai->initSpell(SHIELD_OF_RIGHTEOUSNESS_1);
    // Warrior auras
    DEFENSIVE_STANCE              = 71;   //Def Stance
    BERSERKER_STANCE              = 2458; //Ber Stance
    BATTLE_STANCE                 = 2457; //Bat Stance

    FORBEARANCE                   = 25771; // cannot be protected

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = ai->initSpell(GIFT_OF_THE_NAARU_PALADIN); // draenei
    STONEFORM                     = ai->initSpell(STONEFORM_ALL); // dwarf
    EVERY_MAN_FOR_HIMSELF         = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
}

bool PlayerbotPaladinAI::HealTarget(Unit &target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    if ((hp < 40)
        && (HOLY_LIGHT > 0)
        && (ai->GetManaPercent() >= 34)
        && (ai->CastSpell(HOLY_LIGHT, target)))
        return true;

    if ((hp < 35)
        && (HOLY_SHOCK > 0)
        && (ai->GetManaPercent() >= 21)
        && (ai->CastSpell(HOLY_SHOCK, target)))
        return true;

    if ((hp < 30)
        && (FLASH_OF_LIGHT > 0)
        && (ai->GetManaPercent() >= 8)
        && (ai->CastSpell(FLASH_OF_LIGHT, target)))
        return true;

    if ((hp < 25)
        && (LAY_ON_HANDS > 0)
        && (ai->GetHealthPercent() > 30)
        && (ai->GetManaPercent() >= 8)
        && (ai->CastSpell(LAY_ON_HANDS, target)))
        return true;

    return false;
}

bool PlayerbotPaladinAI::DoNextCombatManeuver(Unit *pTarget)
{
    Unit* pVictim = pTarget->getVictim();
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    ai->SetInFront(pTarget);
    Player *m_bot = GetPlayerBot();
    Group *m_group = m_bot->GetGroup();
    float dist = m_bot->GetDistance(pTarget);

    uint32 masterHP = pMaster->GetHealth() * 100 / pMaster->GetMaxHealth();

    if (pMaster->isAlive())
    {
        if ((masterHP < 25)
            && (HAND_OF_PROTECTION > 0)
            && (!pMaster->HasAura(FORBEARANCE, EFFECT_INDEX_0))
            && (!pMaster->HasAura(HAND_OF_PROTECTION, EFFECT_INDEX_0))
            && (!pMaster->HasAura(DIVINE_PROTECTION, EFFECT_INDEX_0))
            && (!pMaster->HasAura(DIVINE_SHIELD, EFFECT_INDEX_0))
            && (ai->CastSpell(HAND_OF_PROTECTION, *pMaster)))
            return true;
    }

    if (m_group
        && (pVictim != m_bot))
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!m_groupMember || !m_groupMember->isAlive())
                continue;

            uint32 memberHP = m_groupMember->GetHealth() * 100 / m_groupMember->GetMaxHealth();
            if ((memberHP < 40)
                && (ai->GetManaPercent() >= 40)
                && (HealTarget(*m_groupMember, memberHP)))
                return true;
        }
    }

    if ((RIGHTEOUS_FURY > 0)
        && (!m_bot->HasAura(RIGHTEOUS_FURY, EFFECT_INDEX_0))
        && (ai->CastSpell(RIGHTEOUS_FURY, *m_bot)))
        return true;

    if ((SHADOW_RESISTANCE_AURA > 0)
        && (!m_bot->HasAura(SHADOW_RESISTANCE_AURA, EFFECT_INDEX_0))
        && (pTarget->getClass() == CLASS_WARLOCK)
        && (ai->CastSpell(SHADOW_RESISTANCE_AURA, *m_bot)))
        return true;

    if ((DEVOTION_AURA > 0)
        && (!m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0))
        && (pTarget->getClass() == CLASS_WARRIOR)
        && (ai->CastSpell(DEVOTION_AURA, *m_bot)))
        return true;

    if ((FIRE_RESISTANCE_AURA > 0)
        && (!m_bot->HasAura(FIRE_RESISTANCE_AURA, EFFECT_INDEX_0))
        && (pTarget->getClass() == CLASS_MAGE)
        && (ai->CastSpell(FIRE_RESISTANCE_AURA, *m_bot)))
        return true;

    if ((RETRIBUTION_AURA > 0)
        && (!m_bot->HasAura(RETRIBUTION_AURA, EFFECT_INDEX_0))
        && (pTarget->getClass() == CLASS_PRIEST)
        && (ai->CastSpell(RETRIBUTION_AURA, *m_bot)))
        return true;

    if ((DEVOTION_AURA > 0)
        && (!m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0))
        && (pTarget->getClass() == CLASS_SHAMAN)
        && (ai->CastSpell(DEVOTION_AURA, *m_bot)))
        return true;

    if ((DEVOTION_AURA > 0)
        && (!m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0))
        && (pTarget->getClass() == CLASS_ROGUE)
        && (ai->CastSpell(DEVOTION_AURA, *m_bot)))
        return true;

    if ((DEVOTION_AURA > 0)
        && (!m_bot->HasAura(DEVOTION_AURA, EFFECT_INDEX_0))
        && (pTarget->getClass() == CLASS_PALADIN)
        && (ai->CastSpell(DEVOTION_AURA, *m_bot)))
        return true;

    if ((ai->GetHealthPercent() <= 40) || (pMaster->GetHealth() <= (pMaster->GetMaxHealth() * 0.4)))
        SpellSequence = Healing;
    else
        SpellSequence = Combat;

    switch (SpellSequence)
    {
        case Combat:

            if ((JUDGEMENT_OF_LIGHT > 0)
                && (!pTarget->HasAura(JUDGEMENT_OF_LIGHT, EFFECT_INDEX_0))
                && (CombatCounter < 1)
                && (ai->GetManaPercent() >= 5)
                && (ai->CastSpell(JUDGEMENT_OF_LIGHT, *pTarget)))
            {
                CombatCounter++;
                return true;
            }

            if ((SEAL_OF_COMMAND > 0)
                && (!m_bot->HasAura(SEAL_OF_COMMAND, EFFECT_INDEX_0))
                && (CombatCounter < 2)
                && (ai->GetManaPercent() >= 14)
                && (ai->CastSpell(SEAL_OF_COMMAND, *m_bot)))
            {
                CombatCounter++;
                return true;
            }

            if ((HAMMER_OF_JUSTICE > 0)
                && (!pTarget->HasAura(HAMMER_OF_JUSTICE, EFFECT_INDEX_0))
                && (CombatCounter < 3)
                && (ai->GetManaPercent() >= 3)
                && (ai->CastSpell(HAMMER_OF_JUSTICE, *pTarget)))
            {
                CombatCounter++;
                return true;
            }

            if ((CRUSADER_STRIKE > 0)
                && (CombatCounter < 4)
                && (ai->GetManaPercent() >= 5)
                && (ai->CastSpell(CRUSADER_STRIKE, *pTarget)))
            {
                CombatCounter++;
                return true;
            }

            if ((AVENGING_WRATH > 0)
                && (CombatCounter < 5)
                && (!m_bot->HasAura(AVENGING_WRATH, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 8)
                && (ai->CastSpell(AVENGING_WRATH, *m_bot)))
            {
                CombatCounter++;
                return true;
            }

            if ((SACRED_SHIELD > 0)
                && (CombatCounter < 6)
                && (pVictim == m_bot)
                && (ai->GetHealthPercent() < 70)
                && (!m_bot->HasAura(SACRED_SHIELD, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 12)
                && (ai->CastSpell(SACRED_SHIELD, *m_bot)))
            {
                CombatCounter++;
                return true;
            }

            if ((DIVINE_STORM > 0)
                && (CombatCounter < 7)
                && (ai->GetAttackerCount() >= 3)
                && (dist <= ATTACK_DISTANCE)
                && (ai->GetManaPercent() >= 12)
                && (ai->CastSpell(DIVINE_STORM, *pTarget)))
            {
                CombatCounter++;
                return true;
            }

            if ((HAMMER_OF_WRATH > 0)
                && (CombatCounter < 8)
                && (pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20)
                && (ai->GetManaPercent() >= 14)
                && (ai->CastSpell(HAMMER_OF_WRATH, *pTarget)))
            {
                CombatCounter++;
                return true;
            }

            if ((HOLY_WRATH > 0)
                && (CombatCounter < 9)
                && (ai->GetAttackerCount() >= 3)
                && (dist <= ATTACK_DISTANCE)
                && (ai->GetManaPercent() >= 24)
                && (ai->CastSpell(HOLY_WRATH, *pTarget)))
            {
                CombatCounter++;
                return true;
            }

            if ((HAND_OF_SACRIFICE > 0)
                && (pVictim == pMaster)
                && (!pMaster->HasAura(HAND_OF_SACRIFICE, EFFECT_INDEX_0))
                && (CombatCounter < 10)
                && (ai->GetManaPercent() >= 6)
                && (ai->CastSpell(HAND_OF_SACRIFICE, *pMaster)))
            {
                CombatCounter++;
                return true;
            }

            if ((DIVINE_PROTECTION > 0)
                && (pVictim == m_bot)
                && (!m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0))
                && (ai->GetHealthPercent() < 30)
                && (CombatCounter < 11)
                && (ai->GetManaPercent() >= 3)
                && (ai->CastSpell(DIVINE_PROTECTION, *m_bot)))
            {
                CombatCounter++;
                return true;
            }

            if ((RIGHTEOUS_DEFENSE > 0)
                && (pVictim != m_bot)
                && (ai->GetHealthPercent() > 70)
                && (CombatCounter < 12)
                && (ai->CastSpell(RIGHTEOUS_DEFENSE, *pTarget)))
            {
                CombatCounter++;
                return true;
            }

            if ((DIVINE_PLEA > 0)
                && (!m_bot->HasAura(DIVINE_PLEA, EFFECT_INDEX_0))
                && (ai->GetManaPercent() < 50)
                && (CombatCounter < 13)
                && (ai->CastSpell(DIVINE_PLEA, *m_bot)))
            {
                CombatCounter++;
                return true;
            }

            if ((DIVINE_FAVOR > 0)
                && (!m_bot->HasAura(DIVINE_FAVOR, EFFECT_INDEX_0))
                && (CombatCounter < 14)
                && (ai->CastSpell(DIVINE_FAVOR, *m_bot)))
            {
                CombatCounter++;
                return true;
            }

            CombatCounter = 0;
            break;

        case Healing:

            if ((ai->GetHealthPercent() <= 40)
                && (HealTarget(*m_bot, ai->GetHealthPercent())))
                return true;

            if ((masterHP <= 40)
                && (HealTarget(*pMaster, masterHP)))
                return true;

            CombatCounter = 0;
            break;
    }

    if ((AVENGING_WRATH > 0)
        && (!m_bot->HasAura(AVENGING_WRATH, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 8)
        && (ai->CastSpell(AVENGING_WRATH, *m_bot)))
        return true;

    if ((DIVINE_SHIELD > 0)
        && (ai->GetHealthPercent() < 30)
        && (pVictim == m_bot)
        && (!m_bot->HasAura(FORBEARANCE, EFFECT_INDEX_0))
        && (!m_bot->HasAura(DIVINE_SHIELD, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 3)
        && (ai->CastSpell(DIVINE_SHIELD, *m_bot)))
        return true;

    if ((DIVINE_SACRIFICE > 0)
        && (ai->GetHealthPercent() > 50)
        && (pVictim != m_bot)
        && (!m_bot->HasAura(DIVINE_SACRIFICE, EFFECT_INDEX_0))
        && (ai->CastSpell(DIVINE_SACRIFICE, *m_bot)))
        return true;

    return false;
}

bool PlayerbotPaladinAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return false;

    if ((GREATER_BLESSING_OF_MIGHT > 0)
        && (!m_bot->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (ai->CastSpell(GREATER_BLESSING_OF_MIGHT, *m_bot)))
        return true;

    if ((BLESSING_OF_MIGHT > 0)
        && (!m_bot->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (!m_bot->HasAura(BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (ai->CastSpell(BLESSING_OF_MIGHT, *m_bot)))
        return true;

    if ((DIVINE_FAVOR > 0)
        && (!m_bot->HasAura(DIVINE_FAVOR, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 3)
        && (ai->CastSpell(DIVINE_FAVOR , *m_bot)))
        return true;

    if ((BLESSING_OF_WISDOM > 0)
        && (pMaster->getClass() == CLASS_PRIEST)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (ai->CastSpell(BLESSING_OF_WISDOM, * (pMaster))))
        return true;

    if ((GREATER_BLESSING_OF_WISDOM > 0)
        && (pMaster->getClass() == CLASS_PRIEST)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (ai->CastSpell(GREATER_BLESSING_OF_WISDOM, * (pMaster))))
        return true;

    if ((BLESSING_OF_WISDOM > 0)
        && (pMaster->getClass() == CLASS_MAGE)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (ai->CastSpell(BLESSING_OF_WISDOM, * (pMaster))))
        return true;

    if ((GREATER_BLESSING_OF_WISDOM > 0)
        && (pMaster->getClass() == CLASS_MAGE)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (ai->CastSpell(GREATER_BLESSING_OF_WISDOM, * (pMaster))))
        return true;

    if ((BLESSING_OF_WISDOM > 0)
        && (pMaster->getClass() == CLASS_PALADIN)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (ai->CastSpell(BLESSING_OF_WISDOM, * (pMaster))))
        return true;

    if ((GREATER_BLESSING_OF_WISDOM > 0)
        && (pMaster->getClass() == CLASS_PALADIN)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (ai->CastSpell(GREATER_BLESSING_OF_WISDOM, * (pMaster))))
        return true;

    if ((BLESSING_OF_WISDOM > 0)
        && (pMaster->getClass() == CLASS_WARLOCK)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (ai->CastSpell(BLESSING_OF_WISDOM, * (pMaster))))
        return true;

    if ((GREATER_BLESSING_OF_WISDOM > 0)
        && (pMaster->getClass() == CLASS_WARLOCK)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_WISDOM, EFFECT_INDEX_0))
        && (ai->CastSpell(GREATER_BLESSING_OF_WISDOM, * (pMaster))))
        return true;

    if ((BLESSING_OF_MIGHT > 0)
        && (pMaster->getClass() == CLASS_WARRIOR)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (!pMaster->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
        && (ai->CastSpell(BLESSING_OF_MIGHT, * (pMaster))))
        return true;

    if ((GREATER_BLESSING_OF_MIGHT > 0)
        && (pMaster->getClass() == CLASS_WARRIOR)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (!pMaster->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
        && (ai->CastSpell(GREATER_BLESSING_OF_MIGHT, * (pMaster))))
        return true;

    if ((BLESSING_OF_KINGS > 0)
        && (pMaster->getClass() == CLASS_WARRIOR)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_KINGS, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BLESSING_OF_KINGS, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
        && (ai->CastSpell(BLESSING_OF_KINGS, * (pMaster))))
        return true;

    if ((GREATER_BLESSING_OF_KINGS > 0)
        && (pMaster->getClass() == CLASS_WARRIOR)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_KINGS, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
        && (ai->CastSpell(GREATER_BLESSING_OF_KINGS, * (pMaster))))
        return true;

    if ((BLESSING_OF_MIGHT > 0)
        && (pMaster->getClass() == CLASS_ROGUE)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (ai->CastSpell(BLESSING_OF_MIGHT, * (pMaster))))
        return true;

    if ((GREATER_BLESSING_OF_MIGHT > 0)
        && (pMaster->getClass() == CLASS_ROGUE)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (ai->CastSpell(GREATER_BLESSING_OF_MIGHT, * (pMaster))))
        return true;

    if ((BLESSING_OF_MIGHT > 0)
        && (pMaster->getClass() == CLASS_SHAMAN)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (!pMaster->HasAura(BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (ai->CastSpell(BLESSING_OF_MIGHT, * (pMaster))))
        return true;

    if ((GREATER_BLESSING_OF_MIGHT > 0)
        && (pMaster->getClass() == CLASS_SHAMAN)
        && (!pMaster->HasAura(GREATER_BLESSING_OF_MIGHT, EFFECT_INDEX_0))
        && (ai->CastSpell(GREATER_BLESSING_OF_MIGHT, * (pMaster))))
        return true;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* drinkOrFoodItem = ai->FindDrink();
    Item* bandageItem = ai->FindBandage();

    if (drinkOrFoodItem
        && (ai->GetManaPercent() < 40))
    {
        ai->UseItem(*drinkOrFoodItem);
        ai->SetIgnoreUpdateTime(30);
        return true;
    }

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    drinkOrFoodItem = ai->FindFood();

    if (drinkOrFoodItem
        && ai->GetHealthPercent() < 40)
    {
        ai->UseItem(*drinkOrFoodItem);
        ai->SetIgnoreUpdateTime(30);
        return true;
    }

    if (!drinkOrFoodItem
        && bandageItem
        && !m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0)
        && ai->GetHealthPercent() < 70)
    {
        ai->UseItem(*bandageItem);
        ai->SetIgnoreUpdateTime(8);
        return true;
    }

    if (pMaster->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = pMaster->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(uint64(itr->guid));
            if (!tPlayer || !tPlayer->isAlive())
                continue;

            if (HealTarget(*tPlayer, tPlayer->GetHealth() * 100 / tPlayer->GetMaxHealth()))
                return true;
        }
    }

    return false;
}
