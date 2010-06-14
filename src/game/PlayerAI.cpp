#include "PlayerAI.h"
#include "PlayerAIMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

PlayerAI::PlayerAI(Player* const player) : m_player(player), m_victim(NULL), m_ignoreAIUpdatesUntilTime(0)
{
	m_enable = false;
}

PlayerAI::~PlayerAI() 
{
}

void PlayerAI::UpdateAI(const uint32 p_time)
{
	if(m_ignoreAIUpdatesUntilTime > time(0))
		return;

    m_ignoreAIUpdatesUntilTime = time(0) + 10;

    if(!IsEnable())
        return;

	Player* pPlayer = GetPlayer();
	if(!pPlayer)
		return;

	Unit* pVictim = GetVictim();
    if(!pVictim || !pVictim->isAlive())
    {
        pVictim = FindNewVictim(pPlayer);
        pPlayer->GetMotionMaster()->Clear( true );
        pPlayer->clearUnitState( UNIT_STAT_CHASE ); //UNIT_STAT_FOLLOW
        pPlayer->SetStandState(UNIT_STAND_STATE_STAND);
        MovementUpdate(pPlayer);
    }

	if(pVictim && pVictim->isAlive())
    {
		pPlayer->GetMotionMaster()->Clear(true);
		pPlayer->clearUnitState(UNIT_STAND_STATE_STAND);
		pPlayer->GetMotionMaster()->MoveChase(pVictim);
		pPlayer->SetInFront(pVictim);
		pPlayer->SendCreateUpdateToPlayer(pPlayer);
		pPlayer->Attack(pVictim, true);
    }
}

Unit* PlayerAI::FindNewVictim(Player* const player)
{
    Unit* victim = NULL;
    float max_range = 200.0f;

    MaNGOS::NearestAttackableUnitInObjectRangeCheck u_check(player, player, max_range);
    MaNGOS::UnitLastSearcher<MaNGOS::NearestAttackableUnitInObjectRangeCheck> checker(player, victim, u_check);
    Cell::VisitAllObjects(player, checker, max_range);

	SetVictim(victim);
    return victim;
}

void PlayerAI::MovementUpdate(Player* const pPlayer)
{
    // send heartbeats to world
    WorldPacket data;
    pPlayer->BuildHeartBeatMsg( &data );
    pPlayer->SendMessageToSet( &data, false );
}