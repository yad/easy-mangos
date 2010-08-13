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

#include "PlayerbotWarlockAI.h"

class PlayerbotAI;

PlayerbotWarlockAI::PlayerbotWarlockAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
    m_demonSummonFailed = false;
}

PlayerbotWarlockAI::~PlayerbotWarlockAI() {}

void PlayerbotWarlockAI::InitSpells(PlayerbotAI* const ai)
{
    // DESTRUCTION
    SHADOW_BOLT           = ai->initSpell(SHADOW_BOLT_1);
    IMMOLATE              = ai->initSpell(IMMOLATE_1);
    INCINERATE            = ai->initSpell(INCINERATE_1);
    SEARING_PAIN          = ai->initSpell(SEARING_PAIN_1);
    CONFLAGRATE           = ai->initSpell(CONFLAGRATE_1);
    SHADOWFURY            = ai->initSpell(SHADOWFURY_1);
    CHAOS_BOLT            = ai->initSpell(CHAOS_BOLT_1);
    SHADOWFLAME           = ai->initSpell(SHADOWFLAME_1);
    HELLFIRE              = ai->initSpell(HELLFIRE_1);
    RAIN_OF_FIRE          = ai->initSpell(RAIN_OF_FIRE_1);
    SOUL_FIRE             = ai->initSpell(SOUL_FIRE_1); // soul shard spells
    SHADOWBURN            = ai->initSpell(SHADOWBURN_1);
    // CURSE
    CURSE_OF_WEAKNESS     = ai->initSpell(CURSE_OF_WEAKNESS_1);
    CURSE_OF_THE_ELEMENTS = ai->initSpell(CURSE_OF_THE_ELEMENTS_1);
    CURSE_OF_AGONY        = ai->initSpell(CURSE_OF_AGONY_1);
    CURSE_OF_EXHAUSTION   = ai->initSpell(CURSE_OF_EXHAUSTION_1);
    CURSE_OF_TONGUES      = ai->initSpell(CURSE_OF_TONGUES_1);
    CURSE_OF_DOOM         = ai->initSpell(CURSE_OF_DOOM_1);
    // AFFLICTION
    CORRUPTION            = ai->initSpell(CORRUPTION_1);
    DRAIN_SOUL            = ai->initSpell(DRAIN_SOUL_1);
    DRAIN_LIFE            = ai->initSpell(DRAIN_LIFE_1);
    DRAIN_MANA            = ai->initSpell(DRAIN_MANA_1);
    LIFE_TAP              = ai->initSpell(LIFE_TAP_1);
    UNSTABLE_AFFLICTION   = ai->initSpell(UNSTABLE_AFFLICTION_1);
    HAUNT                 = ai->initSpell(HAUNT_1);
    SEED_OF_CORRUPTION    = ai->initSpell(SEED_OF_CORRUPTION_1);
    DARK_PACT             = ai->initSpell(DARK_PACT_1);
    HOWL_OF_TERROR        = ai->initSpell(HOWL_OF_TERROR_1);
    FEAR                  = ai->initSpell(FEAR_1);
    // DEMONOLOGY
    DEMON_SKIN            = ai->initSpell(DEMON_SKIN_1);
    DEMON_ARMOR           = ai->initSpell(DEMON_ARMOR_1);
    FEL_ARMOR             = ai->initSpell(FEL_ARMOR_1);
    SHADOW_WARD           = ai->initSpell(SHADOW_WARD_1);
    SOULSHATTER           = ai->initSpell(SOULSHATTER_1);
    SOUL_LINK             = ai->initSpell(SOUL_LINK_1);
    SOUL_LINK_AURA        = 25228; // dummy aura applied, after spell SOUL_LINK
    HEALTH_FUNNEL         = ai->initSpell(HEALTH_FUNNEL_1);
    DETECT_INVISIBILITY   = ai->initSpell(DETECT_INVISIBILITY_1);
    // demon summon
    SUMMON_IMP            = ai->initSpell(SUMMON_IMP_1);
    SUMMON_VOIDWALKER     = ai->initSpell(SUMMON_VOIDWALKER_1);
    SUMMON_SUCCUBUS       = ai->initSpell(SUMMON_SUCCUBUS_1);
    SUMMON_FELHUNTER      = ai->initSpell(SUMMON_FELHUNTER_1);
    SUMMON_FELGUARD       = ai->initSpell(SUMMON_FELGUARD_1);
    // demon skills should be initialized on demons
    BLOOD_PACT            = 0; // imp skill
    CONSUME_SHADOWS       = 0; // voidwalker skill
    FEL_INTELLIGENCE      = 0; // felhunter skill

    RECENTLY_BANDAGED     = 11196; // first aid check

    // racial
    ARCANE_TORRENT        = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES); // blood elf
    ESCAPE_ARTIST         = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    BLOOD_FURY            = ai->initSpell(BLOOD_FURY_WARLOCK); // orc
    WILL_OF_THE_FORSAKEN  = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

