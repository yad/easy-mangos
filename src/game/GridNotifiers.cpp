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

#include "GridNotifiers.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "UpdateData.h"
#include "Item.h"
#include "Map.h"
#include "Transports.h"
#include "ObjectAccessor.h"

using namespace MaNGOS;

void
VisibleChangesNotifier::Visit(PlayerMapType &m)
{
    for(PlayerMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        Player* player = iter->getSource();
        if(player == &i_object)
            continue;

        player->UpdateVisibilityOf(player->GetViewPoint(),&i_object);
    }
}

void
VisibleNotifier::SendToSelf()
{
    if (Transport* transport = i_player.GetTransport())
        for(Transport::PlayerSet::const_iterator itr = transport->GetPassengers().begin();itr!=transport->GetPassengers().end();++itr)
        {
            if(vis_guids.find((*itr)->GetGUID()) != vis_guids.end())
            {
                vis_guids.erase((*itr)->GetGUID());

                i_player.UpdateVisibilityOf(&i_viewPoint,(*itr), i_data, i_visibleNow);

                //if(!(*itr)->isNeedNotify(NOTIFY_PLAYER_VISIBILITY))
                    (*itr)->UpdateVisibilityOf((*itr),&i_player);
            }
        }

    for(Player::ClientGUIDs::const_iterator it = vis_guids.begin();it != vis_guids.end(); ++it)
    {
        i_player.m_clientGUIDs.erase(*it);
        i_data.AddOutOfRangeGUID(*it);

        if(!IS_PLAYER_GUID(*it))
            continue;

        if(Player *plr = ObjectAccessor::FindPlayer(*it))
            if(plr->IsInWorld() && !plr->isNeedNotify(NOTIFY_VISIBILITY_CHANGED))
                plr->UpdateVisibilityOf(plr->GetViewPoint(), &i_player);
    }

    if(!i_data.HasData())// if Update data empty, i_visibleNow set empty too
        return;

    WorldPacket packet;
    i_data.BuildPacket(&packet);
    i_player.GetSession()->SendPacket(&packet);

    for(std::set<Unit*>::const_iterator it = i_visibleNow.begin(); it != i_visibleNow.end(); ++it)
        (*it)->SendInitialVisiblePacketsFor(&i_player);
}

inline void PlayerCreatureRelocationWorker(Player* pl, Creature* c)
{
    // Creature AI reaction
    if(!c->hasUnitState(UNIT_STAT_FLEEING))
    {
        if( c->AI() && c->AI()->IsVisible(pl) && !c->IsInEvadeMode() )
            c->AI()->MoveInLineOfSight(pl);
    }
}

inline void CreatureCreatureRelocationWorker(Creature* c1, Creature* c2)
{
    if(!c1->hasUnitState(UNIT_STAT_FLEEING))
    {
        if( c1->AI() && c1->AI()->IsVisible(c2) && !c1->IsInEvadeMode() )
            c1->AI()->MoveInLineOfSight(c2);
    }

    if(!c2->hasUnitState(UNIT_STAT_FLEEING))
    {
        if( c2->AI() && c2->AI()->IsVisible(c1) && !c2->IsInEvadeMode() )
            c2->AI()->MoveInLineOfSight(c1);
    }
}

void PlayerRelocationNotifier::Visit(CreatureMapType &m)
{
    bool relocated_for_ai = i_player.isAlive() && !i_player.isInFlight() && 
        i_player.isNeedNotify(NOTIFY_VISIBILITY_CHANGED) && (&i_player == &i_viewPoint);

    for(CreatureMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        Creature * c = iter->getSource();

        vis_guids.erase(c->GetGUID());

        i_player.UpdateVisibilityOf(&i_viewPoint,c,i_data,i_visibleNow);

        if (relocated_for_ai && c->isAlive() && !c->NotifyExecuted(NOTIFY_VISIBILITY_CHANGED))
            PlayerCreatureRelocationWorker(&i_player, c);
    }
}

