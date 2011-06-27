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
    SHADOWFORM                    = ai->initSpell(SHADOWFORM_1);
    VAMPIRIC_EMBRACE              = ai->initSpell(VAMPIRIC_EMBRACE_1);
    SHADOW_WORD_DEATH             = ai->initSpell(SHADOW_WORD_DEATH_1);
    DISPERSION                    = ai->initSpell(DISPERSION_1);

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
    PAIN_SUPPRESSION              = ai->initSpell(PAIN_SUPPRESSION_1);

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

bool PlayerbotPriestAI::DoProtectSelfAction()
{
    PlayerbotAI *ai = GetAI();
    Player *m_bot = GetPlayerBot();
    
    if (m_bot->GetHealthPercent() < 40 && ai->Cast(DESPERATE_PRAYER))
        return true;
    
    static const uint32 spells[] = {DISPERSION, PAIN_SUPPRESSION, POWER_WORD_SHIELD};
    static uint32 elt = sizeof(spells) / sizeof(uint32);

    for (uint32 i = 0; i < elt; i++)
        if (ai->Cast(spells[i]))
            return true;

    return false;
}

bool PlayerbotPriestAI::HealTarget(Unit* target)
{
    PlayerbotAI* ai = GetAI();
    Player *m_bot = GetPlayerBot();
    uint8 hp = target->GetHealthPercent();

    if (target->isInCombat() && hp < 80 && ai->Cast(POWER_WORD_SHIELD, target))
        return true;

    if (hp < 40 && ai->Cast(FLASH_HEAL, target))
        return true;
    if (hp < 60 && ai->Cast(GREATER_HEAL, target))
        return true;
    if (hp < 80 && ai->Cast(RENEW, target, IF_TARGET_HAS_NOT_AURA_FROM_CURRENT_CASTER))
        return true;
    
    return false;
}

