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

#include "PlayerbotPriestAI.h"

class PlayerbotAI;

PlayerbotPriestAI::PlayerbotPriestAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

void PlayerbotPriestAI::InitSpells(PlayerbotAI* const ai)
{
    RENEW                         = ai->initSpell(RENEW_1);
    HEAL                          = ai->initSpell(HEAL_1);
    LESSER_HEAL                   = ai->initSpell(LESSER_HEAL_1);
    GREATER_HEAL                  = ai->initSpell(GREATER_HEAL_1);
    FLASH_HEAL                    = ai->initSpell(FLASH_HEAL_1);
    RESURRECTION                  = ai->initSpell(RESURRECTION_1);
    SMITE                         = ai->initSpell(SMITE_1);
    MANA_BURN                     = ai->initSpell(MANA_BURN_1);
    HOLY_NOVA                     = ai->initSpell(HOLY_NOVA_1);
    HOLY_FIRE                     = ai->initSpell(HOLY_FIRE_1);
    DESPERATE_PRAYER              = ai->initSpell(DESPERATE_PRAYER_1);
    PRAYER_OF_HEALING             = ai->initSpell(PRAYER_OF_HEALING_1);
    CIRCLE_OF_HEALING             = ai->initSpell(CIRCLE_OF_HEALING_1);
    BINDING_HEAL                  = ai->initSpell(BINDING_HEAL_1);
    PRAYER_OF_MENDING             = ai->initSpell(PRAYER_OF_MENDING_1);

    // SHADOW
    FADE                          = ai->initSpell(FADE_1);
    SHADOW_WORD_PAIN              = ai->initSpell(SHADOW_WORD_PAIN_1);
    MIND_BLAST                    = ai->initSpell(MIND_BLAST_1);
    SCREAM                        = ai->initSpell(PSYCHIC_SCREAM_1);
    MIND_FLAY                     = ai->initSpell(MIND_FLAY_1);
    DEVOURING_PLAGUE              = ai->initSpell(DEVOURING_PLAGUE_1);
    SHADOW_PROTECTION             = ai->initSpell(SHADOW_PROTECTION_1);
    VAMPIRIC_TOUCH                = ai->initSpell(VAMPIRIC_TOUCH_1);
    PRAYER_OF_SHADOW_PROTECTION   = ai->initSpell(PRAYER_OF_SHADOW_PROTECTION_1);
    SHADOWFIEND                   = ai->initSpell(SHADOWFIEND_1);
    MIND_SEAR                     = ai->initSpell(MIND_SEAR_1);

    // DISCIPLINE
    PENANCE                       = ai->initSpell(PENANCE_1);
    INNER_FIRE                    = ai->initSpell(INNER_FIRE_1);
    POWER_WORD_SHIELD             = ai->initSpell(POWER_WORD_SHIELD_1);
    POWER_WORD_FORTITUDE          = ai->initSpell(POWER_WORD_FORTITUDE_1);
    PRAYER_OF_FORTITUDE           = ai->initSpell(PRAYER_OF_FORTITUDE_1);
    FEAR_WARD                     = ai->initSpell(FEAR_WARD_1);
    DIVINE_SPIRIT                 = ai->initSpell(DIVINE_SPIRIT_1);
    PRAYER_OF_SPIRIT              = ai->initSpell(PRAYER_OF_SPIRIT_1);
    MASS_DISPEL                   = ai->initSpell(MASS_DISPEL_1);
    POWER_INFUSION                = ai->initSpell(POWER_INFUSION_1);
    INNER_FOCUS                   = ai->initSpell(INNER_FOCUS_1);

    RECENTLY_BANDAGED  = 11196; // first aid check

    // racial
    ARCANE_TORRENT                = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES);
    GIFT_OF_THE_NAARU             = ai->initSpell(GIFT_OF_THE_NAARU_PRIEST); // draenei
    STONEFORM                     = ai->initSpell(STONEFORM_ALL); // dwarf
    EVERY_MAN_FOR_HIMSELF         = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    SHADOWMELD                    = ai->initSpell(SHADOWMELD_ALL);
    BERSERKING                    = ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN          = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotPriestAI::~PlayerbotPriestAI() {}

bool PlayerbotPriestAI::HealTarget(Unit *target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return false;

//    return ((hp < 80 && !ai->HasAura(RENEW, target) &&  ai->CastSpell(RENEW, target)) ||
//        (hp < 60 && ai->CastSpell(HEAL, target)) ||
//        (hp < 30 && ai->CastSpell(FLASH_HEAL, target)) );

    if (hp < 25 && ai->GetManaPercent() >= 20 && ai->CastSpell(FLASH_HEAL, target))
        return true;
    else if (hp < 30 && ai->GetManaPercent() >= 36 && ai->CastSpell(GREATER_HEAL, target))
        return true;
    else if (hp < 33 && ai->GetManaPercent() >= 27 && ai->CastSpell(BINDING_HEAL, target))
        return true;
    else if (hp < 40 && ai->GetManaPercent() >= 54 && ai->CastSpell(PRAYER_OF_HEALING, target))
        return true;
    else if (hp < 50 && ai->GetManaPercent() >= 24 && ai->CastSpell(CIRCLE_OF_HEALING, target))
        return true;
    else if (hp < 60 && ai->GetManaPercent() >= 36 && ai->CastSpell(HEAL, target))
        return true;
    else if (hp < 80 && ai->GetManaPercent() >= 19 && ai->CastSpell(RENEW, target))
        return true;

    return false;
}

