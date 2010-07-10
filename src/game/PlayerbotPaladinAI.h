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

#ifndef _PlayerbotPaladinAI_H
#define _PlayerbotPaladinAI_H

#include "PlayerbotClassAI.h"

enum
{
    Combat,
    Healing
};

class MANGOS_DLL_SPEC PlayerbotPaladinAI : PlayerbotClassAI
{
    public:
        PlayerbotPaladinAI(Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotPaladinAI();
        void InitSpells(PlayerbotAI* const ai);

        bool DoNextCombatManeuver(Unit*);
        bool DoNonCombatActions();

    private:
        bool HealTarget(Unit& target, uint8 hp);

        uint32 RETRIBUTION_AURA, SEAL_OF_COMMAND, JUDGEMENT_OF_LIGHT, JUDGEMENT_OF_WISDOM, GREATER_BLESSING_OF_WISDOM, GREATER_BLESSING_OF_MIGHT, BLESSING_OF_WISDOM, BLESSING_OF_MIGHT, HAMMER_OF_JUSTICE, RIGHTEOUS_FURY, CRUSADER_AURA, CRUSADER_STRIKE, AVENGING_WRATH, DIVINE_STORM, JUDGEMENT_OF_JUSTICE;
        uint32 FLASH_OF_LIGHT, HOLY_LIGHT, DIVINE_SHIELD, HAMMER_OF_WRATH, CONSECRATION, CONCENTRATION_AURA, DIVINE_FAVOR, SACRED_SHIELD, HOLY_SHOCK, HOLY_WRATH, LAY_ON_HANDS, EXORCISM, DIVINE_PLEA;
        uint32 GREATER_BLESSING_OF_KINGS, BLESSING_OF_KINGS, HAND_OF_PROTECTION, SHADOW_RESISTANCE_AURA, DEVOTION_AURA, FIRE_RESISTANCE_AURA, FROST_RESISTANCE_AURA, DEFENSIVE_STANCE, BERSERKER_STANCE, BATTLE_STANCE, DIVINE_SACRIFICE, DIVINE_PROTECTION, DIVINE_INTERVENTION, HOLY_SHIELD, AVENGERS_SHIELD, RIGHTEOUS_DEFENSE, GREATER_BLESSING_OF_SANCTUARY, HAND_OF_SACRIFICE, SHIELD_OF_RIGHTEOUSNESS;
        uint32 FORBEARANCE;
        uint32 RECENTLY_BANDAGED;
        uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, CombatCounter, HealCounter;
};

#endif

