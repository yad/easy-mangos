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

#include "Creature.h"
#include "CreatureAI.h"
#include "MapManager.h"
#include "FleeingMovementGenerator.h"
#include "DestinationHolderImp.h"
#include "ObjectAccessor.h"

#define MIN_QUIET_DISTANCE 28.0f
#define MAX_QUIET_DISTANCE 43.0f

template<class T>
void FleeingMovementGenerator<T>::_setTargetLocation(T &owner)
{
    if(!&owner)
        return;

    // ignore in case other no reaction state
    if (owner.hasUnitState(UNIT_STAT_CAN_NOT_REACT & ~UNIT_STAT_FLEEING))
        return;

    float x, y, z;
    if(!_getPoint(owner, x, y, z))
        return;

    owner.addUnitState(UNIT_STAT_FLEEING_MOVE);
    Traveller<T> traveller(owner);
    
    PathInfo path(&owner, x, y, z);
    if(!(path.getPathType() & PATHFIND_NORMAL))
    {
        i_nextCheckTime.Reset(urand(1000, 1500));
        return;
    }

    PointPath pointPath = path.getFullPath();

    float speed = traveller.Speed() * 0.001f; // in ms
    uint32 traveltime = uint32(pointPath.GetTotalLength() / speed);
    SplineFlags flags = (owner.GetTypeId() == TYPEID_UNIT) ? ((Creature*)&owner)->GetSplineFlags() : SPLINEFLAG_WALKMODE;
    owner.SendMonsterMoveByPath(pointPath, 1, std::min<uint32>(pointPath.size(), 5), flags, traveltime);

    PathNode p = pointPath[std::min<uint32>(pointPath.size()-1, 4)];
    owner.UpdateAllowedPositionZ(p.x, p.y, p.z);

    i_destinationHolder.SetDestination(traveller, p.x, p.y, p.z, false);

    i_nextCheckTime.Reset(traveltime + urand(800, 1500));
}

template<class T>
bool FleeingMovementGenerator<T>::_getPoint(T &owner, float &x, float &y, float &z)
{
    if(!&owner)
        return false;

    float dist_from_caster, angle_to_caster;
    if(Unit *fright = ObjectAccessor::GetUnit(owner, i_frightGUID))
    {
        dist_from_caster = fright->GetDistance(&owner);
        if(dist_from_caster > 0.2f)
            angle_to_caster = fright->GetAngle(&owner);
        else
            angle_to_caster = frand(0, 2*M_PI_F);
    }
    else
    {
        dist_from_caster = 0.0f;
        angle_to_caster = frand(0, 2*M_PI_F);
    }

    float dist, angle;
    if(dist_from_caster < MIN_QUIET_DISTANCE)
    {
        dist = frand(0.4f, 1.3f)*(MIN_QUIET_DISTANCE - dist_from_caster);
        angle = angle_to_caster + frand(-M_PI_F/8, M_PI_F/8);
    }
    else if(dist_from_caster > MAX_QUIET_DISTANCE)
    {
        dist = frand(0.4f, 1.0f)*(MAX_QUIET_DISTANCE - MIN_QUIET_DISTANCE);
        angle = -angle_to_caster + frand(-M_PI_F/4, M_PI_F/4);
    }
    else    // we are inside quiet range
    {
        dist = frand(0.6f, 1.2f)*(MAX_QUIET_DISTANCE - MIN_QUIET_DISTANCE);
        angle = frand(0, 2*M_PI_F);
    }

    float curr_x, curr_y, curr_z;
    owner.GetPosition(curr_x, curr_y, curr_z);

    x = curr_x + dist*cos(angle);
    y = curr_y + dist*sin(angle);
    z = curr_z;

    owner.UpdateAllowedPositionZ(x, y, z);
    
    return true;
}

template<class T>
void FleeingMovementGenerator<T>::Initialize(T &owner)
{
    owner.addUnitState(UNIT_STAT_FLEEING|UNIT_STAT_FLEEING_MOVE);

    if(owner.GetTypeId() == TYPEID_UNIT)
    {
        ((Creature*)&owner)->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        ((Creature*)&owner)->SetTargetGuid(ObjectGuid());
    }

    _setTargetLocation(owner);
}


