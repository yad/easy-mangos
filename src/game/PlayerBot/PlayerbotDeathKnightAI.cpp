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

#include "PlayerbotDeathKnightAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotDeathKnightAI::PlayerbotDeathKnightAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

void PlayerbotDeathKnightAI::InitSpells(PlayerbotAI* const ai)
{
    PLAGUE_STRIKE     = ai->initSpell(PLAGUE_STRIKE_1); // Unholy
    DEATH_GRIP        = ai->initSpell(DEATH_GRIP_1);
    DEATH_COIL        = ai->initSpell(DEATH_COIL_DEATH_KNIGHT_1);
    DEATH_STRIKE      = ai->initSpell(DEATH_STRIKE_1);
    UNHOLY_BLIGHT     = 0; // Passive
    SCOURGE_STRIKE    = ai->initSpell(SCOURGE_STRIKE_1);
    DEATH_AND_DECAY   = ai->initSpell(DEATH_AND_DECAY_1);
    CORPSE_EXPLOSION  = ai->initSpell(CORPSE_EXPLOSION_1);
    BONE_SHIELD       = ai->initSpell(BONE_SHIELD_1); // buffs
    ANTI_MAGIC_SHELL  = ai->initSpell(ANTI_MAGIC_SHELL_1);
    ANTI_MAGIC_ZONE   = ai->initSpell(ANTI_MAGIC_ZONE_1);
    GHOUL_FRENZY      = ai->initSpell(GHOUL_FRENZY_1);
    RAISE_DEAD        = ai->initSpell(RAISE_DEAD_1); // pets
    SUMMON_GARGOYLE   = ai->initSpell(SUMMON_GARGOYLE_1);
    ARMY_OF_THE_DEAD  = ai->initSpell(ARMY_OF_THE_DEAD_1);
    ICY_TOUCH         = ai->initSpell(ICY_TOUCH_1); // Frost
    OBLITERATE        = ai->initSpell(OBLITERATE_1);
    HOWLING_BLAST     = ai->initSpell(HOWLING_BLAST_1);
    FROST_STRIKE      = ai->initSpell(FROST_STRIKE_1);
    CHAINS_OF_ICE     = ai->initSpell(CHAINS_OF_ICE_1);
    RUNE_STRIKE       = ai->initSpell(RUNE_STRIKE_1);
    ICY_CLUTCH        = 0; // No such spell
    MIND_FREEZE       = ai->initSpell(MIND_FREEZE_1);
    HUNGERING_COLD    = ai->initSpell(HUNGERING_COLD_1);
    KILLING_MACHINE   = 0; // Passive
    DEATHCHILL        = ai->initSpell(DEATHCHILL_1);
    HORN_OF_WINTER    = ai->initSpell(HORN_OF_WINTER_1);
    ICEBOUND_FORTITUDE = ai->initSpell(ICEBOUND_FORTITUDE_1);
    EMPOWER_WEAPON    = ai->initSpell(EMPOWER_RUNE_WEAPON_1);
    UNBREAKABLE_ARMOR = ai->initSpell(UNBREAKABLE_ARMOR_1);
    BLOOD_STRIKE      = ai->initSpell(BLOOD_STRIKE_1); // Blood
    PESTILENCE        = ai->initSpell(PESTILENCE_1);
    STRANGULATE       = ai->initSpell(STRANGULATE_1);
    BLOOD_BOIL        = ai->initSpell(BLOOD_BOIL_1);
    HEART_STRIKE      = ai->initSpell(HEART_STRIKE_1);
    DANCING_WEAPON    = ai->initSpell(DANCING_RUNE_WEAPON_1);
    DARK_COMMAND      = ai->initSpell(DARK_COMMAND_1);
    MARK_OF_BLOOD     = ai->initSpell(MARK_OF_BLOOD_1); // buffs
    RUNE_TAP          = ai->initSpell(RUNE_TAP_1);
    VAMPIRIC_BLOOD    = ai->initSpell(VAMPIRIC_BLOOD_1);
    DEATH_PACT        = ai->initSpell(DEATH_PACT_1);
    HYSTERIA          = ai->initSpell(HYSTERIA_1);
    UNHOLY_PRESENCE   = ai->initSpell(UNHOLY_PRESENCE_1); // presence (TODO: better spell == presence)
    FROST_PRESENCE    = ai->initSpell(FROST_PRESENCE_1);
    BLOOD_PRESENCE    = ai->initSpell(BLOOD_PRESENCE_1);

    RECENTLY_BANDAGED = 11196; // first aid check

    // racial
    ARCANE_TORRENT          = ai->initSpell(ARCANE_TORRENT_DEATH_KNIGHT); // blood elf
    GIFT_OF_THE_NAARU       = ai->initSpell(GIFT_OF_THE_NAARU_DEATH_KNIGHT); // draenei
    STONEFORM               = ai->initSpell(STONEFORM_ALL); // dwarf
    ESCAPE_ARTIST           = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF   = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    BLOOD_FURY              = ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP               = ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING              = ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN    = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}

