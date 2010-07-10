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

#ifndef _PlayerbotWarlockAI_H
#define _PlayerbotWarlockAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_CURSES,
    SPELL_AFFLICTION,
    SPELL_DESTRUCTION,
    SPELL_DEMONOLOGY
};

class MANGOS_DLL_SPEC PlayerbotWarlockAI : PlayerbotClassAI
{
    public:
        PlayerbotWarlockAI(Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotWarlockAI();
        void InitSpells(PlayerbotAI* const ai);

        bool DoNextCombatManeuver(Unit*);
        bool DoNonCombatActions();

    private:
        bool m_demonSummonFailed;

        uint32 CURSE_OF_WEAKNESS, CURSE_OF_AGONY, CURSE_OF_EXHAUSTION, CURSE_OF_TONGUES, CURSE_OF_THE_ELEMENTS, CURSE_OF_DOOM;
        uint32 CORRUPTION, DRAIN_SOUL, DRAIN_LIFE, DRAIN_MANA, LIFE_TAP, UNSTABLE_AFFLICTION, HAUNT, ATROCITY, SEED_OF_CORRUPTION, DARK_PACT, HOWL_OF_TERROR, FEAR;
        uint32 SHADOW_BOLT, IMMOLATE, INCINERATE, SEARING_PAIN, CONFLAGRATE, SOUL_FIRE, SHADOWFURY, CHAOS_BOLT, SHADOWFLAME, HELLFIRE, RAIN_OF_FIRE, SHADOWBURN;
        uint32 DEMON_SKIN, DEMON_ARMOR, SHADOW_WARD, FEL_ARMOR, SOULSHATTER, SOUL_LINK, SOUL_LINK_AURA, HEALTH_FUNNEL, DETECT_INVISIBILITY;
        uint32 SUMMON_IMP, SUMMON_VOIDWALKER, SUMMON_SUCCUBUS, SUMMON_FELHUNTER, SUMMON_FELGUARD;
        uint32 BLOOD_PACT, CONSUME_SHADOWS, FEL_INTELLIGENCE;
        uint32 RECENTLY_BANDAGED;
        uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, LastSpellCurse, LastSpellAffliction, LastSpellDestruction;
};

#endif
