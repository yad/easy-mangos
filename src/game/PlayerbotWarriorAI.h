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

#ifndef _PlayerbotWarriorAI_H
#define _PlayerbotWarriorAI_H

#include "PlayerbotClassAI.h"

enum
{
    WarriorSpellPreventing,
    WarriorBattle,
    WarriorDefensive,
    WarriorBerserker
};

class MANGOS_DLL_SPEC PlayerbotWarriorAI : PlayerbotClassAI
{
    public:
        PlayerbotWarriorAI(Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotWarriorAI();
        void InitSpells(PlayerbotAI* const ai);

        bool DoFirstCombatManeuver(Unit*);
        bool DoNextCombatManeuver(Unit*);
        bool DoNonCombatActions();

    private:
        uint32 BATTLE_STANCE, CHARGE, HEROIC_STRIKE, REND, THUNDER_CLAP, HAMSTRING, MOCKING_BLOW, RETALIATION, SWEEPING_STRIKES, MORTAL_STRIKE, BLADESTORM, HEROIC_THROW, SHATTERING_THROW;
        uint32 DEFENSIVE_STANCE, BLOODRAGE, SUNDER_ARMOR, TAUNT, SHIELD_BASH, REVENGE, SHIELD_BLOCK, DISARM, SHIELD_WALL, SHIELD_SLAM, VIGILANCE, DEVASTATE, SHOCKWAVE, CONCUSSION_BLOW, SPELL_REFLECTION, LAST_STAND;
        uint32 BERSERKER_STANCE, BATTLE_SHOUT, DEMORALIZING_SHOUT, OVERPOWER, CLEAVE, INTIMIDATING_SHOUT, EXECUTE, CHALLENGING_SHOUT, SLAM, INTERCEPT, DEATH_WISH, BERSERKER_RAGE, WHIRLWIND, PUMMEL, BLOODTHIRST, RECKLESSNESS, RAMPAGE, HEROIC_FURY, COMMANDING_SHOUT, ENRAGED_REGENERATION, PIERCING_HOWL;
        uint32 RECENTLY_BANDAGED;
        uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence;
};

#endif
