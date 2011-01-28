
#include "ChargeMovementGenerator.h"

template<class T, class U>
ChargeMovementGeneratorMedium<T, U>::ChargeMovementGeneratorMedium(Unit* target, const uint32 triggeredSpellId)
    : PathMovementBase(), m_target(target), m_triggeredSpellId(triggeredSpellId)
{
}

template<class T, class U>
bool ChargeMovementGeneratorMedium<T, U>::Update(T &owner, const uint32 &diff)
{
    if (!&owner)
        return true;

    // if the unit can't move, stop charge
    if (owner.hasUnitState(UNIT_STAT_NOT_MOVE))
        return true;

    // if there is no path, stop charge
    if (i_path.empty())
        return true;

    Traveller<T> traveller(owner);
    traveller.SetCustomSpeed(CHARGE_SPEED);

    if (i_destinationHolder.UpdateTraveller(traveller, diff, false, false))
        if (!IsActive(owner))
            return true;

    if (i_destinationHolder.HasArrived())
    {
        ++i_currentNode;

        // if we are at the last node, stop charge
        if (i_currentNode >= i_path.size())
            return true;

        MoveToNextNode(traveller);
    }

    return false;
}

template<class T, class U>
void ChargeMovementGeneratorMedium<T, U>::MoveToNextNode(Traveller<T> &traveller)
{
    PathNode &node = i_path[i_currentNode];
    i_destinationHolder.SetDestination(traveller, node.x, node.y, node.z, false);
}

template<class T, class U>
void ChargeMovementGeneratorMedium<T, U>::LoadPath(T &owner)
{
    // set the destination
    float x, y, z;
    m_target->GetContactPoint(&owner, x, y, z);

    // get the path to the destination
    PathInfo path(&owner, x, y, z);
    i_path = path.getFullPath();

    // start movement
    Traveller<T> traveller(owner);
    traveller.SetCustomSpeed(CHARGE_SPEED);
    MoveToNextNode(traveller);

    // send path to client
    uint32 transitTime = uint32(i_path.GetTotalLength() / (CHARGE_SPEED / IN_MILLISECONDS));
    owner.MonsterMoveByPath(i_path, 1, i_path.size(), transitTime);
}

template<class T, class U>
void ChargeMovementGeneratorMedium<T, U>::Initialize(T &owner)
{
    owner.addUnitState(UNIT_STAT_CHARGE|UNIT_STAT_CHARGE_MOVE);

    LoadPath(owner);

    // TODO: disable player bar?
}

template<class T, class U>
void ChargeMovementGeneratorMedium<T, U>::Finalize(T &owner)
{
    owner.clearUnitState(UNIT_STAT_CHARGE|UNIT_STAT_CHARGE_MOVE);

    if (i_currentNode >= i_path.size() && m_target)
    {
        // we are at the destination, turn to target and cast spell
        owner.SetInFront(m_target);

        if (m_triggeredSpellId)
            owner.CastSpell(m_target, m_triggeredSpellId, true);
    }
}

template<class T, class U>
void ChargeMovementGeneratorMedium<T, U>::Interrupt(T &owner)
{
    owner.clearUnitState(UNIT_STAT_CHARGE|UNIT_STAT_CHARGE_MOVE);
}

template<class T, class U>
void ChargeMovementGeneratorMedium<T, U>::Reset(T &owner)
{
    Initialize(owner);
}

template ChargeMovementGeneratorMedium<Player, ChargeMovementGenerator<Player> >::ChargeMovementGeneratorMedium(Unit*, const uint32);
template void ChargeMovementGeneratorMedium<Player, ChargeMovementGenerator<Player> >::Finalize(Player &);
template void ChargeMovementGeneratorMedium<Player, ChargeMovementGenerator<Player> >::Initialize(Player &);
template void ChargeMovementGeneratorMedium<Player, ChargeMovementGenerator<Player> >::Interrupt(Player &);
template void ChargeMovementGeneratorMedium<Player, ChargeMovementGenerator<Player> >::LoadPath(Player &);
template void ChargeMovementGeneratorMedium<Player, ChargeMovementGenerator<Player> >::MoveToNextNode(PlayerTraveller &);
template void ChargeMovementGeneratorMedium<Player, ChargeMovementGenerator<Player> >::Reset(Player &);
template bool ChargeMovementGeneratorMedium<Player, ChargeMovementGenerator<Player> >::Update(Player &, const uint32 &);

template ChargeMovementGeneratorMedium<Creature, ChargeMovementGenerator<Creature> >::ChargeMovementGeneratorMedium(Unit*, const uint32);
template void ChargeMovementGeneratorMedium<Creature, ChargeMovementGenerator<Creature> >::Finalize(Creature &);
template void ChargeMovementGeneratorMedium<Creature, ChargeMovementGenerator<Creature> >::Initialize(Creature &);
template void ChargeMovementGeneratorMedium<Creature, ChargeMovementGenerator<Creature> >::Interrupt(Creature &);
template void ChargeMovementGeneratorMedium<Creature, ChargeMovementGenerator<Creature> >::LoadPath(Creature &);
template void ChargeMovementGeneratorMedium<Creature, ChargeMovementGenerator<Creature> >::MoveToNextNode(CreatureTraveller &);
template void ChargeMovementGeneratorMedium<Creature, ChargeMovementGenerator<Creature> >::Reset(Creature &);
template bool ChargeMovementGeneratorMedium<Creature, ChargeMovementGenerator<Creature> >::Update(Creature &, const uint32 &);
