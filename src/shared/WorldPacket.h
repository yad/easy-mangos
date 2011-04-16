/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
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

#ifndef MANGOSSERVER_WORLDPACKET_H
#define MANGOSSERVER_WORLDPACKET_H

#include "Common.h"
#include "ByteBuffer.h"

// Note: m_opcode and size stored in platfom dependent format
// ignore endianess until send, and converted at receive
class WorldPacket : public ByteBuffer
{
    public:
                                                            // just container for later use
        WorldPacket()                                       : ByteBuffer(0), m_opcode(0)
        {
        }
        explicit WorldPacket(uint16 opcode, size_t res=200) : ByteBuffer(res), m_opcode(opcode) { }
                                                            // copy constructor
        WorldPacket(const WorldPacket &packet)              : ByteBuffer(packet), m_opcode(packet.m_opcode)
        {
        }

        void Initialize(uint16 opcode, size_t newres=200)
        {
            clear();
            _storage.reserve(newres);
            m_opcode = opcode;
        }

        uint16 GetOpcode() const { return m_opcode; }
        void SetOpcode(uint16 opcode) { m_opcode = opcode; }

        void Print()
        {
            switch (m_opcode)
            {
                case SMSG_MONSTER_MOVE:
                case SMSG_UPDATE_WORLD_STATE:
                case SMSG_COMPRESSED_UPDATE_OBJECT:
                case MSG_MOVE_SET_FACING:
                case MSG_MOVE_STOP:
                case MSG_MOVE_HEARTBEAT:
                case MSG_MOVE_STOP_STRAFE:
                case MSG_MOVE_START_STRAFE_LEFT:
                case SMSG_UPDATE_OBJECT:
                case MSG_MOVE_START_FORWARD:
                case MSG_MOVE_START_STRAFE_RIGHT:
                case SMSG_DESTROY_OBJECT:
                case MSG_MOVE_START_BACKWARD:
                case SMSG_AURA_UPDATE_ALL:
                case MSG_MOVE_FALL_LAND:
                case MSG_MOVE_JUMP:
                case CMSG_NAME_QUERY:
                case CMSG_STANDSTATECHANGE:
                case CMSG_QUERY_TIME:
                case CMSG_CREATURE_QUERY:
                case CMSG_GAMEOBJECT_QUERY:
                case SMSG_SPELL_GO:
                case SMSG_TIME_SYNC_REQ:
                case CMSG_TIME_SYNC_RESP:
                case SMSG_EMOTE:
                case SMSG_POWER_UPDATE:
                case SMSG_AURA_UPDATE:
                case SMSG_PERIODICAURALOG:
                    break;
                default:
                {
                    const char* oc = LookupOpcodeName(m_opcode);
                    std::ostringstream out;
                    out << oc;
                    sLog.outString(out.str().c_str());
                    break;
                }  
            }
        }

    protected:
        uint16 m_opcode;
};
#endif