void PlayerRelocationNotifier::Visit(PlayerMapType &m)
{
    for(PlayerMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        Player* plr = iter->getSource();
        if(plr == &i_player)
            continue;

        vis_guids.erase(plr->GetGUID());

        i_player.UpdateVisibilityOf(&i_viewPoint,plr,i_data,i_visibleNow);

        if (plr->isNeedNotify(NOTIFY_VISIBILITY_CHANGED))
            continue;

        plr->UpdateVisibilityOf(plr->GetViewPoint(),&i_player);
    }
}

void CreatureRelocationNotifier::Visit(PlayerMapType &m)
{
    bool relocated_for_ai = i_creature.isAlive();

    for(PlayerMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        Player * pl = iter->getSource();

        // update visibility for players that not relocated since last reset
        if(!pl->isNeedNotify(NOTIFY_VISIBILITY_CHANGED))
            pl->UpdateVisibilityOf(pl->GetViewPoint(), &i_creature);

        if(relocated_for_ai && pl->isAlive() && !pl->isInFlight() && !pl->NotifyExecuted(NOTIFY_VISIBILITY_CHANGED))
            PlayerCreatureRelocationWorker(pl, &i_creature);
    }
}

void CreatureRelocationNotifier::Visit(CreatureMapType &m)
{
    if(!i_creature.isAlive())
        return;

    for(CreatureMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        Creature* c = iter->getSource();
        if( c != &i_creature && c->isAlive() && !c->NotifyExecuted(NOTIFY_VISIBILITY_CHANGED))
            CreatureCreatureRelocationWorker(c, &i_creature);
    }
}

void
MessageDeliverer::Visit(PlayerMapType &m)
{
    for(PlayerMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        if (i_toSelf || iter->getSource() != &i_player)
        {
            if (!i_player.InSamePhase(iter->getSource()))
                continue;

            if(WorldSession* session = iter->getSource()->GetSession())
                session->SendPacket(i_message);
        }
    }
}

void
ObjectMessageDeliverer::Visit(PlayerMapType &m)
{
    for(PlayerMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        if(!iter->getSource()->InSamePhase(i_phaseMask))
            continue;

        if(WorldSession* session = iter->getSource()->GetSession())
            session->SendPacket(i_message);
    }
}

void
MessageDistDeliverer::Visit(PlayerMapType &m)
{
    for(PlayerMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        if ((i_toSelf || iter->getSource() != &i_player ) &&
            (!i_ownTeamOnly || iter->getSource()->GetTeam() == i_player.GetTeam() ) &&
            (!i_dist || iter->getSource()->IsWithinDist(&i_player,i_dist)))
        {
            if (!i_player.InSamePhase(iter->getSource()))
                continue;

            if (WorldSession* session = iter->getSource()->GetSession())
                session->SendPacket(i_message);
        }
    }
}

void
ObjectMessageDistDeliverer::Visit(PlayerMapType &m)
{
    for(PlayerMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        if (!i_dist || iter->getSource()->IsWithinDist(&i_object,i_dist))
        {
            if (!i_object.InSamePhase(iter->getSource()))
                continue;

            if (WorldSession* session = iter->getSource()->GetSession())
                session->SendPacket(i_message);
        }
    }
}

template<class T> void
ObjectUpdater::Visit(GridRefManager<T> &m)
{
    for(typename GridRefManager<T>::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        iter->getSource()->Update(i_timeDiff);
    }
}

bool CannibalizeObjectCheck::operator()(Corpse* u)
{
    // ignore bones
    if(u->GetType()==CORPSE_BONES)
        return false;

    Player* owner = ObjectAccessor::FindPlayer(u->GetOwnerGUID());

    if( !owner || i_funit->IsFriendlyTo(owner))
        return false;

    if(i_funit->IsWithinDistInMap(u, i_range) )
        return true;

    return false;
}

template void ObjectUpdater::Visit<GameObject>(GameObjectMapType &);
template void ObjectUpdater::Visit<DynamicObject>(DynamicObjectMapType &);
