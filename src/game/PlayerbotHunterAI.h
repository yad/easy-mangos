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

#ifndef _PLAYERHUNTERAI_H
#define _PLAYERHUNTERAI_H

#include "PlayerbotClassAI.h"

enum
{
    SPELL_HUNTER
};

class MANGOS_DLL_SPEC PlayerbotHunterAI : PlayerbotClassAI
{
    public:
        PlayerbotHunterAI(Player* const bot, PlayerbotAI* const ai);
        virtual ~PlayerbotHunterAI();
        void InitSpells(PlayerbotAI* const ai);
        bool HasPet(Player* bot);

        bool DoNextCombatManeuver(Unit*);
        bool DoNonCombatActions();

    private:
        bool m_petSummonFailed;
        bool m_rangedCombat;

        uint32 PET_SUMMON, PET_DISMISS, PET_REVIVE, PET_MEND, PET_FEED, BAD_ATTITUDE, SONIC_BLAST, NETHER_SHOCK, DEMORALIZING_SCREECH, INTIMIDATION;
        uint32 AUTO_SHOT, HUNTERS_MARK, ARCANE_SHOT, CONCUSSIVE_SHOT, DISTRACTING_SHOT, MULTI_SHOT, EXPLOSIVE_SHOT, SERPENT_STING, SCORPID_STING, VIPER_STING, WYVERN_STING, AIMED_SHOT, STEADY_SHOT, CHIMERA_SHOT, VOLLEY, BLACK_ARROW, KILL_SHOT;
        uint32 RAPTOR_STRIKE, WING_CLIP, MONGOOSE_BITE, DISENGAGE, DETERRENCE;
        uint32 BEAR_TRAP, FREEZING_TRAP, IMMOLATION_TRAP, FROST_TRAP, EXPLOSIVE_TRAP, ARCANE_TRAP, SNAKE_TRAP;
        uint32 ASPECT_OF_THE_HAWK, ASPECT_OF_THE_MONKEY, RAPID_FIRE, TRUESHOT_AURA, MISDIRECTION;
        uint32 RECENTLY_BANDAGED;
        uint32 ARCANE_TORRENT, GIFT_OF_THE_NAARU, STONEFORM, ESCAPE_ARTIST, EVERY_MAN_FOR_HIMSELF, SHADOWMELD, BLOOD_FURY, WAR_STOMP, BERSERKING, WILL_OF_THE_FORSAKEN;
};

#endif

