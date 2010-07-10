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

#ifndef _PLAYERBOTDRUIDAI_H
#define _PLAYERBOTDRUIDAI_H

#include "PlayerbotClassAI.h"

enum
{
    DruidCombat,
    DruidTank,
    DruidHeal,
    DruidSpell
};

class MANGOS_DLL_SPEC PlayerbotDruidAI : PlayerbotClassAI
{
    public:
        PlayerbotDruidAI(Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotDruidAI();
        void InitSpells(PlayerbotAI* const ai);

        bool DoNextCombatManeuver(Unit*);
        bool DoNonCombatActions();

    private:
        bool HealTarget(Unit& target, uint8 hp);
        uint32 CAT_FORM, BEAR_FORM, DIRE_BEAR_FORM, MOONKIN_FORM, TREE_OF_LIFE;
        uint32 CLAW, COWER, TIGERS_FURY, RAKE, RIP, FEROCIOUS_BITE, MAIM, MANGLE;
        uint32 BASH, MAUL, SWIPE, DEMORALIZING_ROAR, CHALLENGING_ROAR, GROWL, ENRAGE;
        uint32 MOONFIRE, ROOTS, WRATH, STARFALL, STARFIRE, INSECT_SWARM, FAERIE_FIRE, FORCE_OF_NATURE, HURRICANE;
        uint32 MARK_OF_THE_WILD, THORNS, MANA_REJUVENATION, INNERVATE, BARKSKIN;
        uint32 LIFEBLOOM, REJUVENATION, REGROWTH, NOURISH, HEALING_TOUCH, WILD_GROWTH, SWIFTMEND, TRANQUILITY;
        uint32 RECENTLY_BANDAGED;
        uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;

        uint32 SpellSequence, DruidSpellCombat;
};

#endif
