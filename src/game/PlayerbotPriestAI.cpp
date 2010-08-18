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

PlayerbotPriestAI::~PlayerbotPriestAI() {}

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

bool PlayerbotPriestAI::HealTarget(Unit &target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    if ((hp < 25)
        && (FLASH_HEAL > 0)
        && (ai->GetManaPercent() >= 20)
        && (ai->CastSpell(FLASH_HEAL, target)))
        return true;

    else if ((hp < 30)
        && (GREATER_HEAL > 0)
        && (ai->GetManaPercent() >= 36)
        && (ai->CastSpell(GREATER_HEAL, target)))
        return true;

    else if ((hp < 33)
        && (BINDING_HEAL > 0)
        && (ai->GetManaPercent() >= 27)
        && (ai->CastSpell(BINDING_HEAL, target)))
        return true;

    else if ((hp < 40)
        && (PRAYER_OF_HEALING > 0)
        && (ai->GetManaPercent() >= 54)
        && (ai->CastSpell(PRAYER_OF_HEALING, target)))
        return true;

    else if ((hp < 50)
        && (CIRCLE_OF_HEALING > 0)
        && (ai->GetManaPercent() >= 24)
        && (ai->CastSpell(CIRCLE_OF_HEALING, target)))
        return true;

    else if ((hp < 60)
        && (HEAL > 0)
        && (ai->GetManaPercent() >= 36)
        && (ai->CastSpell(HEAL, target)))
        return true;

    else if ((hp < 80)
        && (RENEW > 0)
        && (ai->GetManaPercent() >= 19)
        && (ai->CastSpell(RENEW, target)))
        return true;

    return false;
}

