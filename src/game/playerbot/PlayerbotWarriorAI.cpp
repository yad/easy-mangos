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

bool PlayerbotWarriorAI::DoFirstCombatManeuver(Unit *pTarget)
{
    Player *m_bot = GetPlayerBot();
    PlayerbotAI *ai = GetAI();
    float fTargetDist = m_bot->GetDistance(pTarget);

    if (DEFENSIVE_STANCE > 0 && !m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0) && ai->CastSpell(DEFENSIVE_STANCE))
    {

        return true;
    }
    else if (TAUNT > 0 && m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0) && ai->CastSpell(TAUNT, pTarget))
    {

        return false;
    }
    else if (BATTLE_STANCE > 0 && !m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0) && ai->CastSpell(BATTLE_STANCE))
    {

        return true;
    }
    else if (BATTLE_STANCE > 0 && CHARGE > 0 && m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
    {
        if (fTargetDist < 8.0f)
            return false;
        else if (fTargetDist > 25.0f)
            return true;
        else if (CHARGE > 0 && ai->CastSpell(CHARGE, pTarget))
        {
            float x, y, z;
            pTarget->GetContactPoint(m_bot, x, y, z, 3.666666f);
            m_bot->Relocate(x, y, z);


            return false;
        }
    }

    return false;
}

void PlayerbotWarriorAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI *ai = GetAI();
    if (!ai)
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Player* m_master = ai->GetLeader();
    if (!m_master)
        return;

    //ai->SetMovementTarget( PlayerbotAI::MOVEMENT_FOLLOW, m_master ); // dont want to melee mob

    // Damage Attacks

    ai->SetInFront(pTarget);
    Unit* pVictim = pTarget->getVictim();
    float fTargetDist = m_bot->GetDistance(pTarget);

    // decide what stance to use
    if (!m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0) && ai->CastSpell(DEFENSIVE_STANCE))
    {
    }
    else if (!m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0) && ai->CastSpell(BATTLE_STANCE))
    {
    }

    // get spell sequence
    if (pTarget->IsNonMeleeSpellCasted(true))
        SpellSequence = WarriorSpellPreventing;
    else if (m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
        SpellSequence = WarriorBattle;
    else if (m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
        SpellSequence = WarriorDefensive;
    else if (m_bot->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0))
        SpellSequence = WarriorBerserker;

    // do shouts, berserker rage, etc...
    if (BERSERKER_RAGE > 0 && !m_bot->HasAura(BERSERKER_RAGE, EFFECT_INDEX_0) && ai->CastSpell(BERSERKER_RAGE))
    {
    }
    else if (DEMORALIZING_SHOUT > 0 && ai->GetRageAmount() >= 10 && !pTarget->HasAura(DEMORALIZING_SHOUT, EFFECT_INDEX_0) && ai->CastSpell(DEMORALIZING_SHOUT))
    {
    }
    else if (BATTLE_SHOUT > 0 && ai->GetRageAmount() >= 10 && !m_bot->HasAura(BATTLE_SHOUT, EFFECT_INDEX_0) && ai->CastSpell(BATTLE_SHOUT))
    {
    }


    switch (SpellSequence)
    {
        case WarriorSpellPreventing:

            if (SHIELD_BASH > 0 && ai->GetRageAmount() >= 10 && ai->CastSpell(SHIELD_BASH, pTarget))
        {}
            else if (PUMMEL > 0 && ai->GetRageAmount() >= 10 && ai->CastSpell(PUMMEL, pTarget))
        {}
            else if (SPELL_REFLECTION > 0 && ai->GetRageAmount() >= 15 && !m_bot->HasAura(SPELL_REFLECTION, EFFECT_INDEX_0) && ai->CastSpell(SPELL_REFLECTION, m_bot))
        {}
            break;

        case WarriorBattle:

            if (EXECUTE > 0 && ai->GetRageAmount() >= 15 && pTarget->GetHealth() < pTarget->GetMaxHealth() * 0.2 && ai->CastSpell(EXECUTE, pTarget))
        {}
            else if (LAST_STAND > 0 && !m_bot->HasAura(LAST_STAND, EFFECT_INDEX_0) && m_bot->GetHealth() < m_bot->GetMaxHealth() * 0.5 && ai->CastSpell(LAST_STAND, m_bot))
        {}
            else if (BLOODRAGE > 0 && ai->GetRageAmount() < 50 && !m_bot->HasAura(BLOODRAGE, EFFECT_INDEX_0) && ai->CastSpell(BLOODRAGE, m_bot))
        {}
            else if (DEATH_WISH > 0 && ai->GetRageAmount() >= 10 && !m_bot->HasAura(DEATH_WISH, EFFECT_INDEX_0) && ai->CastSpell(DEATH_WISH, m_bot))
        {}
            else if (RETALIATION > 0 && pVictim == m_bot && !m_bot->HasAura(RETALIATION, EFFECT_INDEX_0) && ai->CastSpell(RETALIATION, m_bot))
        {}
            else if (DEMORALIZING_SHOUT > 0 && ai->GetRageAmount() >= 10 && !pTarget->HasAura(DEMORALIZING_SHOUT, EFFECT_INDEX_0) && ai->CastSpell(DEMORALIZING_SHOUT, pTarget))
        {}
            else if (SWEEPING_STRIKES > 0 && ai->GetRageAmount() >= 30 && !m_bot->HasAura(SWEEPING_STRIKES, EFFECT_INDEX_0) && ai->CastSpell(SWEEPING_STRIKES, m_bot))
        {}
            else if (BLADESTORM > 0 && ai->GetRageAmount() >= 25 && pVictim == m_bot && !m_bot->HasAura(BLADESTORM, EFFECT_INDEX_0) && ai->CastSpell(BLADESTORM, pTarget))
        {}
            else if (MORTAL_STRIKE > 0 && ai->GetRageAmount() >= 30 && !pTarget->HasAura(MORTAL_STRIKE, EFFECT_INDEX_0) && ai->CastSpell(MORTAL_STRIKE, pTarget))
        {}
            else if (INTIMIDATING_SHOUT > 0 && ai->GetRageAmount() >= 25 && ai->CastSpell(INTIMIDATING_SHOUT, pTarget))
        {}
            else if (THUNDER_CLAP > 0 && ai->GetRageAmount() >= 20 && pVictim == m_bot && !pTarget->HasAura(THUNDER_CLAP, EFFECT_INDEX_0) && ai->CastSpell(THUNDER_CLAP, pTarget))
        {}
            else if (ENRAGED_REGENERATION > 0 && ai->GetRageAmount() >= 15 && !m_bot->HasAura(BERSERKER_RAGE, EFFECT_INDEX_0) && !m_bot->HasAura(ENRAGED_REGENERATION, EFFECT_INDEX_0) && m_bot->GetHealth() < m_bot->GetMaxHealth() * 0.5 && ai->CastSpell(ENRAGED_REGENERATION, m_bot))
        {}
            else if (SHOCKWAVE > 0 && ai->GetRageAmount() >= 15 && pVictim == m_bot && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && !pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0) && !pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0) && !pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0) && ai->CastSpell(SHOCKWAVE, pTarget))
        {}
            else if (REND > 0 && ai->GetRageAmount() >= 10 && !pTarget->HasAura(REND, EFFECT_INDEX_0) && ai->CastSpell(REND, pTarget))
        {}
            else if (HAMSTRING > 0 && ai->GetRageAmount() >= 10 && !pTarget->HasAura(HAMSTRING, EFFECT_INDEX_0) && ai->CastSpell(HAMSTRING, pTarget))
        {}
            else if (CHALLENGING_SHOUT > 0 && ai->GetRageAmount() >= 5 && pVictim != m_bot && ai->GetHealthPercent() > 25 && !pTarget->HasAura(MOCKING_BLOW, EFFECT_INDEX_0) && !pTarget->HasAura(CHALLENGING_SHOUT, EFFECT_INDEX_0) && ai->CastSpell(CHALLENGING_SHOUT, pTarget))
        {}
            else if (BLOODTHIRST > 0 && ai->GetRageAmount() >= 20 && !m_bot->HasAura(BLOODTHIRST, EFFECT_INDEX_0) && m_bot->GetHealth() < m_bot->GetMaxHealth() * 0.7 && ai->CastSpell(BLOODTHIRST, pTarget))
        {}
            else if (CLEAVE > 0 && ai->GetRageAmount() >= 20 && ai->CastSpell(CLEAVE, pTarget))
        {}
            else if (HEROIC_STRIKE > 0 && ai->GetRageAmount() >= 15 && ai->CastSpell(HEROIC_STRIKE, pTarget))
        {}
            else if (CONCUSSION_BLOW > 0 && ai->GetRageAmount() >= 15 && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && !pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0) && !pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0) && !pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0) && ai->CastSpell(CONCUSSION_BLOW, pTarget))
        {}
            else if (SLAM > 0 && ai->GetRageAmount() >= 15 && ai->CastSpell(SLAM, pTarget))
        {}
            else if (PIERCING_HOWL > 0 && ai->GetRageAmount() >= 10 && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && !pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0) && !pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0) && !pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0) && ai->CastSpell(PIERCING_HOWL, pTarget))
        {}
            else if (MOCKING_BLOW > 0 && ai->GetRageAmount() >= 10 && pVictim != m_bot && ai->GetHealthPercent() > 25 && !pTarget->HasAura(MOCKING_BLOW, EFFECT_INDEX_0) && !pTarget->HasAura(CHALLENGING_SHOUT, EFFECT_INDEX_0) && ai->CastSpell(MOCKING_BLOW, pTarget))
        {}
            else if (OVERPOWER > 0 && ai->GetRageAmount() >= 5 && ai->CastSpell(OVERPOWER, pTarget))
        {}
            else if (SUNDER_ARMOR > 0 && ai->CastSpell(SUNDER_ARMOR, pTarget))
        {}
            else if (SHATTERING_THROW > 0 && !pTarget->HasAura(SHATTERING_THROW, EFFECT_INDEX_0) && ai->CastSpell(SHATTERING_THROW, pTarget))
        {}
            else if (HEROIC_THROW > 0 && ai->CastSpell(HEROIC_THROW, pTarget))
        {}
            else if (m_bot->getRace() == RACE_TAUREN && !pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0) && !pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0) && !pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0) && !pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0) && ai->CastSpell(WAR_STOMP, pTarget))
        {}
            else if (m_bot->getRace() == RACE_HUMAN && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && ai->CastSpell(EVERY_MAN_FOR_HIMSELF, m_bot))
        {}
            else if (m_bot->getRace() == RACE_UNDEAD && m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM) && ai->CastSpell(WILL_OF_THE_FORSAKEN, m_bot))
        {}
            else if (m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState(AURA_STATE_DEADLY_POISON) && ai->CastSpell(STONEFORM, m_bot))
        {}
            else if (m_bot->getRace() == RACE_GNOME && m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) && ai->CastSpell(ESCAPE_ARTIST, m_bot))
        {}
            else if (m_bot->getRace() == RACE_NIGHTELF && pVictim == m_bot && ai->GetHealthPercent() < 25 && !m_bot->HasAura(SHADOWMELD, EFFECT_INDEX_0) && ai->CastSpell(SHADOWMELD, m_bot))
        {}
            else if (m_bot->getRace() == RACE_ORC && !m_bot->HasAura(BLOOD_FURY, EFFECT_INDEX_0) && ai->CastSpell(BLOOD_FURY, m_bot))
        {}
            else if (m_bot->getRace() == RACE_TROLL && !m_bot->HasAura(BERSERKING, EFFECT_INDEX_0) && ai->CastSpell(BERSERKING, m_bot))
        {}
            else if (m_bot->getRace() == RACE_DRAENEI && ai->GetHealthPercent() < 25 && !m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0) && ai->CastSpell(GIFT_OF_THE_NAARU, m_bot))
        {}
            break;

        case WarriorDefensive:

            if (DISARM > 0 && ai->GetRageAmount() >= 15 && !pTarget->HasAura(DISARM, EFFECT_INDEX_0) && ai->CastSpell(DISARM, pTarget))
        {}
            else if (SUNDER_ARMOR > 0 && ai->GetRageAmount() >= 15 && ai->CastSpell(SUNDER_ARMOR, pTarget))
        {}
            else if (REVENGE > 0 && ai->GetRageAmount() >= 5 && ai->CastSpell(REVENGE, pTarget))
        {}
            else if (SHIELD_BLOCK > 0 && !m_bot->HasAura(SHIELD_BLOCK, EFFECT_INDEX_0) && ai->CastSpell(SHIELD_BLOCK, m_bot))
        {}
            else if (SHIELD_WALL > 0 && !m_bot->HasAura(SHIELD_WALL, EFFECT_INDEX_0) && ai->CastSpell(SHIELD_WALL, m_bot))
        {}
            break;

        case WarriorBerserker:

            if (WHIRLWIND > 0 && ai->GetRageAmount() >= 25 && ai->CastSpell(WHIRLWIND, pTarget))
        {}
            break;
    }

}

void PlayerbotWarriorAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    if (!ai)
        return;

    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    Player* m_master = ai->GetLeader();
    if (!m_master)
        return;

    // TODO (by Runsttren): check if shout aura bot has is casted by this bot,
    // otherwise cast other useful shout
    // If the bot is protect talented, she/he needs stamina not attack power.
    // With stance change can the shout change to.
    // Inserted line to battle shout m_bot->HasAura( COMMANDING_SHOUT, EFFECT_INDEX_0)
    // Natsukawa
    if (((COMMANDING_SHOUT > 0 && !m_bot->HasAura(COMMANDING_SHOUT, EFFECT_INDEX_0)) ||
         (BATTLE_SHOUT > 0 && !m_bot->HasAura(BATTLE_SHOUT, EFFECT_INDEX_0))) &&
        ai->GetRageAmount() < 10 && BLOODRAGE > 0 && !m_bot->HasAura(BLOODRAGE, EFFECT_INDEX_0))
        // we do have a useful shout, no rage coming but can cast bloodrage... do it
        ai->CastSpell(BLOODRAGE, m_bot);
    else if (COMMANDING_SHOUT > 0 && !m_bot->HasAura(COMMANDING_SHOUT, EFFECT_INDEX_0))
        // use commanding shout now
        ai->CastSpell(COMMANDING_SHOUT, m_bot);
    else if (BATTLE_SHOUT > 0 && !m_bot->HasAura(BATTLE_SHOUT, EFFECT_INDEX_0) && !m_bot->HasAura(COMMANDING_SHOUT, EFFECT_INDEX_0))
        // use battle shout
        ai->CastSpell(BATTLE_SHOUT, m_bot);

    // buff master with VIGILANCE
    if (VIGILANCE > 0)
        (!m_master->HasAura(VIGILANCE, EFFECT_INDEX_0) && ai->CastSpell(VIGILANCE, m_master));

} // end DoNonCombatActions
