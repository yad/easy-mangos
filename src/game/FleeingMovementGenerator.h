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

#ifndef MANGOS_FLEEINGMOVEMENTGENERATOR_H
#define MANGOS_FLEEINGMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "DestinationHolder.h"
#include "Traveller.h"
#include "PathFinder.h"

template<class T>
class MANGOS_DLL_SPEC FleeingMovementGenerator
: public MovementGeneratorMedium< T, FleeingMovementGenerator<T> >
{
    public:
        FleeingMovementGenerator(uint64 fright) : i_frightGUID(fright), i_nextCheckTime(0) {}

        void Initialize(T &);
        void Finalize(T &);
        void Interrupt(T &);
        void Reset(T &);
        bool Update(T &, const uint32 &);

        MovementGeneratorType GetMovementGeneratorType() const { return FLEEING_MOTION_TYPE; }

    private:
        void _setTargetLocation(T &owner);
        bool _getPoint(T &owner, float &x, float &y, float &z);

        uint64 i_frightGUID;

        TimeTracker i_nextCheckTime;
        DestinationHolder< Traveller<T> > i_destinationHolder;
};

class MANGOS_DLL_SPEC TimedFleeingMovementGenerator
: public FleeingMovementGenerator<Creature>
{
    public:
        TimedFleeingMovementGenerator(uint64 fright, uint32 time) :
            FleeingMovementGenerator<Creature>(fright),
            i_totalFleeTime(time) {}

        MovementGeneratorType GetMovementGeneratorType() const { return TIMED_FLEEING_MOTION_TYPE; }
        bool Update(Unit &, const uint32 &);
        void Finalize(Unit &);

    private:
        TimeTracker i_totalFleeTime;
};

#endif
