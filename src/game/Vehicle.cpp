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

#include "Common.h"
#include "Log.h"
#include "Vehicle.h"
#include "Unit.h"
#include "Util.h"
#include "WorldPacket.h"
#include "InstanceData.h"

Vehicle::Vehicle() : Creature(CREATURE_SUBTYPE_VEHICLE), m_vehicleId(0), m_vehicleInfo(NULL), m_spawnduration(0),
                     despawn(false), m_creation_time(0), m_VehicleData(NULL)
{
    m_updateFlag = (UPDATEFLAG_LIVING | UPDATEFLAG_HAS_POSITION | UPDATEFLAG_VEHICLE);
}

Vehicle::~Vehicle()
{
}

void Vehicle::AddToWorld()
{
    ///- Register the vehicle for guid lookup
    if(!IsInWorld())
        GetMap()->GetObjectsStore().insert<Vehicle>(GetGUID(), (Vehicle*)this);

    Unit::AddToWorld();
}

void Vehicle::RemoveFromWorld()
{
    ///- Remove the vehicle from the accessor
    if(IsInWorld())
        GetMap()->GetObjectsStore().erase<Vehicle>(GetGUID(), (Vehicle*)NULL);

    ///- Don't call the function for Creature, normal mobs + totems go in a different storage
    Unit::RemoveFromWorld();
}

void Vehicle::Respawn()
{
    Creature::Respawn();
    InstallAllAccessories();
}

void Vehicle::setDeathState(DeathState s)                       // overwrite virtual Creature::setDeathState and Unit::setDeathState
{
    Creature::setDeathState(s);
    if(s == JUST_DIED)
    {
        if(GetVehicleFlags() & VF_DESPAWN_NPC)
            Dismiss();
        else
            RemoveAllPassengers();
    }
}

void Vehicle::Update(uint32 diff)
{
    Creature::Update(diff);

    if(despawn)
    {
        m_spawnduration -= diff;
        if(m_spawnduration < 0)
            Dismiss();
        despawn = false;
    }

    if(m_regenTimer <= diff)
    {
        RegeneratePower(getPowerType());
        m_regenTimer = 4000;
    }
    else
        m_regenTimer -= diff;
}

void Vehicle::RegeneratePower(Powers power)
{
    uint32 curValue = GetPower(power);
    uint32 maxValue = GetMaxPower(power);

    if (curValue >= maxValue)
        return;

    float addvalue = 0.0f;

    // hack: needs more research of power type from the dbc. 
    // It must contains some info about vehicles like Salvaged Chopper.
    if(m_vehicleInfo->m_powerType == POWER_TYPE_PYRITE)
        return;

    addvalue = 20.0f;

    ModifyPower(power, (int32)addvalue);
}

bool Vehicle::Create(uint32 guidlow, Map *map, uint32 phaseMask, uint32 Entry, uint32 vehicleId, uint32 team, const CreatureData *data)
{
    SetMap(map);
    SetPhaseMask(phaseMask,false);

    CreatureInfo const *cinfo = sObjectMgr.GetCreatureTemplate(Entry);
    if(!cinfo)
    {
        sLog.outErrorDb("Creature entry %u does not exist.", Entry);
        return false;
    }

    Object::_Create(guidlow, Entry, HIGHGUID_VEHICLE);

    if(!UpdateEntry(Entry, team, data))
        return false;

    if(!vehicleId)
    {
        CreatureDataAddon const *cainfo = GetCreatureAddon();
        if(!cainfo)
            return false;
        vehicleId = cainfo->vehicle_id;
    }
    if(!SetVehicleId(vehicleId))
        return false;

    LoadCreaturesAddon();

    m_regenHealth = false;
    m_creation_time = getMSTime();

    SetFloatValue(UNIT_FIELD_HOVERHEIGHT, 1.0f);
    //RemoveMonsterMoveFlag(MONSTER_MOVE_WALK);

    //Notify the map's instance data.
    //Only works if you create the object in it, not if it is moves to that map.
    //Normally non-players do not teleport to other maps.
    if(map->IsDungeon() && ((InstanceMap*)map)->GetInstanceData())
    {
        ((InstanceMap*)map)->GetInstanceData()->OnCreatureCreate(this);
    }
    
    if(m_vehicleInfo->m_powerType == POWER_TYPE_STEAM)
    {
        setPowerType(POWER_ENERGY);
        SetMaxPower(POWER_ENERGY, 100);
        SetPower(POWER_ENERGY, 100);
    }
    else if(m_vehicleInfo->m_powerType == POWER_TYPE_PYRITE)
    {
        setPowerType(POWER_ENERGY);
        SetMaxPower(POWER_ENERGY, 50);
        SetPower(POWER_ENERGY, 50);
    }
    else
    {
        for (uint32 i = 0; i < MAX_VEHICLE_SPELLS; ++i)
        {
            if(!GetVehicleData()->v_spells[i])
                continue;
            SpellEntry const *spellInfo = sSpellStore.LookupEntry(GetVehicleData()->v_spells[i]);
            if(!spellInfo)
                continue;

            if(spellInfo->powerType == POWER_MANA)
                break;

            if(spellInfo->powerType == POWER_ENERGY)
            {
                setPowerType(POWER_ENERGY);
                SetMaxPower(POWER_ENERGY, 100);
                SetPower(POWER_ENERGY, 100);
                break;
            }
        }
    }
    SetHealth(GetMaxHealth());
    InstallAllAccessories();

    return true;
}