bool PlayerbotPriestAI::DoCombatManeuver(Unit *pTarget, bool cac)
{
    //Unit* pVictim = pTarget->getVictim();
    PlayerbotAI* ai = GetAI();
    Player* m_master = ai->GetLeader();
    Player *m_bot = GetPlayerBot();
    Group *m_group = m_bot->GetGroup();
    GroupReference *ref = (m_group) ? m_group->GetFirstMember() : NULL;
    ObjectGuid const targetOfTarget = pTarget->GetTargetGuid();
    uint32 membersWithLessHp = 0;
    uint32 membersWithVeryLessHp = 0;

    switch (m_bot->getRole())
    {
    case PriestHoly:
        // High priority to target of target
        if (targetOfTarget.IsPlayer())
        {
            Player *tank = sObjectMgr.GetPlayer(targetOfTarget);

            if (tank->isAlive() && tank->IsInSameRaidWith(m_bot) && tank->GetHealthPercent() < 80)
            {
                // Cast binding heal if target and caster have low health
                if (m_bot != tank && m_bot->GetHealthPercent() < 60 && ai->Cast(BINDING_HEAL, tank))
                    return true;
                else if (HealTarget(tank))
                    return true;
            }
        }

        // Next, priority to healing multiple-target spells
        do
        {
            Player *g_member = (ref) ? ref->getSource() : m_bot;

            if (!g_member->isAlive())
                continue;

            if (g_member->GetHealthPercent() < 80)
            {
                membersWithLessHp++;

                if (g_member->GetHealthPercent() < 60)
                {
                    membersWithVeryLessHp++;
                    if (membersWithVeryLessHp > 2 && ai->Cast(CIRCLE_OF_HEALING, g_member))
                        return true;
                }
                else if (membersWithLessHp > 2 && ai->Cast(PRAYER_OF_HEALING, g_member))
                    return true;
            }

        }while (ref = (ref) ? ref->next() : NULL);

        // At last, heal only members who need to be healed
        ref = (m_group) ? m_group->GetFirstMember() : NULL;
        do
        {
            Player *g_member = (ref) ? ref->getSource() : m_bot;

            if (!g_member->isAlive())
                continue;

            if (g_member->GetHealthPercent() < 80)
            {
                // Cast binding heal if target and caster have low health
                if (m_bot != g_member && m_bot->GetHealthPercent() < 60 && ai->Cast(BINDING_HEAL, g_member))
                    return true;
                else if (HealTarget(g_member))
                    return true;
            }

        }while (ref = (ref) ? ref->next() : NULL);
        break;

    case PriestDiscipline:
    case PriestShadow:
        static const uint32 SpellShadow[] = {SHADOW_WORD_PAIN, DEVOURING_PLAGUE, VAMPIRIC_TOUCH, MIND_BLAST};
        static const uint32 elt = sizeof(SpellShadow)/sizeof(uint32);
        char *SpellFirstTarget = "1111";
        char *SpellAllTargets = "1010";
        uint32 numberTargets = 0;
        uint32 numberTargetsWithin5f = 0;

        /*
        [Chjuci] : Petit exemple de l'utilisation de la nouvelle méthode Cast()

        La ligne suivante va faire en sorte que le bot lance le sort MIND_BLAST(attaque mentale) à condition qu'il y ai 
        l'aura du sort POWER_INFUSION(infusion de puissance) sur le lanceur de sort.

        ai->Cast(MIND_BLAST, pTarget, NO_CONDITION, POWER_INFUSION, IF_CASTER_HAS_AURA);

        En d'autres termes, la ligne ci dessus ne lancera le sort que si on aura exécuté une ligne semblable à la suivante au préalable :
        ai->Cast(POWER_INFUSION);

        PS: par défaut le 2e argument (target) est le lanceur de sort(m_bot)
            par défaut le 3e argument (condition numéro 1) est IF_TARGET_HAS_NOT_AURA qui est la condition qu'on utilise le + souvent
        */


        // Count number of targets
        do
        {
            Player *g_member = (ref) ? ref->getSource() : m_bot;

            if (!g_member->isAlive())
                continue;

            for (Unit::AttackerSet::const_iterator itr = g_member->getAttackers().begin(); itr != g_member->getAttackers().end(); itr++)
            {
                if ((*itr)->IsWithinDist(m_bot, 36.0f))
                {
                    numberTargets++;
                    if ((*itr)->GetHealthPercent() <= 35 && ai->Cast(SHADOW_WORD_DEATH, (*itr)))
                        return true;
                }
                if ((*itr)->IsWithinDist(m_bot, 5.0f))
                    numberTargetsWithin5f++;
            }

        }while (ref = (ref) ? ref->next() : NULL);

        // Spells with Area of Effect
        if (numberTargetsWithin5f >= 5 && !MIND_SEAR)
        {
            if (m_bot->HasAura(SHADOWFORM))
                m_bot->RemoveAurasDueToSpell(SHADOWFORM);
            if (ai->Cast(HOLY_NOVA))
                return true;
        }

        if (ai->Cast(SHADOWFORM))
            return true;

        if (numberTargets >= 5 && ai->Cast(MIND_SEAR, pTarget, IF_TARGET_HAS_NOT_AURA_FROM_CURRENT_CASTER))
            return true;

        // Casting spell cycle
        for (uint32 i = 0; i < elt; ++i)
        {
            // Cycle for main target
            if (SpellFirstTarget[i] == '1' && ai->Cast(SpellShadow[i], pTarget, IF_TARGET_HAS_NOT_AURA_FROM_CURRENT_CASTER))
                return true;

            // Cycle for others targets
            if (SpellAllTargets[i] == '1' && numberTargets > 1)
            {
                ref = (m_group) ? m_group->GetFirstMember() : NULL;
                do
                {
                    Player *g_member = (ref) ? ref->getSource() : m_bot;

                    if (!g_member->isAlive())
                        continue;

                    for (Unit::AttackerSet::const_iterator itr = g_member->getAttackers().begin(); itr != g_member->getAttackers().end(); itr++)
                         if (ai->Cast(SpellShadow[i], (*itr), IF_TARGET_HAS_NOT_AURA_FROM_CURRENT_CASTER))
                             return true;

                }while (ref = (ref) ? ref->next() : NULL);
            }
        }

        // If all targets are affected by damage over time, cast mind flay on main target
        if (ai->Cast(MIND_FLAY, pTarget, IF_TARGET_HAS_NOT_AURA_FROM_CURRENT_CASTER))
            return true;

        break;
    }
    return false;
}

void PlayerbotPriestAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    Player *m_bot = GetPlayerBot();
    Player *m_master = ai->GetLeader();
    Group *m_group = m_bot->GetGroup();
    GroupReference *ref = (m_group) ? m_group->GetFirstMember() : NULL;

    if (m_bot->getRole() != PriestHoly)
    {
        if (ai->Cast(SHADOWFORM))
            return;
        if (ai->Cast(VAMPIRIC_EMBRACE))
            return;
    }

    static const uint32 MinorGlyphs[] = {58009, 58015, 58228};           // power word fortitude - shadow protection - ombrefiel
    static const uint32 ShadowMajorGlyphs[] = {55681, 55682, 55687};     // shadow word pain - shadow word death - mind flay
    static const uint32 HolyMajorGlyphs[] = {55674, 55672, 55680};       // renew - power word shield - prayer of healing

    for (uint32 i = 0; i < 3; i++)
    {
        if (!m_bot->HasSpell(MinorGlyphs[i]))
            m_bot->learnSpell(MinorGlyphs[i], true);

        if (m_bot->HasSpell(ShadowMajorGlyphs[i]) && m_bot->getRole() == PriestHoly)
            m_bot->removeSpell(ShadowMajorGlyphs[i]);

        if (m_bot->HasSpell(HolyMajorGlyphs[i]) && m_bot->getRole() != PriestHoly)
            m_bot->removeSpell(HolyMajorGlyphs[i]);

        if (!m_bot->HasSpell(ShadowMajorGlyphs[i]) && m_bot->getRole() != PriestHoly)
            m_bot->learnSpell(ShadowMajorGlyphs[i], true);

        if (!m_bot->HasSpell(HolyMajorGlyphs[i]) && m_bot->getRole() == PriestHoly)
            m_bot->learnSpell(HolyMajorGlyphs[i], true);
    }

    // selfbuff goes first
    if (ai->Cast(INNER_FIRE))
        return;

    // buff and heal group
    ref = (m_group) ? m_group->GetFirstMember() : NULL;
    do
    {
        Player *g_member = (ref) ? ref->getSource() : m_bot;

        if (!g_member->isAlive())
            if (ai->Cast(RESURRECTION, g_member))
                return;

        if (HealTarget(g_member))
            return;

        if (BuffPlayer(g_member))
            return;

    }while (ref = (ref) ? ref->next() : NULL);


    // Buff master with group buffs
    if (m_group && m_master->isAlive())
    {
        if (ai->Cast(PRAYER_OF_FORTITUDE, m_master) || ai->Cast(PRAYER_OF_FORTITUDE))
            return;

        if (ai->Cast(PRAYER_OF_SPIRIT, m_master) || ai->Cast(PRAYER_OF_SPIRIT))
            return;
    }

    return;
}

bool PlayerbotPriestAI::BuffPlayer(Player* target)
{
    PlayerbotAI *ai = GetAI();
    Pet *pet = target->GetPet();

    if (!PRAYER_OF_FORTITUDE)
    {
        if (pet && pet->isAlive() && ai->Cast(POWER_WORD_FORTITUDE, pet))
            return true;

        if (target->isAlive() && ai->Cast(POWER_WORD_FORTITUDE, target))
            return true;
    }

    if (!PRAYER_OF_SPIRIT && (target->getClass() == CLASS_DRUID || target->getPowerType() == POWER_MANA) && target->isAlive() && ai->Cast(DIVINE_SPIRIT, target))
        return true;

    return false;
}
