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
#include "PlayerbotDruidAI.h"

class PlayerbotAI;

PlayerbotDruidAI::PlayerbotDruidAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

void PlayerbotDruidAI::InitSpells(PlayerbotAI* const ai)
{
    MOONFIRE                      = ai->initSpell(MOONFIRE_1); // attacks
    STARFIRE                      = ai->initSpell(STARFIRE_1);
    STARFALL                      = ai->initSpell(STARFALL_1);
    WRATH                         = ai->initSpell(WRATH_1);
    ROOTS                         = ai->initSpell(ENTANGLING_ROOTS_1);
    INSECT_SWARM                  = ai->initSpell(INSECT_SWARM_1);
    FORCE_OF_NATURE               = ai->initSpell(FORCE_OF_NATURE_1);
    HURRICANE                     = ai->initSpell(HURRICANE_1);
    MARK_OF_THE_WILD              = ai->initSpell(MARK_OF_THE_WILD_1); // buffs
    THORNS                        = ai->initSpell(THORNS_1);
    BARKSKIN                      = ai->initSpell(BARKSKIN_1);
    INNERVATE                     = ai->initSpell(INNERVATE_1);
    MANA_REJUVENATION             = 0; // Who on earth put it here?
    FAERIE_FIRE                   = ai->initSpell(FAERIE_FIRE_1); // debuffs
    REJUVENATION                  = ai->initSpell(REJUVENATION_1); // heals
    REGROWTH                      = ai->initSpell(REGROWTH_1);
    WILD_GROWTH                   = ai->initSpell(WILD_GROWTH_1);
    LIFEBLOOM                     = ai->initSpell(LIFEBLOOM_1);
    NOURISH                       = ai->initSpell(NOURISH_1);
    HEALING_TOUCH                 = ai->initSpell(HEALING_TOUCH_1);
    SWIFTMEND                     = ai->initSpell(SWIFTMEND_1);
    TRANQUILITY                   = ai->initSpell(TRANQUILITY_1);
    // Druid Forms
    MOONKIN_FORM                  = ai->initSpell(MOONKIN_FORM_1);
    DIRE_BEAR_FORM                = ai->initSpell(DIRE_BEAR_FORM_1);
    BEAR_FORM                     = ai->initSpell(BEAR_FORM_1);
    CAT_FORM                      = ai->initSpell(CAT_FORM_1);
    TREE_OF_LIFE                  = ai->initSpell(TREE_OF_LIFE_1);
    // Cat Attack type's
    RAKE                          = ai->initSpell(RAKE_1);
    CLAW                          = ai->initSpell(CLAW_1); // 45
    COWER                         = ai->initSpell(COWER_1); // 20
    MANGLE                        = ai->initSpell(MANGLE_1); // 45
    TIGERS_FURY                   = ai->initSpell(TIGERS_FURY_1);
    // Cat Finishing Move's
    RIP                           = ai->initSpell(RIP_1); // 30
    FEROCIOUS_BITE                = ai->initSpell(FEROCIOUS_BITE_1); // 35
    MAIM                          = ai->initSpell(MAIM_1); // 35
    // Bear/Dire Bear Attacks & Buffs
    BASH                          = ai->initSpell(BASH_1);
    MAUL                          = ai->initSpell(MAUL_1); // 15
    SWIPE                         = ai->initSpell(SWIPE_BEAR_1); // 20
    DEMORALIZING_ROAR             = ai->initSpell(DEMORALIZING_ROAR_1); // 10
    CHALLENGING_ROAR              = ai->initSpell(CHALLENGING_ROAR_1);
    ENRAGE                        = ai->initSpell(ENRAGE_1);
    GROWL                         = ai->initSpell(GROWL_1);

    RECENTLY_BANDAGED             = 11196; // first aid check

    // racial
    SHADOWMELD                    = ai->initSpell(SHADOWMELD_ALL);
    WAR_STOMP                     = ai->initSpell(WAR_STOMP_ALL); // tauren
}

PlayerbotDruidAI::~PlayerbotDruidAI() {}