bool PlayerbotPriestAI::DoNextCombatManeuver(Unit *pTarget)
{
    Unit* pVictim = pTarget->getVictim();

    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    ai->SetMovementOrder(PlayerbotAI::MOVEMENT_FOLLOW, pMaster);

    Player *m_bot = GetPlayerBot();
    Group *m_group = m_bot->GetGroup();

    if ((ai->GetHealthPercent() < 15)
        && (FADE > 0)
        && (!m_bot->HasAura(FADE, EFFECT_INDEX_0))
        && (ai->CastSpell(FADE, *m_bot)))
        return true;

    if ((ai->GetHealthPercent() < 25)
        && (POWER_WORD_SHIELD > 0)
        && (!m_bot->HasAura(POWER_WORD_SHIELD, EFFECT_INDEX_0))
        && (ai->CastSpell(POWER_WORD_SHIELD)))
        return true;

    if ((ai->GetHealthPercent() < 35)
        && (DESPERATE_PRAYER > 0)
        && (ai->CastSpell(DESPERATE_PRAYER, *m_bot)))
        return true;

    if ((ai->GetHealthPercent() < 80)
        && HealTarget(*m_bot, ai->GetHealthPercent()))
        return true;

    uint32 masterHP = pMaster->GetHealth() * 100 / pMaster->GetMaxHealth();
    if (pMaster->isAlive())
    {
        if ((masterHP < 25)
            && (POWER_WORD_SHIELD > 0)
            && (!pMaster->HasAura(POWER_WORD_SHIELD, EFFECT_INDEX_0))
            && (ai->CastSpell(POWER_WORD_SHIELD, *pMaster)))
            return true;

        if ((masterHP < 80)
            && HealTarget(*pMaster, masterHP))
            return true;
    }

    if (m_group)
    {
        Group::MemberSlotList const& groupSlot = m_group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *m_groupMember = sObjectMgr.GetPlayer(itr->guid);
            if (!m_groupMember || !m_groupMember->isAlive())
                continue;

            uint32 memberHP = m_groupMember->GetHealth() * 100 / m_groupMember->GetMaxHealth();
            if ((memberHP < 25)
                && HealTarget(*m_groupMember, memberHP))
                return true;
        }
    }

    ai->SetInFront(pTarget);
    float dist = m_bot->GetDistance(pTarget);

    switch (SpellSequence)
    {
        case SPELL_HOLY:

            if ((SMITE > 0)
                && (LastSpellHoly < 1)
                && (!pTarget->HasAura(SMITE, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 17)
                && (ai->CastSpell(SMITE, *pTarget)))
            {
                SpellSequence = SPELL_SHADOWMAGIC;
                LastSpellHoly++;
                return true;
            }

            if ((MANA_BURN > 0)
                && (LastSpellHoly < 2)
                && (pTarget->GetPower(POWER_MANA) > 0)
                && (ai->GetManaPercent() < 70)
                && (ai->GetManaPercent() >= 14)
                && (ai->CastSpell(MANA_BURN, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(3);
                SpellSequence = SPELL_SHADOWMAGIC;
                LastSpellHoly++;
                return true;
            }

            if ((HOLY_NOVA > 0)
                && (LastSpellHoly < 3)
                && (dist <= ATTACK_DISTANCE)
                && (ai->GetManaPercent() >= 22)
                && (ai->CastSpell(HOLY_NOVA)))
            {
                SpellSequence = SPELL_SHADOWMAGIC;
                LastSpellHoly++;
                return true;
            }

            if ((HOLY_FIRE > 0)
                && (LastSpellHoly < 4)
                && (!pTarget->HasAura(HOLY_FIRE, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 13)
                && (ai->CastSpell(HOLY_FIRE, *pTarget)))
            {
                SpellSequence = SPELL_SHADOWMAGIC;
                LastSpellHoly++;
                return true;
            }

            if ((PRAYER_OF_MENDING > 0)
                && (LastSpellHoly < 5)
                && (pVictim == pMaster)
                && (pMaster->GetHealth() <= pMaster->GetMaxHealth() * 0.7)
                && (!pMaster->HasAura(PRAYER_OF_MENDING, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 15)
                && (ai->CastSpell(PRAYER_OF_MENDING, *pMaster)))
            {
                SpellSequence = SPELL_SHADOWMAGIC;
                LastSpellHoly++;
                return true;
            }

             LastSpellHoly = 0;
             SpellSequence = SPELL_SHADOWMAGIC;
             break;

        case SPELL_SHADOWMAGIC:
            if ((SHADOW_WORD_PAIN > 0)
                && (LastSpellShadowMagic < 1)
                && (!pTarget->HasAura(SHADOW_WORD_PAIN, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 25)
                && (ai->CastSpell(SHADOW_WORD_PAIN, *pTarget)))
            {
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic++;
                return true;
            }

            if ((MIND_BLAST > 0)
                && (LastSpellShadowMagic < 2)
                && (ai->GetManaPercent() >= 19)
                && (ai->CastSpell(MIND_BLAST, *pTarget)))
            {
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic++;
                return true;
            }

           if ((SCREAM > 0)
                && (LastSpellShadowMagic < 3)
                && (ai->GetAttackerCount() >= 3)
                && (ai->GetManaPercent() >= 15)
                && (ai->CastSpell(SCREAM)))
            {
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic++;
                return true;
            }

            if ((MIND_FLAY > 0)
                && (LastSpellShadowMagic < 4)
                && (!pTarget->HasAura(MIND_FLAY, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 10)
                && (ai->CastSpell(MIND_FLAY, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(3);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic++;
                return true;
            }

            if ((DEVOURING_PLAGUE > 0)
                && (LastSpellShadowMagic < 5)
                && (!pTarget->HasAura(DEVOURING_PLAGUE, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 28)
                && (ai->CastSpell(DEVOURING_PLAGUE, *pTarget)))
            {
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic++;
                return true;
            }

            if ((SHADOW_PROTECTION > 0)
                && (LastSpellShadowMagic < 6)
                && (ai->GetManaPercent() >= 60)
                && (ai->CastSpell(SHADOW_PROTECTION, *pTarget)))
            {
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic++;
                return true;
            }

            if ((VAMPIRIC_TOUCH > 0)
                && (LastSpellShadowMagic < 7)
                && (!pTarget->HasAura(VAMPIRIC_TOUCH, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 18)
                && (ai->CastSpell(VAMPIRIC_TOUCH, *pTarget)))
            {
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic++;
                return true;
            }

            if ((SHADOWFIEND > 0)
                && (LastSpellShadowMagic < 8)
                && (ai->CastSpell(SHADOWFIEND)))
            {
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic++;
                return true;
            }

            if ((MIND_SEAR > 0)
                && (LastSpellShadowMagic < 9)
                && (ai->GetAttackerCount() >= 3)
                && (ai->GetManaPercent() >= 28)
                && (ai->CastSpell(MIND_SEAR, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(5);
                SpellSequence = SPELL_DISCIPLINE;
                LastSpellShadowMagic++;
                return true;
            }

            LastSpellShadowMagic = 0;
            SpellSequence = SPELL_DISCIPLINE;
            break;

        case SPELL_DISCIPLINE:
            if ((FEAR_WARD > 0)
                && (LastSpellDiscipline < 1)
                && (ai->GetManaPercent() >= 3)
                && (ai->CastSpell(FEAR_WARD, *pMaster)))
            {
                SpellSequence = SPELL_HOLY;
                LastSpellDiscipline++;
                return true;
            }

            if ((POWER_INFUSION > 0)
                && (LastSpellDiscipline < 2)
                && (ai->GetManaPercent() >= 16)
                && (ai->CastSpell(POWER_INFUSION, *pMaster)))
            {
                SpellSequence = SPELL_HOLY;
                LastSpellDiscipline++;
                return true;
            }

            if ((MASS_DISPEL > 0)
                && (LastSpellDiscipline < 3)
                && (ai->GetManaPercent() >= 33)
                && (ai->CastSpell(MASS_DISPEL)))
            {
                SpellSequence = SPELL_HOLY;
                LastSpellDiscipline++;
                return true;
            }

            if ((INNER_FOCUS > 0)
                && (!m_bot->HasAura(INNER_FOCUS, EFFECT_INDEX_0))
                && (LastSpellDiscipline < 4)
                && (ai->CastSpell(INNER_FOCUS, *m_bot)))
            {
                SpellSequence = SPELL_HOLY;
                LastSpellDiscipline++;
                return true;
            }

            if ((PENANCE > 0)
                && (LastSpellDiscipline < 5)
                && (ai->GetManaPercent() >= 16)
                && (ai->CastSpell(PENANCE)))
            {
                SpellSequence = SPELL_HOLY;
                LastSpellDiscipline++;
                return true;
            }

            LastSpellDiscipline++;
            SpellSequence = SPELL_HOLY;
    }

    return false;
}

bool PlayerbotPriestAI::DoNonCombatActions()
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

    SpellSequence = SPELL_HOLY;

    if ((POWER_WORD_FORTITUDE > 0)
        && (!m_bot->HasAura(POWER_WORD_FORTITUDE, EFFECT_INDEX_0)
        && ai->CastSpell(POWER_WORD_FORTITUDE, *m_bot)))
        return true;

    if ((INNER_FIRE > 0)
        && (!m_bot->HasAura(INNER_FIRE, EFFECT_INDEX_0))
        && (ai->CastSpell(INNER_FIRE, *m_bot)))
        return true;

    if ((POWER_WORD_FORTITUDE > 0)
        && (!pMaster->HasAura(POWER_WORD_FORTITUDE, EFFECT_INDEX_0))
        && (ai->CastSpell(POWER_WORD_FORTITUDE, * (pMaster))))
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
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer)
                continue;

            if ((!tPlayer->isAlive())
                && (!tPlayer->GetPlayerbotAI())
                && (ai->CastSpell(RESURRECTION, *tPlayer)))
            {
                ai->SetIgnoreUpdateTime(17);
                return true;
            }

            if (tPlayer->isAlive())
            {
                if ((!tPlayer->HasAura(POWER_WORD_FORTITUDE, EFFECT_INDEX_0))
                   && (ai->CastSpell(POWER_WORD_FORTITUDE, *tPlayer)))
                    return true;
                if (HealTarget(*tPlayer, tPlayer->GetHealth() * 100 / tPlayer->GetMaxHealth()))
                    return true;
            }
        }
    }

    return false;
}
