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

#ifndef _PLAYERBOTMGR_H
#define _PLAYERBOTMGR_H

#include "Common.h"

class WorldPacket;
class Player;
class Unit;
class Object;
class Item;
class PlayerbotClassAI;

class MANGOS_DLL_SPEC PlayerbotMgr
{
public:
    PlayerbotMgr();
    virtual ~PlayerbotMgr();

    // This is called from Unit.cpp and is called every second (I think)
    void UpdateAI(const uint32 p_time);

    // This is called whenever the master sends a packet to the server.
    // These packets can be viewed, but not edited.
    // It allows bot creators to craft AI in response to a master's actions.
    // For a list of opcodes that can be caught see Opcodes.cpp (CMSG_* opcodes only)
    // Notice: that this is static which means it is called once for all bots of the master.
    void HandleMasterIncomingPacket(const WorldPacket& packet);
    void HandleMasterOutgoingPacket(const WorldPacket& packet);

    static void AddAllBots();

    void LogoutPlayerBot(ObjectGuid guid);
    Player* GetPlayerBot (ObjectGuid guid) const;
    Player* GetLeader() const { return m_master; };
    void SetLeader(Player* pl) { m_master = pl; };

    void LogoutAllBots();
    void OnBotLogin(Player * const bot);
    void Stay();

protected:
    Player* m_master;
};

#endif
