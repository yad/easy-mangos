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

void PlayerbotWarriorAI::ReinitCycles()
{
    LastSpellArms = 0;
    LastSpellProtection = 0;
    LastSpellFury = 0;
}

void PlayerbotWarriorAI::InitSpells(PlayerbotAI* const ai)
{
    BATTLE_STANCE           = ai->initSpell(BATTLE_STANCE_1);
    CHARGE                  = ai->initSpell(CHARGE_1);
    OVERPOWER               = ai->initSpell(OVERPOWER_1);
    HEROIC_STRIKE           = ai->initSpell(HEROIC_STRIKE_1);
    REND                    = ai->initSpell(REND_1);
    THUNDER_CLAP            = ai->initSpell(THUNDER_CLAP_1); 
    HAMSTRING               = ai->initSpell(HAMSTRING_1); 
    MOCKING_BLOW            = ai->initSpell(MOCKING_BLOW_1); 
    RETALIATION             = ai->initSpell(RETALIATION_1); 
    SWEEPING_STRIKES        = ai->initSpell(SWEEPING_STRIKES_1);
    MORTAL_STRIKE           = ai->initSpell(MORTAL_STRIKE_1); 
    BLADESTORM              = ai->initSpell(BLADESTORM_1); 
    HEROIC_THROW            = ai->initSpell(HEROIC_THROW_1); 
    SHATTERING_THROW        = ai->initSpell(SHATTERING_THROW_1); 
    BLOODRAGE               = ai->initSpell(BLOODRAGE_1);    
    DEFENSIVE_STANCE        = ai->initSpell(DEFENSIVE_STANCE_1);    
    DEVASTATE               = ai->initSpell(DEVASTATE_1);    
    SUNDER_ARMOR            = ai->initSpell(SUNDER_ARMOR_1);    
    TAUNT                   = ai->initSpell(TAUNT_1);    
    SHIELD_BASH             = ai->initSpell(SHIELD_BASH_1);    
    REVENGE                 = ai->initSpell(REVENGE_1);    
    SHIELD_BLOCK            = ai->initSpell(SHIELD_BLOCK_1);    
    DISARM                  = ai->initSpell(DISARM_1);    
    SHIELD_WALL             = ai->initSpell(SHIELD_WALL_1);    
    SHIELD_SLAM             = ai->initSpell(SHIELD_SLAM_1);    
    VIGILANCE               = ai->initSpell(VIGILANCE_1);    
    DEVASTATE               = ai->initSpell(DEVASTATE_1);    
    SHOCKWAVE               = ai->initSpell(SHOCKWAVE_1);    
    CONCUSSION_BLOW         = ai->initSpell(CONCUSSION_BLOW_1);    
    SPELL_REFLECTION        = ai->initSpell(SPELL_REFLECTION_1);    
    LAST_STAND              = ai->initSpell(LAST_STAND_1);    
    BATTLE_SHOUT            = ai->initSpell(BATTLE_SHOUT_1);    
    DEMORALIZING_SHOUT      = ai->initSpell(DEMORALIZING_SHOUT_1);    
    CLEAVE                  = ai->initSpell(CLEAVE_1);    
    INTIMIDATING_SHOUT      = ai->initSpell(INTIMIDATING_SHOUT_1);    
    EXECUTE                 = ai->initSpell(EXECUTE_1);    
    CHALLENGING_SHOUT       = ai->initSpell(CHALLENGING_SHOUT_1);    
    SLAM                    = ai->initSpell(SLAM_1);    
    BERSERKER_STANCE        = ai->initSpell(BERSERKER_STANCE_1);    
    INTERCEPT               = ai->initSpell(INTERCEPT_1);    
    DEATH_WISH              = ai->initSpell(DEATH_WISH_1);    
    BERSERKER_RAGE          = ai->initSpell(BERSERKER_RAGE_1);    
    WHIRLWIND               = ai->initSpell(WHIRLWIND_1);    
    PUMMEL                  = ai->initSpell(PUMMEL_1);    
    BLOODTHIRST             = ai->initSpell(BLOODTHIRST_1);    
    RECKLESSNESS            = ai->initSpell(RECKLESSNESS_1);    
    RAMPAGE                 = 0;
    HEROIC_FURY             = ai->initSpell(HEROIC_FURY_1);    
    COMMANDING_SHOUT        = ai->initSpell(COMMANDING_SHOUT_1);    
    ENRAGED_REGENERATION    = ai->initSpell(ENRAGED_REGENERATION_1);    
    PIERCING_HOWL           = ai->initSpell(PIERCING_HOWL_1);    
    RECENTLY_BANDAGED       = 11196;
    GIFT_OF_THE_NAARU       = ai->initSpell(GIFT_OF_THE_NAARU_WARRIOR);
    STONEFORM               = ai->initSpell(STONEFORM_ALL);
    ESCAPE_ARTIST           = ai->initSpell(ESCAPE_ARTIST_ALL);
    EVERY_MAN_FOR_HIMSELF   = ai->initSpell(EVERY_MAN_FOR_HIMSELF_ALL);
    SHADOWMELD              = ai->initSpell(SHADOWMELD_ALL);
    BLOOD_FURY              = ai->initSpell(BLOOD_FURY_MELEE_CLASSES);
    WAR_STOMP               = ai->initSpell(WAR_STOMP_ALL);
    BERSERKING              = ai->initSpell(BERSERKING_ALL);
    WILL_OF_THE_FORSAKEN    = ai->initSpell(WILL_OF_THE_FORSAKEN_ALL);
}

