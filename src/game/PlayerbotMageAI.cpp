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
    ARCANE_MISSILES = ai->getSpellId("arcane missiles");
    ARCANE_EXPLOSION = ai->getSpellId("arcane explosion");
    COUNTERSPELL = ai->getSpellId("counterspell");
    SLOW = ai->getSpellId("slow");
    ARCANE_BARRAGE = ai->getSpellId("arcane barrage");
    ARCANE_BLAST = ai->getSpellId("arcane blast");
    ARCANE_POWER = ai->getSpellId("arcane power");
    DAMPEN_MAGIC = ai->getSpellId("dampen magic");
    AMPLIFY_MAGIC = ai->getSpellId("amplify magic");
    MAGE_ARMOR = ai->getSpellId("mage armor");
    MIRROR_IMAGE = ai->getSpellId("mirror image");
    ARCANE_INTELLECT = ai->getSpellId("arcane intellect");
    ARCANE_BRILLIANCE = ai->getSpellId("arcane brilliance");
    DALARAN_INTELLECT = ai->getSpellId("dalaran intellect");
    DALARAN_BRILLIANCE = ai->getSpellId("dalaran brilliance");
    MANA_SHIELD = ai->getSpellId("mana shield");
    CONJURE_WATER = ai->getSpellId("conjure water");
    CONJURE_FOOD = ai->getSpellId("conjure food");
    FIREBALL = ai->getSpellId("fireball");
    FIRE_BLAST = ai->getSpellId("fire blast");
    FLAMESTRIKE = ai->getSpellId("flamestrike");
    SCORCH = ai->getSpellId("scorch");
    PYROBLAST = ai->getSpellId("pyroblast");
    BLAST_WAVE = ai->getSpellId("blast wave");
    COMBUSTION = ai->getSpellId("combustion");
    DRAGONS_BREATH = ai->getSpellId("dragon's breath");
    LIVING_BOMB = ai->getSpellId("living bomb");
    FROSTFIRE_BOLT = ai->getSpellId("frostfire bolt");
    FIRE_WARD = ai->getSpellId("fire ward");
    MOLTEN_ARMOR = ai->getSpellId("molten armor");
    ICY_VEINS = ai->getSpellId("icy veins");
    DEEP_FREEZE = ai->getSpellId("deep freeze");
    FROSTBOLT = ai->getSpellId("frostbolt");
    FROST_NOVA = ai->getSpellId("frost nova");
    BLIZZARD = ai->getSpellId("blizzard");
    CONE_OF_COLD = ai->getSpellId("cone of cold");
    ICE_BARRIER = ai->getSpellId("ice barrier");
    SUMMON_WATER_ELEMENTAL = ai->getSpellId("summon water elemental");
    FROST_WARD = ai->getSpellId("frost ward");
    ICE_LANCE = ai->getSpellId("ice lance");
    FROST_ARMOR = ai->getSpellId("frost armor");
    ICE_ARMOR = ai->getSpellId("ice armor");
    ICE_BLOCK = ai->getSpellId("ice block");
    COLD_SNAP = ai->getSpellId("cold snap");
    RECENTLY_BANDAGED = 11196;
    ARCANE_TORRENT = ai->getSpellId("arcane torrent");
    GIFT_OF_THE_NAARU = ai->getSpellId("gift of the naaru");
    ESCAPE_ARTIST = ai->getSpellId("escape artist");
    EVERY_MAN_FOR_HIMSELF = ai->getSpellId("every man for himself");
    BERSERKING = ai->getSpellId("berserking");
    WILL_OF_THE_FORSAKEN = ai->getSpellId("will of the forsaken");
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