bool Vehicle::SetVehicleId(uint32 vehicleid)
{
    VehicleEntry const *vehicleInfo = sVehicleStore.LookupEntry(vehicleid);
    if(!vehicleInfo)
        return false;

    m_vehicleId = vehicleid;
    m_vehicleInfo = vehicleInfo;

    // can be NULL
    VehicleDataStructure const *VDStructure = sObjectMgr.GetVehicleData(vehicleid);
    if(VDStructure)
        m_VehicleData = VDStructure;

    InitSeats();
    EmptySeatsCountChanged();
    return true;
}

void Vehicle::InitSeats()
{
    m_Seats.clear();

    for(uint32 i = 0; i < MAX_SEAT; ++i)
    {
        uint32 seatId = m_vehicleInfo->m_seatID[i];
        if(seatId)
        {
            if(VehicleSeatEntry const *veSeat = sVehicleSeatStore.LookupEntry(seatId))
            {
                VehicleSeat newseat;
                newseat.seatInfo = veSeat;
                newseat.passenger = NULL;
                newseat.flags = SEAT_FREE;
                newseat.vs_flags = sObjectMgr.GetSeatFlags(seatId);
                m_Seats[i] = newseat;
            }
        }
    }
    // NOTE : there can be vehicles without seats (eg. 180) - probably some TEST vehicles
}
void Vehicle::ChangeSeatFlag(uint8 seat, uint8 flag)
{
    SeatMap::iterator i_seat = m_Seats.find(seat);
    // this should never happen
    if(i_seat == m_Seats.end())
        return;

    if(i_seat->second.flags != flag)
    {
        i_seat->second.flags = flag;
        EmptySeatsCountChanged();
    }
}
Vehicle* Vehicle::FindFreeSeat(int8 *seatid, bool force)
{
    SeatMap::const_iterator i_seat = m_Seats.find(*seatid);
    if(i_seat == m_Seats.end())
        return GetFirstEmptySeat(seatid, force);
    if((i_seat->second.flags & (SEAT_FULL | SEAT_VEHICLE_FULL)) || (!force && (i_seat->second.vs_flags & SF_UNACCESSIBLE)))
        return GetNextEmptySeat(seatid, true, force);
    if(i_seat->second.flags & SEAT_VEHICLE_FREE)
    {
        // this should never be NULL
        if(Vehicle *v = (Vehicle*)i_seat->second.passenger)
            return v->FindFreeSeat(seatid, force);
        return NULL;
    }
    return this;
}

Vehicle* Vehicle::GetFirstEmptySeat(int8 *seatId, bool force)
{
    for(SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if(itr->second.flags & SEAT_FREE)
        {
            if(!force && (itr->second.vs_flags & SF_UNACCESSIBLE))
                continue;

            *seatId = itr->first;
            return this;
        }
        else if(itr->second.flags & SEAT_VEHICLE_FREE)
        {
            *seatId = itr->first;
            if(Vehicle *v = (Vehicle*)itr->second.passenger)
                return v->FindFreeSeat(seatId, force);
        }
    }

    return NULL;
}

