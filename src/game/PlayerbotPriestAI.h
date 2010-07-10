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

#ifndef _PLAYERBOTPRIESTAI_H
#define _PLAYERBOTPRIESTAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_HOLY,
    SPELL_SHADOWMAGIC,
    SPELL_DISCIPLINE
};

class MANGOS_DLL_SPEC PlayerbotPriestAI : PlayerbotClassAI
{
    public:
        PlayerbotPriestAI(Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotPriestAI();
        void InitSpells(PlayerbotAI* const ai);

        bool DoNextCombatManeuver(Unit*);
        bool DoNonCombatActions();

    private:
        bool HealTarget(Unit& target, uint8 hp);

        uint32 GREAT_HEAL, RENEW, HEAL, FLASH_HEAL, REZZ, SMITE, CLEARCASTING, HOLY_NOVA, HOLY_FIRE, DESPERATE_PRAYER, PRAYER_OF_HEALING, CIRCLE_OF_HEALING, BINDING_HEAL, PRAYER_OF_MENDING, MANA_BURN;
        uint32 FADE, PAIN, MIND_BLAST, SCREAM, MIND_FLAY, DEVOURING_PLAGUE, SHADOW_PROTECTION, VAMPIRIC_TOUCH, PRAYER_OF_SHADOW_PROTECTION, SHADOWFIEND, MIND_SEAR;
        uint32 PWS, INNER_FIRE, FORTITUDE, TOUCH_OF_WEAKNESS, FEAR_WARD, DSPIRIT, POWER_INFUSION, MASS_DISPEL, PENANCE, DIVINE_SPIRIT, INNER_FOCUS;
        uint32 RECENTLY_BANDAGED;
        uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, LastSpellHoly, LastSpellShadowMagic, LastSpellDiscipline;
};

#endif
