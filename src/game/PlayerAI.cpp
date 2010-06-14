#include "PlayerAI.h"
#include "PlayerAIMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

PlayerAI::PlayerAI(Player* const player) : m_player(player), m_victim(NULL), m_ignoreAIUpdatesUntilTime(0)
{
    m_enable = false;
    m_need_to_cure = false;
    m_position_x, m_position_y = 0.5f;
}

PlayerAI::~PlayerAI() 
{
}

void PlayerAI::UpdateAI(const uint32 p_time)
{
    if(m_ignoreAIUpdatesUntilTime > time(0))
        return;

    m_ignoreAIUpdatesUntilTime = time(0) + 1;

    if(!IsEnable())
        return;

    Player* pPlayer = GetPlayer();
    if(!pPlayer)
        return;

    if(!pPlayer->isAlive())
    {
        SetVictim(NULL);
        return;
    }

    float health = float(pPlayer->GetHealth());
    float maxhealth = float(pPlayer->GetMaxHealth());
    float percent = health / maxhealth * 100.0f;

    if (percent < 30.0f) 
        m_need_to_cure = true;
    else if (m_need_to_cure && (percent > 90.0f))
        m_need_to_cure = false;
    if(m_need_to_cure && pPlayer->getAttackers().empty())
        return;

    MovementUpdate();
    Unit* pVictim = GetVictim();
    if(!pVictim || !pVictim->isAlive())
    {
        DoLoot();
        pVictim = FindNewVictim();
        pPlayer->GetMotionMaster()->Clear( true );
        pPlayer->SetStandState( UNIT_STAND_STATE_STAND );
    }

    if(pVictim && pVictim->isAlive())
    {
        if(!IsMoving())
        {
            pPlayer->GetMotionMaster()->Clear( true );
            pPlayer->clearUnitState( UNIT_STAND_STATE_STAND );
            float x, y, z, cx, cz;
            pVictim->GetPosition(x, y, z);
            CalculateXY(cx, cz);
            pPlayer->GetMotionMaster()->MovePoint(pVictim->GetMapId(), x-cx, y-cz, z);
        }
        pPlayer->SetInFront(pVictim);
        pVictim->SetInFront(pPlayer);
        pVictim->SendCreateUpdateToPlayer(pPlayer);
        pPlayer->Attack(pVictim, true);
    }
}

Unit* PlayerAI::FindNewVictim()
{
    Player* player = GetPlayer();
    Unit* victim = NULL;
    float max_range = 200.0f;

    MaNGOS::NearestAttackableUnitInObjectRangeCheck u_check(player, player, max_range);
    MaNGOS::UnitLastSearcher<MaNGOS::NearestAttackableUnitInObjectRangeCheck> checker(player, victim, u_check);
    Cell::VisitAllObjects(player, checker, max_range);

    SetVictim(victim);
    return victim;
}

void PlayerAI::MovementUpdate()
{
    Player* player = GetPlayer();

    // send heartbeats to world
    WorldPacket data;
    player->BuildHeartBeatMsg( &data );
    player->SendMessageToSet( &data, false );

    // call set position (updates states, exploration, etc.)
    // pPlayer->SetPosition( pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), false );    
}

bool PlayerAI::IsMoving() 
{
    Player* player = GetPlayer();
    return (player->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE ? false : true);
}

void PlayerAI::CalculateXY(float &x, float &y)
{
    x = GetPositionX();
    y = GetPositionY();

    if(x == 0.5f && y == 0.5f)
        y = -0.5f;
    else if(x == 0.5f && y == -0.5f)
        x = -0.5f;
    else if(x == -0.5f && y == -0.5f)
        y = 0.5f;
    else
        x = 0.5f;

    SetPositionX(x);
    SetPositionY(y);
}

void PlayerAI::DoLoot()
{
    Player* player = GetPlayer();
    Unit* victim = GetVictim();

    if( victim )
    {
        Creature *c = player->GetMap()->GetCreature( victim->GetGUID() );
        if( !c || c->getDeathState()!=CORPSE )
            return;

        if( player->IsWithinDistInMap( c, INTERACTION_DISTANCE ) )
        {
            // check for needed items
            player->SendLoot( victim->GetGUID(), LOOT_CORPSE );
            Loot *loot = &c->loot;
            uint32 lootNum = loot->GetMaxSlotInLootFor( player );
            for( uint32 l=0; l<lootNum; l++ )
            {
                QuestItem *qitem=0, *ffaitem=0, *conditem=0;
                LootItem *item = loot->LootItemInSlot( l, player, &qitem, &ffaitem, &conditem );
                if( !item )
                    continue;

                if( !qitem && item->is_blocked )
                {
                    player->SendLootRelease( player->GetLootGUID() );
                    continue;
                }

                // release loot
                player->GetSession()->DoLootRelease( victim->GetGUID() );

                // clear movement target, take next target on next update
                player->GetMotionMaster()->Clear();
            }
        }
    }
}