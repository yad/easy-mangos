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

typedef UNORDERED_MAP<uint64, Player*> PlayerBotMap;

class MANGOS_DLL_SPEC PlayerbotMgr
{
    public:
        PlayerbotMgr();
        virtual ~PlayerbotMgr();

        void UpdateAI(const uint32 p_time);

        void HandleMasterIncomingPacket(const WorldPacket& packet);

        static void AddAllBots(int nbBotsWanted);

        void AddPlayerBot(uint64 guid, uint8 fclass);
        void LogoutPlayerBot(uint64 guid);
        Player* GetPlayerBot(uint64 guid) const;
        Player* GetMaster() const { return m_master; };
        void SetMaster(Player* pl) { m_master = pl; };

        void OnBotLogin(Player* bot);
        void RealPlayerLogout(Player* player);
        void Stay(Player* player);

    public:
        bool m_confDebugWhisper;

    private:
        Player* m_master;
};

#endif
