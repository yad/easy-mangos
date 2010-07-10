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

class PlayerbotAI;

PlayerbotWarriorAI::PlayerbotWarriorAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

PlayerbotWarriorAI::~PlayerbotWarriorAI() {}

void PlayerbotWarriorAI::InitSpells(PlayerbotAI* const ai)
{
    BATTLE_STANCE = ai->getSpellId("battle stance");
    CHARGE = ai->getSpellId("charge");
    OVERPOWER = ai->getSpellId("overpower");
    HEROIC_STRIKE = ai->getSpellId("heroic strike");
    REND = ai->getSpellId("rend");
    THUNDER_CLAP = ai->getSpellId("thunder");
    HAMSTRING = ai->getSpellId("hamstring");
    MOCKING_BLOW = ai->getSpellId("mocking blow");
    RETALIATION = ai->getSpellId("retaliation");
    SWEEPING_STRIKES = ai->getSpellId("sweeping strikes");
    MORTAL_STRIKE = ai->getSpellId("mortal strike");
    BLADESTORM = ai->getSpellId("bladestorm");
    HEROIC_THROW = ai->getSpellId("heroic throw");
    SHATTERING_THROW = ai->getSpellId("shattering throw");
    BLOODRAGE = ai->getSpellId("bloodrage");
    DEFENSIVE_STANCE = ai->getSpellId("defensive stance");
    DEVASTATE = ai->getSpellId("devastate");
    SUNDER_ARMOR = ai->getSpellId("sunder armor");
    TAUNT = ai->getSpellId("taunt");
    SHIELD_BASH = ai->getSpellId("shield bash");
    REVENGE = ai->getSpellId("revenge");
    SHIELD_BLOCK = ai->getSpellId("shield block");
    DISARM = ai->getSpellId("disarm");
    SHIELD_WALL = ai->getSpellId("shield wall");
    SHIELD_SLAM = ai->getSpellId("shield slam");
    VIGILANCE = ai->getSpellId("vigilance");
    DEVASTATE = ai->getSpellId("devastate");
    SHOCKWAVE = ai->getSpellId("shockwave");
    CONCUSSION_BLOW = ai->getSpellId("blow");
    SPELL_REFLECTION = ai->getSpellId("spell reflection");
    LAST_STAND = ai->getSpellId("last stand");
    BATTLE_SHOUT = ai->getSpellId("battle shout");
    DEMORALIZING_SHOUT = ai->getSpellId("demoralizing shout");
    CLEAVE = ai->getSpellId("cleave");
    INTIMIDATING_SHOUT = ai->getSpellId("shout");
    EXECUTE = ai->getSpellId("execute");
    CHALLENGING_SHOUT = ai->getSpellId("challenging shout");
    SLAM = ai->getSpellId("slam");
    BERSERKER_STANCE = ai->getSpellId("berserker stance");
    INTERCEPT = ai->getSpellId("intercept");
    DEATH_WISH = ai->getSpellId("death wish");
    BERSERKER_RAGE = ai->getSpellId("berserker rage");
    WHIRLWIND = ai->getSpellId("whirlwind");
    PUMMEL = ai->getSpellId("pummel");
    BLOODTHIRST = ai->getSpellId("bloodthirst");
    RECKLESSNESS = ai->getSpellId("recklessness");
    RAMPAGE = ai->getSpellId("rampage");
    HEROIC_FURY = ai->getSpellId("heroic fury");
    COMMANDING_SHOUT = ai->getSpellId("commanding shout");
    ENRAGED_REGENERATION = ai->getSpellId("enraged regeneration");
    PIERCING_HOWL = ai->getSpellId("piercing howl");
    RECENTLY_BANDAGED = 11196;
    GIFT_OF_THE_NAARU = ai->getSpellId("gift of the naaru");
    STONEFORM = ai->getSpellId("stoneform");
    ESCAPE_ARTIST = ai->getSpellId("escape artist");
    EVERY_MAN_FOR_HIMSELF = ai->getSpellId("every man for himself");
    SHADOWMELD = ai->getSpellId("shadowmeld");
    BLOOD_FURY = ai->getSpellId("blood fury");
    WAR_STOMP = ai->getSpellId("war stomp");
    BERSERKING = ai->getSpellId("berserking");
    WILL_OF_THE_FORSAKEN = ai->getSpellId("will of the forsaken");
}

