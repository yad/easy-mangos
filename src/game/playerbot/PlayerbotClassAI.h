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

#ifndef _PLAYERBOTCLASSAI_H
#define _PLAYERBOTCLASSAI_H

#include "Common.h"
#include "../World.h"
#include "../SpellMgr.h"
#include "../Player.h"
#include "../ObjectMgr.h"
#include "WorldPacket.h"
#include "../Unit.h"
#include "../SharedDefines.h"
#include "PlayerbotAI.h"

class Player;
class PlayerbotAI;

class MANGOS_DLL_SPEC PlayerbotClassAI
{
public:
    PlayerbotClassAI(Player * const bot, PlayerbotAI * const ai);
    virtual ~PlayerbotClassAI();

    // all combat actions go here
    virtual bool DoCombatManeuver(Unit*, bool);
    virtual void DoFastBuffOnOneself();
    virtual bool DoEvadeAction();
    virtual bool DoProtectSelfAction();

    // all non combat actions go here, ex buffs, heals, rezzes
    virtual void DoNonCombatActions();

    // buff a specific player, usually a real PC who is not in group
    virtual bool BuffPlayer(Player* target);
    virtual void InitSpells(PlayerbotAI* const ai);
    virtual void ReinitCycles();

    // Utilities
    Player* GetPlayerBot() { return m_bot; }
    PlayerbotAI* GetAI (){ return m_ai; };


private:
    Player* m_bot;
    PlayerbotAI* m_ai;
};

#endif