bool PlayerbotDruidAI::HealTarget(Unit* target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return false;

    if (hp < 70 && !ai->HasAura(REJUVENATION, target) && !ai->HasAura(REGROWTH, target) && ai->GetManaPercent() >= 21 && ai->CastSpell(REJUVENATION, target))
        return true;
    else if (hp < 60 && !ai->HasAura(LIFEBLOOM, target) && ai->GetManaPercent() >= 28 && ai->CastSpell(LIFEBLOOM, target))
        return true;
    else if (hp < 55 && !ai->HasAura(REGROWTH, target) && !ai->HasAura(REJUVENATION, target) && ai->GetManaPercent() >= 33 && ai->CastSpell(REGROWTH, target))
        return true;
    else if (hp < 50 && ai->HasAura(REJUVENATION, target) || ai->HasAura(REGROWTH, target) && ai->GetManaPercent() >= 16 && ai->CastSpell(SWIFTMEND, target))
        return true;
    else if (hp < 45 && !ai->HasAura(WILD_GROWTH, target) && ai->GetManaPercent() >= 26 && ai->CastSpell(WILD_GROWTH, target))
        return true;
    else if (hp < 30 && ai->GetManaPercent() >= 18 && ai->CastSpell(NOURISH, target))
        return true;
    else if (hp < 25 && ai->GetManaPercent() >= 38 && ai->CastSpell(HEALING_TOUCH, target))
        return true;

    return false;
}

