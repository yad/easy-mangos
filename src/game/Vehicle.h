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

#ifndef MANGOSSERVER_VEHICLE_H
#define MANGOSSERVER_VEHICLE_H

#include "ObjectGuid.h"
#include "Creature.h"
#include "Unit.h"
#include "ObjectMgr.h"

struct VehicleSeat
{
    VehicleSeatEntry const *seatInfo;
    Unit* passenger;
    uint8 flags;
    uint32 vs_flags;
};

enum VehicleSeatFlags
{
    SEAT_FREE           = 0x01,                             // free seat
    SEAT_FULL           = 0x02,                             // seat occupied by player/creature
    // special cases
    SEAT_VEHICLE_FREE   = 0x04,                             // seat occupied by vehicle, but that vehicle is free
    SEAT_VEHICLE_FULL   = 0x08                              // seat occupied by vehicle and that vehicle is full too
};

enum PowerType
{
    POWER_TYPE_PYRITE = 41,
    POWER_TYPE_STEAM  = 61
};

#define MAX_SEAT 8

typedef std::map<int8, VehicleSeat> SeatMap;

class Vehicle : public Creature
{
    public:
        explicit Vehicle();
        virtual ~Vehicle();

        void AddToWorld();
        void RemoveFromWorld();

        void Die();
        bool Create (uint32 guidlow, Map *map, uint32 phaseMask, uint32 Entry, uint32 vehicleId, uint32 team, const CreatureData *data = NULL);

        void setDeathState(DeathState s);                   // overwrite virtual Creature::setDeathState and Unit::setDeathState
        void Update(uint32 diff);                           // overwrite virtual Creature::Update and Unit::Update

        void RegeneratePower(Powers power);

        uint32 GetVehicleId() { return m_vehicleId; }
        bool SetVehicleId(uint32 vehicleid);

        void InitSeats();

        void ChangeSeatFlag(uint8 seat, uint8 flag);
        Vehicle* FindFreeSeat(int8 *seatid, bool force = true);
        Vehicle* GetNextEmptySeat(int8 *seatId, bool next = true, bool force = true);
        Vehicle* GetFirstEmptySeat(int8 *seatId, bool force = true);
        int8 GetEmptySeatsCount(bool force = true);
        void EmptySeatsCountChanged();
        int8 GetTotalSeatsCount() { return m_Seats.size(); }
        bool HasEmptySeat(int8 seatId) const;
        int8 GetNextEmptySeatNum(int8 seatId, bool next) const;

        void Dismiss();

        void RellocatePassengers(Map *map);
        void AddPassenger(Unit *unit, int8 seatId, bool force = true);
        void RemovePassenger(Unit *unit);
        void RemoveAllPassengers();

        bool HasSpell(uint32 spell) const;
        void SetSpawnDuration(int32 duration)
        {
            duration < 1 ? despawn = false : despawn = true;
            m_spawnduration = duration;
        }
        VehicleDataStructure const* GetVehicleData() { return m_VehicleData; }
        uint32 GetVehicleFlags() { return m_VehicleData ? m_VehicleData->v_flags : NULL; }
        uint32 GetCreationTime() { return m_creation_time; }
        void BuildVehicleActionBar(Player *plr) const;
        void InstallAllAccessories();
        Unit *GetPassenger(int8 seatId) const;
    protected:
        uint32 m_vehicleId;
        VehicleEntry const *m_vehicleInfo;
        VehicleDataStructure const *m_VehicleData;
        uint32 m_creation_time;
        SeatMap m_Seats;
        bool despawn;
        int32 m_spawnduration;
        void InstallAccessory(uint32 entry, int8 seatId, bool isVehicle = false, bool minion = true);
    private:
        void SaveToDB(uint32, uint8)                        // overwrited of Creature::SaveToDB     - don't must be called
        {
            ASSERT(false);
        }
        void DeleteFromDB()                                 // overwrited of Creature::DeleteFromDB - don't must be called
        {
            ASSERT(false);
        }
};
#endif
