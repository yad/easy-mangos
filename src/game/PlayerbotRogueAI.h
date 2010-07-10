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

#ifndef _PlayerbotRogueAI_H
#define _PlayerbotRogueAI_H

#include "PlayerbotClassAI.h"

enum
{
    RogueCombat,
    RogueSpellPreventing,
    RogueThreat,
    RogueStealth
};

class MANGOS_DLL_SPEC PlayerbotRogueAI : PlayerbotClassAI
{
    public:
        PlayerbotRogueAI(Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotRogueAI();
        void InitSpells(PlayerbotAI* const ai);

        bool DoFirstCombatManeuver(Unit*);
        bool DoNextCombatManeuver(Unit*);
        bool DoNonCombatActions();

    private:
        uint32 SINISTER_STRIKE, BACKSTAB, GOUGE, EVASION, SPRINT, KICK, FEINT, SHIV, FAN_OF_KNIVES;
        uint32 SHADOWSTEP, STEALTH, VANISH, HEMORRHAGE, BLIND, SHADOW_DANCE, CLOAK_OF_SHADOWS, TRICK_TRADE, CRIPPLING_POISON, DEADLY_POISON, MIND_NUMBING_POISON, GHOSTLY_STRIKE, DISTRACT, PREPARATION, PREMEDITATION;
        uint32 EVISCERATE, SLICE_DICE, GARROTE, EXPOSE_ARMOR, AMBUSH, RUPTURE, DISMANTLE, CHEAP_SHOT, KIDNEY_SHOT, MUTILATE, ENVENOM, DEADLY_THROW;
        uint32 RECENTLY_BANDAGED;
        uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, LastSpellCombat, LastSpellSubtlety, LastSpellAssassination, Aura;
};

#endif
