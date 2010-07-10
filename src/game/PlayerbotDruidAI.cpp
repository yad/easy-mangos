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

PlayerbotDruidAI::~PlayerbotDruidAI() {}

void PlayerbotDruidAI::InitSpells(PlayerbotAI* const ai)
{
    MOONFIRE = ai->getSpellId("moonfire");
    STARFIRE = ai->getSpellId("starfire");
    STARFALL = ai->getSpellId("starfall");
    WRATH = ai->getSpellId("wrath");
    ROOTS = ai->getSpellId("roots");
    INSECT_SWARM = ai->getSpellId("insect swarm");
    FORCE_OF_NATURE = ai->getSpellId("force of nature");
    HURRICANE = ai->getSpellId("hurricane");
    MARK_OF_THE_WILD = ai->getSpellId("mark of the wild");
    THORNS = ai->getSpellId("thorns");
    BARKSKIN = ai->getSpellId("barkskin");
    INNERVATE = ai->getSpellId("innervate");
    MANA_REJUVENATION = ai->getSpellId("mana rejuvenation");
    FAERIE_FIRE = ai->getSpellId("faerie fire");
    REJUVENATION = ai->getSpellId("rejuvenation");
    REGROWTH = ai->getSpellId("regrowth");
    WILD_GROWTH = ai->getSpellId("wild growth");
    LIFEBLOOM = ai->getSpellId("lifebloom");
    NOURISH = ai->getSpellId("nourish");
    HEALING_TOUCH = ai->getSpellId("healing touch");
    SWIFTMEND = ai->getSpellId("swiftmend");
    TRANQUILITY = ai->getSpellId("tranquility");
    MOONKIN_FORM = ai->getSpellId("moonkin form");
    DIRE_BEAR_FORM = ai->getSpellId("dire bear form");
    BEAR_FORM = ai->getSpellId("bear form");
    CAT_FORM = ai->getSpellId("cat form");
    TREE_OF_LIFE = 33891;
    RAKE = ai->getSpellId("rake");
    CLAW = ai->getSpellId("claw");
    COWER = ai->getSpellId("cower");
    MANGLE = ai->getSpellId("mangle");
    TIGERS_FURY = ai->getSpellId("tigers's fury");
    RIP = ai->getSpellId("rip");
    FEROCIOUS_BITE = ai->getSpellId("ferocious bite");
    MAIM = ai->getSpellId("maim");
    BASH = ai->getSpellId("bash");
    MAUL = ai->getSpellId("maul");
    SWIPE = ai->getSpellId("swipe");
    DEMORALIZING_ROAR = ai->getSpellId("roar");
    CHALLENGING_ROAR = ai->getSpellId("challenging roar");
    ENRAGE = ai->getSpellId("enrage");
    GROWL = ai->getSpellId("growl");
    RECENTLY_BANDAGED = 11196;
    SHADOWMELD = ai->getSpellId("shadowmeld");
    WAR_STOMP = ai->getSpellId("war stomp");
}

