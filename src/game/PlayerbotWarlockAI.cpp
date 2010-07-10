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
    SHADOW_BOLT = ai->getSpellId("shadow bolt");
    IMMOLATE = ai->getSpellId("immolate");
    INCINERATE = ai->getSpellId("incinerate");
    SEARING_PAIN = ai->getSpellId("searing pain");
    CONFLAGRATE = ai->getSpellId("conflagrate");
    SHADOWFURY = ai->getSpellId("shadowfury");
    CHAOS_BOLT = ai->getSpellId("chaos bolt");
    SHADOWFLAME = ai->getSpellId("shadowflame");
    HELLFIRE = ai->getSpellId("hellfire");
    RAIN_OF_FIRE = ai->getSpellId("rain of fire");
    SOUL_FIRE = ai->getSpellId("soul fire");
    SHADOWBURN = ai->getSpellId("shadowburn");
    CURSE_OF_WEAKNESS = ai->getSpellId("curse of weakness");
    CURSE_OF_THE_ELEMENTS = ai->getSpellId("curse of the elements");
    CURSE_OF_AGONY = ai->getSpellId("curse of agony");
    CURSE_OF_EXHAUSTION = ai->getSpellId("curse of exhaustion");
    CURSE_OF_TONGUES = ai->getSpellId("curse of tongues");
    CURSE_OF_DOOM = ai->getSpellId("curse of doom");
    CORRUPTION = ai->getSpellId("corruption");
    DRAIN_SOUL = ai->getSpellId("drain soul");
    DRAIN_LIFE = ai->getSpellId("drain life");
    DRAIN_MANA = ai->getSpellId("drain mana");
    LIFE_TAP = ai->getSpellId("life tap");
    UNSTABLE_AFFLICTION = ai->getSpellId("unstable affliction");
    HAUNT = ai->getSpellId("haunt");
    ATROCITY = ai->getSpellId("atrocity");
    SEED_OF_CORRUPTION = ai->getSpellId("seed of corruption");
    DARK_PACT = ai->getSpellId("dark pact");
    HOWL_OF_TERROR = ai->getSpellId("howl of terror");
    FEAR = ai->getSpellId("fear");
    DEMON_SKIN = ai->getSpellId("demon skin");
    DEMON_ARMOR = ai->getSpellId("demon armor");
    FEL_ARMOR = ai->getSpellId("fel armor");
    SHADOW_WARD = ai->getSpellId("shadow ward");
    SOULSHATTER = ai->getSpellId("soulshatter");
    SOUL_LINK = ai->getSpellId("soul link");
    SOUL_LINK_AURA = 25228;
    HEALTH_FUNNEL = ai->getSpellId("health funnel");
    DETECT_INVISIBILITY = ai->getSpellId("detect invisibility");
    SUMMON_IMP = ai->getSpellId("summon imp");
    SUMMON_VOIDWALKER = ai->getSpellId("summon voidwalker");
    SUMMON_SUCCUBUS = ai->getSpellId("summon succubus");
    SUMMON_FELHUNTER = ai->getSpellId("summon felhunter");
    SUMMON_FELGUARD = ai->getSpellId("summon felguard");
    BLOOD_PACT = ai->getSpellId("blood pact");
    CONSUME_SHADOWS = ai->getSpellId("consume shadows");
    FEL_INTELLIGENCE = ai->getSpellId("fel intelligence");
    RECENTLY_BANDAGED = 11196;
    ARCANE_TORRENT = ai->getSpellId("arcane torrent");
    ESCAPE_ARTIST = ai->getSpellId("escape artist");
    EVERY_MAN_FOR_HIMSELF = ai->getSpellId("every man for himself");
    BLOOD_FURY = ai->getSpellId("blood fury");
    WILL_OF_THE_FORSAKEN = ai->getSpellId("will of the forsaken");
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

            if ((ATROCITY > 0)
                && (LastSpellAffliction < 8)
                && (!pTarget->HasAura(ATROCITY, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 21)
                && (ai->CastSpell(ATROCITY, *pTarget)))
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