Vehicle* Vehicle::GetNextEmptySeat(int8 *seatId, bool next, bool force)
{
    SeatMap::const_iterator i_seat = m_Seats.find(*seatId);
    if(i_seat == m_Seats.end()) return GetFirstEmptySeat(seatId, force);

    while((i_seat->second.flags & (SEAT_FULL | SEAT_VEHICLE_FULL)) || (!force && (i_seat->second.vs_flags & SF_UNACCESSIBLE)))
    {
        if(next)
        {
            ++i_seat;
            if(i_seat == m_Seats.end())
                i_seat = m_Seats.begin();
        }
        else
        {
            if(i_seat == m_Seats.begin())
                i_seat = m_Seats.end();
            --i_seat;
        }
        if(i_seat->first == *seatId)
            return NULL;
    }
    *seatId = i_seat->first;
    if(i_seat->second.flags & SEAT_VEHICLE_FREE)
    {
        if(Vehicle *v = (Vehicle*)i_seat->second.passenger)
            return v->FindFreeSeat(seatId, force);
        return NULL;
    }

    return this;
}

int8 Vehicle::GetEmptySeatsCount(bool force)
{
    int8 count = 0;
    for(SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
   {
        if(itr->second.flags & (SEAT_FREE | SEAT_VEHICLE_FREE))
        {
            if(!force && (itr->second.vs_flags & SF_UNACCESSIBLE))
                continue;

            count++;
        }
    }

    return count;
}
int8 Vehicle::GetNextEmptySeatNum(int8 seatId, bool next) const
{
    SeatMap::const_iterator seat = m_Seats.find(seatId);
    if(seat == m_Seats.end()) return -1;
    while(seat->second.passenger || !seat->second.seatInfo->IsUsable())
    {
        if(next)
        {
            ++seat;
            if(seat == m_Seats.end())
                seat = m_Seats.begin();
        }
        else
        {
            if(seat == m_Seats.begin())
                seat = m_Seats.end();
            --seat;
        }
        if(seat->first == seatId)
            return -1; // no available seat
    }
    return seat->first;
}

bool Vehicle::HasEmptySeat(int8 seatId) const
{
    SeatMap::const_iterator seat = m_Seats.find(seatId);
    if(seat == m_Seats.end()) return false;
    return !seat->second.passenger;
}

void Vehicle::EmptySeatsCountChanged()
{
    uint8 m_count = GetTotalSeatsCount();
    uint8 p_count = GetEmptySeatsCount(false);
    uint8 u_count = GetEmptySeatsCount(true);

    // seats accesibles by players
    if(p_count > 0)
        SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    else
        RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

    if(u_count == m_count)
    {
        RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
    }
    else
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

    if(uint64 vehicleGUID = GetVehicleGUID())
    {
        if(Vehicle *vehicle = ObjectAccessor::GetVehicle(vehicleGUID))
        {
            if(u_count > 0)
                vehicle->ChangeSeatFlag(m_movementInfo.GetTransportSeat(), SEAT_VEHICLE_FREE);
            else
                vehicle->ChangeSeatFlag(m_movementInfo.GetTransportSeat(), SEAT_VEHICLE_FULL);
        }
    }
}



void Vehicle::Dismiss()
{
    RemoveAllPassengers();
    SendObjectDeSpawnAnim(GetGUID());
    CombatStop();
    AddObjectToRemoveList();
}

void Vehicle::RellocatePassengers(Map *map)
{
    for(SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if(itr->second.flags & SEAT_FULL)
        {
            // passenger cant be NULL here
            Unit *passengers = itr->second.passenger;
            assert(passengers);

            float xx = GetPositionX() + passengers->m_movementInfo.GetTransportPos()->x;
            float yy = GetPositionY() + passengers->m_movementInfo.GetTransportPos()->y;
            float zz = GetPositionZ() + passengers->m_movementInfo.GetTransportPos()->z;
            //float oo = passengers->m_SeatData.Orientation;
            // this is not correct, we should recalculate
            // actual rotation depending on vehicle
            float oo = passengers->GetOrientation();

            if(passengers->GetTypeId() == TYPEID_PLAYER)
                ((Player*)passengers)->SetPosition(xx, yy, zz, oo);
            else
                map->CreatureRelocation((Creature*)passengers, xx, yy, zz, oo);
        }
        else if(itr->second.flags & (SEAT_VEHICLE_FULL | SEAT_VEHICLE_FREE))
        {
            // passenger cant be NULL here
            Unit *passengers = itr->second.passenger;
            assert(passengers);

            float xx = GetPositionX() + passengers->m_movementInfo.GetTransportPos()->x;
            float yy = GetPositionY() + passengers->m_movementInfo.GetTransportPos()->y;
            float zz = GetPositionZ() + passengers->m_movementInfo.GetTransportPos()->z;
            //float oo = passengers->m_SeatData.Orientation;
            // this is not correct, we should recalculate
            // actual rotation depending on vehicle
            float oo = passengers->GetOrientation();

            map->CreatureRelocation((Creature*)passengers, xx, yy, zz, oo);
        }
    }
}

void Vehicle::AddPassenger(Unit *unit, int8 seatId, bool force)
{
    SeatMap::iterator seat;
    seat = m_Seats.find(seatId);

    // this should never happen
    if(seat == m_Seats.end())
        return;

    unit->SetVehicleGUID(GetGUID());
    unit->m_movementInfo.AddMovementFlag(MOVEFLAG_ONTRANSPORT);
    unit->m_movementInfo.AddMovementFlag(MOVEFLAG_ROOT);

    seat->second.passenger = unit;
    if(unit->GetTypeId() == TYPEID_UNIT && ((Creature*)unit)->isVehicle())
    {
        if(((Vehicle*)unit)->GetEmptySeatsCount(true) == 0)
            seat->second.flags = SEAT_VEHICLE_FULL;
        else
            seat->second.flags = SEAT_VEHICLE_FREE;
    }
    else
    {
        seat->second.flags = SEAT_FULL;
    }

    if(unit->GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data0(SMSG_FORCE_MOVE_ROOT, 10);
        data0 << unit->GetPackGUID();
        data0 << (uint32)((seat->second.vs_flags & SF_CAN_CAST) ? 2 : 0);
        unit->SendMessageToSet(&data0,true);
    }

    if(unit->GetTypeId() == TYPEID_PLAYER)
    {
        uint8 allowMove = 1;
        if(GetVehicleFlags() & VF_MOVEMENT)
            allowMove = 0;
        ((Player*)unit)->SetMover(this);
        ((Player*)unit)->SetClientControl(this, 1);
        ((Player*)unit)->GetCamera().SetView(this);
    }

    if(seat->second.vs_flags & SF_MAIN_RIDER)
    {
        if(!(GetVehicleFlags() & VF_MOVEMENT))
        {
            GetMotionMaster()->Clear(false);
            GetMotionMaster()->MoveIdle();
            SetCharmerGUID(unit->GetGUID());
            unit->SetUInt64Value(UNIT_FIELD_CHARM, GetGUID());
            if(canFly() || HasAuraType(SPELL_AURA_FLY) || HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED))
            {
                WorldPacket data3(SMSG_MOVE_SET_CAN_FLY, 12);
                data3<< GetPackGUID();
                data3 << (uint32)(0);
                SendMessageToSet(&data3,false);
            }
        }

        SpellClickInfoMapBounds clickPair = sObjectMgr.GetSpellClickInfoMapBounds(GetEntry());
        for(SpellClickInfoMap::const_iterator itr = clickPair.first; itr != clickPair.second; ++itr)
        {
            if (unit->GetTypeId() == TYPEID_UNIT || itr->second.IsFitToRequirements((Player*)unit))
            {
                Unit *caster = (itr->second.castFlags & 0x1) ? unit : this;
                Unit *target = (itr->second.castFlags & 0x2) ? unit : this;

                caster->CastSpell(target, itr->second.spellId, true);
            }
        }
        if(unit->GetTypeId() == TYPEID_PLAYER)
        {
            // it should be added only on rider enter?
            if(((Player*)unit)->GetGroup())
                ((Player*)unit)->SetGroupUpdateFlag(GROUP_UPDATE_VEHICLE);

            BuildVehicleActionBar((Player*)unit);
        }

        if(!(GetVehicleFlags() & VF_FACTION))
            setFaction(unit->getFaction());

        if(GetVehicleFlags() & VF_CANT_MOVE)
        {
            WorldPacket data2(SMSG_FORCE_MOVE_ROOT, 10);
            data2<< GetPackGUID();
            data2 << (uint32)(2);
            SendMessageToSet(&data2,false);
        }

        if(GetVehicleFlags() & VF_NON_SELECTABLE)
            SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
    if(seat->second.vs_flags & SF_UNATTACKABLE)
        unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    EmptySeatsCountChanged();
}

void Vehicle::RemovePassenger(Unit *unit)
{
    SeatMap::iterator seat;
    for(seat = m_Seats.begin(); seat != m_Seats.end(); ++seat)
    {
        if((seat->second.flags & (SEAT_FULL | SEAT_VEHICLE_FREE | SEAT_VEHICLE_FULL)) && seat->second.passenger == unit)
        {
            unit->SetVehicleGUID(0);
            if(unit->GetTypeId() == TYPEID_PLAYER)
            {
                ((Player*)unit)->SetMover(unit);
                ((Player*)unit)->SetClientControl(unit, 1);
                ((Player*)unit)->GetCamera().SetView(unit);
            }

            if(seat->second.vs_flags & SF_MAIN_RIDER)
            {
                RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
                if(unit->GetTypeId() == TYPEID_PLAYER)
                {
                    ((Player*)unit)->RemovePetActionBar();

                    if(((Player*)unit)->GetGroup())
                        ((Player*)unit)->SetGroupUpdateFlag(GROUP_UPDATE_VEHICLE);
                }
                unit->SetCharm(NULL);
                SetCharmerGUID(NULL);
                setFaction(GetCreatureInfo()->faction_A);
            }
            if(GetVehicleFlags() & VF_NON_SELECTABLE)
                RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            if(seat->second.vs_flags & SF_UNATTACKABLE)
                unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            // restore player control
            if(unit->GetTypeId() == TYPEID_PLAYER)
            {
                if(seat->second.vs_flags & SF_CAN_CAST)
                {
                    WorldPacket data0(SMSG_FORCE_MOVE_UNROOT, 10);
                    data0 << unit->GetPackGUID();
                    data0 << (uint32)(2);                        // can rotate
                    unit->SendMessageToSet(&data0,true);
                }
                else
                {
                    WorldPacket data1(SMSG_FORCE_MOVE_UNROOT, 10);
                    data1 << unit->GetPackGUID();
                    data1 << (uint32)(0);                        // cannot rotate
                    unit->SendMessageToSet(&data1,true);
                }
            }
            unit->m_movementInfo.RemoveMovementFlag(MOVEFLAG_ONTRANSPORT);
            unit->m_movementInfo.RemoveMovementFlag(MOVEFLAG_ROOT);
            unit->m_movementInfo.ClearTransportData();

            seat->second.passenger = NULL;
            seat->second.flags = SEAT_FREE;
            EmptySeatsCountChanged();
            break;
        }
    }
}

void Vehicle::RemoveAllPassengers()
{
    for(SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if(itr->second.flags & SEAT_FULL)
        {
            if(Unit *passenger = itr->second.passenger)                     // this cant be NULL, but..
                passenger->ExitVehicle();
        }
        else if(itr->second.flags & (SEAT_VEHICLE_FULL | SEAT_VEHICLE_FREE))
        {
            if(Unit *passenger = itr->second.passenger)                     // this cant be NULL, but..
            {
                passenger->ExitVehicle();
                ((Vehicle*)passenger)->Dismiss();
            }
        }
    }
    // make sure everything is cleared
    InitSeats();
}

bool Vehicle::HasSpell(uint32 spell) const
{
    if(!m_VehicleData)
        return false;

    for(uint8 j = 0; j < MAX_VEHICLE_SPELLS; j++)
    {
        if(m_VehicleData->v_spells[j] == spell)
            return true;
    }

    return false;
}

void Vehicle::BuildVehicleActionBar(Player *plr) const
{
    WorldPacket data(SMSG_PET_SPELLS, 8+2+4+4+4*10+1+1);
    data << uint64(GetGUID());
    data << uint16(0x00000000);                     // creature family, not used in vehicles
    data << uint32(0x00000000);                     // unk
    data << uint32(0x00000101);                     // react state

    for(uint32 i = 0; i <= MAX_VEHICLE_SPELLS; ++i)
    {
        data << uint16(m_VehicleData ? m_VehicleData->v_spells[i] : NULL) << uint8(0) << uint8(i+8);
    }

    data << uint8(0);                               //aditional spells in spellbook, not used in vehicles

    uint8 cooldownsCount = m_CreatureSpellCooldowns.size() + m_CreatureCategoryCooldowns.size();
    data << uint8(cooldownsCount);
    time_t curTime = time(NULL);

    for(CreatureSpellCooldowns::const_iterator itr = m_CreatureSpellCooldowns.begin(); itr != m_CreatureSpellCooldowns.end(); ++itr)
    {
        time_t cooldown = (itr->second > curTime) ? (itr->second - curTime) * IN_MILLISECONDS : 0;

        data << uint32(itr->first);                         // spellid
        data << uint16(0);                                  // spell category?
        data << uint32(cooldown);                           // cooldown
        data << uint32(0);                                  // category cooldown
    }

    for(CreatureSpellCooldowns::const_iterator itr = m_CreatureCategoryCooldowns.begin(); itr != m_CreatureCategoryCooldowns.end(); ++itr)
    {
        time_t cooldown = (itr->second > curTime) ? (itr->second - curTime) * IN_MILLISECONDS : 0;

        data << uint32(itr->first);                         // spellid
        data << uint16(0);                                  // spell category?
        data << uint32(0);                                  // cooldown
        data << uint32(cooldown);                           // category cooldown
    }

    plr->GetSession()->SendPacket(&data);

    data.Initialize(SMSG_PET_GUIDS, 12);
    data << uint32(1);                                        // count
    data << uint64(GetGUID());
    plr->GetSession()->SendPacket(&data);
}
void Vehicle::InstallAllAccessories()
{
    if(!GetMap())
       return;

    CreatureDataAddon const *cainfo = GetCreatureAddon();
    if(!cainfo || !cainfo->passengers)
        return;
    for (CreatureDataAddonPassengers const* cPassanger = cainfo->passengers; cPassanger->seat_idx != -1; ++cPassanger)
    {
        // Continue if seat already taken
        if(GetPassenger(cPassanger->seat_idx))
            continue;

        uint32 guid = 0;
        bool isVehicle = false;
        // Set guid and check whatever it is
        if(cPassanger->guid != 0)
            guid = cPassanger->guid;
        else
        {
            CreatureDataAddon const* passAddon;
            passAddon = ObjectMgr::GetCreatureTemplateAddon(cPassanger->entry);
            if(passAddon && passAddon->vehicle_id != 0)
                isVehicle = true;
            else
                guid = sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT);
        }
        // Create it
        Creature *pPassenger = new Creature;
        if(!isVehicle)
        {
            uint32 entry = cPassanger->entry;
            if(entry == 0)
            {
                CreatureData const* data = sObjectMgr.GetCreatureData(guid);
                if(!data)
                    continue;
                entry = data->id;
            }     
            
            if(!pPassenger->Create(guid, GetMap(), GetPhaseMask(), entry, 0))
                continue;
            pPassenger->LoadFromDB(guid, GetMap());
            pPassenger->Relocate(GetPositionX(), GetPositionY(), GetPositionZ());
            GetMap()->Add(pPassenger);
            pPassenger->AIM_Initialize();
        }
        else
            pPassenger = (Creature*)SummonVehicle(cPassanger->entry, GetPositionX(), GetPositionY(), GetPositionZ(), 0);
        // Enter vehicle...
        pPassenger->EnterVehicle(this, cPassanger->seat_idx, true);
        // ...and send update. Without this, client wont show this new creature/vehicle...
        WorldPacket data;
        pPassenger->BuildHeartBeatMsg(&data);
        pPassenger->SendMessageToSet(&data, false);
    }
}

Unit *Vehicle::GetPassenger(int8 seatId) const
{
    SeatMap::const_iterator seat = m_Seats.find(seatId);
    if(seat == m_Seats.end()) return NULL;
    return seat->second.passenger;
}
void Vehicle::Die()
{
    for (SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
        if(Unit *passenger = itr->second.passenger)
            if(((Creature*)passenger)->isVehicle())
                ((Vehicle*)passenger)->Dismiss();
    RemoveAllPassengers();
}