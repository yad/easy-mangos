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

#include "PlayerbotShamanAI.h"

class PlayerbotAI;

PlayerbotShamanAI::PlayerbotShamanAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

void PlayerbotShamanAI::InitSpells(PlayerbotAI* const ai)
{
    // restoration
    CHAIN_HEAL               = ai->initSpell(CHAIN_HEAL_1);
    HEALING_WAVE             = ai->initSpell(HEALING_WAVE_1);
    LESSER_HEALING_WAVE      = ai->initSpell(LESSER_HEALING_WAVE_1);
    RIPTIDE                  = ai->initSpell(RIPTIDE_1);
    ANCESTRAL_SPIRIT         = ai->initSpell(ANCESTRAL_SPIRIT_1);
    EARTH_SHIELD             = ai->initSpell(EARTH_SHIELD_1);
    WATER_SHIELD             = ai->initSpell(WATER_SHIELD_1);
    EARTHLIVING_WEAPON       = ai->initSpell(EARTHLIVING_WEAPON_1);
    TREMOR_TOTEM             = ai->initSpell(TREMOR_TOTEM_1); // totems
    HEALING_STREAM_TOTEM     = ai->initSpell(HEALING_STREAM_TOTEM_1);
    MANA_SPRING_TOTEM        = ai->initSpell(MANA_SPRING_TOTEM_1);
    MANA_TIDE_TOTEM          = ai->initSpell(MANA_TIDE_TOTEM_1);
    // enhancement
    FOCUSED                  = 0; // Focused what?
    STORMSTRIKE              = ai->initSpell(STORMSTRIKE_1);
    LAVA_LASH                = ai->initSpell(LAVA_LASH_1);
    SHAMANISTIC_RAGE         = ai->initSpell(SHAMANISTIC_RAGE_1);
    BLOODLUST                = ai->initSpell(BLOODLUST_1);
    HEROISM                  = ai->initSpell(HEROISM_1);
    FERAL_SPIRIT             = ai->initSpell(FERAL_SPIRIT_1);
    LIGHTNING_SHIELD         = ai->initSpell(LIGHTNING_SHIELD_1);
    ROCKBITER_WEAPON         = ai->initSpell(ROCKBITER_WEAPON_1);
    FLAMETONGUE_WEAPON       = ai->initSpell(FLAMETONGUE_WEAPON_1);
    FROSTBRAND_WEAPON        = ai->initSpell(FROSTBRAND_WEAPON_1);
    WINDFURY_WEAPON          = ai->initSpell(WINDFURY_WEAPON_1);
    STONESKIN_TOTEM          = ai->initSpell(STONESKIN_TOTEM_1); // totems
    STRENGTH_OF_EARTH_TOTEM  = ai->initSpell(STRENGTH_OF_EARTH_TOTEM_1);
    FROST_RESISTANCE_TOTEM   = ai->initSpell(FROST_RESISTANCE_TOTEM_1);
    FLAMETONGUE_TOTEM        = ai->initSpell(FLAMETONGUE_TOTEM_1);
    FIRE_RESISTANCE_TOTEM    = ai->initSpell(FIRE_RESISTANCE_TOTEM_1);
    GROUNDING_TOTEM          = ai->initSpell(GROUNDING_TOTEM_1);
    NATURE_RESISTANCE_TOTEM  = ai->initSpell(NATURE_RESISTANCE_TOTEM_1);
    WIND_FURY_TOTEM          = ai->initSpell(WINDFURY_TOTEM_1);
    STONESKIN_TOTEM          = ai->initSpell(STONESKIN_TOTEM_1);
    WRATH_OF_AIR_TOTEM       = ai->initSpell(WRATH_OF_AIR_TOTEM_1);
    EARTH_ELEMENTAL_TOTEM    = ai->initSpell(EARTH_ELEMENTAL_TOTEM_1);
    // elemental
    LIGHTNING_BOLT           = ai->initSpell(LIGHTNING_BOLT_1);
    EARTH_SHOCK              = ai->initSpell(EARTH_SHOCK_1);
    FLAME_SHOCK              = ai->initSpell(FLAME_SHOCK_1);
    PURGE                    = ai->initSpell(PURGE_1);
    WIND_SHOCK               = 0; //NPC spell
    FROST_SHOCK              = ai->initSpell(FROST_SHOCK_1);
    CHAIN_LIGHTNING          = ai->initSpell(CHAIN_LIGHTNING_1);
    LAVA_BURST               = ai->initSpell(LAVA_BURST_1);
    HEX                      = ai->initSpell(HEX_1);
    STONECLAW_TOTEM          = ai->initSpell(STONECLAW_TOTEM_1); // totems
    SEARING_TOTEM            = ai->initSpell(SEARING_TOTEM_1);
    FIRE_NOVA_TOTEM          = 0; // NPC only spell, check FIRE_NOVA_1
    MAGMA_TOTEM              = ai->initSpell(MAGMA_TOTEM_1);
    EARTHBIND_TOTEM          = ai->initSpell(EARTHBIND_TOTEM_1);
    TOTEM_OF_WRATH           = ai->initSpell(TOTEM_OF_WRATH_1);
    FIRE_ELEMENTAL_TOTEM     = ai->initSpell(FIRE_ELEMENTAL_TOTEM_1);

    RECENTLY_BANDAGED        = 11196; // first aid check

    // racial
    GIFT_OF_THE_NAARU        = ai->initSpell(GIFT_OF_THE_NAARU_SHAMAN); // draenei
    BLOOD_FURY               = ai->initSpell(BLOOD_FURY_SHAMAN); // orc
    WAR_STOMP                = ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING               = ai->initSpell(BERSERKING_ALL); // troll
}