PlayerbotDeathKnightAI::~PlayerbotDeathKnightAI() {}

void PlayerbotDeathKnightAI::DoNextCombatManeuver(Unit* pTarget)
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

    // DK Attacks: Unholy, Frost & Blood

    // damage spells
    ai->SetInFront(pTarget);  //<---
    Unit* pVictim = pTarget->getVictim();
    Pet *pet = m_bot->GetPet();
    float dist = m_bot->GetDistance(pTarget);

    switch (SpellSequence)
    {
        case SPELL_DK_UNHOLY:
            if (!ai->HasAura(UNHOLY_PRESENCE, m_bot) && !ai->HasAura(BLOOD_PRESENCE, m_bot) && !ai->HasAura(FROST_PRESENCE, m_bot) && ai->CastSpell(UNHOLY_PRESENCE, m_bot))
                return;
            // check for BONE_SHIELD in combat
            else if (!ai->HasAura(BONE_SHIELD, m_bot) && !ai->HasAura(ARMY_OF_THE_DEAD, m_bot) && ai->CastSpell(BONE_SHIELD, m_bot))
                return;
                
            if (!ai->HasAura(ARMY_OF_THE_DEAD, m_bot) && ai->GetAttackerCount() >= 5 && LastSpellUnholyDK < 1 && ai->CastSpell(ARMY_OF_THE_DEAD))
            {
                ai->SetIgnoreUpdateTime(7);
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (!ai->HasAura(PLAGUE_STRIKE, pTarget) && LastSpellUnholyDK < 2 && ai->CastSpell(PLAGUE_STRIKE, pTarget))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (!ai->HasAura(DEATH_GRIP, pTarget) && LastSpellUnholyDK < 3 && ai->CastSpell(DEATH_GRIP, pTarget))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (LastSpellUnholyDK < 4 && ai->GetRunicPower() >= 40 && ai->CastSpell(DEATH_COIL, pTarget))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (!ai->HasAura(DEATH_STRIKE, pTarget) && LastSpellUnholyDK < 5 && ai->CastSpell(DEATH_STRIKE, pTarget))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (!ai->HasAura(UNHOLY_BLIGHT, pTarget) && LastSpellUnholyDK < 6 && ai->CastSpell(UNHOLY_BLIGHT))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (LastSpellUnholyDK < 7 && ai->CastSpell(SCOURGE_STRIKE, pTarget))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && !ai->HasAura(DEATH_AND_DECAY, pTarget) && LastSpellUnholyDK < 8 && ai->CastSpell(DEATH_AND_DECAY))
            {
                ai->SetIgnoreUpdateTime(1);
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (!ai->HasAura(ARMY_OF_THE_DEAD, m_bot) && !ai->HasAura(SUMMON_GARGOYLE, pTarget) && LastSpellUnholyDK < 9 && ai->GetRunicPower() >= 60 && ai->CastSpell(SUMMON_GARGOYLE, pTarget))
            {
                ai->SetIgnoreUpdateTime(2);
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (dist <= ATTACK_DISTANCE && LastSpellUnholyDK < 10 && ai->CastSpell(CORPSE_EXPLOSION, pTarget))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (pTarget->IsNonMeleeSpellCasted(true) && !ai->HasAura(ANTI_MAGIC_SHELL, m_bot) && LastSpellUnholyDK < 11 && ai->GetRunicPower() >= 20 && ai->CastSpell(ANTI_MAGIC_SHELL, m_bot))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (pTarget->IsNonMeleeSpellCasted(true) && !ai->HasAura(ANTI_MAGIC_SHELL, m_bot) && LastSpellUnholyDK < 12 && ai->CastSpell(ANTI_MAGIC_ZONE, m_bot))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (!pet && !ai->HasAura(ARMY_OF_THE_DEAD, m_bot) && LastSpellUnholyDK < 13 && ai->CastSpell(RAISE_DEAD))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else if (pet && pVictim == pet && !ai->HasAura(GHOUL_FRENZY, pet) && LastSpellUnholyDK < 14 && ai->CastSpell(GHOUL_FRENZY, pet))
            {
                SpellSequence = SPELL_DK_FROST;
                ++LastSpellUnholyDK;
            }
            else
            {
                SpellSequence = SPELL_DK_FROST;            
                LastSpellUnholyDK = 0;
            }

            return;

        case SPELL_DK_FROST:        
            if (!ai->HasAura(FROST_PRESENCE, m_bot) && !ai->HasAura(BLOOD_PRESENCE, m_bot) && !ai->HasAura(UNHOLY_PRESENCE, m_bot) && ai->CastSpell(FROST_PRESENCE, m_bot))
                return;
            else if (!ai->HasAura(DEATHCHILL, m_bot) && !ai->HasAura(KILLING_MACHINE, m_bot) && ai->CastSpell(DEATHCHILL, m_bot))
                return;
            else if (!ai->HasAura(KILLING_MACHINE, m_bot) && !ai->HasAura(DEATHCHILL, m_bot) && ai->CastSpell(KILLING_MACHINE, m_bot))
                return;
                
            if (!ai->HasAura(ICY_TOUCH, pTarget) && LastSpellFrostDK < 1 && ai->CastSpell(ICY_TOUCH, pTarget))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (LastSpellFrostDK < 2 && ai->CastSpell(OBLITERATE, pTarget))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (LastSpellFrostDK < 3 && ai->GetRunicPower() >= 40 && ai->CastSpell(FROST_STRIKE, pTarget))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (ai->GetAttackerCount() >= 3 && LastSpellFrostDK < 4 && ai->CastSpell(HOWLING_BLAST, pTarget))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (!ai->HasAura(CHAINS_OF_ICE, pTarget) && LastSpellFrostDK < 5 && ai->CastSpell(CHAINS_OF_ICE, pTarget))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (LastSpellFrostDK < 6 && ai->GetRunicPower() >= 20 && ai->CastSpell(RUNE_STRIKE, pTarget))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (!ai->HasAura(ICY_CLUTCH, pTarget) && LastSpellFrostDK < 7 && ai->CastSpell(ICY_CLUTCH, pTarget))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (ai->GetHealthPercent() < 50 && pVictim == m_bot && !ai->HasAura(ICEBOUND_FORTITUDE, m_bot) && LastSpellFrostDK < 8 && ai->GetRunicPower() >= 20 && ai->CastSpell(ICEBOUND_FORTITUDE, m_bot))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (pTarget->IsNonMeleeSpellCasted(true) && dist <= ATTACK_DISTANCE && LastSpellFrostDK < 9 && ai->GetRunicPower() >= 20 && ai->CastSpell(MIND_FREEZE, pTarget))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (ai->GetAttackerCount() >= 3 && dist <= ATTACK_DISTANCE && LastSpellFrostDK < 10 && ai->GetRunicPower() >= 40 && ai->CastSpell(HUNGERING_COLD, pTarget))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (ai->GetRunicPower() < 20 && LastSpellFrostDK < 11 && ai->CastSpell(EMPOWER_WEAPON, m_bot))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else if (!ai->HasAura(UNBREAKABLE_ARMOR, m_bot) && ai->GetHealthPercent() < 70 && pVictim == m_bot && LastSpellFrostDK < 12 && ai->CastSpell(UNBREAKABLE_ARMOR, m_bot))
            {
                SpellSequence = SPELL_DK_BLOOD;
                ++LastSpellFrostDK;
            }
            else
            {
                SpellSequence = SPELL_DK_BLOOD;
                LastSpellFrostDK = 0;                
            }

            return;

        case SPELL_DK_BLOOD:        
            if (!ai->HasAura(BLOOD_PRESENCE, m_bot) && !ai->HasAura(UNHOLY_PRESENCE, m_bot) && !ai->HasAura(FROST_PRESENCE, m_bot) && ai->CastSpell(BLOOD_PRESENCE, m_bot))
                return;
                
            if (!ai->HasAura(MARK_OF_BLOOD, pTarget) && LastSpellBloodDK < 1 && ai->CastSpell(MARK_OF_BLOOD, pTarget))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else if (LastSpellBloodDK < 2 && ai->CastSpell(BLOOD_STRIKE, pTarget))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                LastSpellBloodDK =  LastSpellBloodDK + 1;
            }
            else if (dist <= ATTACK_DISTANCE && ai->GetAttackerCount() >= 3 && LastSpellBloodDK < 3 && ai->CastSpell(PESTILENCE, pTarget))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else if (!ai->HasAura(STRANGULATE, pTarget) && LastSpellBloodDK < 4 && ai->CastSpell(STRANGULATE, pTarget))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else if (ai->GetAttackerCount() >= 5 && dist <= ATTACK_DISTANCE && LastSpellBloodDK < 5 && ai->CastSpell(BLOOD_BOIL, pTarget))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else if (LastSpellBloodDK < 6 && ai->CastSpell(HEART_STRIKE, pTarget))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else if (ai->GetHealthPercent() < 70 && !ai->HasAura(VAMPIRIC_BLOOD, m_bot) && LastSpellBloodDK < 7 && ai->CastSpell(VAMPIRIC_BLOOD, m_bot))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else if (ai->GetHealthPercent() < 70 && !ai->HasAura(VAMPIRIC_BLOOD, m_bot) && LastSpellBloodDK < 8 && ai->CastSpell(RUNE_TAP, m_bot))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else if (ai->GetHealthPercent() > 25 && !ai->HasAura(HYSTERIA, m_bot) && LastSpellBloodDK < 9 && ai->CastSpell(HYSTERIA, m_bot))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else if (!ai->HasAura(DANCING_WEAPON, m_bot) && ai->GetRunicPower() >= 60 && LastSpellBloodDK < 10 && ai->CastSpell(DANCING_WEAPON, pTarget))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else if (ai->GetHealthPercent() > 50 && pVictim != m_bot && !ai->HasAura(DARK_COMMAND, pTarget) && dist <= ATTACK_DISTANCE && LastSpellBloodDK < 11 && ai->CastSpell(DARK_COMMAND, pTarget))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else if (pet && ai->GetHealthPercent() < 50 && LastSpellBloodDK < 12 && ai->GetRunicPower() >= 40 && ai->CastSpell(DEATH_PACT, pet))
            {
                SpellSequence = SPELL_DK_UNHOLY;
                ++LastSpellBloodDK;
            }
            else
            {
                LastSpellBloodDK = 0;
                SpellSequence = SPELL_DK_UNHOLY;
            }
            
            return;
            
    }

}

void PlayerbotDeathKnightAI::DoNonCombatActions()
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

    SpellSequence = SPELL_DK_UNHOLY;

    // buff master with HORN_OF_WINTER
    if (!ai->HasAura(HORN_OF_WINTER, m_master) && ai->CastSpell(HORN_OF_WINTER, m_master))
        return;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItemFood = ai->FindFood();
    Item* pItemBandage = ai->FindBandage();        
    
    // hp check
    if (pItemFood && ai->GetHealthPercent() < 30)
    {
        ai->UseItem(pItemFood);
        ai->SetIgnoreUpdateTime(30);
        return;
    }
    else if (!pItemFood && pItemBandage && !ai->HasAura(RECENTLY_BANDAGED, m_bot) && ai->GetHealthPercent() < 70)
    {
        ai->UseItem(pItemBandage);
        ai->SetIgnoreUpdateTime(8);
        return;
    }
}
