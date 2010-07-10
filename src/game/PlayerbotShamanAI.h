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

#ifndef _PLAYERBOTSHAMANAI_H
#define _PLAYERBOTSHAMANAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_ENHANCEMENT,
    SPELL_RESTORATION,
    SPELL_ELEMENTAL
};

class MANGOS_DLL_SPEC PlayerbotShamanAI : PlayerbotClassAI
{
    public:
        PlayerbotShamanAI(Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotShamanAI();
        void InitSpells(PlayerbotAI* const ai);

        bool DoNextCombatManeuver(Unit*);
        bool DoNonCombatActions();

    private:
        bool HealTarget(Unit& target, uint8 hp);

        uint32 ROCKBITER_WEAPON, STONESKIN_TOTEM, LIGHTNING_SHIELD, FLAMETONGUE_WEAPON, STRENGTH_OF_EARTH_TOTEM, FOCUSED, FROSTBRAND_WEAPON, FROST_RESISTANCE_TOTEM, FLAMETONGUE_TOTEM, FIRE_RESISTANCE_TOTEM, WINDFURY_WEAPON, GROUNDING_TOTEM, NATURE_RESISTANCE_TOTEM, WIND_FURY_TOTEM, STORMSTRIKE, LAVA_LASH, SHAMANISTIC_RAGE, WRATH_OF_AIR_TOTEM, EARTH_ELEMENTAL_TOTEM, BLOODLUST, HEROISM, FERAL_SPIRIT;
        uint32 HEALING_WAVE, LESSER_HEALING_WAVE, ANCESTRAL_SPIRIT, TREMOR_TOTEM, HEALING_STREAM_TOTEM, MANA_SPRING_TOTEM, CHAIN_HEAL, MANA_TIDE_TOTEM, EARTH_SHIELD, WATER_SHIELD, EARTHLIVING_WEAPON, RIPTIDE;
        uint32 LIGHTNING_BOLT, EARTH_SHOCK, STONECLAW_TOTEM, FLAME_SHOCK, SEARING_TOTEM, PURGE, FIRE_NOVA_TOTEM, WIND_SHOCK, FROST_SHOCK, MAGMA_TOTEM, CHAIN_LIGHTNING, TOTEM_OF_WRATH, FIRE_ELEMENTAL_TOTEM, LAVA_BURST, EARTHBIND_TOTEM, HEX;
        uint32 RECENTLY_BANDAGED;
        uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, LastSpellEnhancement, LastSpellRestoration, LastSpellElemental;
};

#endif