bool PlayerbotWarriorAI::DoFirstCombatManeuver(Unit *pTarget)
{
    Player *m_bot = GetPlayerBot();
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    PlayerbotAI::CombatOrderType co = ai->GetCombatOrder();
    float fTargetDist = m_bot->GetDistance(pTarget);

    if ((co&PlayerbotAI::ORDERS_TANK)
        && (DEFENSIVE_STANCE > 0)
        && (!m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
        && (ai->CastSpell(DEFENSIVE_STANCE)))
        return true;

    if ((co&PlayerbotAI::ORDERS_TANK)
        && (TAUNT > 0)
        && (m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
        && (ai->CastSpell(TAUNT, *pTarget)))
        return false;

    if ((BATTLE_STANCE > 0)
        && (!m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
        && (ai->CastSpell(BATTLE_STANCE)))
        return true;

    if ((BATTLE_STANCE > 0)
        && (CHARGE > 0)
        && (m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0)))
    {
        if (fTargetDist < 8.0f)
            return false;

        if (fTargetDist > 25.0f)
            return true;

        if ((CHARGE > 0)
            && (ai->CastSpell(CHARGE, *pTarget)))
        {
            float x, y, z;
            pTarget->GetContactPoint(m_bot, x, y, z, 3.666666f);
            m_bot->Relocate(x, y, z);
            return false;
        }
    }

    return false;
}

bool PlayerbotWarriorAI::DoNextCombatManeuver(Unit *pTarget)
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
    float fTargetDist = m_bot->GetDistance(pTarget);
    PlayerbotAI::CombatOrderType co = ai->GetCombatOrder();

    if ((co&PlayerbotAI::ORDERS_TANK)
        && (!m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
        && (ai->CastSpell(DEFENSIVE_STANCE)))
        return true;

    if (!(co&PlayerbotAI::ORDERS_TANK)
        && (!m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
        && (ai->CastSpell(BATTLE_STANCE)))
        return true;

    if (pTarget->IsNonMeleeSpellCasted(true))
        SpellSequence = WarriorSpellPreventing;
    else if (m_bot->HasAura(BATTLE_STANCE, EFFECT_INDEX_0))
        SpellSequence = WarriorBattle;
    else if (m_bot->HasAura(DEFENSIVE_STANCE, EFFECT_INDEX_0))
        SpellSequence = WarriorDefensive;
    else if (m_bot->HasAura(BERSERKER_STANCE, EFFECT_INDEX_0))
        SpellSequence = WarriorBerserker;

    if ((BERSERKER_RAGE > 0)
        && (!m_bot->HasAura(BERSERKER_RAGE, EFFECT_INDEX_0))
        && (ai->CastSpell(BERSERKER_RAGE)))
        return true;

    if ((DEMORALIZING_SHOUT > 0)
        && (ai->GetRageAmount() >= 10)
        && (!pTarget->HasAura(DEMORALIZING_SHOUT, EFFECT_INDEX_0))
        && (ai->CastSpell(DEMORALIZING_SHOUT)))
        return true;

    if ((BATTLE_SHOUT > 0)
        && (ai->GetRageAmount() >= 10)
        && (!m_bot->HasAura(BATTLE_SHOUT, EFFECT_INDEX_0))
        && (ai->CastSpell(BATTLE_SHOUT)))
        return true;

    switch (SpellSequence)
    {
        case WarriorSpellPreventing:

            if ((SHIELD_BASH > 0)
                && (ai->GetRageAmount() >= 10)
                && (ai->CastSpell(SHIELD_BASH, *pTarget)))
                return true;

            if ((PUMMEL > 0)
                && (ai->GetRageAmount() >= 10)
                && (ai->CastSpell(PUMMEL, *pTarget)))
                return true;

            if ((SPELL_REFLECTION > 0)
                && (ai->GetRageAmount() >= 15)
                && (!m_bot->HasAura(SPELL_REFLECTION, EFFECT_INDEX_0))
                && (ai->CastSpell(SPELL_REFLECTION, *m_bot)))
                return true;

            break;

        case WarriorBattle:

            if ((EXECUTE > 0)
                && (ai->GetRageAmount() >= 15)
                && (pTarget->GetHealth() < pTarget->GetMaxHealth() *0.2)
                && (ai->CastSpell(EXECUTE, *pTarget)))
                return true;

            if ((LAST_STAND > 0)
                && (!m_bot->HasAura(LAST_STAND, EFFECT_INDEX_0))
                && (m_bot->GetHealth() < m_bot->GetMaxHealth() *0.5)
                && (ai->CastSpell(LAST_STAND, *m_bot)))
                return true;

            if ((BLOODRAGE > 0)
                && (ai->GetRageAmount() < 50)
                && (!m_bot->HasAura(BLOODRAGE, EFFECT_INDEX_0))
                && (ai->CastSpell(BLOODRAGE, *m_bot)))
                return true;

            if ((DEATH_WISH > 0)
                && (ai->GetRageAmount() >= 10)
                && (!m_bot->HasAura(DEATH_WISH, EFFECT_INDEX_0))
                && (ai->CastSpell(DEATH_WISH, *m_bot)))
                return true;

            if ((RETALIATION > 0)
                && (pVictim == m_bot)
                && (ai->GetAttackerCount() >= 2)
                && (!m_bot->HasAura(RETALIATION, EFFECT_INDEX_0))
                && (ai->CastSpell(RETALIATION, *m_bot)))
                return true;

            if ((DEMORALIZING_SHOUT > 0)
                && (ai->GetRageAmount() >= 10)
                && (!pTarget->HasAura(DEMORALIZING_SHOUT, EFFECT_INDEX_0))
                && (ai->CastSpell(DEMORALIZING_SHOUT, *pTarget)))
                return true;

            if ((SWEEPING_STRIKES > 0)
                && (ai->GetRageAmount() >= 30)
                && (ai->GetAttackerCount() >= 2)
                && (!m_bot->HasAura(SWEEPING_STRIKES, EFFECT_INDEX_0))
                && (ai->CastSpell(SWEEPING_STRIKES, *m_bot)))
                return true;

            if ((BLADESTORM > 0)
                && (ai->GetRageAmount() >= 25)
                && (pVictim == m_bot)
                && (!m_bot->HasAura(BLADESTORM, EFFECT_INDEX_0))
                && (ai->GetAttackerCount() >= 3)
                && (ai->CastSpell(BLADESTORM, *pTarget)))
                return true;

            if ((MORTAL_STRIKE > 0)
                && (ai->GetRageAmount() >= 30)
                && (!pTarget->HasAura(MORTAL_STRIKE, EFFECT_INDEX_0))
                && (ai->CastSpell(MORTAL_STRIKE, *pTarget)))
                return true;

            if ((INTIMIDATING_SHOUT > 0)
                && (ai->GetRageAmount() >= 25)
                && (ai->GetAttackerCount() > 5)
                && (ai->CastSpell(INTIMIDATING_SHOUT, *pTarget)))
                return true;

            if ((THUNDER_CLAP > 0)
                && (ai->GetRageAmount() >= 20)
                && (pVictim == m_bot)
                && (!pTarget->HasAura(THUNDER_CLAP, EFFECT_INDEX_0))
                && (ai->CastSpell(THUNDER_CLAP, *pTarget)))
                return true;

            if ((ENRAGED_REGENERATION > 0)
                && (ai->GetRageAmount() >= 15)
                && (!m_bot->HasAura(BERSERKER_RAGE, EFFECT_INDEX_0))
                && (!m_bot->HasAura(ENRAGED_REGENERATION, EFFECT_INDEX_0))
                && (m_bot->GetHealth() < m_bot->GetMaxHealth() *0.5)
                && (ai->CastSpell(ENRAGED_REGENERATION, *m_bot)))
                return true;

            if ((SHOCKWAVE > 0)
                && (ai->GetRageAmount() >= 15)
                && (pVictim == m_bot)
                && (!pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0))
                && (!pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0))
                && (ai->CastSpell(SHOCKWAVE, *pTarget)))
                return true;

            if ((REND > 0)
                && (ai->GetRageAmount() >= 10)
                && (!pTarget->HasAura(REND, EFFECT_INDEX_0))
                && (ai->CastSpell(REND, *pTarget)))
                return true;

            if ((HAMSTRING > 0)
                && (ai->GetRageAmount() >= 10)
                && (!pTarget->HasAura(HAMSTRING, EFFECT_INDEX_0))
                && (ai->CastSpell(HAMSTRING, *pTarget)))
                return true;

            if ((CHALLENGING_SHOUT > 0)
                && (ai->GetRageAmount() >= 5)
                && (pVictim != m_bot)
                && (ai->GetHealthPercent() > 25)
                && (!pTarget->HasAura(MOCKING_BLOW, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CHALLENGING_SHOUT, EFFECT_INDEX_0))
                && (ai->CastSpell(CHALLENGING_SHOUT, *pTarget)))
                return true;

            if ((BLOODTHIRST > 0)
                && (ai->GetRageAmount() >= 20)
                && (!m_bot->HasAura(BLOODTHIRST, EFFECT_INDEX_0))
                && (m_bot->GetHealth() < m_bot->GetMaxHealth() *0.7)
                && (ai->CastSpell(BLOODTHIRST, *pTarget)))
                return true;

            if ((CLEAVE > 0)
                && (ai->GetRageAmount() >= 20)
                && (ai->CastSpell(CLEAVE, *pTarget)))
                return true;

            if ((HEROIC_STRIKE > 0)
                && (ai->GetRageAmount() >= 15)
                && (ai->CastSpell(HEROIC_STRIKE, *pTarget)))
                return true;

            if ((CONCUSSION_BLOW > 0)
                && (ai->GetRageAmount() >= 15)
                && (!pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0))
                && (!pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0))
                && (ai->CastSpell(CONCUSSION_BLOW, *pTarget)))
                return true;

            if ((SLAM > 0)
                && (ai->GetRageAmount() >= 15)
                && (ai->CastSpell(SLAM, *pTarget)))
                return true;

            if ((PIERCING_HOWL > 0)
                && (ai->GetRageAmount() >= 10)
                && (ai->GetAttackerCount() >= 3)
                && (!pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0))
                && (!pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0))
                && (ai->CastSpell(PIERCING_HOWL, *pTarget)))
                return true;

            if ((MOCKING_BLOW > 0)
                && (ai->GetRageAmount() >= 10)
                && (pVictim != m_bot)
                && (ai->GetHealthPercent() > 25)
                && (!pTarget->HasAura(MOCKING_BLOW, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CHALLENGING_SHOUT, EFFECT_INDEX_0))
                && (ai->CastSpell(MOCKING_BLOW, *pTarget)))
                return true;

            if ((OVERPOWER > 0)
                && (ai->GetRageAmount() >= 5)
                && (ai->CastSpell(OVERPOWER, *pTarget)))
                return true;

            if ((SUNDER_ARMOR > 0)
                && (ai->CastSpell(SUNDER_ARMOR, *pTarget)))
                return true;

            if ((SHATTERING_THROW > 0)
                && (!pTarget->HasAura(SHATTERING_THROW, EFFECT_INDEX_0))
                && (ai->CastSpell(SHATTERING_THROW, *pTarget)))
                return true;

            if ((HEROIC_THROW > 0)
                && (ai->CastSpell(HEROIC_THROW, *pTarget)))
                return true;

            if ((m_bot->getRace() == RACE_TAUREN)
                && (!pTarget->HasAura(WAR_STOMP, EFFECT_INDEX_0))
                && (!pTarget->HasAura(PIERCING_HOWL, EFFECT_INDEX_0))
                && (!pTarget->HasAura(SHOCKWAVE, EFFECT_INDEX_0))
                && (!pTarget->HasAura(CONCUSSION_BLOW, EFFECT_INDEX_0))
                && (ai->CastSpell(WAR_STOMP, *pTarget)))
                return true;

            if ((m_bot->getRace() == RACE_HUMAN)
                && ((m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM)))
                && (ai->CastSpell(EVERY_MAN_FOR_HIMSELF, *m_bot)))
                return true;

            if ((m_bot->getRace() == RACE_UNDEAD_PLAYER)
                && ((m_bot->HasAuraType(SPELL_AURA_MOD_FEAR) || m_bot->HasAuraType(SPELL_AURA_MOD_CHARM)))
                && (ai->CastSpell(WILL_OF_THE_FORSAKEN, *m_bot)))
                return true;

            if ((m_bot->getRace() == RACE_DWARF)
                && (m_bot->HasAuraState(AURA_STATE_DEADLY_POISON))
                && (ai->CastSpell(STONEFORM, *m_bot)))
                return true;

            if ((m_bot->getRace() == RACE_GNOME)
                && ((m_bot->hasUnitState(UNIT_STAT_STUNNED) || m_bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED)))
                && (ai->CastSpell(ESCAPE_ARTIST, *m_bot)))
                return true;

            if ((m_bot->getRace() == RACE_NIGHTELF)
                && (pVictim == m_bot)
                && (ai->GetHealthPercent() < 25)
                && (!m_bot->HasAura(SHADOWMELD, EFFECT_INDEX_0))
                && (ai->CastSpell(SHADOWMELD, *m_bot)))
                return true;

            if ((m_bot->getRace() == RACE_ORC)
                && (!m_bot->HasAura(BLOOD_FURY, EFFECT_INDEX_0))
                && (ai->CastSpell(BLOOD_FURY, *m_bot)))
                return true;

            if ((m_bot->getRace() == RACE_TROLL)
                && (!m_bot->HasAura(BERSERKING, EFFECT_INDEX_0))
                && (ai->CastSpell(BERSERKING, *m_bot)))
                return true;

            if ((m_bot->getRace() == RACE_DRAENEI)
                && (ai->GetHealthPercent() < 25)
                && (!m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0))
                && (ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot)))
                return true;

            break;

        case WarriorDefensive:

            if ((DISARM > 0)
                && (ai->GetRageAmount() >= 15)
                && (!pTarget->HasAura(DISARM, EFFECT_INDEX_0))
                && (ai->CastSpell(DISARM, *pTarget)))
                return true;

            if ((SUNDER_ARMOR > 0)
                && (ai->GetRageAmount() >= 15)
                && (ai->CastSpell(SUNDER_ARMOR, *pTarget)))
                return true;

            if ((REVENGE > 0)
                && (ai->GetRageAmount() >= 5)
                && (ai->CastSpell(REVENGE, *pTarget)))
                return true;

            if ((SHIELD_BLOCK > 0)
                && (!m_bot->HasAura(SHIELD_BLOCK, EFFECT_INDEX_0))
                && (ai->CastSpell(SHIELD_BLOCK, *m_bot)))
                return true;

            if ((SHIELD_WALL > 0)
                && (!m_bot->HasAura(SHIELD_WALL, EFFECT_INDEX_0))
                && (ai->CastSpell(SHIELD_WALL, *m_bot)))
                return true;

            break;

        case WarriorBerserker:

            if ((WHIRLWIND > 0)
                && (ai->GetRageAmount() >= 25)
                && (ai->CastSpell(WHIRLWIND, *pTarget)))
                return true;

            break;
    }

    return false;
}

