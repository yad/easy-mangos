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
#include "PlayerbotWarriorAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;

PlayerbotWarriorAI::PlayerbotWarriorAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
    LastSpellArms = 0;
    LastSpellProtection = 0;
    LastSpellFury = 0;
}

void PlayerbotWarriorAI::InitSpells(PlayerbotAI* const ai)
{
    BATTLE_STANCE           = ai->initSpell(BATTLE_STANCE_1); //ARMS
    CHARGE                  = ai->initSpell(CHARGE_1); //ARMS
    OVERPOWER               = ai->initSpell(OVERPOWER_1); // ARMS
    HEROIC_STRIKE           = ai->initSpell(HEROIC_STRIKE_1); //ARMS
    REND                    = ai->initSpell(REND_1); //ARMS
    THUNDER_CLAP            = ai->initSpell(THUNDER_CLAP_1);  //ARMS
    HAMSTRING               = ai->initSpell(HAMSTRING_1);  //ARMS
    MOCKING_BLOW            = ai->initSpell(MOCKING_BLOW_1);  //ARMS
    RETALIATION             = ai->initSpell(RETALIATION_1);  //ARMS
    SWEEPING_STRIKES        = ai->initSpell(SWEEPING_STRIKES_1); //ARMS
    MORTAL_STRIKE           = ai->initSpell(MORTAL_STRIKE_1);  //ARMS
    BLADESTORM              = ai->initSpell(BLADESTORM_1);  //ARMS
    HEROIC_THROW            = ai->initSpell(HEROIC_THROW_1);  //ARMS
    SHATTERING_THROW        = ai->initSpell(SHATTERING_THROW_1);  //ARMS
    BLOODRAGE               = ai->initSpell(BLOODRAGE_1); //PROTECTION
    DEFENSIVE_STANCE        = ai->initSpell(DEFENSIVE_STANCE_1); //PROTECTION
    DEVASTATE               = ai->initSpell(DEVASTATE_1); //PROTECTION
    SUNDER_ARMOR            = ai->initSpell(SUNDER_ARMOR_1); //PROTECTION
    TAUNT                   = ai->initSpell(TAUNT_1); //PROTECTION
    SHIELD_BASH             = ai->initSpell(SHIELD_BASH_1); //PROTECTION
    REVENGE                 = ai->initSpell(REVENGE_1); //PROTECTION
    SHIELD_BLOCK            = ai->initSpell(SHIELD_BLOCK_1); //PROTECTION
    DISARM                  = ai->initSpell(DISARM_1); //PROTECTION
    SHIELD_WALL             = ai->initSpell(SHIELD_WALL_1); //PROTECTION
    SHIELD_SLAM             = ai->initSpell(SHIELD_SLAM_1); //PROTECTION
    VIGILANCE               = ai->initSpell(VIGILANCE_1); //PROTECTION
    DEVASTATE               = ai->initSpell(DEVASTATE_1); //PROTECTION
    SHOCKWAVE               = ai->initSpell(SHOCKWAVE_1); //PROTECTION
    CONCUSSION_BLOW         = ai->initSpell(CONCUSSION_BLOW_1); //PROTECTION
    SPELL_REFLECTION        = ai->initSpell(SPELL_REFLECTION_1); //PROTECTION
    LAST_STAND              = ai->initSpell(LAST_STAND_1); //PROTECTION
    BATTLE_SHOUT            = ai->initSpell(BATTLE_SHOUT_1); //FURY
    DEMORALIZING_SHOUT      = ai->initSpell(DEMORALIZING_SHOUT_1); //FURY
    CLEAVE                  = ai->initSpell(CLEAVE_1); //FURY
    INTIMIDATING_SHOUT      = ai->initSpell(INTIMIDATING_SHOUT_1); //FURY
    EXECUTE                 = ai->initSpell(EXECUTE_1); //FURY
    CHALLENGING_SHOUT       = ai->initSpell(CHALLENGING_SHOUT_1); //FURY
    SLAM                    = ai->initSpell(SLAM_1); //FURY
    BERSERKER_STANCE        = ai->initSpell(BERSERKER_STANCE_1); //FURY
    INTERCEPT               = ai->initSpell(INTERCEPT_1); //FURY
    DEATH_WISH              = ai->initSpell(DEATH_WISH_1); //FURY
    BERSERKER_RAGE          = ai->initSpell(BERSERKER_RAGE_1); //FURY
    WHIRLWIND               = ai->initSpell(WHIRLWIND_1); //FURY
    PUMMEL                  = ai->initSpell(PUMMEL_1); //FURY
    BLOODTHIRST             = ai->initSpell(BLOODTHIRST_1); //FURY
    RECKLESSNESS            = ai->initSpell(RECKLESSNESS_1); //FURY
    RAMPAGE                 = 0; // passive
    HEROIC_FURY             = ai->initSpell(HEROIC_FURY_1); //FURY
    COMMANDING_SHOUT        = ai->initSpell(COMMANDING_SHOUT_1); //FURY
    ENRAGED_REGENERATION    = ai->initSpell(ENRAGED_REGENERATION_1); //FURY
    PIERCING_HOWL           = ai->initSpell(PIERCING_HOWL_1); //FURY

    RECENTLY_BANDAGED       = 11196; // first aid check

    // racial
    GIFT_OF_THE_NAARU       = ai->initSpell(GIFT_OF_THE_NAARU_WARRIOR); // draenei
    STONEFORM               = ai->initSpell(STONEFORM_ALL); // dwarf
    ESCAPE_ARTIST           = ai->initSpell(ESCAPE_ARTIST_ALL); // gnome
    EVERY_MAN_FOR_HIMSELF   = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL); // human
    SHADOWMELD              = ai->initSpell(SHADOWMELD_ALL); // night elf
    BLOOD_FURY              = ai->initSpell(BLOOD_FURY_MELEE_CLASSES); // orc
    WAR_STOMP               = ai->initSpell(WAR_STOMP_ALL); // tauren
    BERSERKING              = ai->initSpell(BERSERKING_ALL); // troll
    WILL_OF_THE_FORSAKEN    = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL); // undead
}
PlayerbotWarriorAI::~PlayerbotWarriorAI() {}