void PlayerbotPriestAI::DoNextCombatManeuver(Unit* pTarget)
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

    ai->SetMovementOrder(PlayerbotAI::MOVEMENT_FOLLOW, m_master);   // dont want to melee mob

    Group *m_group = m_bot->GetGroup();

    // Heal myself
    if (ai->GetHealthPercent() < 15 && !ai->HasAura(FADE, m_bot) && ai->CastSpell(FADE, m_bot))
        return;
    else if (ai->GetHealthPercent() < 25 && !ai->HasAura(POWER_WORD_SHIELD, m_bot) && ai->CastSpell(POWER_WORD_SHIELD))
        return;
    else if (ai->GetHealthPercent() < 35 && ai->CastSpell(DESPERATE_PRAYER, m_bot))
        return;
    else if (ai->GetHealthPercent() < 80 && HealTarget(m_bot, ai->GetHealthPercent()))
        return;

    // Heal master
    uint32 masterHP = m_master->GetHealth() * 100 / m_master->GetMaxHealth();
    if (m_master->isAlive())
    {
        if (masterHP < 25 && !ai->HasAura(POWER_WORD_SHIELD, m_master) && ai->CastSpell(POWER_WORD_SHIELD, m_master))
            return;
        else if (masterHP < 80 && HealTarget(m_master, masterHP))
            return;
    }

    // Heal group
    if (m_group)
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!m_groupMember || !m_groupMember->isAlive())
                continue;

            uint32 memberHP = m_groupMember->GetHealth() * 100 / m_groupMember->GetMaxHealth();
            if (memberHP < 25 && HealTarget(m_groupMember, memberHP))
                return;
        }
    }

    // Damage Spells
    ai->SetInFront(pTarget);
    float dist = m_bot->GetDistance(pTarget);

    switch (SpellSequence)
    {
        case SPELL_HOLY:
            if (LastSpellHoly < 1 && !ai->HasAura(SMITE, pTarget) && ai->GetManaPercent() >= 17 && ai->CastSpell(SMITE, pTarget))
            {
                SpellSequence = SPELL_SHADOWMAGIC;
                ++LastSpellHoly;
            }
            else if (LastSpellHoly < 2 && pTarget->GetPower(POWER_MANA) > 0 && ai->GetManaPercent() < 70 && ai->GetManaPercent() >= 14 && ai->CastSpell(MANA_BURN, pTarget))
            {
                ai->SetIgnoreUpdateTime(3);
                SpellSequence = SPELL_SHADOWMAGIC;
                ++LastSpellHoly;
            }
            else if (LastSpellHoly < 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 22 && ai->CastSpell(HOLY_NOVA))
            {
                SpellSequence = SPELL_SHADOWMAGIC;
                ++LastSpellHoly;
            }
            else if (LastSpellHoly < 4 && !ai->HasAura(HOLY_FIRE, pTarget) && ai->GetManaPercent() >= 13 && ai->CastSpell(HOLY_FIRE, pTarget))
            {
                SpellSequence = SPELL_SHADOWMAGIC;
                ++LastSpellHoly;
            }
            else if (LastSpellHoly < 5 && pVictim == m_master && m_master->GetHealth() <= m_master->GetMaxHealth() * 0.7 && !ai->HasAura(PRAYER_OF_MENDING, m_master) && ai->GetManaPercent() >= 15 && ai->CastSpell(PRAYER_OF_MENDING, m_master))
            {
                SpellSequence = SPELL_SHADOWMAGIC;
                ++LastSpellHoly;
            }
            else
            {
                SpellSequence = SPELL_SHADOWMAGIC;
                LastSpellHoly = 0;
            }

            return;

        case SPELL_SHADOWMAGIC:
            if (LastSpellShadowMagic < 1 && !ai->HasAura(SHADOW_WORD_PAIN, pTarget) && ai->GetManaPercent() >= 25 && ai->CastSpell(SHADOW_WORD_PAIN, pTarget))
            {
                SpellSequence = SPELL_DISCIPLINE;
                ++LastSpellShadowMagic;
            }
            else if (LastSpellShadowMagic < 2 && ai->GetManaPercent() >= 19 && ai->CastSpell(MIND_BLAST, pTarget))
            {
                SpellSequence = SPELL_DISCIPLINE;
                ++LastSpellShadowMagic;
            }
            else if (LastSpellShadowMagic < 3 && ai->GetAttackerCount() >= 3 && ai->GetManaPercent() >= 15 && ai->CastSpell(SCREAM))
            {
                SpellSequence = SPELL_DISCIPLINE;
                ++LastSpellShadowMagic;
            }
            else if (LastSpellShadowMagic < 4 && !ai->HasAura(MIND_FLAY, pTarget) && ai->GetManaPercent() >= 10 && ai->CastSpell(MIND_FLAY, pTarget))
            {
                ai->SetIgnoreUpdateTime(3);
                SpellSequence = SPELL_DISCIPLINE;
                ++LastSpellShadowMagic;
            }
            else if (LastSpellShadowMagic < 5 && !ai->HasAura(DEVOURING_PLAGUE, pTarget) && ai->GetManaPercent() >= 28 && ai->CastSpell(DEVOURING_PLAGUE, pTarget))
            {
                SpellSequence = SPELL_DISCIPLINE;
                ++LastSpellShadowMagic;
            }
            else if (LastSpellShadowMagic < 6 && ai->GetManaPercent() >= 60 && ai->CastSpell(SHADOW_PROTECTION, pTarget))
            {
                SpellSequence = SPELL_DISCIPLINE;
                ++LastSpellShadowMagic;
            }
            else if (LastSpellShadowMagic < 7 && !ai->HasAura(VAMPIRIC_TOUCH, pTarget) && ai->GetManaPercent() >= 18 && ai->CastSpell(VAMPIRIC_TOUCH, pTarget))
            {
                SpellSequence = SPELL_DISCIPLINE;
                ++LastSpellShadowMagic;
            }
            else if (LastSpellShadowMagic < 8 && ai->CastSpell(SHADOWFIEND))
            {
                SpellSequence = SPELL_DISCIPLINE;
                ++LastSpellShadowMagic;
            }
            else if (LastSpellShadowMagic < 9 && ai->GetAttackerCount() >= 3 && ai->GetManaPercent() >= 28 && ai->CastSpell(MIND_SEAR, pTarget))
            {
                ai->SetIgnoreUpdateTime(5);
                SpellSequence = SPELL_DISCIPLINE;
                ++LastSpellShadowMagic;
            }
            else
            {
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic = 0;
            }

            return;

        case SPELL_DISCIPLINE:
            if (LastSpellDiscipline < 1 && ai->GetManaPercent() >= 3 && ai->CastSpell(FEAR_WARD, m_master))
            {
                SpellSequence = SPELL_HOLY;
                ++LastSpellDiscipline;
            }
            else if (LastSpellDiscipline < 2 && ai->GetManaPercent() >= 16 && ai->CastSpell(POWER_INFUSION, m_master))
            {
                SpellSequence = SPELL_HOLY;
                ++LastSpellDiscipline;
            }
            else if (LastSpellDiscipline < 3 && ai->GetManaPercent() >= 33 && ai->CastSpell(MASS_DISPEL))
            {
                SpellSequence = SPELL_HOLY;
                ++LastSpellDiscipline;
            }
            else if (!ai->HasAura(INNER_FOCUS, m_bot) && LastSpellDiscipline < 4 && ai->CastSpell(INNER_FOCUS, m_bot))
            {
                SpellSequence = SPELL_HOLY;
                ++LastSpellDiscipline;
            }
            else if (LastSpellDiscipline < 5 && ai->GetManaPercent() >= 16 && ai->CastSpell(PENANCE))
            {
                SpellSequence = SPELL_HOLY;
                ++LastSpellDiscipline;
            }
            else
            {
                SpellSequence = SPELL_HOLY;
                LastSpellDiscipline = 0;
            }

            return;
    }
}

