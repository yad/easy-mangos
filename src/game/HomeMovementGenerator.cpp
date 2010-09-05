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

#include "HomeMovementGenerator.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Traveller.h"
#include "DestinationHolderImp.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"

void
HomeMovementGenerator<Creature>::Initialize(Creature & owner)
{
    owner.RemoveSplineFlag(SPLINEFLAG_WALKMODE);
    _setTargetLocation(owner);
}

void
HomeMovementGenerator<Creature>::Reset(Creature &)
{
}

void
HomeMovementGenerator<Creature>::_setTargetLocation(Creature & owner)
{
    if (owner.hasUnitState(UNIT_STAT_NOT_MOVE))
        return;

    float x, y, z;

    // at apply we can select more nice return points base at current movegen
    if (owner.GetMotionMaster()->empty() || !owner.GetMotionMaster()->top()->GetResetPosition(owner,x,y,z))
        owner.GetRespawnCoord(x, y, z);

    if(!i_path)
        i_path = new PathInfo(&owner, x, y, z);
    else
        i_path->Update(x, y, z);

    // we always have a path to the target - shortcut in worst case
    // even is this map don't have mmaps at all
    i_path->getNextPosition(x, y, z);

    CreatureTraveller traveller(owner);
    i_destinationHolder.SetDestination(traveller, x, y, z);
    owner.clearUnitState(UNIT_STAT_ALL_STATE);
}

bool
HomeMovementGenerator<Creature>::Update(Creature &owner, const uint32& time_diff)
{
    CreatureTraveller traveller( owner);
    bool reachedHome = false;

    if (i_destinationHolder.UpdateTraveller(traveller, time_diff, false))
    {
        if (!IsActive(owner))                               // force stop processing (movement can move out active zone with cleanup movegens list)
            return true;                                    // not expire now, but already lost

        bool needNewDest = false;
        if(i_path && i_destinationHolder.HasArrived())
        {
            float end_x, end_y, end_z, next_x, next_y, next_z, cur_x, cur_y, cur_z;
            float dist = owner.GetObjectBoundingRadius();
            i_path->getNextPosition(next_x, next_y, next_z);
            i_path->getEndPosition(end_x, end_y, end_z);
            i_destinationHolder.GetLocationNow(owner.GetBaseMap(), cur_x, cur_y, cur_z);

            reachedHome = isSamePoint(dist, cur_x, cur_y, cur_z, end_x, end_y, end_z);
            needNewDest = !isSamePoint(dist, next_x, next_y, next_z, end_x, end_y, end_z);
        }

        if (!i_path || needNewDest)
            _setTargetLocation(owner);
    }

    if (reachedHome)
    {
        owner.AddSplineFlag(SPLINEFLAG_WALKMODE);

        // restore orientation of not moving creature at returning to home
        if(owner.GetDefaultMovementType()==IDLE_MOTION_TYPE)
        {
            if(CreatureData const* data = sObjectMgr.GetCreatureData(owner.GetDBTableGUIDLow()))
            {
                owner.SetOrientation(data->orientation);
                WorldPacket packet;
                owner.BuildHeartBeatMsg(&packet);
                owner.SendMessageToSet(&packet, false);
            }
        }

        owner.LoadCreaturesAddon(true);
        owner.AI()->JustReachedHome();
        return false;
    }

    return true;
}