PlayerbotShamanAI::~PlayerbotShamanAI() {}

bool PlayerbotShamanAI::HealTarget(Unit *target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
        return false;

    if (hp < 30 && ai->GetManaPercent() >= 32 && ai->CastSpell(HEALING_WAVE, target))
        return true;
    else if (hp < 45 && ai->GetManaPercent() >= 19 && ai->CastSpell(LESSER_HEALING_WAVE, target))
        return true;
    else if (hp < 55 && !ai->HasAura(RIPTIDE, target) && ai->GetManaPercent() >= 21 && ai->CastSpell(RIPTIDE, target))
        return true;
    else if (hp < 70 && ai->GetManaPercent() >= 24 && ai->CastSpell(CHAIN_HEAL, target))
        return true;

    return false;
}

void PlayerbotShamanAI::DoNextCombatManeuver(Unit* pTarget)
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

    /*switch (ai->GetScenarioType())
    {
        //SCENARIO_PVEEASY,
        //SCENARIO_PVEHARD,
        //SCENARIO_DUEL,
        //SCENARIO_PVPEASY,
        //SCENARIO_PVPHARD
        //return;
    }*/

    ai->SetMovementOrder(PlayerbotAI::MOVEMENT_FOLLOW, m_master);   // dont want to melee mob <----changed

    Group *m_group = m_bot->GetGroup();

    // Heal myself
    if (ai->GetHealthPercent() < 30 && ai->GetManaPercent() >= 32 && ai->CastSpell(HEALING_WAVE))
        return;
    else if (ai->GetHealthPercent() < 50 && ai->GetManaPercent() >= 19 && ai->CastSpell(LESSER_HEALING_WAVE))
        return;
    else if (ai->GetHealthPercent() < 70 && HealTarget(m_bot, ai->GetHealthPercent()))
        return;

    // Heal master
    uint32 masterHP = m_master->GetHealth() * 100 / m_master->GetMaxHealth();
    if (m_master->isAlive())
    {
        if (masterHP < 30 && ai->GetManaPercent() >= 32 && ai->CastSpell(HEALING_WAVE, m_master))
            return;
        else if (masterHP < 70 && HealTarget(m_master, masterHP))
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
            if (memberHP < 30 && HealTarget(m_groupMember, memberHP))
                return;
        }
    }

    // Damage Spells
    ai->SetInFront(pTarget);

    switch (SpellSequence)
    {
        case SPELL_ENHANCEMENT:
            if (LastSpellEnhancement == 1 && !ai->HasAura(STRENGTH_OF_EARTH_TOTEM, m_bot) && ai->GetManaPercent() >= 13 && ai->CastSpell(STRENGTH_OF_EARTH_TOTEM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 5 && !ai->HasAura(STONESKIN_TOTEM, m_bot) && !ai->HasAura(STRENGTH_OF_EARTH_TOTEM, m_bot) && ai->GetManaPercent() >= 13 && ai->CastSpell(STONESKIN_TOTEM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 2 && ai->CastSpell(FOCUSED, pTarget))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 10 && !ai->HasAura(FROST_RESISTANCE_TOTEM, m_bot) && !ai->HasAura(TOTEM_OF_WRATH, m_bot) && !ai->HasAura(FLAMETONGUE_TOTEM, m_bot) && ai->GetManaPercent() >= 10 && ai->CastSpell(FROST_RESISTANCE_TOTEM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 15 && !ai->HasAura(FLAMETONGUE_TOTEM, m_bot) && !ai->HasAura(TOTEM_OF_WRATH, m_bot) && !ai->HasAura(FROST_RESISTANCE_TOTEM, m_bot) && ai->GetManaPercent() >= 14 && ai->CastSpell(FLAMETONGUE_TOTEM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 20 && !ai->HasAura(FIRE_RESISTANCE_TOTEM, m_bot) && !ai->HasAura(HEALING_STREAM_TOTEM, m_bot) && !ai->HasAura(MANA_SPRING_TOTEM, m_bot) && ai->GetManaPercent() >= 10 && ai->CastSpell(FIRE_RESISTANCE_TOTEM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 25 && !ai->HasAura(GROUNDING_TOTEM, m_bot) && !ai->HasAura(WRATH_OF_AIR_TOTEM, m_bot) && !ai->HasAura(WIND_FURY_TOTEM, m_bot) && ai->GetManaPercent() >= 5 && ai->CastSpell(GROUNDING_TOTEM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 30 && !ai->HasAura(NATURE_RESISTANCE_TOTEM, m_bot) && !ai->HasAura(WRATH_OF_AIR_TOTEM, m_bot) && !ai->HasAura(GROUNDING_TOTEM, m_bot) && !ai->HasAura(WIND_FURY_TOTEM, m_bot) && ai->GetManaPercent() >= 10 && ai->CastSpell(NATURE_RESISTANCE_TOTEM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 35 && !ai->HasAura(WIND_FURY_TOTEM, m_bot) && !ai->HasAura(WRATH_OF_AIR_TOTEM, m_bot) && !ai->HasAura(GROUNDING_TOTEM, m_bot) && ai->GetManaPercent() >= 11 && ai->CastSpell(WIND_FURY_TOTEM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 4 && !ai->HasAura(STORMSTRIKE, pTarget) && ai->GetManaPercent() >= 8 && ai->CastSpell(STORMSTRIKE, pTarget))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 6 && ai->GetManaPercent() >= 4 && ai->CastSpell(LAVA_LASH, pTarget))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 7 && ai->GetManaPercent() >= 12 && ai->CastSpell(FERAL_SPIRIT))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (!ai->HasAura(WRATH_OF_AIR_TOTEM, m_bot) && !ai->HasAura(GROUNDING_TOTEM, m_bot) && LastSpellEnhancement == 40 && ai->CastSpell(WRATH_OF_AIR_TOTEM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 45 && ai->GetManaPercent() >= 24 && ai->CastSpell(EARTH_ELEMENTAL_TOTEM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 8 && !ai->HasAura(BLOODLUST, m_master) && ai->GetManaPercent() >= 26 && ai->CastSpell(BLOODLUST))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (LastSpellEnhancement == 10 && !ai->HasAura(HEROISM, m_master) && ai->GetManaPercent() >= 26 && ai->CastSpell(HEROISM))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else if (!ai->HasAura(SHAMANISTIC_RAGE, m_bot) && LastSpellEnhancement == 11 && ai->CastSpell(SHAMANISTIC_RAGE, m_bot))
            {
                SpellSequence = SPELL_RESTORATION;
                ++LastSpellEnhancement;
            }
            else
            {
                SpellSequence = SPELL_RESTORATION;
                LastSpellEnhancement = 1;
            }

            return;

        case SPELL_RESTORATION:
            if (LastSpellRestoration < 3 && ai->GetHealthPercent() < 50 && !ai->HasAura(HEALING_STREAM_TOTEM, m_bot) && !ai->HasAura(MANA_SPRING_TOTEM, m_bot) && ai->GetManaPercent() >= 4 && ai->CastSpell(HEALING_STREAM_TOTEM))
            {
                SpellSequence = SPELL_ELEMENTAL;
                ++LastSpellRestoration;
            }
            else if (LastSpellRestoration < 4 && !ai->HasAura(MANA_SPRING_TOTEM, m_bot) && !ai->HasAura(HEALING_STREAM_TOTEM, m_bot) && ai->GetManaPercent() >= 5 && ai->CastSpell(MANA_SPRING_TOTEM))
            {
                SpellSequence = SPELL_ELEMENTAL;
                ++LastSpellRestoration;
            }
            else if (LastSpellRestoration < 5 && ai->GetManaPercent() < 50 && ai->GetManaPercent() >= 3 && ai->CastSpell(MANA_TIDE_TOTEM))
            {
                SpellSequence = SPELL_ELEMENTAL;
                ++LastSpellRestoration;
            }
            /*else if (LastSpellRestoration < 6 && !ai->HasAura(STRENGTH_OF_EARTH_TOTEM, m_bot) && ai->GetManaPercent() >= 2 && ai->CastSpell(TREMOR_TOTEM))
            {
                SpellSequence = SPELL_ELEMENTAL;
                ++LastSpellRestoration;
            }*/
            else
            {
                SpellSequence = SPELL_ELEMENTAL;
                LastSpellRestoration = 0;
            }

            return;

        case SPELL_ELEMENTAL:
            if (LastSpellElemental == 1 && ai->GetManaPercent() >= 13 && ai->CastSpell(LIGHTNING_BOLT, pTarget))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 2 && !ai->HasAura(SEARING_TOTEM, pTarget) && !ai->HasAura(TOTEM_OF_WRATH, m_bot) && ai->GetManaPercent() >= 9 && ai->CastSpell(SEARING_TOTEM))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (ai->GetHealthPercent() < 51 && LastSpellElemental == 3 && !ai->HasAura(STONECLAW_TOTEM, pTarget) && !ai->HasAura(EARTHBIND_TOTEM, pTarget) && !ai->HasAura(STRENGTH_OF_EARTH_TOTEM, m_bot) && ai->GetManaPercent() >= 8 && ai->CastSpell(STONECLAW_TOTEM))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 4 && !ai->HasAura(FLAME_SHOCK, pTarget) && ai->GetManaPercent() >= 22 && ai->CastSpell(FLAME_SHOCK, pTarget))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 5 && ai->HasAura(FLAME_SHOCK, pTarget) && ai->GetManaPercent() >= 10 && ai->CastSpell(LAVA_BURST, pTarget))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 6 && !ai->HasAura(TOTEM_OF_WRATH, m_bot) && ai->GetManaPercent() >= 35 && ai->CastSpell(MAGMA_TOTEM))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 7 && !ai->HasAura(EARTHBIND_TOTEM, pTarget) && !ai->HasAura(STRENGTH_OF_EARTH_TOTEM, m_bot) && ai->GetManaPercent() >= 5 && ai->CastSpell(EARTHBIND_TOTEM))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 8 && ai->GetManaPercent() >= 23 && ai->CastSpell(EARTH_SHOCK, pTarget))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 9 && ai->GetManaPercent() >= 8 && ai->CastSpell(PURGE, pTarget))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 10 && ai->GetManaPercent() >= 8 && ai->CastSpell(WIND_SHOCK, pTarget))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 11 && ai->GetManaPercent() >= 33 && ai->CastSpell(FIRE_NOVA_TOTEM))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 12 && !ai->HasAura(FROST_SHOCK, pTarget) && ai->GetManaPercent() >= 23 && ai->CastSpell(FROST_SHOCK, pTarget))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 13 && ai->GetManaPercent() >= 33 && ai->CastSpell(CHAIN_LIGHTNING, pTarget))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 14 && !ai->HasAura(TOTEM_OF_WRATH, m_bot) && ai->GetManaPercent() >= 5 && ai->CastSpell(TOTEM_OF_WRATH))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            else if (LastSpellElemental == 15 && ai->GetManaPercent() >= 23 && ai->CastSpell(FIRE_ELEMENTAL_TOTEM))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }
            /*else if (LastSpellElemental == 16 && !ai->HasAura(HEX, pTarget) && ai->GetManaPercent() >= 3 && ai->CastSpell(HEX))
            {
                SpellSequence = SPELL_ENHANCEMENT;
                ++LastSpellElemental;
            }*/
            else
            {
                SpellSequence = SPELL_ENHANCEMENT;
                LastSpellElemental = 1;
            }

            return;
    }
}

