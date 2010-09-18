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
#include "PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotPaladinAI::PlayerbotPaladinAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

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

PlayerbotPaladinAI::~PlayerbotPaladinAI() {}

bool PlayerbotPaladinAI::HealTarget(Unit *target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return false;

    if (hp < 40 && ai->GetManaPercent() >= 34 && ai->CastSpell(HOLY_LIGHT, target))
        return true;
    else if (hp < 35 && ai->GetManaPercent() >= 21 && ai->CastSpell(HOLY_SHOCK, target))
        return true;
    else if (hp < 30 && ai->GetManaPercent() >= 8 && ai->CastSpell(FLASH_OF_LIGHT, target))
        return true;
    else if (hp < 25 && ai->GetHealthPercent() > 30 && ai->GetManaPercent() >= 8 && ai->CastSpell(LAY_ON_HANDS, target))
        return true;

    return false;
}

void PlayerbotPaladinAI::DoNextCombatManeuver(Unit* pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Player* m_master = ai->GetMaster();
    if (!m_master)
        return;

    Unit* pVictim = pTarget->getVictim();

    /*switch (ai->GetScenarioType())
    {
        //SCENARIO_PVEEASY,
        //SCENARIO_PVEHARD,
        //SCENARIO_DUEL,
        //SCENARIO_PVPEASY,
        //SCENARIO_PVPHARD
        //return;
    }*/

    // damage spells
    ai->SetInFront(pTarget);
    Group *m_group = m_bot->GetGroup();
    float dist = m_bot->GetDistance(pTarget);

    //Shield master if low hp.
    uint32 masterHP = m_master->GetHealth() * 100 / m_master->GetMaxHealth();

    if (m_master->isAlive() && masterHP < 25 && !ai->HasAura(FORBEARANCE, m_master) && !ai->HasAura(HAND_OF_PROTECTION, m_master) && !ai->HasAura(DIVINE_PROTECTION, m_master) && !ai->HasAura(DIVINE_SHIELD, m_master) && ai->CastSpell(HAND_OF_PROTECTION, m_master))
        return;

    // heal group inside combat, but do not heal if tank
    if (m_group && pVictim != m_bot)  // possible tank
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!m_groupMember || !m_groupMember->isAlive())
                continue;

            uint32 memberHP = m_groupMember->GetHealth() * 100 / m_groupMember->GetMaxHealth();
            // do not heal bots without plenty of mana for master & self
            if (memberHP < 40 && ai->GetManaPercent() >= 40 && HealTarget(m_groupMember, memberHP))
                return;
        }
    }

    if (!ai->HasAura(RIGHTEOUS_FURY, m_bot) && ai->CastSpell(RIGHTEOUS_FURY, m_bot))
        return;
    else if (!ai->HasAura(SHADOW_RESISTANCE_AURA, m_bot) && pTarget->getClass() == CLASS_WARLOCK && ai->CastSpell(SHADOW_RESISTANCE_AURA, m_bot))
        return;
    else if (!ai->HasAura(DEVOTION_AURA, m_bot) && pTarget->getClass() == CLASS_WARRIOR && ai->CastSpell(DEVOTION_AURA, m_bot))
        return;
    else if (!ai->HasAura(FIRE_RESISTANCE_AURA, m_bot) && pTarget->getClass() == CLASS_MAGE && ai->CastSpell(FIRE_RESISTANCE_AURA, m_bot))
        return;
    else if (!ai->HasAura(RETRIBUTION_AURA, m_bot) && pTarget->getClass() == CLASS_PRIEST && ai->CastSpell(RETRIBUTION_AURA, m_bot))
        return;
    else if (!ai->HasAura(DEVOTION_AURA, m_bot) && pTarget->getClass() == CLASS_SHAMAN && ai->CastSpell(DEVOTION_AURA, m_bot))
        return;
    else if (!ai->HasAura(DEVOTION_AURA, m_bot) && pTarget->getClass() == CLASS_ROGUE && ai->CastSpell(DEVOTION_AURA, m_bot))
        return;
    else if (!ai->HasAura(DEVOTION_AURA, m_bot) && pTarget->getClass() == CLASS_PALADIN && ai->CastSpell(DEVOTION_AURA, m_bot))
        return;

    if (ai->GetHealthPercent() <= 40 || m_master->GetHealth() <= m_master->GetMaxHealth() * 0.4)
        SpellSequence = Healing;
    else
        SpellSequence = Combat;

    switch (SpellSequence)
    {
        case Combat:
            if (!ai->HasAura(JUDGEMENT_OF_LIGHT, pTarget) && CombatCounter < 1 && ai->GetManaPercent() >= 5 && ai->CastSpell(JUDGEMENT_OF_LIGHT, pTarget))
                ++CombatCounter;
            else if (!ai->HasAura(SEAL_OF_COMMAND, m_bot) && CombatCounter < 2 && ai->GetManaPercent() >= 14 && ai->CastSpell(SEAL_OF_COMMAND, m_bot))
                ++CombatCounter;
            else if (!ai->HasAura(HAMMER_OF_JUSTICE, pTarget) && CombatCounter < 3 && ai->GetManaPercent() >= 3 && ai->CastSpell(HAMMER_OF_JUSTICE, pTarget))
                ++CombatCounter;
            else if (CombatCounter < 4 && ai->GetManaPercent() >= 5 && ai->CastSpell(CRUSADER_STRIKE, pTarget))
                ++CombatCounter;
            else if (CombatCounter < 5 && !ai->HasAura(AVENGING_WRATH, m_bot) && ai->GetManaPercent() >= 8 && ai->CastSpell(AVENGING_WRATH, m_bot))
                ++CombatCounter;
            else if (CombatCounter < 6 && pVictim == m_bot && ai->GetHealthPercent() < 70 && !ai->HasAura(SACRED_SHIELD, m_bot) && ai->GetManaPercent() >= 12 && ai->CastSpell(SACRED_SHIELD, m_bot))
                ++CombatCounter;
            else if (CombatCounter < 7 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 12 && ai->CastSpell(DIVINE_STORM, pTarget))
                ++CombatCounter;
            else if (CombatCounter < 8 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.20 && ai->GetManaPercent() >= 14 && ai->CastSpell(HAMMER_OF_WRATH, pTarget))
                ++CombatCounter;
            else if (CombatCounter < 9 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 24 && ai->CastSpell(HOLY_WRATH, pTarget))
                ++CombatCounter;
            else if (pVictim == m_master && !ai->HasAura(HAND_OF_SACRIFICE, m_master) && CombatCounter < 10 && ai->GetManaPercent() >= 6 && ai->CastSpell(HAND_OF_SACRIFICE, m_master))
                ++CombatCounter;
            else if (pVictim == m_bot && !ai->HasAura(FORBEARANCE, m_bot) && ai->GetHealthPercent() < 30 && CombatCounter < 11 && ai->GetManaPercent() >= 3 && ai->CastSpell(DIVINE_PROTECTION, m_bot))
                ++CombatCounter;
            else if (pVictim != m_bot && ai->GetHealthPercent() > 70 && CombatCounter < 12 && ai->CastSpell(RIGHTEOUS_DEFENSE, pTarget))
                ++CombatCounter;
            else if (!ai->HasAura(DIVINE_PLEA, m_bot) && ai->GetManaPercent() < 50 && CombatCounter < 13 && ai->CastSpell(DIVINE_PLEA, m_bot))
                ++CombatCounter;
            else if (!ai->HasAura(DIVINE_FAVOR, m_bot) && CombatCounter < 14 && ai->CastSpell(DIVINE_FAVOR, m_bot))
                ++CombatCounter;
            else
                CombatCounter = 0;

            return;

        case Healing:
            if (ai->GetHealthPercent() <= 40 && HealTarget(m_bot, ai->GetHealthPercent()))
                return;
            else if (masterHP <= 40 && HealTarget(m_master, masterHP))
                return;
            else
               CombatCounter = 0;

            return;
    }

    if (!ai->HasAura(AVENGING_WRATH, m_bot) && ai->GetManaPercent() >= 8 && ai->CastSpell(AVENGING_WRATH, m_bot))
        return;
    else if (ai->GetHealthPercent() < 30 && pVictim == m_bot && !ai->HasAura(FORBEARANCE, m_bot) && !ai->HasAura(DIVINE_SHIELD, m_bot) && ai->GetManaPercent() >= 3 && ai->CastSpell(DIVINE_SHIELD, m_bot))
        return;
    else if (ai->GetHealthPercent() > 50 && pVictim != m_bot && !ai->HasAura(DIVINE_SACRIFICE, m_bot) && ai->CastSpell(DIVINE_SACRIFICE, m_bot))
        return;
}

void PlayerbotPaladinAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Player* m_master = ai->GetMaster();
    if (!m_master)
        return;

    // buff myself
    if (!ai->HasAura(GREATER_BLESSING_OF_MIGHT, m_bot) && ai->CastSpell(GREATER_BLESSING_OF_MIGHT, m_bot))
        return;
    else if (!ai->HasAura(GREATER_BLESSING_OF_MIGHT, m_bot) && !ai->HasAura(BLESSING_OF_MIGHT, m_bot) && ai->CastSpell(BLESSING_OF_MIGHT, m_bot))
        return;
    else if (!ai->HasAura(DIVINE_FAVOR, m_bot) && ai->GetManaPercent() >= 3 && ai->CastSpell(DIVINE_FAVOR, m_bot))
        return;
    /*else if (ai->CastSpell(SEAL_OF_COMMAND, m_bot))
        return;*/

    //Select Class buff seq.
    //Process Who is my master --> get the player class --> aura already present if not then proced --> cast the spell

    //Priest
    else if (m_master->getClass() == CLASS_PRIEST && !ai->HasAura(GREATER_BLESSING_OF_WISDOM, m_master) && !ai->HasAura(BLESSING_OF_WISDOM, m_master) && ai->CastSpell(BLESSING_OF_WISDOM, m_master))
        return;
    else if (m_master->getClass() == CLASS_PRIEST && !ai->HasAura(GREATER_BLESSING_OF_WISDOM, m_master) && ai->CastSpell(GREATER_BLESSING_OF_WISDOM, m_master))
        return;
    //Mage
    else if (m_master->getClass() == CLASS_MAGE && !ai->HasAura(GREATER_BLESSING_OF_WISDOM, m_master) && !ai->HasAura(BLESSING_OF_WISDOM, m_master) && ai->CastSpell(BLESSING_OF_WISDOM, m_master))
        return;
    else if (m_master->getClass() == CLASS_MAGE && !ai->HasAura(GREATER_BLESSING_OF_WISDOM, m_master) && ai->CastSpell(GREATER_BLESSING_OF_WISDOM, m_master))
        return;
    //Paladin
    else if (m_master->getClass() == CLASS_PALADIN && !ai->HasAura(GREATER_BLESSING_OF_WISDOM, m_master) && !ai->HasAura(BLESSING_OF_WISDOM, m_master) && ai->CastSpell(BLESSING_OF_WISDOM, m_master))
        return;
    else if (m_master->getClass() == CLASS_PALADIN && !ai->HasAura(GREATER_BLESSING_OF_WISDOM, m_master) && ai->CastSpell(GREATER_BLESSING_OF_WISDOM, m_master))
        return;
    //Warlock
    else if (m_master->getClass() == CLASS_WARLOCK && !ai->HasAura(GREATER_BLESSING_OF_WISDOM, m_master) && !ai->HasAura(BLESSING_OF_WISDOM, m_master) && ai->CastSpell(BLESSING_OF_WISDOM, m_master))
        return;
    else if (m_master->getClass() == CLASS_WARLOCK && !ai->HasAura(GREATER_BLESSING_OF_WISDOM, m_master) && ai->CastSpell(GREATER_BLESSING_OF_WISDOM, m_master))
        return;
    //Warrior
    else if (m_master->getClass() == CLASS_WARRIOR && !ai->HasAura(GREATER_BLESSING_OF_MIGHT, m_master) && !ai->HasAura(BLESSING_OF_MIGHT, m_master) && !ai->HasAura(DEFENSIVE_STANCE, m_master) && ai->CastSpell(BLESSING_OF_MIGHT, m_master))
        return;
    else if (m_master->getClass() == CLASS_WARRIOR && !ai->HasAura(GREATER_BLESSING_OF_MIGHT, m_master) && !ai->HasAura(DEFENSIVE_STANCE, m_master) && ai->CastSpell(GREATER_BLESSING_OF_MIGHT, m_master))
        return;
    else if (m_master->getClass() == CLASS_WARRIOR && !ai->HasAura(GREATER_BLESSING_OF_KINGS, m_master) && !ai->HasAura(BLESSING_OF_KINGS, m_master) && !ai->HasAura(BERSERKER_STANCE, m_master) && !ai->HasAura(BATTLE_STANCE, m_master) && ai->CastSpell(BLESSING_OF_KINGS, m_master))
        return;
    else if (m_master->getClass() == CLASS_WARRIOR && !ai->HasAura(GREATER_BLESSING_OF_KINGS, m_master) && !ai->HasAura(BERSERKER_STANCE, m_master) && !ai->HasAura(BATTLE_STANCE, m_master) && ai->CastSpell(GREATER_BLESSING_OF_KINGS, m_master))
        return;
    //Rogue
    else if (m_master->getClass() == CLASS_ROGUE && !ai->HasAura(GREATER_BLESSING_OF_MIGHT, m_master) && !ai->HasAura(BLESSING_OF_MIGHT, m_master) && ai->CastSpell(BLESSING_OF_MIGHT, m_master))
        return;
    else if (m_master->getClass() == CLASS_ROGUE && !ai->HasAura(GREATER_BLESSING_OF_MIGHT, m_master) && ai->CastSpell(GREATER_BLESSING_OF_MIGHT, m_master))
        return;
    //Shaman
    else if (m_master->getClass() == CLASS_SHAMAN && !ai->HasAura(GREATER_BLESSING_OF_MIGHT, m_master) && !ai->HasAura(BLESSING_OF_MIGHT, m_master) && ai->CastSpell(BLESSING_OF_MIGHT, m_master))
        return;
    else if (m_master->getClass() == CLASS_SHAMAN && !ai->HasAura(GREATER_BLESSING_OF_MIGHT, m_master) && ai->CastSpell(GREATER_BLESSING_OF_MIGHT, m_master))
        return;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItemDrink = ai->FindDrink();
    Item* pItemFood = ai->FindFood();
    Item* pItemBandage = ai->FindBandage();

    // mana check
    if (pItemDrink && ai->GetManaPercent() < 40)
    {
        ai->UseItem(*pItemDrink);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
    // hp check original
    else if (pItemFood && ai->GetHealthPercent() < 40)
    {
        ai->UseItem(*pItemFood);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
    else if (!pItemFood && pItemBandage && !ai->HasAura(RECENTLY_BANDAGED, m_bot) && ai->GetHealthPercent() < 70)
    {
        ai->UseItem(*pItemBandage);
        ai->SetIgnoreUpdateTime(8);
        return;
    }

    // heal group
    if (m_master->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_master->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || !tPlayer->isAlive())
                continue;

            // heal player
            if (HealTarget(tPlayer, tPlayer->GetHealth() * 100 / tPlayer->GetMaxHealth()))
                return;
        }
    }
}
