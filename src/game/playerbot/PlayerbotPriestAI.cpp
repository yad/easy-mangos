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

bool PlayerbotPriestAI::HealTarget(Unit* target)
{
    PlayerbotAI* ai = GetAI();
    Player *m_bot = GetPlayerBot();
    uint8 hp = target->GetHealthPercent();

    if (target->isInCombat() && hp < 80 && !target->HasAura(WEAKENED_SOUL) && !target->HasAura(POWER_WORD_SHIELD) && ai->CastSpell(POWER_WORD_SHIELD, target))
        return true;

    if (hp < 40 && ai->CastSpell(FLASH_HEAL, target))
        return true;
    else if (hp < 60 && ai->CastSpell(GREATER_HEAL, target))
        return true;
    else if (hp < 80 && !target->HasAuraFromUnit(RENEW, m_bot) && ai->CastSpell(RENEW, target))
        return true;
    else
        return false;
} // end HealTarget

void PlayerbotPriestAI::DoCombatManeuver(Unit *pTarget, bool cac)
{
    Unit* pVictim = pTarget->getVictim();
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* m_master = ai->GetLeader();
    if (!m_master)
        return;

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
            Player *tank = sObjectMgr.GetPlayer(pTarget->GetTargetGuid());

            if (tank->isAlive() && tank->IsInSameRaidWith(m_bot) && tank->GetHealthPercent() < 80)
            {
                // Cast binding heal if target and caster have low health
                if (m_bot != tank && m_bot->GetHealthPercent() < 60 && ai->CastSpell(BINDING_HEAL, tank))
                    return;
                else if (HealTarget(tank))
                    return;
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
                    if (membersWithVeryLessHp > 2 && ai->CastSpell(CIRCLE_OF_HEALING, g_member))
                        return;
                }
                else if (membersWithLessHp > 2 && ai->CastSpell(PRAYER_OF_HEALING, g_member))
                    return;
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
                if (m_bot != g_member && m_bot->GetHealthPercent() < 60 && ai->CastSpell(BINDING_HEAL, g_member))
                    return;
                else if (HealTarget(g_member))
                    return;
            }

        }while (ref = (ref) ? ref->next() : NULL);
        break;

    case PriestDiscipline:
    case PriestShadow:
        ai->SetInFront(pTarget);
        static const uint32 SpellShadow[] = {SHADOW_WORD_PAIN, DEVOURING_PLAGUE, VAMPIRIC_TOUCH, MIND_BLAST};
        static const uint32 elt = sizeof(SpellShadow)/sizeof(uint32);
        char *SpellFirstTarget = "11110";
        char *SpellAllTargets = "10100";
        uint32 numberTargets = 0;
        uint32 numberTargetsWithin5f = 0;

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
                    if ((*itr)->GetHealthPercent() <= 35 && ai->CastSpell(SHADOW_WORD_DEATH, (*itr)))
                        return;
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
            if (ai->CastSpell(HOLY_NOVA))
                return;
        }

        if (!m_bot->HasAura(SHADOWFORM) && ai->SelfBuff(SHADOWFORM))
            return;

        if (numberTargets >= 5 && !pTarget->HasAuraFromUnit(MIND_SEAR, m_bot) && ai->CastSpell(MIND_SEAR, pTarget))
            return;

        // Casting spell cycle
        for (uint32 i = 0; i < elt; ++i)
        {
            // Cycle for main target
            if (SpellFirstTarget[i] == '1' && !pTarget->HasAuraFromUnit(SpellShadow[i], m_bot) && ai->CastSpell(SpellShadow[i], pTarget))
                return;

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
                         if (!(*itr)->HasAuraFromUnit(SpellShadow[i], m_bot) && ai->CastSpell(SpellShadow[i], (*itr)))
                             return;

                }while (ref = (ref) ? ref->next() : NULL);
            }
        }

        // If all targets are affected by damage over time, cast mind flay on main target
        if (!pTarget->HasAuraFromUnit(MIND_FLAY, m_bot) && ai->CastSpell(MIND_FLAY, pTarget))
            return;

        break;
    }
} // end DoCombatManeuver

void PlayerbotPriestAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    if (!ai)
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Player* m_master = ai->GetLeader();
    if (!m_master)
        return;

    if (m_bot->getRole() != PriestHoly)
    {
        if (!m_bot->HasAura(SHADOWFORM) && ai->SelfBuff(SHADOWFORM))
            return;
        if (!m_bot->HasAura(VAMPIRIC_EMBRACE) && ai->SelfBuff(VAMPIRIC_EMBRACE))
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
    if (ai->SelfBuff(INNER_FIRE))
        return;

    // buff and heal master's group
    if (m_master->GetGroup())
    {
        // Buff master with group buffs
        if (m_master->isAlive())
        {
            if (PRAYER_OF_FORTITUDE && ai->HasSpellReagents(PRAYER_OF_FORTITUDE) && ai->Buff(PRAYER_OF_FORTITUDE, m_master))
                return;

            if (PRAYER_OF_SPIRIT && ai->HasSpellReagents(PRAYER_OF_SPIRIT) && ai->Buff(PRAYER_OF_SPIRIT, m_master))
                return;
        }

        Group::MemberSlotList const& groupSlot = m_master->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || tPlayer == m_bot)
                continue;

            // first rezz em
            if (!tPlayer->isAlive())
            {
                if (!ai->CastSpell(RESURRECTION, tPlayer))
                    continue;
            }
            else
            {
                // buff and heal
                if (BuffPlayer(tPlayer))
                    return;

                if (HealTarget(tPlayer))
                    return;
            }
        }
    }
    else
    {
        if (m_master->isAlive())
        {
            if (BuffPlayer(m_master))
                return;
            if (HealTarget(m_master))
                return;
        }
        else
            ai->CastSpell(RESURRECTION, m_master);
    }

    BuffPlayer(m_bot);
} // end DoNonCombatActions

bool PlayerbotPriestAI::BuffPlayer(Player* target)
{
    PlayerbotAI * ai = GetAI();
    Pet * pet = target->GetPet();

    if (pet && pet->isAlive() && ai->Buff(POWER_WORD_FORTITUDE, pet))
        return true;

    if (target->isAlive() && ai->Buff(POWER_WORD_FORTITUDE, target))
        return true;

    if ((target->getClass() == CLASS_DRUID || target->getPowerType() == POWER_MANA) && target->isAlive() && ai->Buff(DIVINE_SPIRIT, target))
        return true;

    return false;
}
