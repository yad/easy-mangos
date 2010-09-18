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

PlayerbotMageAI::~PlayerbotMageAI() {}

void PlayerbotMageAI::DoNextCombatManeuver(Unit* pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* m_bot = GetPlayerBot();
    if (!m_bot)
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

    //ai->SetMovementOrder( PlayerbotAI::MOVEMENT_FOLLOW, m_master ); // dont want to melee mob

    // Damage Spells (primitive example)
    ai->SetInFront(pTarget);
    Unit* pVictim = pTarget->getVictim();
    float dist = m_bot->GetDistance(pTarget);

    switch (SpellSequence)
    {
        case SPELL_FROST:        
            if (!ai->HasAura(ICY_VEINS, m_bot) && LastSpellFrost < 1 && ai->GetManaPercent() >= 3 && ai->CastSpell(ICY_VEINS, m_bot))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 2 && !ai->HasAura(FROSTBOLT, pTarget) && ai->GetManaPercent() >= 16 && ai->CastSpell(FROSTBOLT, pTarget))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 3 && !ai->HasAura(FROST_WARD, m_bot) && ai->GetManaPercent() >= 19 && ai->CastSpell(FROST_WARD, m_bot))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 4 && dist <= ATTACK_DISTANCE && !ai->HasAura(FROST_NOVA, pTarget) && ai->GetManaPercent() >= 10 && ai->CastSpell(FROST_NOVA, pTarget))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 5 && ai->GetManaPercent() >= 7 && ai->CastSpell(ICE_LANCE, pTarget))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 6 && ai->GetAttackerCount() >= 5 && ai->GetManaPercent() >= 89 && ai->CastSpell(BLIZZARD, pTarget))
            {
                ai->SetIgnoreUpdateTime(8);
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 7 && dist <= ATTACK_DISTANCE && !ai->HasAura(CONE_OF_COLD, pTarget) && ai->GetManaPercent() >= 35 && ai->CastSpell(CONE_OF_COLD, pTarget))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 8 && ai->HasAura(AURA_STATE_FROZEN, pTarget) && !ai->HasAura(DEEP_FREEZE, pTarget) && ai->GetManaPercent() >= 9 && ai->CastSpell(DEEP_FREEZE, pTarget))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 9 && pVictim == m_bot && !ai->HasAura(ICE_BARRIER, m_bot) && ai->GetHealthPercent() < 50 && ai->GetManaPercent() >= 30 && ai->CastSpell(ICE_BARRIER, m_bot))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 10 && ai->GetManaPercent() >= 16 && ai->CastSpell(SUMMON_WATER_ELEMENTAL))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 11 && pVictim == m_bot && !ai->HasAura(ICE_BLOCK, m_bot) && ai->GetHealthPercent() < 30 && ai->CastSpell(ICE_BLOCK, m_bot))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else if (LastSpellFrost < 12 && ai->CastSpell(COLD_SNAP, m_bot))
            {
                SpellSequence = SPELL_FIRE;
                ++LastSpellFrost;
            }
            else
                LastSpellFrost = 0;
                
            return;
            
        case SPELL_FIRE:        
            if (!ai->HasAura(FIRE_WARD, m_bot) && LastSpellFire < 1 && ai->GetManaPercent() >= 3 && ai->CastSpell(FIRE_WARD, m_bot))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else if (!ai->HasAura(COMBUSTION, m_bot) && LastSpellFire < 2 && ai->CastSpell(COMBUSTION, m_bot))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else if (LastSpellFire < 3 && ai->GetManaPercent() >= 23 && ai->CastSpell(FIREBALL, pTarget))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else if (LastSpellFire < 4 && ai->GetManaPercent() >= 25 && ai->CastSpell(FIRE_BLAST, pTarget))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else if (LastSpellFire < 5 && ai->GetManaPercent() >= 35 && ai->CastSpell(FLAMESTRIKE, pTarget))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else if (LastSpellFire < 6 && ai->GetManaPercent() >= 10 && ai->CastSpell(SCORCH, pTarget))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else if (LastSpellFire < 7 && !ai->HasAura(PYROBLAST, pTarget) && ai->GetManaPercent() >= 27 && ai->CastSpell(PYROBLAST, pTarget))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else if (LastSpellFire < 8 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 34 && ai->CastSpell(BLAST_WAVE, pTarget))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else if (LastSpellFire < 9 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 37 && ai->CastSpell(DRAGONS_BREATH, pTarget))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else if (LastSpellFire < 10 && !ai->HasAura(LIVING_BOMB, pTarget) && ai->GetManaPercent() >= 27 && ai->CastSpell(LIVING_BOMB, pTarget))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else if (LastSpellFire < 11 && !ai->HasAura(FROSTFIRE_BOLT, pTarget) && ai->GetManaPercent() >= 14 && ai->CastSpell(FROSTFIRE_BOLT, pTarget))
            {
                SpellSequence = SPELL_ARCANE;
                ++LastSpellFire;
            }
            else
                LastSpellFire = 0;
                
            return;

        case SPELL_ARCANE:
            if (LastSpellArcane < 1 && ai->GetManaPercent() >= 37 && ai->CastSpell(ARCANE_POWER, pTarget))
            {
                SpellSequence = SPELL_FROST;
                ++LastSpellArcane;
            }
            else if (LastSpellArcane < 2 && ai->GetManaPercent() >= 37 && ai->CastSpell(ARCANE_MISSILES, pTarget))
            {
                ai->SetIgnoreUpdateTime(3);
                SpellSequence = SPELL_FROST;
                ++LastSpellArcane;
            }
            else if (LastSpellArcane < 3 && ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && ai->GetManaPercent() >= 27 && ai->CastSpell(ARCANE_EXPLOSION, pTarget))
            {
                SpellSequence = SPELL_FROST;
                ++LastSpellArcane;
            }
            else if (pTarget->IsNonMeleeSpellCasted(true) && LastSpellArcane < 4 && ai->GetManaPercent() >= 9 && ai->CastSpell(COUNTERSPELL, pTarget))
            {
                SpellSequence = SPELL_FROST;
                ++LastSpellArcane;
            }
            else if (LastSpellArcane < 5 && !ai->HasAura(SLOW, pTarget) && ai->GetManaPercent() >= 12 && ai->CastSpell(SLOW, pTarget))
            {
                SpellSequence = SPELL_FROST;
                ++LastSpellArcane;
            }
            else if (LastSpellArcane < 6 && ai->GetManaPercent() >= 27 && ai->CastSpell(ARCANE_BARRAGE, pTarget))
            {
                SpellSequence = SPELL_FROST;
                ++LastSpellArcane;
            }
            else if (LastSpellArcane < 7 && ai->GetManaPercent() >= 8 && ai->CastSpell(ARCANE_BLAST, pTarget))
            {
                SpellSequence = SPELL_FROST;
                ++LastSpellArcane;
            }
            else if (LastSpellArcane < 8 && ai->GetManaPercent() >= 10 && ai->CastSpell(MIRROR_IMAGE))
            {
                SpellSequence = SPELL_FROST;
                ++LastSpellArcane;
            }
            else if (LastSpellArcane < 9 && ai->GetHealthPercent() < 70 && pVictim == m_bot && !ai->HasAura(MANA_SHIELD, m_bot) && ai->GetManaPercent() >= 8 && ai->CastSpell(MANA_SHIELD, m_bot))
            {
                SpellSequence = SPELL_FROST;
                ++LastSpellArcane;
            }
            else
            {
                SpellSequence = SPELL_FROST;            
                LastSpellArcane = 0;
            }
                
            return;            
    }
}

