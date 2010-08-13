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

#include "PlayerbotMageAI.h"

class PlayerbotAI;

PlayerbotMageAI::PlayerbotMageAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

PlayerbotMageAI::~PlayerbotMageAI() {}

void PlayerbotMageAI::InitSpells(PlayerbotAI* const ai)
{
    ARCANE_MISSILES         = ai->initSpell(ARCANE_MISSILES_1);
    ARCANE_EXPLOSION        = ai->initSpell(ARCANE_EXPLOSION_1);
    COUNTERSPELL            = ai->initSpell(COUNTERSPELL_1);
    SLOW                    = ai->initSpell(SLOW_1);
    ARCANE_BARRAGE          = ai->initSpell(ARCANE_BARRAGE_1);
    ARCANE_BLAST            = ai->initSpell(ARCANE_BLAST_1);
    ARCANE_POWER            = ai->initSpell(ARCANE_POWER_1);
    DAMPEN_MAGIC            = ai->initSpell(DAMPEN_MAGIC_1);
    AMPLIFY_MAGIC           = ai->initSpell(AMPLIFY_MAGIC_1);
    MAGE_ARMOR              = ai->initSpell(MAGE_ARMOR_1);
    MIRROR_IMAGE            = ai->initSpell(MIRROR_IMAGE_1);
    ARCANE_INTELLECT        = ai->initSpell(ARCANE_INTELLECT_1);
    ARCANE_BRILLIANCE       = ai->initSpell(ARCANE_BRILLIANCE_1);
    DALARAN_INTELLECT       = ai->initSpell(DALARAN_INTELLECT_1);
    DALARAN_BRILLIANCE      = ai->initSpell(DALARAN_BRILLIANCE_1);
    MANA_SHIELD             = ai->initSpell(MANA_SHIELD_1);
    CONJURE_WATER           = ai->initSpell(CONJURE_WATER_1);
    CONJURE_FOOD            = ai->initSpell(CONJURE_FOOD_1);
    FIREBALL                = ai->initSpell(FIREBALL_1);
    FIRE_BLAST              = ai->initSpell(FIRE_BLAST_1);
    FLAMESTRIKE             = ai->initSpell(FLAMESTRIKE_1);
    SCORCH                  = ai->initSpell(SCORCH_1);
    PYROBLAST               = ai->initSpell(PYROBLAST_1);
    BLAST_WAVE              = ai->initSpell(BLAST_WAVE_1);
    COMBUSTION              = ai->initSpell(COMBUSTION_1);
    DRAGONS_BREATH          = ai->initSpell(DRAGONS_BREATH_1);
    LIVING_BOMB             = ai->initSpell(LIVING_BOMB_1);
    FROSTFIRE_BOLT          = ai->initSpell(FROSTFIRE_BOLT_1);
    FIRE_WARD               = ai->initSpell(FIRE_WARD_1);
    MOLTEN_ARMOR            = ai->initSpell(MOLTEN_ARMOR_1);
    ICY_VEINS               = ai->initSpell(ICY_VEINS_1);
    DEEP_FREEZE             = ai->initSpell(DEEP_FREEZE_1);
    FROSTBOLT               = ai->initSpell(FROSTBOLT_1);
    FROST_NOVA              = ai->initSpell(FROST_NOVA_1);
    BLIZZARD                = ai->initSpell(BLIZZARD_1);
    CONE_OF_COLD            = ai->initSpell(CONE_OF_COLD_1);
    ICE_BARRIER             = ai->initSpell(ICE_BARRIER_1);
    SUMMON_WATER_ELEMENTAL  = ai->initSpell(SUMMON_WATER_ELEMENTAL_1);
    FROST_WARD              = ai->initSpell(FROST_WARD_1);
    ICE_LANCE               = ai->initSpell(ICE_LANCE_1);
    FROST_ARMOR             = ai->initSpell(FROST_ARMOR_1);
    ICE_ARMOR               = ai->initSpell(ICE_ARMOR_1);
    ICE_BLOCK               = ai->initSpell(ICE_BLOCK_1);
    COLD_SNAP               = ai->initSpell(COLD_SNAP_1);

    RECENTLY_BANDAGED       = 11196; // first aid check

    // racial
    ARCANE_TORRENT          = ai->initSpell(ARCANE_TORRENT_MANA_CLASSES); // blood elf
    GIFT_OF_THE_NAARU       = ai->initSpell(GIFT_OF_THE_NAARU_MAGE); // draenei
    ESCAPE_ARTIST           = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF   = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    BERSERKING              = ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN    = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

bool PlayerbotMageAI::DoNextCombatManeuver(Unit *pTarget)
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
    float dist = m_bot->GetDistance(pTarget);

    switch (SpellSequence)
    {
        case SPELL_FROST:

            if ((ICY_VEINS > 0)
                && (!m_bot->HasAura(ICY_VEINS, EFFECT_INDEX_0))
                && (LastSpellFrost < 1)
                && (ai->GetManaPercent() >= 3)
                && (ai->CastSpell(ICY_VEINS, *m_bot)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((FROSTBOLT > 0)
                && (LastSpellFrost < 2)
                && (!pTarget->HasAura(FROSTBOLT, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 16)
                && (ai->CastSpell(FROSTBOLT, *pTarget)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((FROST_WARD > 0)
                && (LastSpellFrost < 3)
                && (!m_bot->HasAura(FROST_WARD, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 19)
                && (ai->CastSpell(FROST_WARD, *m_bot)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((FROST_NOVA > 0)
                && (LastSpellFrost < 4)
                && (dist <= ATTACK_DISTANCE)
                && (!pTarget->HasAura(FROST_NOVA, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 10)
                && (ai->CastSpell(FROST_NOVA, *pTarget)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((ICE_LANCE > 0)
                && (LastSpellFrost < 5)
                && (ai->GetManaPercent() >= 7)
                && (ai->CastSpell(ICE_LANCE, *pTarget)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((BLIZZARD > 0)
                && (LastSpellFrost < 6)
                && (ai->GetAttackerCount() >= 5)
                && (ai->GetManaPercent() >= 89)
                && (ai->CastSpell(BLIZZARD, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(8);
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((CONE_OF_COLD > 0)
                && (LastSpellFrost < 7)
                && (dist <= ATTACK_DISTANCE)
                && (!pTarget->HasAura(CONE_OF_COLD, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 35)
                && (ai->CastSpell(CONE_OF_COLD, *pTarget)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((DEEP_FREEZE > 0)
                && (LastSpellFrost < 8)
                && (pTarget->HasAura(AURA_STATE_FROZEN, EFFECT_INDEX_0))
                && (!pTarget->HasAura(DEEP_FREEZE, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 9)
                && (ai->CastSpell(DEEP_FREEZE, *pTarget)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((ICE_BARRIER > 0)
                && (LastSpellFrost < 9)
                && (pVictim == m_bot)
                && (!m_bot->HasAura(ICE_BARRIER, EFFECT_INDEX_0))
                && (ai->GetHealthPercent() < 50)
                && (ai->GetManaPercent() >= 30)
                && (ai->CastSpell(ICE_BARRIER, *m_bot)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((SUMMON_WATER_ELEMENTAL > 0)
                && (LastSpellFrost < 10)
                && (ai->GetManaPercent() >= 16)
                && (ai->CastSpell(SUMMON_WATER_ELEMENTAL)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((ICE_BLOCK > 0)
                && (LastSpellFrost < 11)
                && (pVictim == m_bot)
                && (!m_bot->HasAura(ICE_BLOCK, EFFECT_INDEX_0))
                && (ai->GetHealthPercent() < 30)
                && (ai->CastSpell(ICE_BLOCK, *m_bot)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            if ((COLD_SNAP > 0)
                && (LastSpellFrost < 12)
                && (ai->CastSpell(COLD_SNAP, *m_bot)))
            {
                SpellSequence = SPELL_FIRE;
                LastSpellFrost++;
                return true;
            }

            LastSpellFrost = 0;
            break;

        case SPELL_FIRE:

            if ((FIRE_WARD > 0)
                && (!m_bot->HasAura(FIRE_WARD, EFFECT_INDEX_0))
                && (LastSpellFire < 1)
                && (ai->GetManaPercent() >= 3)
                && (ai->CastSpell(FIRE_WARD, *m_bot)))
            {

                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            if ((COMBUSTION > 0)
                && (!m_bot->HasAura(COMBUSTION, EFFECT_INDEX_0))
                && (LastSpellFire < 2)
                && (ai->CastSpell(COMBUSTION, *m_bot)))
            {
                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            if ((FIREBALL > 0)
                && (LastSpellFire < 3)
                && (ai->GetManaPercent() >= 23)
                && (ai->CastSpell(FIREBALL, *pTarget)))
            {
                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            if ((FIRE_BLAST > 0)
                && (LastSpellFire < 4)
                && (ai->GetManaPercent() >= 25)
                && (ai->CastSpell(FIRE_BLAST, *pTarget)))
            {
                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            if ((FLAMESTRIKE > 0)
                && (LastSpellFire < 5)
                && (ai->GetManaPercent() >= 35)
                && (ai->CastSpell(FLAMESTRIKE, *pTarget)))
            {
                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            if ((SCORCH > 0)
                && (LastSpellFire < 6)
                && (ai->GetManaPercent() >= 10)
                && (ai->CastSpell(SCORCH, *pTarget)))
            {
                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            if ((PYROBLAST > 0)
                && (LastSpellFire < 7)
                && (!pTarget->HasAura(PYROBLAST, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 27)
                && (ai->CastSpell(PYROBLAST, *pTarget)))
            {
                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            if ((BLAST_WAVE > 0)
                && (LastSpellFire < 8)
                && (ai->GetAttackerCount() >= 3)
                && (dist <= ATTACK_DISTANCE)
                && (ai->GetManaPercent() >= 34)
                && (ai->CastSpell(BLAST_WAVE, *pTarget)))
            {
                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            if ((DRAGONS_BREATH > 0)
                && (LastSpellFire < 9)
                && (dist <= ATTACK_DISTANCE)
                && (ai->GetManaPercent() >= 37)
                && (ai->CastSpell(DRAGONS_BREATH, *pTarget)))
            {
                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            if ((LIVING_BOMB > 0)
                && (LastSpellFire < 10)
                && (!pTarget->HasAura(LIVING_BOMB, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 27)
                && (ai->CastSpell(LIVING_BOMB, *pTarget)))
            {
                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            if ((FROSTFIRE_BOLT > 0)
                && (LastSpellFire < 11)
                && (!pTarget->HasAura(FROSTFIRE_BOLT, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 14)
                && (ai->CastSpell(FROSTFIRE_BOLT, *pTarget)))
            {
                SpellSequence = SPELL_ARCANE;
                LastSpellFire++;
                return true;
            }

            LastSpellFire = 0;
            break;

        case SPELL_ARCANE:

            if ((ARCANE_POWER > 0)
                && (LastSpellArcane < 1)
                && (ai->GetManaPercent() >= 37)
                && (ai->CastSpell(ARCANE_POWER, *pTarget)))
            {
                SpellSequence = SPELL_FROST;
                LastSpellArcane++;
                return true;
            }

            if ((ARCANE_MISSILES > 0)
                && (LastSpellArcane < 2)
                && (ai->GetManaPercent() >= 37)
                && (ai->CastSpell(ARCANE_MISSILES, *pTarget)))
            {
                ai->SetIgnoreUpdateTime(3);
                SpellSequence = SPELL_FROST;
                LastSpellArcane++;
                return true;
            }

            if ((ARCANE_EXPLOSION > 0)
                && (LastSpellArcane < 3)
                && (ai->GetAttackerCount() >= 3)
                && (dist <= ATTACK_DISTANCE)
                && (ai->GetManaPercent() >= 27)
                && (ai->CastSpell(ARCANE_EXPLOSION, *pTarget)))
            {
                SpellSequence = SPELL_FROST;
                LastSpellArcane++;
                return true;
            }

            if ((COUNTERSPELL > 0)
                && (pTarget->IsNonMeleeSpellCasted(true))
                && (LastSpellArcane < 4)
                && (ai->GetManaPercent() >= 9)
                && (ai->CastSpell(COUNTERSPELL, *pTarget)))
            {
                SpellSequence = SPELL_FROST;
                LastSpellArcane++;
                return true;
            }

            if ((SLOW > 0)
                && (LastSpellArcane < 5)
                && (!pTarget->HasAura(SLOW, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 12)
                && (ai->CastSpell(SLOW, *pTarget)))
            {
                SpellSequence = SPELL_FROST;
                LastSpellArcane++;
                return true;
            }

            if ((ARCANE_BARRAGE > 0)
                && (LastSpellArcane < 6)
                && (ai->GetManaPercent() >= 27)
                && (ai->CastSpell(ARCANE_BARRAGE, *pTarget)))
            {
                SpellSequence = SPELL_FROST;
                LastSpellArcane++;
                return true;
            }

            if ((ARCANE_BLAST > 0)
                && (LastSpellArcane < 7)
                && (ai->GetManaPercent() >= 8)
                && (ai->CastSpell(ARCANE_BLAST, *pTarget)))
            {
                SpellSequence = SPELL_FROST;
                LastSpellArcane++;
                return true;
            }

            if ((MIRROR_IMAGE > 0)
                && (LastSpellArcane < 8)
                && (ai->GetManaPercent() >= 10)
                && (ai->CastSpell(MIRROR_IMAGE)))
            {
                SpellSequence = SPELL_FROST;
                LastSpellArcane++;
                return true;
            }

            if ((MANA_SHIELD > 0)
                && (LastSpellArcane < 9)
                && (ai->GetHealthPercent() < 70)
                && (pVictim == m_bot)
                && (!m_bot->HasAura(MANA_SHIELD, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 8)
                && (ai->CastSpell(MANA_SHIELD, *m_bot)))
            {
                SpellSequence = SPELL_FROST;
                LastSpellArcane++;
                return true;
            }

            LastSpellArcane = 0;
            SpellSequence = SPELL_FROST;
            break;
    }

    return false;
}

bool PlayerbotMageAI::DoNonCombatActions()
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

    SpellSequence = SPELL_FROST;

    if ((DALARAN_BRILLIANCE > 0)
        && (!pMaster->HasAura(DALARAN_BRILLIANCE, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 81)
        && (ai->CastSpell(DALARAN_BRILLIANCE, *pMaster)))
        return true;

    if ((ARCANE_BRILLIANCE > 0)
        && (!pMaster->HasAura(ARCANE_BRILLIANCE, EFFECT_INDEX_0))
        && (!pMaster->HasAura(DALARAN_BRILLIANCE, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 97)
        && (ai->CastSpell(ARCANE_BRILLIANCE, *pMaster)))
        return true;

    if ((DALARAN_INTELLECT > 0)
        && (!m_bot->HasAura(DALARAN_INTELLECT, EFFECT_INDEX_0))
        && (!m_bot->HasAura(DALARAN_BRILLIANCE, EFFECT_INDEX_0))
        && (!m_bot->HasAura(ARCANE_BRILLIANCE, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 31)
        && (ai->CastSpell(DALARAN_INTELLECT, *m_bot)))
        return true;

    if ((ARCANE_INTELLECT > 0)
        && (!m_bot->HasAura(ARCANE_INTELLECT, EFFECT_INDEX_0))
        && (!m_bot->HasAura(DALARAN_BRILLIANCE, EFFECT_INDEX_0))
        && (!m_bot->HasAura(ARCANE_BRILLIANCE, EFFECT_INDEX_0))
        && (!m_bot->HasAura(DALARAN_INTELLECT, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 37)
        && (ai->CastSpell(ARCANE_INTELLECT, *m_bot)))
        return true;

    if ((MOLTEN_ARMOR > 0)
        && (!m_bot->HasAura(MOLTEN_ARMOR, EFFECT_INDEX_0))
        && (!m_bot->HasAura(MAGE_ARMOR, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 31)
        && (ai->CastSpell(MOLTEN_ARMOR, *m_bot)))
        return true;

    if ((MAGE_ARMOR > 0)
        && (!m_bot->HasAura(MAGE_ARMOR, EFFECT_INDEX_0))
        && (!m_bot->HasAura(MOLTEN_ARMOR, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 31)
        && (ai->CastSpell(MAGE_ARMOR, *m_bot)))
        return true;

    if ((ICE_ARMOR > 0)
        && (!m_bot->HasAura(ICE_ARMOR, EFFECT_INDEX_0))
        && (!m_bot->HasAura(MOLTEN_ARMOR, EFFECT_INDEX_0))
        && (!m_bot->HasAura(MAGE_ARMOR, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 34)
        && (ai->CastSpell(ICE_ARMOR, *m_bot)))
        return true;

    if ((FROST_ARMOR > 0)
        && (!m_bot->HasAura(FROST_ARMOR, EFFECT_INDEX_0))
        && (!m_bot->HasAura(MOLTEN_ARMOR, EFFECT_INDEX_0))
        && (!m_bot->HasAura(MAGE_ARMOR, EFFECT_INDEX_0))
        && (!m_bot->HasAura(ICE_ARMOR, EFFECT_INDEX_0))
        && (ai->GetManaPercent() >= 34)
        && (ai->CastSpell(FROST_ARMOR, *m_bot)))
        return true;

    if (pMaster->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = pMaster->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(uint64(itr->guid));
            if (!tPlayer || !tPlayer->isAlive())
                continue;

            if ((!tPlayer->HasAura(ARCANE_INTELLECT, EFFECT_INDEX_0))
                && (!tPlayer->HasAura(DALARAN_BRILLIANCE, EFFECT_INDEX_0))
                && (!tPlayer->HasAura(ARCANE_BRILLIANCE, EFFECT_INDEX_0))
                && (!tPlayer->HasAura(DALARAN_INTELLECT, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 37)
                && (ai->CastSpell(ARCANE_INTELLECT, *tPlayer)))
                return true;

            if ((!tPlayer->HasAura(DALARAN_INTELLECT, EFFECT_INDEX_0))
                && (!tPlayer->HasAura(DALARAN_BRILLIANCE, EFFECT_INDEX_0))
                && (!tPlayer->HasAura(ARCANE_BRILLIANCE, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 31)
                && (ai->CastSpell(DALARAN_INTELLECT, *tPlayer)))
                return true;

            if ((!tPlayer->HasAura(DAMPEN_MAGIC, EFFECT_INDEX_0))
                && (!tPlayer->HasAura(AMPLIFY_MAGIC, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 32)
                && (ai->CastSpell(DAMPEN_MAGIC, *tPlayer)))
                return true;

            if ((!tPlayer->HasAura(AMPLIFY_MAGIC, EFFECT_INDEX_0))
                && (!tPlayer->HasAura(DAMPEN_MAGIC, EFFECT_INDEX_0))
                && (ai->GetManaPercent() >= 32)
                && (ai->CastSpell(AMPLIFY_MAGIC, *tPlayer)))
                return true;
        }
    }

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* drinkOrFoodItem = ai->FindDrink();
    Item* bandageItem = ai->FindBandage();

    if (!drinkOrFoodItem
        && (CONJURE_WATER > 0)
        && (ai->GetBaseManaPercent() >= 48))
        ai->CastSpell(CONJURE_WATER, *m_bot);
    {
        ai->SetIgnoreUpdateTime(3);
        return true;
    }

    if (drinkOrFoodItem
        && (ai->GetManaPercent() < 30))
    {
        ai->UseItem(*drinkOrFoodItem);
        ai->SetIgnoreUpdateTime(30);
        return true;
    }

    drinkOrFoodItem = ai->FindFood();

    if (!drinkOrFoodItem
        && (CONJURE_FOOD > 0)
        && (ai->GetBaseManaPercent() >= 48))
        ai->CastSpell(CONJURE_FOOD, *m_bot);
    {
        ai->SetIgnoreUpdateTime(3);
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

    return false;
}
