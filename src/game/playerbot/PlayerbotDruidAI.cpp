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
    LastSpellDruidFeralCat = 0;
    LastSpellDruidFeralBear = 0;
    LastSpellDruidBalance = 0;
}

void PlayerbotDruidAI::ReinitCycles()
{
    LastSpellDruidFeralCat = 0;
    LastSpellDruidFeralBear = 0;
    LastSpellDruidBalance = 0;
}

void PlayerbotDruidAI::InitSpells(PlayerbotAI* const ai)
{
    MOONFIRE                      = ai->initSpell(MOONFIRE_1);
    STARFIRE                      = ai->initSpell(STARFIRE_1);
    STARFALL                      = ai->initSpell(STARFALL_1);
    WRATH                         = ai->initSpell(WRATH_1);
    ROOTS                         = ai->initSpell(ENTANGLING_ROOTS_1);
    INSECT_SWARM                  = ai->initSpell(INSECT_SWARM_1);
    FORCE_OF_NATURE               = ai->initSpell(FORCE_OF_NATURE_1);
    HURRICANE                     = ai->initSpell(HURRICANE_1);
    MARK_OF_THE_WILD              = ai->initSpell(MARK_OF_THE_WILD_1);
    GIFT_OF_THE_WILD              = ai->initSpell(GIFT_OF_THE_WILD_1);
    THORNS                        = ai->initSpell(THORNS_1);
    BARKSKIN                      = ai->initSpell(BARKSKIN_1);
    INNERVATE                     = ai->initSpell(INNERVATE_1);
    FAERIE_FIRE                   = ai->initSpell(FAERIE_FIRE_1);
    REJUVENATION                  = ai->initSpell(REJUVENATION_1);
    REGROWTH                      = ai->initSpell(REGROWTH_1);
    WILD_GROWTH                   = ai->initSpell(WILD_GROWTH_1);
    LIFEBLOOM                     = ai->initSpell(LIFEBLOOM_1);
    NOURISH                       = ai->initSpell(NOURISH_1);
    HEALING_TOUCH                 = ai->initSpell(HEALING_TOUCH_1);
    SWIFTMEND                     = ai->initSpell(SWIFTMEND_1);
    TRANQUILITY                   = ai->initSpell(TRANQUILITY_1);
    REVIVE                        = ai->initSpell(REVIVE_1);
    MOONKIN_FORM                  = ai->initSpell(MOONKIN_FORM_1);
    BEAR_FORM                     = ai->initSpell(BEAR_FORM_1);
    CAT_FORM                      = ai->initSpell(CAT_FORM_1);
    TREE_OF_LIFE                  = ai->initSpell(TREE_OF_LIFE_1);
    TRAVEL_FORM                   = ai->initSpell(TRAVEL_FORM_1);
    CLAW                          = ai->initSpell(CLAW_1);
    COWER                         = ai->initSpell(COWER_1);
    SAVAGE_ROAR                   = ai->initSpell(SAVAGE_ROAR_1);
    MANGLE_CAT                    = ai->initSpell(MANGLE_CAT_1);
    RAKE                          = ai->initSpell(RAKE_1);
    SHRED                         = ai->initSpell(SHRED_1);
    TIGERS_FURY                   = ai->initSpell(TIGERS_FURY_1);
    RIP                           = ai->initSpell(RIP_1);
    FEROCIOUS_BITE                = ai->initSpell(FEROCIOUS_BITE_1);
    BERSERK                       = ai->initSpell(BERSERK_1);
    PULVERIZE                     = ai->initSpell(PULVERIZE_1);
    MAIM                          = ai->initSpell(MAIM_1);
    BASH                          = ai->initSpell(BASH_1);
    MAUL                          = ai->initSpell(MAUL_1);
    SWIPE                         = ai->initSpell(SWIPE_BEAR_1);
    DEMORALIZING_ROAR             = ai->initSpell(DEMORALIZING_ROAR_1);
    CHALLENGING_ROAR              = ai->initSpell(CHALLENGING_ROAR_1);
    ENRAGE                        = ai->initSpell(ENRAGE_1);
    GROWL                         = ai->initSpell(GROWL_1);
    SURVIVAL_INSTINCTS            = ai->initSpell(SURVIVAL_INSTINCTS_1);
    RECENTLY_BANDAGED             = 11196;
    SHADOWMELD                    = ai->initSpell(SHADOWMELD_ALL);
    WAR_STOMP                     = ai->initSpell(WAR_STOMP_ALL);
    FAERIE_FIRE_FERAL             = ai->initSpell(FAERIE_FIRE_FERAL_1);
    MANGLE_BEAR                   = ai->initSpell(MANGLE_BEAR_1);
    LACERATE                      = ai->initSpell(LACERATE_1);
    SWIPE_BEAR                    = ai->initSpell(SWIPE_BEAR_1);
}