void PlayerbotShamanAI::DoNonCombatActions()
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

    SpellSequence = SPELL_ENHANCEMENT;

    // buff master with EARTH_SHIELD
    if (!ai->HasAura(EARTH_SHIELD, m_master) && ai->CastSpell(EARTH_SHIELD, m_master))
        return;

    // buff myself with WATER_SHIELD, LIGHTNING_SHIELD
    else if (!ai->HasAura(WATER_SHIELD, m_bot) && !ai->HasAura(LIGHTNING_SHIELD, m_bot) && ai->CastSpell(WATER_SHIELD, m_bot))
        return;
    else if (!ai->HasAura(LIGHTNING_SHIELD, m_bot) && !ai->HasAura(WATER_SHIELD, m_bot) && ai->CastSpell(LIGHTNING_SHIELD, m_bot))
        return;
    /*
    // buff myself weapon
    if (!ai->HasAura(ROCKBITER_WEAPON, m_bot) && !ai->HasAura(EARTHLIVING_WEAPON, m_bot) && !ai->HasAura(WINDFURY_WEAPON, m_bot) && !ai->HasAura(FLAMETONGUE_WEAPON, m_bot) && !ai->HasAura(FROSTBRAND_WEAPON, m_bot) && ai->CastSpell(ROCKBITER_WEAPON,m_bot))
        return;
    else if (!ai->HasAura(EARTHLIVING_WEAPON, m_bot) && !ai->HasAura(EARTHLIVING_WEAPON, m_bot) && !ai->HasAura(FLAMETONGUE_WEAPON, m_bot) && !ai->HasAura(FROSTBRAND_WEAPON, m_bot) && !ai->HasAura(ROCKBITER_WEAPON, m_bot) && ai->CastSpell(WINDFURY_WEAPON,m_bot))
        return;
    else if (!ai->HasAura(WINDFURY_WEAPON, m_bot) && !ai->HasAura(EARTHLIVING_WEAPON, m_bot) && !ai->HasAura(FLAMETONGUE_WEAPON, m_bot) && !ai->HasAura(FROSTBRAND_WEAPON, m_bot) && !ai->HasAura(ROCKBITER_WEAPON, m_bot) && ai->CastSpell(WINDFURY_WEAPON,m_bot))
        return;
    else if (!ai->HasAura(FLAMETONGUE_WEAPON, m_bot) && !ai->HasAura(EARTHLIVING_WEAPON, m_bot) && !ai->HasAura(WINDFURY_WEAPON, m_bot) && !ai->HasAura(FROSTBRAND_WEAPON, m_bot) && !ai->HasAura(ROCKBITER_WEAPON, m_bot) && ai->CastSpell(FLAMETONGUE_WEAPON,m_bot))
        return;
    else if (!ai->HasAura(FROSTBRAND_WEAPON, m_bot) && !ai->HasAura(EARTHLIVING_WEAPON, m_bot) && !ai->HasAura(WINDFURY_WEAPON, m_bot) && !ai->HasAura(FLAMETONGUE_WEAPON, m_bot) && !ai->HasAura(ROCKBITER_WEAPON, m_bot) && ai->CastSpell(FROSTBRAND_WEAPON,m_bot))
        return;
    */

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItemDrink = ai->FindDrink();
    Item* pItemFood = ai->FindFood();
    Item* pItemBandage = ai->FindBandage();

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

    // heal master's group
    if (m_master->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_master->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || !tPlayer->isAlive())
                continue;

            // heal
            if (HealTarget(tPlayer, tPlayer->GetHealth() * 100 / tPlayer->GetMaxHealth()))
                return;
        }
    }
}