bool PlayerbotWarlockAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    ai->SetInFront(pTarget);
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();
    Pet *pet = m_bot->GetPet();

    ai->SetInFront(pTarget);

    switch (SpellSequence)
    {
        case SPELL_CURSES:

            if ((CURSE_OF_AGONY > 0)
                && (LastSpellCurse < 1)
                && (!pTarget->HasAura(CURSE_OF_AGONY, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHADOWFLAME, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 14)
                && (ai->CastSpell(CURSE_OF_AGONY, *pTarget)))
            {
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse++;
                return true;
            }

            if ((CURSE_OF_THE_ELEMENTS > 0)
                && (LastSpellCurse < 2)
                && (!pTarget->HasAura(CURSE_OF_THE_ELEMENTS, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHADOWFLAME, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_AGONY, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_WEAKNESS, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 10)
                && (ai->CastSpell(CURSE_OF_THE_ELEMENTS, *pTarget)))
            {
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse++;
                return true;
            }

            if ((CURSE_OF_EXHAUSTION > 0)
                && (LastSpellCurse < 3)
                && (!pTarget->HasAura(CURSE_OF_EXHAUSTION, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHADOWFLAME, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_WEAKNESS, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_AGONY, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_THE_ELEMENTS, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 6)
                && (ai->CastSpell(CURSE_OF_EXHAUSTION, *pTarget)))
            {
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse++;
                return true;
            }

            if ((CURSE_OF_WEAKNESS > 0)
                && (LastSpellCurse < 4)
                && (!pTarget->HasAura(CURSE_OF_WEAKNESS, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHADOWFLAME, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_EXHAUSTION, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_AGONY, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_THE_ELEMENTS, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 14)
                && (ai->CastSpell(CURSE_OF_WEAKNESS, *pTarget)))
            {
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse++;
                return true;
            }

            if ((CURSE_OF_TONGUES > 0)
                && (LastSpellCurse < 5)
                && (!pTarget->HasAura(CURSE_OF_TONGUES, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHADOWFLAME, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_WEAKNESS, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_EXHAUSTION, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_AGONY, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CURSE_OF_THE_ELEMENTS, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 4)
                && (ai->CastSpell(CURSE_OF_TONGUES, *pTarget)))
            {
                SpellSequence = SPELL_AFFLICTION;
                LastSpellCurse++;
                return true;
            }

            LastSpellCurse = 0;
            break;

        case SPELL_AFFLICTION:

            if ((LIFE_TAP > 0)
                && (LastSpellAffliction < 1)
                && (ai->GetManaPercent() <= 50)
                && (ai->CastSpell(LIFE_TAP, *m_bot)))
            {
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellAffliction++;
                return true;
            }

            if ((CORRUPTION > 0)
                && (LastSpellAffliction < 2)
                && (!pTarget->HasAura(CORRUPTION, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHADOWFLAME, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SEED_OF_CORRUPTION, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 19)
                && (ai->CastSpell(CORRUPTION, *pTarget)))
            {
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellCurse++;
                return true;
            }

            if ((DRAIN_SOUL > 0)
                && (LastSpellAffliction < 3)
                && (pTarget->GetHealth() < pTarget->GetMaxHealth() *0.40)
                && (!pTarget->HasAura(DRAIN_SOUL, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 19)
                && (ai->CastSpell(DRAIN_SOUL, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(15);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellCurse++;
                return true;
            }

            if ((DRAIN_LIFE > 0)
                && (LastSpellAffliction < 4)
                && (!pTarget->HasAura(DRAIN_SOUL, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SEED_OF_CORRUPTION, EFFECT_INDEX_0))
                && (!pTarget->HasAura(DRAIN_LIFE, EFFECT_INDEX_0))
                && (!pTarget->HasAura(DRAIN_MANA, EFFECT_INDEX_0))
                && (ai->GetHealthPercent() <= 70)
                && (ai->GetManaPercent() >= 23)
                && (ai->CastSpell(DRAIN_LIFE, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(5);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellCurse++;
                return true;
            }

            if ((DRAIN_MANA > 0)
                && (LastSpellAffliction < 5)
                && (pTarget->GetPower(POWER_MANA) > 0)
                && (!pTarget->HasAura(DRAIN_SOUL, EFFECT_INDEX_0))
                && (!pTarget->HasAura(DRAIN_MANA, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SEED_OF_CORRUPTION, EFFECT_INDEX_0))
                && (!pTarget->HasAura(DRAIN_LIFE, EFFECT_INDEX_0))
                && (ai->GetManaPercent() < 70)
                && (ai->GetManaPercent() >= 17)
                && (ai->CastSpell(DRAIN_MANA, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(5);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellCurse++;
                return true;
            }

            if ((UNSTABLE_AFFLICTION > 0)
                && (LastSpellAffliction < 6)
                && (!pTarget->HasAura(UNSTABLE_AFFLICTION, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHADOWFLAME, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 20)
                && (ai->CastSpell(UNSTABLE_AFFLICTION, *pTarget)))
            {
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellCurse++;
                return true;
            }
            if ((HAUNT > 0)
                && (LastSpellAffliction < 7)
                && (!pTarget->HasAura(HAUNT, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 12)
                && (ai->CastSpell(HAUNT, *pTarget)))
            {
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellCurse++;
                return true;
            }

            if ((SEED_OF_CORRUPTION > 0)
                && (LastSpellAffliction < 9)
                && (!pTarget->HasAura(SEED_OF_CORRUPTION, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 34)
                && (ai->CastSpell(SEED_OF_CORRUPTION, *pTarget)))
            {
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellCurse++;
                return true;
            }

            if ((HOWL_OF_TERROR > 0)
                && (LastSpellAffliction < 10)
                && (!pTarget->HasAura(HOWL_OF_TERROR, EFFECT_INDEX_0))
                && (ai->GetAttackerCount() > 3)
                && (ai->GetManaPercent() >= 11)
                && (ai->CastSpell(HOWL_OF_TERROR, *pTarget)))
            {
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellCurse++;
                return true;
            }

            if ((FEAR > 0)
                && (LastSpellAffliction < 11)
                && (!pTarget->HasAura(FEAR, EFFECT_INDEX_0))
                && (pVictim == m_bot)
                && (ai->GetAttackerCount() >= 2)
                && (ai->GetManaPercent() >= 12)
                && (ai->CastSpell(FEAR, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(1.5);
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellCurse++;
                return true;
            }

            if (pet
                && (DARK_PACT > 0)
                && (LastSpellAffliction < 12)
                && (ai->GetManaPercent() <= 50)
                && (pet->GetPower(POWER_MANA) > 0)
                && (ai->CastSpell(DARK_PACT, *m_bot)))
            {
                SpellSequence = SPELL_DESTRUCTION;
                LastSpellCurse++;
                return true;
            }

            LastSpellAffliction = 0;
            break;

        case SPELL_DESTRUCTION:

            if ((SHADOWFURY > 0)
                && (LastSpellDestruction < 1)
                && (!pTarget->HasAura(SHADOWFURY, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 37)
                && (ai->CastSpell(SHADOWFURY, *pTarget)))
            {
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((SHADOW_BOLT > 0)
                && (LastSpellDestruction < 2)
                && (ai->GetManaPercent() >= 23)
                && (ai->CastSpell(SHADOW_BOLT, *pTarget)))
            {
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((RAIN_OF_FIRE > 0)
                && (LastSpellDestruction < 3)
                && (ai->GetAttackerCount() >= 3)
                && (ai->GetManaPercent() >= 77)
                && (ai->CastSpell(RAIN_OF_FIRE, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(8);
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((SHADOWFLAME > 0)
                && (LastSpellDestruction < 4)
                && (!pTarget->HasAura(SHADOWFLAME, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 25)
                && (ai->CastSpell(SHADOWFLAME, *pTarget)))
            {
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((IMMOLATE > 0)
                && (LastSpellDestruction < 5)
                && (!pTarget->HasAura(IMMOLATE, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHADOWFLAME, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 23)
                && (ai->CastSpell(IMMOLATE, *pTarget)))
            {
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((CONFLAGRATE > 0)
                && (LastSpellDestruction < 6)
                && (ai->GetManaPercent() >= 16)
                && (ai->CastSpell(CONFLAGRATE, *pTarget)))
            {
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((INCINERATE > 0)
                && (LastSpellDestruction < 7)
                && (ai->GetManaPercent() >= 19)
                && (ai->CastSpell(INCINERATE, *pTarget)))
            {
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((SEARING_PAIN > 0)
                && (LastSpellDestruction < 8)
                && (ai->GetManaPercent() >= 11)
                && (ai->CastSpell(SEARING_PAIN, *pTarget)))
            {
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((SOUL_FIRE > 0)
                && (LastSpellDestruction < 9)
                && (ai->GetManaPercent() >= 13)
                && (ai->CastSpell(SOUL_FIRE, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(6);
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((CHAOS_BOLT > 0)
                && (LastSpellDestruction < 10)
                && (ai->GetManaPercent() >= 9)
                && (ai->CastSpell(CHAOS_BOLT, *pTarget)))
            {
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((SHADOWBURN > 0)
                && (LastSpellDestruction < 11)
                && (pTarget->GetHealth() < pTarget->GetMaxHealth() *0.20)
                && (!pTarget->HasAura(SHADOWBURN, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 27)
                && (ai->CastSpell(SHADOWBURN, *pTarget)))
            {
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            if ((HELLFIRE > 0)
                && (LastSpellDestruction < 12)
                && (!m_bot->HasAura(HELLFIRE, EFFECT_INDEX_0))
                && (ai->GetAttackerCount() >= 5)
                && (ai->GetHealthPercent() >= 10)
                && (ai->GetManaPercent() >= 87)
                && (ai->CastSpell(HELLFIRE)))
            {
                ai->SetIgnoreUpdateTime(15);
                SpellSequence = SPELL_CURSES;
                LastSpellDestruction++;
                return true;
            }

            LastSpellDestruction = 0;
            SpellSequence = SPELL_CURSES;
            break;
    }

    return false;
}

bool PlayerbotWarlockAI::DoNonCombatActions()
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

    SpellSequence = SPELL_CURSES;

    Pet *pet = m_bot->GetPet();

    if ((FEL_ARMOR > 0)
        && (!m_bot->HasAura(FEL_ARMOR, EFFECT_INDEX_0))
        && (ai->CastSpell(FEL_ARMOR, *m_bot)))
        return true;

    if ((DEMON_ARMOR > 0)
        && (!m_bot->HasAura(FEL_ARMOR, EFFECT_INDEX_0))
        && (!m_bot->HasAura(DEMON_ARMOR, EFFECT_INDEX_0))
        && (ai->CastSpell(DEMON_ARMOR, *m_bot)))
        return true;

    if ((DEMON_SKIN > 0)
        && (!m_bot->HasAura(DEMON_SKIN, EFFECT_INDEX_0))
        && (!m_bot->HasAura(FEL_ARMOR, EFFECT_INDEX_0))
        && (!m_bot->HasAura(DEMON_ARMOR, EFFECT_INDEX_0))
        && (ai->CastSpell(DEMON_SKIN, *m_bot)))
        return true;

    if ((DETECT_INVISIBILITY > 0)
        && (!m_bot->HasAura(DETECT_INVISIBILITY, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 2)
        && (ai->CastSpell(DETECT_INVISIBILITY, *m_bot)))
        return true;

    if ((DETECT_INVISIBILITY > 0)
        && (!pMaster->HasAura(DETECT_INVISIBILITY, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 2)
        && (ai->CastSpell(DETECT_INVISIBILITY, *pMaster)))
        return true;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* drinkOrFoodItem = ai->FindDrink();
    Item* bandageItem = ai->FindBandage();

    if (drinkOrFoodItem
        && (ai->GetManaPercent() < 25))
    {
        ai->UseItem(*drinkOrFoodItem);
        ai->SetIgnoreUpdateTime(30);
        return true;
    }

    if (pet
        && !drinkOrFoodItem
        && (DARK_PACT > 0)
        && (ai->GetManaPercent() <= 50)
        && (pet->GetPower(POWER_MANA) > 0)
        && (ai->CastSpell(DARK_PACT, *m_bot)))
        return true;

    if (!pet
        && !drinkOrFoodItem
        && (LIFE_TAP > 0)
        && (ai->GetManaPercent() <= 50)
        && (ai->GetHealthPercent() > 25)
        && (ai->CastSpell(LIFE_TAP, *m_bot)))
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

    if (pet
        && !drinkOrFoodItem
        && !bandageItem
        && (CONSUME_SHADOWS > 0)
        && (!m_bot->HasAura(CONSUME_SHADOWS, EFFECT_INDEX_0))
        && (ai->GetHealthPercent() < 75)
        && (ai->CastSpell(CONSUME_SHADOWS, *m_bot)))
        return true;

    if (((SUMMON_FELGUARD > 0)
        || (SUMMON_FELHUNTER > 0)
        || (SUMMON_SUCCUBUS > 0)
        || (SUMMON_VOIDWALKER > 0)
        || (SUMMON_IMP > 0))
        && (!m_demonSummonFailed))
    {
        if (!pet)
        {
            if ((SUMMON_FELGUARD > 0)
                && (ai->CastSpell(SUMMON_FELGUARD, *m_bot)))
                return true;

            if ((SUMMON_FELHUNTER > 0)
                && (ai->CastSpell(SUMMON_FELHUNTER, *m_bot)))
                return true;

            if ((SUMMON_SUCCUBUS > 0)
                && (ai->CastSpell(SUMMON_SUCCUBUS, *m_bot)))
                return true;

            if ((SUMMON_VOIDWALKER > 0)
                && (ai->CastSpell(SUMMON_VOIDWALKER, *m_bot)))
                return true;

            if ((SUMMON_IMP > 0)
                && (ai->GetManaPercent() >= 64)
                && (ai->CastSpell(SUMMON_IMP, *m_bot)))
                return true;

            m_demonSummonFailed = true;
        }
    }

    if (pet
        && (SOUL_LINK > 0)
        && (!m_bot->HasAura(SOUL_LINK_AURA, EFFECT_INDEX_0))
        && (ai->GetBaseManaPercent() >= 16)
        && (ai->CastSpell(SOUL_LINK, *m_bot)))
        return true;

    if (pet
        && (BLOOD_PACT > 0)
        && (!m_bot->HasAura(BLOOD_PACT, EFFECT_INDEX_0))
        && (ai->CastSpell(BLOOD_PACT, *m_bot)))
        return true;

    if (pet
        && (FEL_INTELLIGENCE > 0)
        && (!m_bot->HasAura(FEL_INTELLIGENCE, EFFECT_INDEX_0))
        && (ai->CastSpell(FEL_INTELLIGENCE, *m_bot)))
        return true;

    return false;
}