bool PlayerbotDruidAI::HealTarget(Unit &target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    Player *m_bot = GetPlayerBot();

    if ((hp < 70)
        && (REJUVENATION > 0)
        && (!target.HasAura(REJUVENATION, EFFECT_INDEX_0))
        && (!target.HasAura(REGROWTH, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 21)
        && (ai->CastSpell(REJUVENATION, target)))
        return true;

    if ((hp < 60)
        && (LIFEBLOOM > 0)
        && (!target.HasAura(LIFEBLOOM, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 28)
        && (ai->CastSpell(LIFEBLOOM, target)))
        return true;

    if ((hp < 55)
        && (REGROWTH > 0)
        && (!target.HasAura(REGROWTH, EFFECT_INDEX_0))
        && (!target.HasAura(REJUVENATION, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 33)
        && (ai->CastSpell(REGROWTH, target)))
        return true;

    if ((hp < 50)
        && (SWIFTMEND > 0)
        && (target.HasAura(REJUVENATION, EFFECT_INDEX_0) || target.HasAura(REGROWTH, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 16)
        && (ai->CastSpell(SWIFTMEND, target)))
        return true;

    if ((hp < 45)
        && (WILD_GROWTH > 0)
        && (!target.HasAura(WILD_GROWTH, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 26)
        && (ai->CastSpell(WILD_GROWTH, target)))
        return true;

    if ((hp < 30)
        && (NOURISH > 0)
        && (ai->GetManaPercent() >= 18)
        && (ai->CastSpell(NOURISH, target)))
        return true;

    if ((hp < 25)
        && (HEALING_TOUCH > 0)
        && (ai->GetManaPercent() >= 38)
        && (ai->CastSpell(HEALING_TOUCH, target)))
        return true;

    return false;
}

bool PlayerbotDruidAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    uint32 masterHP = pMaster->GetHealth() * 100 / pMaster->GetMaxHealth();

    ai->SetInFront(pTarget);
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();

    if (pVictim
        && (ai->GetHealthPercent() >= 40)
        && (pMaster->GetHealth() >= pMaster->GetMaxHealth() * 0.4))
    {
        if (pVictim == m_bot)
            SpellSequence = DruidTank;
    }
    else if (pVictim
        && (pTarget->GetHealth() > pTarget->GetMaxHealth() * 0.8))
    {
        if (pVictim != m_bot)
            SpellSequence = DruidSpell;
    }
    else if (ai->GetHealthPercent() <= 40 || pMaster->GetHealth() <= pMaster->GetMaxHealth() * 0.4)
        SpellSequence = DruidHeal;
    else
        SpellSequence = DruidCombat;

    switch (SpellSequence)
    {
        case DruidTank:

            m_bot->SetInFront(pTarget);
            if (pVictim)
                pVictim->Attack(pTarget, true);

            if (m_bot->HasAura(CAT_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(768);

            if ((MOONKIN_FORM > 0)
                && (!m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (ai->CastSpell(MOONKIN_FORM)))
                return true;

            if ((DIRE_BEAR_FORM > 0)
                && (!m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (!m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
                && (ai->CastSpell(DIRE_BEAR_FORM)))
                return true;

            if ((BEAR_FORM > 0)
                && (!m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (!m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
                && (!m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                && (ai->CastSpell(BEAR_FORM)))
                return true;

            if ((DEMORALIZING_ROAR > 0)
                && ((m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))|| m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                && (!m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (!pTarget->HasAura(DEMORALIZING_ROAR, EFFECT_INDEX_0))
                && (ai->GetRageAmount() >= 10)
                && (ai->CastSpell(DEMORALIZING_ROAR, *pTarget)))
                return true;

            if ((FAERIE_FIRE > 0)
                && (DruidSpellCombat < 1)
                && (!pTarget->HasAura(FAERIE_FIRE, EFFECT_INDEX_0))
                && (ai->CastSpell(FAERIE_FIRE, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((MOONFIRE > 0)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (DruidSpellCombat < 2)
                && (!pTarget->HasAura(MOONFIRE, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 24)
                && (ai->CastSpell(MOONFIRE, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((ROOTS > 0)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (DruidSpellCombat < 3)
                && (!pTarget->HasAura(ROOTS, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 8)
                && (ai->CastSpell(ROOTS, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((HURRICANE > 0)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (ai->GetAttackerCount() >= 5)
                && (DruidSpellCombat < 4)
                && (ai->GetManaPercent() >= 91)
                && (ai->CastSpell(HURRICANE, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(10);			
                DruidSpellCombat++;
                return true;
            }

            if ((WRATH > 0)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (DruidSpellCombat < 5)
                && (ai->GetManaPercent() >= 13)
                && (ai->CastSpell(WRATH, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((INSECT_SWARM > 0)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (DruidSpellCombat < 6)
                && (!pTarget->HasAura(INSECT_SWARM, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 9)
                && (ai->CastSpell(INSECT_SWARM , *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((STARFIRE > 0)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (DruidSpellCombat < 7)
                && (ai->GetManaPercent() >= 18)
                && (ai->CastSpell(STARFIRE, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((FORCE_OF_NATURE > 0)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (DruidSpellCombat < 8)
                && (ai->GetManaPercent() >= 12)
                && (ai->CastSpell(FORCE_OF_NATURE)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((STARFALL > 0)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (!m_bot->HasAura(STARFALL, EFFECT_INDEX_0))
                && (ai->GetAttackerCount() >= 3)
                && (DruidSpellCombat < 9)
                && (ai->GetManaPercent() >= 39)
                && (ai->CastSpell(STARFALL, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((BARKSKIN > 0)
                && (pVictim == m_bot)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (ai->GetHealthPercent() < 75)
                && (DruidSpellCombat < 10)
                && (!m_bot->HasAura(BARKSKIN, EFFECT_INDEX_0))
                && (ai->CastSpell(BARKSKIN, *m_bot)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((INNERVATE > 0)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (ai->GetManaPercent() < 50)
                && (DruidSpellCombat < 11)
                && (!m_bot->HasAura(INNERVATE, EFFECT_INDEX_0))
                && (ai->CastSpell(INNERVATE, *m_bot)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((MANA_REJUVENATION > 0)
                && (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                && (ai->GetManaPercent() < 50)
                && (!m_bot->HasAura(INNERVATE, EFFECT_INDEX_0))
                && (DruidSpellCombat < 12)
                && (!m_bot->HasAura(MANA_REJUVENATION, EFFECT_INDEX_0))
                && (ai->CastSpell(MANA_REJUVENATION, *m_bot)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((ENRAGE > 0)
                && ((m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))|| m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                && (DruidSpellCombat < 2)
                && (!m_bot->HasAura(ENRAGE, EFFECT_INDEX_0))
                && (ai->CastSpell(ENRAGE, *m_bot)))
            {
                DruidSpellCombat++;
                DruidSpellCombat++;
                return true;
            }

            if ((SWIPE > 0)
                && ((m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))|| m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                && (DruidSpellCombat < 4)
                && (ai->GetRageAmount() >= 20)
                && (ai->CastSpell(SWIPE, *pTarget)))
            {
                DruidSpellCombat++;
                DruidSpellCombat++;
                return true;
            }

            if ((MAUL > 0)
                && ((m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))|| m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                && (DruidSpellCombat < 6)
                && (ai->GetRageAmount() >= 15)
                && (ai->CastSpell(MAUL, *pTarget)))
            {
                DruidSpellCombat++;
                DruidSpellCombat++;
                return true;
            }

            if ((BASH > 0)
                && ((m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))|| m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                && (!pTarget->HasAura(BASH, EFFECT_INDEX_0))
                && (DruidSpellCombat < 8)
                && (ai->GetRageAmount() >= 10)
                && (ai->CastSpell(BASH, *pTarget)))
            {
                DruidSpellCombat++;
                DruidSpellCombat++;
                return true;
            }

            if ((CHALLENGING_ROAR > 0)
                && ((m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))|| m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                && (pVictim != m_bot)
                && (DruidSpellCombat < 10)
                && (!pTarget->HasAura(CHALLENGING_ROAR, EFFECT_INDEX_0))
                && (!pTarget->HasAura(GROWL, EFFECT_INDEX_0))
                && (ai->GetRageAmount() >= 15)
                && (ai->CastSpell(CHALLENGING_ROAR, *pTarget)))
            {
                DruidSpellCombat++;
                DruidSpellCombat++;
                return true;
            }

            if ((GROWL > 0)
                && ((m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))|| m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                && (pVictim != m_bot)
                && (DruidSpellCombat < 12)
                && (!pTarget->HasAura(CHALLENGING_ROAR, EFFECT_INDEX_0))
                && (!pTarget->HasAura(GROWL, EFFECT_INDEX_0))
                && (ai->CastSpell(GROWL, *pTarget)))
            {
                DruidSpellCombat++;
                DruidSpellCombat++;
                return true;
            }

            DruidSpellCombat = 0;
            break;

        case DruidSpell:

            if (m_bot->HasAura(CAT_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(768);

            if (m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(5487);

            if (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(9634);

            if (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(24858);

            if ((FAERIE_FIRE > 0)
                && (DruidSpellCombat < 1)
                && (!pTarget->HasAura(FAERIE_FIRE, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 8)
                && (ai->CastSpell(FAERIE_FIRE, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((MOONFIRE > 0)
                && (DruidSpellCombat < 2)
                && (!pTarget->HasAura(MOONFIRE, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 24)
                && (ai->CastSpell(MOONFIRE, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((ROOTS > 0)
                && (DruidSpellCombat < 3)
                && (!pTarget->HasAura(ROOTS, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 8)
                && (ai->CastSpell(ROOTS, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((HURRICANE > 0)
                && (ai->GetAttackerCount() >= 5)
                && (DruidSpellCombat < 4)
                && (ai->GetManaPercent() >= 91)
                && (ai->CastSpell(HURRICANE, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(10);			
                DruidSpellCombat++;
                return true;
            }

            if ((WRATH > 0)
                && (DruidSpellCombat < 5)
                && (ai->GetManaPercent() >= 13)
                && (ai->CastSpell(WRATH, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((INSECT_SWARM > 0)
                && (DruidSpellCombat < 6)
                && (!pTarget->HasAura(INSECT_SWARM, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 9)
                && (ai->CastSpell(INSECT_SWARM , *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((STARFIRE > 0)
                && (DruidSpellCombat < 7)
                && (ai->GetManaPercent() >= 18)
                && (ai->CastSpell(STARFIRE, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((FORCE_OF_NATURE > 0)
                && (DruidSpellCombat < 8)
                && (ai->GetManaPercent() >= 12)
                && (ai->CastSpell(FORCE_OF_NATURE)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((STARFALL > 0)
                && (!m_bot->HasAura(STARFALL, EFFECT_INDEX_0))
                && (ai->GetAttackerCount() >= 3)
                && (DruidSpellCombat < 9)
                && (ai->GetManaPercent() >= 39)
                && (ai->CastSpell(STARFALL, *pTarget)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((BARKSKIN > 0)
                && (pVictim == m_bot)
                && (ai->GetHealthPercent() < 75)
                && (DruidSpellCombat < 10)
                && (!m_bot->HasAura(BARKSKIN, EFFECT_INDEX_0))
                && (ai->CastSpell(BARKSKIN, *m_bot)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((INNERVATE > 0)
                && (ai->GetManaPercent() < 50)
                && (DruidSpellCombat < 11)
                && (!m_bot->HasAura(INNERVATE, EFFECT_INDEX_0))
                && (ai->CastSpell(INNERVATE, *m_bot)))
            {
                DruidSpellCombat++;
                return true;
            }

            if ((MANA_REJUVENATION > 0)
                && (ai->GetManaPercent() < 50)
                && (DruidSpellCombat < 12)
                && (!m_bot->HasAura(INNERVATE, EFFECT_INDEX_0))
                && (!m_bot->HasAura(MANA_REJUVENATION, EFFECT_INDEX_0))
                && (ai->CastSpell(MANA_REJUVENATION, *m_bot)))
            {
                DruidSpellCombat++;
                return true;
            }

            DruidSpellCombat = 0;
            break;

        case DruidHeal:

            if (m_bot->HasAura(CAT_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(768);

            if (m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(5487);

            if (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(9634);

            if (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(24858);

            if ((ai->GetHealthPercent() <= 40)
                && HealTarget(*m_bot, ai->GetHealthPercent()))
                return true;

            if ((masterHP <= 40)
                && HealTarget(*pMaster, masterHP))
                return true;

            DruidSpellCombat = 0;
            break;

        case DruidCombat:

            m_bot->SetInFront(pTarget);
            if (pVictim)
                pVictim->Attack(pTarget, true);

            if (m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(5487);

            if (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(9634);

            if (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
                m_bot->RemoveAurasDueToSpell(24858);

            if ((CAT_FORM > 0)
                && (!m_bot->HasAura(CAT_FORM, EFFECT_INDEX_0))
                && (ai->CastSpell(CAT_FORM)))
                return true;

            if ((MAIM > 0)
                && (m_bot->GetComboPoints() >= 1)
                && (pTarget->IsNonMeleeSpellCasted(true))
                && (ai->CastSpell(MAIM, *pTarget)))
                return true;

            if ((RAKE > 0)
                && (m_bot->GetComboPoints() <= 1)
                && (ai->GetEnergyAmount() >= 40)
                && (ai->CastSpell(RAKE, *pTarget)))
                return true;

            if ((CLAW > 0)
                && (m_bot->GetComboPoints() <= 2)
                && (ai->GetEnergyAmount() >= 45)
                && (ai->CastSpell(CLAW, *pTarget)))
                return true;

            if ((MANGLE > 0)
                && (m_bot->GetComboPoints() <= 3)
                && (ai->GetEnergyAmount() >= 45)
                && (ai->CastSpell(MANGLE, *pTarget)))
                return true;

            if ((CLAW > 0)
                && (m_bot->GetComboPoints() <= 4)
                && (ai->GetEnergyAmount() >= 45)
                && (ai->CastSpell(CLAW, *pTarget)))
                return true;

            if (m_bot->GetComboPoints() == 5)
            {
                if ((RIP > 0)
                    && (pTarget->getClass() == CLASS_ROGUE)
                    && (ai->GetEnergyAmount() >= 30)
                    && (ai->CastSpell(RIP, *pTarget)))
                    return true;

                if ((MAIM > 0)
                    && (pTarget->getClass() == CLASS_DRUID)
                    && (ai->GetEnergyAmount() >= 35)
                    && (ai->CastSpell(MAIM, *pTarget)))
                    return true;

                if ((MAIM > 0)
                    && (pTarget->getClass() == CLASS_SHAMAN)
                    && (ai->GetEnergyAmount() >= 35)
                    && (ai->CastSpell(MAIM, *pTarget)))
                    return true;

                if ((MAIM > 0)
                    && (pTarget->getClass() == CLASS_WARLOCK)
                    && (ai->GetEnergyAmount() >= 35)
                    && (ai->CastSpell(MAIM, *pTarget)))
                    return true;

                if ((FEROCIOUS_BITE > 0)
                    && (pTarget->getClass() == CLASS_HUNTER)
                    && (ai->GetEnergyAmount() >= 35)
                    && (ai->CastSpell(FEROCIOUS_BITE, *pTarget)))
                    return true;

                if ((FEROCIOUS_BITE > 0)
                    && (pTarget->getClass() == CLASS_WARRIOR)
                    && (ai->GetEnergyAmount() >= 35)
                    && (ai->CastSpell(FEROCIOUS_BITE, *pTarget)))
                    return true;

                if ((FEROCIOUS_BITE > 0)
                    && (pTarget->getClass() == CLASS_PALADIN)
                    && (ai->GetEnergyAmount() >= 35)
                    && (ai->CastSpell(FEROCIOUS_BITE, *pTarget)))
                    return true;

                if ((FEROCIOUS_BITE > 0)
                    && (pTarget->getClass() == CLASS_DEATH_KNIGHT)
                    && (ai->GetEnergyAmount() >= 25)
                    && (ai->CastSpell(FEROCIOUS_BITE, *pTarget)))
                    return true;

                if ((MAIM > 0)
                    && (pTarget->getClass() == CLASS_MAGE)
                    && (ai->GetEnergyAmount() >= 35)
                    && (ai->CastSpell(MAIM, *pTarget)))
                    return true;

                if ((MAIM > 0)
                    && (pTarget->getClass() == CLASS_PRIEST)
                    && (ai->GetEnergyAmount() >= 35)
                    && (ai->CastSpell(MAIM, *pTarget)))
                    return true;

                if ((MAIM > 0)
                    && (ai->GetEnergyAmount() >= 35)
                    && (ai->CastSpell(MAIM, *pTarget)))
                    return true;
            }

            DruidSpellCombat = 0;
            break;
    }

    return false;
}

bool PlayerbotDruidAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return false;

    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    if (m_bot->HasAura(CAT_FORM, EFFECT_INDEX_0))
        m_bot->RemoveAurasDueToSpell(768);
    if (m_bot->HasAura(BEAR_FORM, EFFECT_INDEX_0))
        m_bot->RemoveAurasDueToSpell(5487);
    if (m_bot->HasAura(DIRE_BEAR_FORM, EFFECT_INDEX_0))
        m_bot->RemoveAurasDueToSpell(9634);
    if (m_bot->HasAura(MOONKIN_FORM, EFFECT_INDEX_0))
        m_bot->RemoveAurasDueToSpell(24858);

    if ((MARK_OF_THE_WILD > 0)
        && (!m_bot->HasAura(MARK_OF_THE_WILD, EFFECT_INDEX_0))
        && (ai->CastSpell(MARK_OF_THE_WILD, *m_bot)))
        return true;

    if ((THORNS > 0)
        && (!m_bot->HasAura(THORNS, EFFECT_INDEX_0))
        && (ai->CastSpell(THORNS, *m_bot)))
        return true;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* drinkOrFoodItem = ai->FindDrink();
    Item* bandageItem = ai->FindBandage();

    if (drinkOrFoodItem
        && (ai->GetManaPercent() < 30))
    {
        ai->UseItem(*drinkOrFoodItem);
        ai->SetIgnoreUpdateTime(30);
        return true;
    }

    if (!drinkOrFoodItem
        && (INNERVATE > 0)
        && (!m_bot->HasAura(INNERVATE, EFFECT_INDEX_0))
        && (ai->GetManaPercent() <= 70)
        && (ai->CastSpell(INNERVATE, *m_bot)))
        return true;

    if (!drinkOrFoodItem
        && (MANA_REJUVENATION > 0)
        && (!m_bot->HasAura(INNERVATE, EFFECT_INDEX_0))
        && (!m_bot->HasAura(MANA_REJUVENATION, EFFECT_INDEX_0))
        && (ai->GetManaPercent() <= 70)
        && (ai->CastSpell(MANA_REJUVENATION, *m_bot)))
        return true;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    drinkOrFoodItem = ai->FindFood();

    if (drinkOrFoodItem
        && (ai->GetHealthPercent() < 30))
    {
        ai->UseItem(*drinkOrFoodItem);
        ai->SetIgnoreUpdateTime(30);
        return true;
    }

    if (!drinkOrFoodItem
        && bandageItem
        && (!m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0))
        && (ai->GetHealthPercent() < 70))
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

            if ((!tPlayer->HasAura(MARK_OF_THE_WILD, EFFECT_INDEX_0))
                && (ai->CastSpell(MARK_OF_THE_WILD, *tPlayer)))
                return true;
            if (HealTarget(*tPlayer, tPlayer->GetHealth() * 100 / tPlayer->GetMaxHealth()))
                return true;
        }
    }

    return false;
}