void PlayerbotMageAI::DoNonCombatActions()
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

    SpellSequence = SPELL_FROST;

    // buff master
    if (!ai->HasAura(DALARAN_BRILLIANCE, m_master) && ai->GetManaPercent() >= 81 && ai->CastSpell(DALARAN_BRILLIANCE, m_master))
        return;
    else if (!ai->HasAura(ARCANE_BRILLIANCE, m_master) && !ai->HasAura(DALARAN_BRILLIANCE, m_master) && ai->GetManaPercent() >= 97 && ai->CastSpell(ARCANE_BRILLIANCE, m_master))
        return;

    // buff myself
    else if (!ai->HasAura(DALARAN_INTELLECT, m_bot) && !ai->HasAura(DALARAN_BRILLIANCE, m_bot) && !ai->HasAura(ARCANE_BRILLIANCE, m_bot) && ai->GetManaPercent() >= 31 && ai->CastSpell(DALARAN_INTELLECT, m_bot))
        return;
    else if (!ai->HasAura(ARCANE_INTELLECT, m_bot) && !ai->HasAura(DALARAN_BRILLIANCE, m_bot) && !ai->HasAura(ARCANE_BRILLIANCE, m_bot) && !ai->HasAura(DALARAN_INTELLECT, m_bot) && ai->GetManaPercent() >= 37 && ai->CastSpell(ARCANE_INTELLECT, m_bot))
        return;
    else if (!ai->HasAura(MOLTEN_ARMOR, m_bot) && !ai->HasAura(MAGE_ARMOR, m_bot) && ai->GetManaPercent() >= 31 && ai->CastSpell(MOLTEN_ARMOR, m_bot))
        return;    
    else if (!ai->HasAura(MAGE_ARMOR, m_bot) && !ai->HasAura(MOLTEN_ARMOR, m_bot) && ai->GetManaPercent() >= 31 && ai->CastSpell(MAGE_ARMOR, m_bot))
        return;    
    else if (!ai->HasAura(ICE_ARMOR, m_bot) && !ai->HasAura(MOLTEN_ARMOR, m_bot) && !ai->HasAura(MAGE_ARMOR, m_bot) && ai->GetManaPercent() >= 34 && ai->CastSpell(ICE_ARMOR, m_bot))
        return;    
    else if (!ai->HasAura(FROST_ARMOR, m_bot) && !ai->HasAura(MOLTEN_ARMOR, m_bot) && !ai->HasAura(MAGE_ARMOR, m_bot) && !ai->HasAura(ICE_ARMOR, m_bot) && ai->GetManaPercent() >= 34 && ai->CastSpell(FROST_ARMOR, m_bot))
        return;    

    // buff master's group
    if (m_master->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = m_master->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = sObjectMgr.GetPlayer(itr->guid);
            if (!tPlayer || !tPlayer->isAlive())
                continue;
            // buff
            if (!ai->HasAura(ARCANE_INTELLECT, tPlayer) && !ai->HasAura(DALARAN_BRILLIANCE, tPlayer) && !ai->HasAura(ARCANE_BRILLIANCE, tPlayer) && !ai->HasAura(DALARAN_INTELLECT, tPlayer) && ai->GetManaPercent() >= 37 && ai->CastSpell(ARCANE_INTELLECT, tPlayer))
                return;
            else if (!ai->HasAura(DALARAN_INTELLECT, tPlayer) && !ai->HasAura(DALARAN_BRILLIANCE, tPlayer) && !ai->HasAura(ARCANE_BRILLIANCE, tPlayer) && ai->GetManaPercent() >= 31 && ai->CastSpell(DALARAN_INTELLECT, tPlayer))
                return;            
            else if (!ai->HasAura(DAMPEN_MAGIC, tPlayer) && !ai->HasAura(AMPLIFY_MAGIC, tPlayer) && ai->GetManaPercent() >= 32 && ai->CastSpell(DAMPEN_MAGIC, tPlayer))
                return;            
            else if (!ai->HasAura(AMPLIFY_MAGIC, tPlayer) && !ai->HasAura(DAMPEN_MAGIC, tPlayer) && ai->GetManaPercent() >= 32 && ai->CastSpell(AMPLIFY_MAGIC, tPlayer))
                return;
        }
    }

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItemDrink = ai->FindDrink();
    Item* pItemFood = ai->FindFood();
    Item* pItemBandage = ai->FindBandage();        
    
    // mana check + conjure water
    if (!pItemDrink && CONJURE_WATER && ai->GetBaseManaPercent() >= 48 && ai->CastSpell(CONJURE_WATER, m_bot))
    {
        ai->SetIgnoreUpdateTime(3);
        return;
    }
    else if (pItemDrink && ai->GetManaPercent() < 30)
    {
        ai->UseItem(*pItemDrink);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
    // hp check + conjure food
    else if (pItemFood && CONJURE_FOOD && ai->GetBaseManaPercent() >= 48 && ai->CastSpell(CONJURE_FOOD, m_bot))
    {
        ai->SetIgnoreUpdateTime(3);
        return;
    }    
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
}