PlayerbotWarriorAI::~PlayerbotWarriorAI() {}

bool PlayerbotWarriorAI::DoProtectSelfAction()
{
    PlayerbotAI *ai = GetAI();
    Player *m_bot = GetPlayerBot();

    if (m_bot->GetHealthPercent() < 40 && ai->Cast(SHIELD_WALL, m_bot))
        return true;

    if (m_bot->GetHealthPercent() < 60 && ai->Cast(ENRAGED_REGENERATION, m_bot))
        return true;

    if (ai->Cast(SPELL_REFLECTION, m_bot))
        return true;

    return false;
}

bool PlayerbotWarriorAI::DoCombatManeuver(Unit *pTarget, bool cac)
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    switch (m_bot->getRole())
    {
        case WarriorArms:
        {
            static const uint32 SpellArms[] = {BLOODTHIRST, WHIRLWIND, BLOODTHIRST, BLOODTHIRST, CLEAVE,
                WHIRLWIND, BLOODTHIRST, BLOODTHIRST, WHIRLWIND, BLOODTHIRST, HEROIC_STRIKE};
            static uint32 elt = sizeof(SpellArms)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                if (ai->CastSpell(SpellArms[(i+LastSpellArms)%elt], pTarget))
                {
                    LastSpellArms = (i+LastSpellArms)%elt;
                    return true;
                }
            }
            break;
        }
        case WarriorProtection:
        {
            static const uint32 SpellProtection[] = {CHARGE, REND, THUNDER_CLAP, CLEAVE, SHOCKWAVE,
                SUNDER_ARMOR, HEROIC_STRIKE, DEVASTATE, SHIELD_SLAM, REVENGE};
            static uint32 elt = sizeof(SpellProtection)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                if (ai->CastSpell(SpellProtection[(i+LastSpellProtection)%elt], pTarget))
                {
                    LastSpellProtection = (i+LastSpellProtection)%elt;
                    return true;
                }
            }
            break;
        }
        case WarriorFury:
        {
            static const uint32 SpellFury[] = {BLOODTHIRST, WHIRLWIND, BLOODTHIRST, BLOODTHIRST, CLEAVE,
                WHIRLWIND, BLOODTHIRST, BLOODTHIRST, WHIRLWIND, BLOODTHIRST, HEROIC_STRIKE};
            static uint32 elt = sizeof(SpellFury)/sizeof(uint32);

            for (uint32 i = 1; i <= elt; ++i)
            {
                if (ai->CastSpell(SpellFury[(i+LastSpellFury)%elt], pTarget))
                {
                    LastSpellFury = (i+LastSpellFury)%elt;
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

void PlayerbotWarriorAI::DoNonCombatActions()
{
    PlayerbotAI* ai = GetAI();
    Player* m_bot = GetPlayerBot();
    Player* m_master = ai->GetLeader();

    if (m_bot->getRole() == WarriorProtection && ai->Cast(DEFENSIVE_STANCE))
        return;
    else if (ai->Cast(BERSERKER_STANCE))
        return;
    else if (ai->Cast(BATTLE_STANCE))
        return;
}