bool PlayerbotWarriorAI::DoNonCombatActions()
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

    if ((((COMMANDING_SHOUT > 0)
        && (!m_bot->HasAura(COMMANDING_SHOUT, EFFECT_INDEX_0)))
        || ((BATTLE_SHOUT > 0)
        && (!m_bot->HasAura(BATTLE_SHOUT, EFFECT_INDEX_0))))
        && (ai->GetRageAmount() < 10)
        && (BLOODRAGE > 0)
        && (!m_bot->HasAura(BLOODRAGE, EFFECT_INDEX_0))
        && (ai->CastSpell(BLOODRAGE, *m_bot)))
        return true;

    if ((COMMANDING_SHOUT > 0)
        && (!m_bot->HasAura(COMMANDING_SHOUT, EFFECT_INDEX_0))
        && (ai->CastSpell(COMMANDING_SHOUT, *m_bot)))
        return true;

    if ((BATTLE_SHOUT > 0)
        && (!m_bot->HasAura(BATTLE_SHOUT, EFFECT_INDEX_0))
        && (!m_bot->HasAura(COMMANDING_SHOUT, EFFECT_INDEX_0))
        && (ai->CastSpell(BATTLE_SHOUT, *m_bot)))
        return true;

    if ((VIGILANCE > 0)
        && (!pMaster->HasAura(VIGILANCE, EFFECT_INDEX_0))
        && (ai->CastSpell(VIGILANCE, *pMaster)))
        return true;

    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* drinkOrFoodItem = ai->FindFood();
    Item* bandageItem = ai->FindBandage();

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

    if (!drinkOrFoodItem
        && !bandageItem
        && (m_bot->getRace() == RACE_DRAENEI)
        && (!m_bot->HasAura(GIFT_OF_THE_NAARU, EFFECT_INDEX_0))
        && (ai->GetHealthPercent() < 70)
        && (ai->CastSpell(GIFT_OF_THE_NAARU, *m_bot)))
        return true;

    return false;
}