template<>
void FleeingMovementGenerator<Player>::Finalize(Player &owner)
{
    owner.clearUnitState(UNIT_STAT_FLEEING|UNIT_STAT_FLEEING_MOVE);
    owner.StopMoving();
}

template<>
void FleeingMovementGenerator<Creature>::Finalize(Creature &owner)
{
    owner.AddSplineFlag(SPLINEFLAG_WALKMODE);
    owner.clearUnitState(UNIT_STAT_FLEEING|UNIT_STAT_FLEEING_MOVE);
}

template<class T>
void FleeingMovementGenerator<T>::Interrupt(T &owner)
{
    // flee state still applied while movegen disabled
    owner.clearUnitState(UNIT_STAT_FLEEING_MOVE);
}

template<class T>
void FleeingMovementGenerator<T>::Reset(T &owner)
{
    Initialize(owner);
}

template<class T>
bool FleeingMovementGenerator<T>::Update(T &owner, const uint32 & time_diff)
{
    if( !&owner || !owner.isAlive() )
        return false;

    // ignore in case other no reaction state
    if (owner.hasUnitState(UNIT_STAT_CAN_NOT_REACT & ~UNIT_STAT_FLEEING))
    {
        owner.clearUnitState(UNIT_STAT_FLEEING_MOVE);
        return true;
    }

    Traveller<T> traveller(owner);

    i_nextCheckTime.Update(time_diff);

    if( (owner.IsStopped() && !i_destinationHolder.HasArrived()) || !i_destinationHolder.HasDestination() )
    {
        _setTargetLocation(owner);
        return true;
    }

    if (i_destinationHolder.UpdateTraveller(traveller, time_diff, false))
    {
        if (!IsActive(owner))                               // force stop processing (movement can move out active zone with cleanup movegens list)
            return true;                                    // not expire now, but already lost

        i_destinationHolder.ResetUpdate(100);
        if(i_nextCheckTime.Passed() && i_destinationHolder.HasArrived())
        {
            _setTargetLocation(owner);
            return true;
        }
    }
    return true;
}

template void FleeingMovementGenerator<Player>::Initialize(Player &);
template void FleeingMovementGenerator<Creature>::Initialize(Creature &);
template bool FleeingMovementGenerator<Player>::_getPoint(Player &, float &, float &, float &);
template bool FleeingMovementGenerator<Creature>::_getPoint(Creature &, float &, float &, float &);
template void FleeingMovementGenerator<Player>::_setTargetLocation(Player &);
template void FleeingMovementGenerator<Creature>::_setTargetLocation(Creature &);
template void FleeingMovementGenerator<Player>::Interrupt(Player &);
template void FleeingMovementGenerator<Creature>::Interrupt(Creature &);
template void FleeingMovementGenerator<Player>::Reset(Player &);
template void FleeingMovementGenerator<Creature>::Reset(Creature &);
template bool FleeingMovementGenerator<Player>::Update(Player &, const uint32 &);
template bool FleeingMovementGenerator<Creature>::Update(Creature &, const uint32 &);

void TimedFleeingMovementGenerator::Finalize(Unit &owner)
{
    owner.clearUnitState(UNIT_STAT_FLEEING|UNIT_STAT_FLEEING_MOVE);
    if (Unit* victim = owner.getVictim())
    {
        if (owner.isAlive())
        {
            owner.AttackStop(true);
            ((Creature*)&owner)->AI()->AttackStart(victim);
        }
    }
}

bool TimedFleeingMovementGenerator::Update(Unit & owner, const uint32 & time_diff)
{
    if( !owner.isAlive() )
        return false;

    // ignore in case other no reaction state
    if (owner.hasUnitState(UNIT_STAT_CAN_NOT_REACT & ~UNIT_STAT_FLEEING))
    {
        owner.clearUnitState(UNIT_STAT_FLEEING_MOVE);
        return true;
    }

    i_totalFleeTime.Update(time_diff);
    if (i_totalFleeTime.Passed())
        return false;

    // This calls grant-parent Update method hiden by FleeingMovementGenerator::Update(Creature &, const uint32 &) version
    // This is done instead of casting Unit& to Creature& and call parent method, then we can use Unit directly
    return MovementGeneratorMedium< Creature, FleeingMovementGenerator<Creature> >::Update(owner, time_diff);
}
