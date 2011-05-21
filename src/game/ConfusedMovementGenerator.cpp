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

#include "ConfusedMovementGenerator.h"
#include "Creature.h"
#include "MapManager.h"
#include "Opcodes.h"
#include "DestinationHolderImp.h"

template<class T>
void ConfusedMovementGenerator<T>::Initialize(T &unit)
{
    // set initial position
    unit.GetPosition(i_x, i_y, i_z);

    if(unit.GetTypeId() == TYPEID_UNIT)
        ((Creature*)&unit)->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

    unit.StopMoving();
    unit.addUnitState(UNIT_STAT_CONFUSED|UNIT_STAT_CONFUSED_MOVE);
}

template<class T>
void ConfusedMovementGenerator<T>::Interrupt(T &unit)
{
    // confused state still applied while movegen disabled
    unit.clearUnitState(UNIT_STAT_CONFUSED_MOVE);
}

template<class T>
void ConfusedMovementGenerator<T>::Reset(T &unit)
{
    i_nextMoveTime.Reset(0);
    i_destinationHolder.ResetUpdate();
    unit.StopMoving();
    unit.addUnitState(UNIT_STAT_CONFUSED|UNIT_STAT_CONFUSED_MOVE);
}

template<class T>
bool ConfusedMovementGenerator<T>::Update(T &unit, const uint32 &diff)
{
    if(!&unit)
        return true;

    // ignore in case other no reaction state
    if (unit.hasUnitState(UNIT_STAT_CAN_NOT_REACT & ~UNIT_STAT_CONFUSED))
        return true;

    if (i_nextMoveTime.Passed())
    {
        // currently moving, update location
        unit.addUnitState(UNIT_STAT_CONFUSED_MOVE);
        Traveller<T> traveller(unit);
        if (i_destinationHolder.UpdateTraveller(traveller, diff, false))
        {
            if (!IsActive(unit))                            // force stop processing (movement can move out active zone with cleanup movegens list)
                return true;                                // not expire now, but already lost

            if (i_destinationHolder.HasArrived())
            {
                // arrived, stop and wait a bit
                unit.StopMoving();

                i_nextMoveTime.Reset(urand(800, 1500));
            }
        }
    }
    else
    {
        // waiting for next move
        i_nextMoveTime.Update(diff);
        if(i_nextMoveTime.Passed())
        {
            // start moving
            unit.addUnitState(UNIT_STAT_CONFUSED_MOVE);
            
            float x = i_x + 10.0f*(rand_norm_f() - 0.5f);
            float y = i_y + 10.0f*(rand_norm_f() - 0.5f);
            float z = i_z;

            unit.UpdateAllowedPositionZ(x, y, z);

            Traveller<T> traveller(unit);

            PathInfo path(&unit, x, y, z);
            if(!(path.getPathType() & PATHFIND_NORMAL))
            {
                i_nextMoveTime.Reset(urand(800, 1000));
                return true;
            }

            PointPath pointPath = path.getFullPath();

            float speed = traveller.Speed() * 0.001f; // in ms
            uint32 traveltime = uint32(pointPath.GetTotalLength() / speed);
            SplineFlags flags = (unit.GetTypeId() == TYPEID_UNIT) ? ((Creature*)&unit)->GetSplineFlags() : SPLINEFLAG_WALKMODE;
            unit.SendMonsterMoveByPath(pointPath, 1, std::min<uint32>(pointPath.size(), 5), flags, traveltime);

            PathNode p = pointPath[std::min<uint32>(pointPath.size()-1, 4)];
            // we do not really need it with mmaps active
            unit.UpdateAllowedPositionZ(p.x, p.y, p.z);
            i_destinationHolder.SetDestination(traveller, p.x, p.y, p.z, false);
        }
    }
    return true;
}

template<>
void ConfusedMovementGenerator<Player>::Finalize(Player &unit)
{
    unit.clearUnitState(UNIT_STAT_CONFUSED|UNIT_STAT_CONFUSED_MOVE);
    unit.StopMoving();
}

template<>
void ConfusedMovementGenerator<Creature>::Finalize(Creature &unit)
{
    unit.clearUnitState(UNIT_STAT_CONFUSED|UNIT_STAT_CONFUSED_MOVE);
    unit.AddSplineFlag(SPLINEFLAG_WALKMODE);
}

template void ConfusedMovementGenerator<Player>::Initialize(Player &player);
template void ConfusedMovementGenerator<Creature>::Initialize(Creature &creature);
template void ConfusedMovementGenerator<Player>::Interrupt(Player &player);
template void ConfusedMovementGenerator<Creature>::Interrupt(Creature &creature);
template void ConfusedMovementGenerator<Player>::Reset(Player &player);
template void ConfusedMovementGenerator<Creature>::Reset(Creature &creature);
template bool ConfusedMovementGenerator<Player>::Update(Player &player, const uint32 &diff);
template bool ConfusedMovementGenerator<Creature>::Update(Creature &creature, const uint32 &diff);