void PlayerbotWarriorAI::DoCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    switch (m_bot->getRole())
    {
        case WarriorArms:
        {
            static uint32 SpellArms[] = {BLOODTHIRST, WHIRLWIND, BLOODTHIRST, BLOODTHIRST, CLEAVE,  WHIRLWIND, BLOODTHIRST, BLOODTHIRST, WHIRLWIND, BLOODTHIRST, HEROIC_STRIKE};
            static uint32 elt = sizeof(SpellArms)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                if (ai->CastSpell(SpellArms[(i+LastSpellArms)%elt], pTarget))
                {
                    LastSpellArms = (i+LastSpellArms)%elt;
                    break;
                }
            }
            break;
        }
        case WarriorProtection:
        {
            static uint32 SpellProtection[] = {CHARGE, REND, THUNDER_CLAP, CLEAVE, SHOCKWAVE, SUNDER_ARMOR, HEROIC_STRIKE, DEVASTATE, SHIELD_SLAM, REVENGE};
            static uint32 elt = sizeof(SpellProtection)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                if (ai->CastSpell(SpellProtection[(i+LastSpellProtection)%elt], pTarget))
                {
                    LastSpellProtection = (i+LastSpellProtection)%elt;
                    break;
                }
            }
            break;
        }
        case WarriorFury:
        {
            static uint32 SpellFury[] = {BLOODTHIRST, WHIRLWIND, BLOODTHIRST, BLOODTHIRST, CLEAVE,  WHIRLWIND, BLOODTHIRST, BLOODTHIRST, WHIRLWIND, BLOODTHIRST, HEROIC_STRIKE};
            static uint32 elt = sizeof(SpellFury)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                if (ai->CastSpell(SpellFury[(i+LastSpellFury)%elt], pTarget))
                {
                    LastSpellFury = (i+LastSpellFury)%elt;
                    break;
                }
            }
            break;
        }
    }

}

void PlayerbotWarriorAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    if (!ai->CastAura(BERSERKER_STANCE, m_bot))
        if (m_bot->getRole() == WarriorProtection && !ai->CastAura(DEFENSIVE_STANCE, m_bot))
            if (ai->CastAura(BATTLE_STANCE, m_bot))
                return;
}