PlayerbotDruidAI::~PlayerbotDruidAI() {}

bool PlayerbotDruidAI::DoProtectSelfAction()
{
    PlayerbotAI *ai = GetAI();
    Player *m_bot = GetPlayerBot();

    if (ai->Cast(SURVIVAL_INSTINCTS, m_bot))
        return true;

    return false;
}

bool PlayerbotDruidAI::HealTarget()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    Unit* target = m_bot;
    float targethp = m_bot->GetHealthPercent();

    if (m_bot == m_master)
    {
        Unit* buffTarget = m_bot->SelectRandomPlayerToBuffHim(10.0f);
        if (buffTarget)
        {
            if (buffTarget->GetHealthPercent() < targethp)
            {
                target = buffTarget;
                targethp = buffTarget->GetHealthPercent();
            }
        }
    }
    else if (m_bot->GetGroup())
    {
        GroupReference *ref = NULL;
        ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            Player* pl = ref->getSource();
            if (pl!=m_bot && pl->isAlive() && !pl->duel)
            {
                if (pl->GetHealthPercent() < targethp)
                {
                    target = pl;
                    targethp = pl->GetHealthPercent();
                }
            }
            ref = ref->next();
        }
        ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            Player* pl = ref->getSource();
            if (pl!=m_bot && pl->isAlive() && !pl->duel && pl->GetPet())
            {
                if (pl->GetPet()->GetHealthPercent() < targethp)
                {
                    target = pl->GetPet();
                    targethp = pl->GetPet()->GetHealthPercent();
                }
            }
            ref = ref->next();
        }
    }

    if (!target || targethp > 70.0f)
        return false;

    if (targethp < 25 && ai->CastSpell(HEALING_TOUCH, target))
        return true;

    if (targethp < 30 && ai->CastSpell(NOURISH, target))
        return true;

    if (targethp < 45 && ai->Cast(WILD_GROWTH, target))
        return true;

    if (targethp < 50 && SWIFTMEND > 0 && (target->HasAura(REJUVENATION) || target->HasAura(REGROWTH)) && ai->CastSpell(SWIFTMEND, target))
        return true;

    if (targethp < 55 && ai->Cast(REGROWTH, target))
        return true;

    if (targethp < 60 && ai->Cast(LIFEBLOOM, target))
        return true;

    if (targethp < 70 && ai->Cast(REJUVENATION, target))
        return true;

    return false;
}

bool PlayerbotDruidAI::DoCombatManeuver(Unit *pTarget, bool cac)
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    if(ChangeForm())
        return true;

    switch (m_bot->getRole())
    {
        case DruidFeralCombat:
        {
            if (m_bot->HasAura(CAT_FORM))
            {
                static const uint32 SpellDruidFeralCat[] = {SAVAGE_ROAR, MANGLE_CAT, RAKE, SHRED, RIP,
                    FEROCIOUS_BITE, TIGERS_FURY, BERSERK, SAVAGE_ROAR};
                static uint32 elt = sizeof(SpellDruidFeralCat)/sizeof(uint32);

                for (uint32 i = 1; i <= elt; ++i)
                {
                    uint32 spellId = SpellDruidFeralCat[(i+LastSpellDruidFeralCat)%elt];
                    if (spellId==BERSERK && ((float)m_bot->GetPower(POWER_ENERGY)/(float)m_bot->GetMaxPower(POWER_ENERGY)) < 0.75f)
                       continue;
                    if (ai->CastSpell(spellId, pTarget))
                    {
                        LastSpellDruidFeralCat = (i+LastSpellDruidFeralCat)%elt;
                        return true;
                    }
                }
                break;
            }
            else
            {
                if (ai->Cast(FAERIE_FIRE_FERAL, pTarget))
                    return true;

                if (ai->Cast(DEMORALIZING_ROAR, pTarget))
                    return true;

                static const uint32 SpellDruidFeralBear[] = {MANGLE_BEAR, LACERATE, LACERATE, LACERATE,
                    SWIPE_BEAR, MANGLE_BEAR, PULVERIZE, TIGERS_FURY, BERSERK, SAVAGE_ROAR};
                static uint32 elt = sizeof(SpellDruidFeralBear)/sizeof(uint32);

                for (uint32 i = 1; i <= elt; ++i)
                {
                    uint32 spellId = SpellDruidFeralBear[(i+LastSpellDruidFeralBear)%elt];
                    if (spellId==BERSERK && ((float)m_bot->GetPower(POWER_ENERGY)/(float)m_bot->GetMaxPower(POWER_ENERGY)) < 0.75f)
                       continue;
                    if (ai->CastSpell(spellId, pTarget))
                    {
                        LastSpellDruidFeralBear = (i+LastSpellDruidFeralBear)%elt;
                        return true;
                    }
                }
                break;
            }
        }
        case DruidRestoration:
        {
            if (HealTarget())
                return true;
            break;
        }
        case DruidBalance:
        {
            if (ai->Cast(FAERIE_FIRE, pTarget))
                return true;

            if (ai->Cast(INSECT_SWARM, pTarget))
                return true;

            if (ai->Cast(MOONFIRE, pTarget))
                return true;

            static const uint32 SpellDruidBalance[] = {STARFALL, WRATH};
            static uint32 elt = sizeof(SpellDruidBalance)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                uint32 spellId = SpellDruidBalance[(i+LastSpellDruidBalance)%elt];
                if (m_bot->HasAura(48518) && STARFIRE) //Eclipse (Lunar)
                    spellId = STARFIRE;
                if (ai->CastSpell(spellId, pTarget))
                {
                    LastSpellDruidBalance = (i+LastSpellDruidBalance)%elt;
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

bool PlayerbotDruidAI::ChangeForm()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();

    switch (m_bot->getRole())
    {
        case DruidRestoration:
        {
            if (ai->Cast(TREE_OF_LIFE, m_bot))
                return true;
            break;
        }
        case DruidBalance:
        {
            if (ai->Cast(MOONKIN_FORM, m_bot))
                return true;
            break;
        }
        //case DruidFeralCombat:
        default:
            break;
    }


    uint8 WarProtection = 0, DruidFeral = 0, DruidFeralBear = 0;
    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (!ref->getSource()->IsInWorld() || !ref->getSource()->isAlive()
                || !m_bot->IsWithinDistInMap(ref->getSource(), MAX_DIST_POS_IN_GROUP))
            {
                ref = ref->next();
                continue;
            }

            switch(ref->getSource()->getRole())
            {
                case WarriorProtection:
                    WarProtection++;
                    break;
                case DruidFeralCombat:
                    if (ref->getSource()->HasAura(BEAR_FORM))
                        DruidFeralBear++;
                    DruidFeral++;
                    break;
                default:
                    break;
            }
            ref = ref->next();
        }
    }

    if (WarProtection > 0 || DruidFeralBear > 0)
    {
        if (ai->Cast(CAT_FORM, m_bot))
            return true;
    }
    else
    {
        if (ai->Cast(BEAR_FORM, m_bot))
            return true;
    }
    return false;
}

void PlayerbotDruidAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    m_bot->RemoveAurasDueToSpell(CAT_FORM_1);
    m_bot->RemoveAurasDueToSpell(BEAR_FORM_1);
    m_bot->RemoveAurasDueToSpell(TREE_OF_LIFE_1);
    m_bot->RemoveAurasDueToSpell(MOONKIN_FORM_1);

    if (ReviveTarget())
        return;

    if (BuffPlayer())
        return;

    if (HealTarget())
        return;
}

bool PlayerbotDruidAI::ReviveTarget()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    /*if (m_bot == m_master)
    {
        //TODO revive random player
    }
    else */if (m_bot->GetGroup())
    {
        GroupReference *ref = NULL;
        ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            Player* pl = ref->getSource();
            if (pl!=m_bot && !pl->isAlive())
            {
                if (ai->Buff(REVIVE, pl))
                    return true;
            }
            ref = ref->next();
        }
    }
    return false;
}

bool PlayerbotDruidAI::BuffPlayer()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    if (m_bot == m_master)
    {
        Unit* buffTarget = m_bot->SelectRandomPlayerToBuffHim(10.0f);
        if (buffTarget)
        {
            if (ai->Buff(MARK_OF_THE_WILD, buffTarget))
                return true;
            if (ai->Buff(THORNS, buffTarget))
                return true;
        }
    }
    else if (m_bot->GetGroup())
    {
        if (m_master->isAlive() && (!m_bot->HasAura(GIFT_OF_THE_WILD) || !m_master->HasAura(GIFT_OF_THE_WILD)) && ai->Buff(GIFT_OF_THE_WILD, m_master))
            return true;

        GroupReference *ref = NULL;
        ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            Player* pl = ref->getSource();
            if (pl!=m_bot && pl->isAlive() && !pl->duel)
            {
                if (!GIFT_OF_THE_WILD && ai->Buff(MARK_OF_THE_WILD, pl))
                    return true;
                if (ai->Buff(THORNS, pl))
                    return true;
            }
            ref = ref->next();
        }
        ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            Player* pl = ref->getSource();
            if (pl!=m_bot && pl->isAlive() && !pl->duel && pl->GetPet())
            {
                if (!GIFT_OF_THE_WILD && ai->Buff(MARK_OF_THE_WILD, pl->GetPet()))
                    return true;
                if (ai->Buff(THORNS, pl->GetPet()))
                    return true;
            }
            ref = ref->next();
        }
    }
    return false;
}