void PlayerbotPriestAI::DoNonCombatActions()
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

    Item* pItemDrink = ai->FindDrink();
    Item* pItemFood = ai->FindFood();
    Item* pItemBandage = ai->FindBandage();

    SpellSequence = SPELL_HOLY;

    // buff myself
    if (!ai->HasAura(POWER_WORD_FORTITUDE, m_bot) && ai->CastSpell(POWER_WORD_FORTITUDE, m_bot))
        return;
    else if (!ai->HasAura(INNER_FIRE, m_bot) && ai->CastSpell(INNER_FIRE, m_bot))
        return;

    // buff master
    else if (!ai->HasAura(POWER_WORD_FORTITUDE, m_master) && ai->CastSpell(POWER_WORD_FORTITUDE, m_master))
        return;

    // mana check
    if (pItemDrink && ai->GetManaPercent() < 30)
    {
        ai->UseItem(*pItemDrink);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
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
            if (!tPlayer)
                continue;

            // first rezz em
            if (!tPlayer->isAlive())
            {
                // rez is only 10 sec, but give time for lag
                if (ai->CastSpell(RESURRECTION, tPlayer))
                    ai->SetIgnoreUpdateTime(12);
            }
            else if (tPlayer->isAlive())
            {
                // buff and heal
                if (!ai->HasAura(POWER_WORD_FORTITUDE, tPlayer) && ai->CastSpell(POWER_WORD_FORTITUDE, tPlayer))
                    return;
                else if (HealTarget(tPlayer, tPlayer->GetHealth() * 100 / tPlayer->GetMaxHealth()))
                    return;
            }
        }
    }
}