void PlayerbotDruidAI::DoNextCombatManeuver(Unit* pTarget)
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

    /*switch (ai->GetScenarioType())
    {
        SCENARIO_PVEEASY,
        SCENARIO_PVEHARD,
        SCENARIO_DUEL,
        SCENARIO_PVPEASY,
        SCENARIO_PVPHARD
        //TODO Implement different AI...
        //return;
    }*/

    uint32 masterHP = m_master->GetHealth() * 100 / m_master->GetMaxHealth();

    ai->SetInFront(pTarget);
    Unit* pVictim = pTarget->getVictim();

    if (pVictim && ai->GetHealthPercent() >= 40 && m_master->GetHealth() >= m_master->GetMaxHealth() * 0.4 && pVictim == m_bot)
        SpellSequence = DruidTank;
    else if (pTarget->GetHealth() > pTarget->GetMaxHealth() * 0.8 && pVictim && pVictim != m_bot)
            SpellSequence = DruidSpell;
    else if (ai->GetHealthPercent() <= 40 || m_master->GetHealth() <= m_master->GetMaxHealth() * 0.4)
        SpellSequence = DruidHeal;
    else
        SpellSequence = DruidCombat;

    switch (SpellSequence)
    {
        case DruidTank: // Its now a tank druid!
            if (!m_bot->HasInArc(M_PI_F, pTarget))
            {
                m_bot->SetInFront(pTarget);
                if (pVictim)
                    pVictim->Attack(pTarget, true);
            }

            if (ai->HasAura(CAT_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(768);

            if (!ai->HasAura(MOONKIN_FORM, m_bot) && ai->CastSpell(MOONKIN_FORM))
                return;
            else if (!ai->HasAura(MOONKIN_FORM, m_bot) && !ai->HasAura(DIRE_BEAR_FORM, m_bot) && ai->CastSpell(DIRE_BEAR_FORM))
                return;
            else if (!ai->HasAura(MOONKIN_FORM, m_bot) && !ai->HasAura(DIRE_BEAR_FORM, m_bot) && !ai->HasAura(BEAR_FORM, m_bot) && ai->CastSpell(BEAR_FORM))
                return;
            else if ((ai->HasAura(DIRE_BEAR_FORM, m_bot) || ai->HasAura(BEAR_FORM, m_bot)) && !ai->HasAura(MOONKIN_FORM, m_bot) && !ai->HasAura(DEMORALIZING_ROAR, pTarget) && ai->GetRageAmount() >= 10 && ai->CastSpell(DEMORALIZING_ROAR, pTarget))
                return;

            if (DruidSpellCombat < 1 && !ai->HasAura(FAERIE_FIRE, pTarget) && ai->CastSpell(FAERIE_FIRE, pTarget))
                ++DruidSpellCombat;
            else if (ai->HasAura(MOONKIN_FORM, m_bot) && DruidSpellCombat < 2 && !ai->HasAura(MOONFIRE, pTarget) && ai->GetManaPercent() >= 24 && ai->CastSpell(MOONFIRE, pTarget))
                ++DruidSpellCombat;
            else if (ai->HasAura(MOONKIN_FORM, m_bot) && DruidSpellCombat < 3 && !ai->HasAura(ROOTS, pTarget) && ai->GetManaPercent() >= 8 && ai->CastSpell(ROOTS, pTarget))
                ++DruidSpellCombat;
            else if (ai->HasAura(MOONKIN_FORM, m_bot) && ai->GetAttackerCount() >= 5 && DruidSpellCombat < 4 && ai->GetManaPercent() >= 91 && ai->CastSpell(HURRICANE, pTarget))
            {
                ai->SetIgnoreUpdateTime(10);
                ++DruidSpellCombat;
            }
            else if (ai->HasAura(MOONKIN_FORM, m_bot) && DruidSpellCombat < 5 && ai->GetManaPercent() >= 13 && ai->CastSpell(WRATH, pTarget))
                ++DruidSpellCombat;
            else if (ai->HasAura(MOONKIN_FORM, m_bot) && DruidSpellCombat < 6 && !ai->HasAura(INSECT_SWARM, pTarget) && ai->GetManaPercent() >= 9 && ai->CastSpell(INSECT_SWARM, pTarget))
                ++DruidSpellCombat;
            else if (ai->HasAura(MOONKIN_FORM, m_bot) && DruidSpellCombat < 7 && ai->GetManaPercent() >= 18 && ai->CastSpell(STARFIRE, pTarget))
                ++DruidSpellCombat;
            else if (ai->HasAura(MOONKIN_FORM, m_bot) && DruidSpellCombat < 8 && ai->GetManaPercent() >= 12 && ai->CastSpell(FORCE_OF_NATURE))
                ++DruidSpellCombat;
            else if (ai->HasAura(MOONKIN_FORM, m_bot) && !ai->HasAura(STARFALL, m_bot) && ai->GetAttackerCount() >= 3 && DruidSpellCombat < 9 && ai->GetManaPercent() >= 39 && ai->CastSpell(STARFALL, pTarget))
                ++DruidSpellCombat;
            else if (pVictim == m_bot && ai->HasAura(MOONKIN_FORM, m_bot) && ai->GetHealthPercent() < 75 && DruidSpellCombat < 10 && !ai->HasAura(BARKSKIN, m_bot) && ai->CastSpell(BARKSKIN, m_bot))
                ++DruidSpellCombat;
            else if (ai->HasAura(MOONKIN_FORM, m_bot) && ai->GetManaPercent() < 50 && DruidSpellCombat < 11 && !ai->HasAura(INNERVATE, m_bot) && ai->CastSpell(INNERVATE, m_bot))
                ++DruidSpellCombat;
            else if (ai->HasAura(MOONKIN_FORM, m_bot) && ai->GetManaPercent() < 50 && !ai->HasAura(INNERVATE, m_bot) && DruidSpellCombat < 12 && !ai->HasAura(MANA_REJUVENATION, m_bot) && ai->CastSpell(MANA_REJUVENATION, m_bot))
                ++DruidSpellCombat;
            else if ((ai->HasAura(DIRE_BEAR_FORM, m_bot) || ai->HasAura(BEAR_FORM, m_bot)) && DruidSpellCombat < 2 && !ai->HasAura(ENRAGE, m_bot) && ai->CastSpell(ENRAGE, m_bot))
                DruidSpellCombat = DruidSpellCombat + 2;
            else if ((ai->HasAura(DIRE_BEAR_FORM, m_bot) || ai->HasAura(BEAR_FORM, m_bot)) && DruidSpellCombat < 4 && ai->GetRageAmount() >= 20 && ai->CastSpell(SWIPE, pTarget))
                DruidSpellCombat = DruidSpellCombat + 2;
            else if ((ai->HasAura(DIRE_BEAR_FORM, m_bot) || ai->HasAura(BEAR_FORM, m_bot)) && DruidSpellCombat < 6 && ai->GetRageAmount() >= 15 && ai->CastSpell(MAUL, pTarget))
                DruidSpellCombat = DruidSpellCombat + 2;
            else if ((ai->HasAura(DIRE_BEAR_FORM, m_bot) || ai->HasAura(BEAR_FORM, m_bot)) && !ai->HasAura(BASH, pTarget) && DruidSpellCombat < 8 && ai->GetRageAmount() >= 10 && ai->CastSpell(BASH, pTarget))
                DruidSpellCombat = DruidSpellCombat + 2;
            else if ((ai->HasAura(DIRE_BEAR_FORM, m_bot) || ai->HasAura(BEAR_FORM, m_bot)) && pVictim != m_bot && DruidSpellCombat < 10 && !ai->HasAura(CHALLENGING_ROAR, pTarget) && !ai->HasAura(GROWL, pTarget) && ai->GetRageAmount() >= 15 && ai->CastSpell(CHALLENGING_ROAR, pTarget))
                DruidSpellCombat = DruidSpellCombat + 2;
            else if ((ai->HasAura(DIRE_BEAR_FORM, m_bot) || ai->HasAura(BEAR_FORM, m_bot)) && pVictim != m_bot && DruidSpellCombat < 12 && !ai->HasAura(CHALLENGING_ROAR, pTarget) && !ai->HasAura(GROWL, pTarget) && ai->CastSpell(GROWL, pTarget))
                DruidSpellCombat = DruidSpellCombat + 2;
            else
                DruidSpellCombat = 0;

            return;

        case DruidSpell:
            if (ai->HasAura(CAT_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(768);
            if (ai->HasAura(BEAR_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(5487);
            if (ai->HasAura(DIRE_BEAR_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(9634);
            if (ai->HasAura(MOONKIN_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(24858);

            if (DruidSpellCombat < 1 && !ai->HasAura(FAERIE_FIRE, pTarget) && ai->GetManaPercent() >= 8 && ai->CastSpell(FAERIE_FIRE, pTarget))
                ++DruidSpellCombat;
            else if (DruidSpellCombat < 2 && !ai->HasAura(MOONFIRE, pTarget) && ai->GetManaPercent() >= 24 && ai->CastSpell(MOONFIRE, pTarget))
                ++DruidSpellCombat;
            else if (DruidSpellCombat < 3 && !ai->HasAura(ROOTS, pTarget) && ai->GetManaPercent() >= 8 && ai->CastSpell(ROOTS, pTarget))
                ++DruidSpellCombat;
            else if (ai->GetAttackerCount() >= 5 && DruidSpellCombat < 4 && ai->GetManaPercent() >= 91 && ai->CastSpell(HURRICANE, pTarget))
            {
                ai->SetIgnoreUpdateTime(10);
                ++DruidSpellCombat;
            }
            else if (DruidSpellCombat < 5 && ai->GetManaPercent() >= 13 && ai->CastSpell(WRATH, pTarget))
                ++DruidSpellCombat;
            else if (DruidSpellCombat < 6 && !ai->HasAura(INSECT_SWARM, pTarget) && ai->GetManaPercent() >= 9 && ai->CastSpell(INSECT_SWARM, pTarget))
                ++DruidSpellCombat;
            else if (DruidSpellCombat < 7 && ai->GetManaPercent() >= 18 && ai->CastSpell(STARFIRE, pTarget))
                ++DruidSpellCombat;
            else if (DruidSpellCombat < 8 && ai->GetManaPercent() >= 12 && ai->CastSpell(FORCE_OF_NATURE))
                ++DruidSpellCombat;
            else if (!ai->HasAura(STARFALL, m_bot) && ai->GetAttackerCount() >= 3 && DruidSpellCombat < 9 && ai->GetManaPercent() >= 39 && ai->CastSpell(STARFALL, pTarget))
                ++DruidSpellCombat;
            else if (pVictim == m_bot && ai->GetHealthPercent() < 75 && DruidSpellCombat < 10 && !ai->HasAura(BARKSKIN, m_bot) && ai->CastSpell(BARKSKIN, m_bot))
                ++DruidSpellCombat;
            else if (ai->GetManaPercent() < 50 && DruidSpellCombat < 11 && !ai->HasAura(INNERVATE, m_bot) && ai->CastSpell(INNERVATE, m_bot))
                ++DruidSpellCombat;
            else if (ai->GetManaPercent() < 50 && DruidSpellCombat < 12 && !ai->HasAura(INNERVATE, m_bot) && !ai->HasAura(MANA_REJUVENATION, m_bot) && ai->CastSpell(MANA_REJUVENATION, m_bot))
                ++DruidSpellCombat;
            else
                DruidSpellCombat = 0;

            return;

        case DruidHeal:
            if (ai->HasAura(CAT_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(768);
            if (ai->HasAura(BEAR_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(5487);
            if (ai->HasAura(DIRE_BEAR_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(9634);
            if (ai->HasAura(MOONKIN_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(24858);

            if (ai->GetHealthPercent() <= 40 && HealTarget(m_bot, ai->GetHealthPercent()))
                return;
            else if (masterHP <= 40 && HealTarget(m_master, masterHP))
                return;
            else
                DruidSpellCombat = 0;

            return;

        case DruidCombat:
            if (!m_bot->HasInArc(M_PI_F, pTarget))
            {
                m_bot->SetInFront(pTarget);
                if (pVictim)
                    pVictim->Attack(pTarget, true);
            }

            if (ai->HasAura(BEAR_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(5487);
            if (ai->HasAura(DIRE_BEAR_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(9634);
            if (ai->HasAura(MOONKIN_FORM, m_bot))
                m_bot->RemoveAurasDueToSpell(24858);

            if (!ai->HasAura(CAT_FORM, m_bot) && ai->CastSpell(CAT_FORM))
                return;
            /*else if (m_bot->GetComboPoints() == 1 && ai->GetEnergyAmount() >= 20 && ai->CastSpell(COWER))
                return;*/
            else if (m_bot->GetComboPoints() >= 1 && pTarget->IsNonMeleeSpellCasted(true) && ai->CastSpell(MAIM, pTarget))
                return;
            else if (m_bot->GetComboPoints() <= 1 && ai->GetEnergyAmount() >= 40 && ai->CastSpell(RAKE, pTarget))
                return;
            else if (m_bot->GetComboPoints() <= 2 && ai->GetEnergyAmount() >= 45 && ai->CastSpell(CLAW, pTarget))
                return;
            else if (m_bot->GetComboPoints() <= 3 && ai->GetEnergyAmount() >= 45 && ai->CastSpell(MANGLE, pTarget))
                return;
            else if (m_bot->GetComboPoints() <= 4 && ai->GetEnergyAmount() >= 45 && ai->CastSpell(CLAW, pTarget))
                return;
            else if (m_bot->GetComboPoints() == 5)
            {
                if (pTarget->getClass() == CLASS_ROGUE && ai->GetEnergyAmount() >= 30 && ai->CastSpell(RIP, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_DRUID && ai->GetEnergyAmount() >= 35 && ai->CastSpell(MAIM, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_SHAMAN && ai->GetEnergyAmount() >= 35 && ai->CastSpell(MAIM, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_WARLOCK && ai->GetEnergyAmount() >= 35 && ai->CastSpell(MAIM, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_HUNTER && ai->GetEnergyAmount() >= 35 && ai->CastSpell(FEROCIOUS_BITE, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_WARRIOR && ai->GetEnergyAmount() >= 35 && ai->CastSpell(FEROCIOUS_BITE, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_PALADIN && ai->GetEnergyAmount() >= 35 && ai->CastSpell(FEROCIOUS_BITE, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_DEATH_KNIGHT && ai->GetEnergyAmount() >= 25 && ai->CastSpell(FEROCIOUS_BITE, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_MAGE && ai->GetEnergyAmount() >= 35 && ai->CastSpell(MAIM, pTarget))
                    return;
                else if (pTarget->getClass() == CLASS_PRIEST && ai->GetEnergyAmount() >= 35 && ai->CastSpell(MAIM, pTarget))
                    return;
                else if (ai->GetEnergyAmount() >= 35 && ai->CastSpell(MAIM, pTarget))
                    return;
            }
            else
                DruidSpellCombat = 0;

            return;
    }
}

void PlayerbotDruidAI::DoNonCombatActions()
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

    if (ai->HasAura(CAT_FORM, m_bot))
        m_bot->RemoveAurasDueToSpell(768);
    if (ai->HasAura(BEAR_FORM, m_bot))
        m_bot->RemoveAurasDueToSpell(5487);
    if (ai->HasAura(DIRE_BEAR_FORM, m_bot))
        m_bot->RemoveAurasDueToSpell(9634);
    if (ai->HasAura(MOONKIN_FORM, m_bot))
        m_bot->RemoveAurasDueToSpell(24858);

     // buff myself with MARK_OF_THE_WILD
    if (!ai->HasAura(MARK_OF_THE_WILD, m_bot) && ai->CastSpell(MARK_OF_THE_WILD, m_bot))
        return;
    // Thorns generates aggro for moonkin
    else if (!ai->HasAura(THORNS, m_bot) && ai->CastSpell(THORNS, m_bot))
        return;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItemDrink = ai->FindDrink();
    Item* pItemFood = ai->FindFood();
    Item* pItemBandage = ai->FindBandage();

    // mana check
    if (pItemDrink && ai->GetManaPercent() < 30)
    {
        ai->UseItem(*pItemDrink);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
    else if (!pItemDrink && !ai->HasAura(INNERVATE, m_bot) && ai->GetManaPercent() <= 70 && ai->CastSpell(INNERVATE, m_bot))
        return;
    else if (!pItemDrink && !ai->HasAura(INNERVATE, m_bot) && !ai->HasAura(MANA_REJUVENATION, m_bot) && ai->GetManaPercent() <= 70 && ai->CastSpell(MANA_REJUVENATION, m_bot))
        return;
    // hp check
    else if (pItemFood && ai->GetHealthPercent() < 30)
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

    // buff and heal master's group
    if (m_master->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_master->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || !tPlayer->isAlive())
                continue;

            // buff and heal
            if (!ai->HasAura(MARK_OF_THE_WILD, tPlayer) && ai->CastSpell(MARK_OF_THE_WILD, tPlayer))
                return;
            //(!ai->HasAura(THORNS, tPlayer) && ai->CastSpell(THORNS, tPlayer));
            else if (HealTarget(tPlayer, tPlayer->GetHealth() * 100 / tPlayer->GetMaxHealth()))
                return;
        }
    }
}
