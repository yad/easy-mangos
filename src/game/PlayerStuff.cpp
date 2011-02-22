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

#include "Player.h"
#include "PlayerBot/PlayerbotAI.h"

void Player::GiveMeBestItemForMyLevel()
{
    ItemPrototype const *bIInSlot[MAX_INVTYPE+4];
    for (uint8 i = INVTYPE_NON_EQUIP; i < MAX_INVTYPE+4; ++i)
        bIInSlot[i] = NULL;

    for (uint32 id = 0; id < sItemStorage.MaxEntry; id++)
    {
        ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(id);
        if(!pProto)
            continue;

        uint16 eDest;
        if (CanEquipNewItem(NULL_SLOT, eDest, id, false)!=EQUIP_ERR_OK)
            continue;

        if(!IsForMyClass(pProto) || !IsNotAllowedItem(pProto))
            continue;

        if (pProto->ItemSet > 0 && !OtherItemsInSetAreAllowedForMe(pProto))
            continue;

        switch( pProto->InventoryType )
        {
            case INVTYPE_HEAD:
            case INVTYPE_SHOULDERS:
            case INVTYPE_CHEST:
            case INVTYPE_WAIST:
            case INVTYPE_LEGS:
            case INVTYPE_FEET:
            case INVTYPE_WRISTS:
            case INVTYPE_HANDS:
            case INVTYPE_CLOAK:
            case INVTYPE_SHIELD:
            case INVTYPE_ROBE:
            {
                if(!bIInSlot[pProto->InventoryType])
                    bIInSlot[pProto->InventoryType] = pProto;
                else
                    bIInSlot[pProto->InventoryType] = BestItemBetween(bIInSlot[pProto->InventoryType], pProto, false);
                break;
            }
            case INVTYPE_NECK:
            case INVTYPE_HOLDABLE:
            {
                if (pProto->StatsCount == 0)
                    break;

                if(!bIInSlot[pProto->InventoryType])
                    bIInSlot[pProto->InventoryType] = pProto;
                else
                    bIInSlot[pProto->InventoryType] = BestItemBetween(bIInSlot[pProto->InventoryType], pProto, false);
                break;
            }
            case INVTYPE_FINGER:
            {
                if (pProto->StatsCount == 0)
                    break;

                if(!bIInSlot[pProto->InventoryType])
                    bIInSlot[pProto->InventoryType] = pProto;
                else if(!bIInSlot[INVTYPE_FINGER2])
                    bIInSlot[INVTYPE_FINGER2] = pProto;
                else
                {
                    ItemPrototype const *tItem = BestItemBetween(bIInSlot[pProto->InventoryType], bIInSlot[INVTYPE_FINGER2], false);
                    if (tItem == bIInSlot[pProto->InventoryType])
                        bIInSlot[INVTYPE_FINGER2] = BestItemBetween(bIInSlot[INVTYPE_FINGER2], pProto, false);
                    else
                        bIInSlot[pProto->InventoryType] = BestItemBetween(bIInSlot[pProto->InventoryType], pProto, false);
                }
                break;
            }
            case INVTYPE_TRINKET:
            {
                if (pProto->StatsCount == 0)
                    break;

                if(!bIInSlot[pProto->InventoryType])
                    bIInSlot[pProto->InventoryType] = pProto;
                else if(!bIInSlot[INVTYPE_TRINKET2])
                    bIInSlot[INVTYPE_TRINKET2] = pProto;
                else
                {
                    ItemPrototype const *tItem = BestItemBetween(bIInSlot[pProto->InventoryType], bIInSlot[INVTYPE_TRINKET2], false);
                    if (tItem == bIInSlot[pProto->InventoryType])
                        bIInSlot[INVTYPE_TRINKET2] = BestItemBetween(bIInSlot[INVTYPE_TRINKET2], pProto, false);
                    else
                        bIInSlot[pProto->InventoryType] = BestItemBetween(bIInSlot[pProto->InventoryType], pProto, false);
                }
                break;
            }
            case INVTYPE_RANGED:
            case INVTYPE_RANGEDRIGHT:
            case INVTYPE_WEAPONMAINHAND:
            {
                if(!bIInSlot[pProto->InventoryType])
                    bIInSlot[pProto->InventoryType] = pProto;
                else
                    bIInSlot[pProto->InventoryType] = BestItemBetween(bIInSlot[pProto->InventoryType], pProto, true);
                break;
            }
            case INVTYPE_WEAPONOFFHAND:
            {
                if(!CanDualWield())
                    break;

                if(!bIInSlot[pProto->InventoryType])
                    bIInSlot[pProto->InventoryType] = pProto;
                else
                    bIInSlot[pProto->InventoryType] = BestItemBetween(bIInSlot[pProto->InventoryType], pProto, true);
                break;
            }
            case INVTYPE_WEAPON:
            {
                if(!bIInSlot[pProto->InventoryType])
                    bIInSlot[pProto->InventoryType] = pProto;
                else if(!bIInSlot[INVTYPE_WEAPON2])
                    bIInSlot[INVTYPE_WEAPON2] = pProto;
                else
                {
                    ItemPrototype const *tItem = BestItemBetween(bIInSlot[pProto->InventoryType], bIInSlot[INVTYPE_WEAPON2], true);
                    if (tItem == bIInSlot[pProto->InventoryType])
                        bIInSlot[INVTYPE_WEAPON2] = BestItemBetween(bIInSlot[INVTYPE_WEAPON2], pProto, true);
                    else
                        bIInSlot[pProto->InventoryType] = BestItemBetween(bIInSlot[pProto->InventoryType], pProto, true);
                }
                break;
            }
            case INVTYPE_2HWEAPON:
            {
                if(!bIInSlot[pProto->InventoryType])
                    bIInSlot[pProto->InventoryType] = pProto;
                else if(!bIInSlot[INVTYPE_2HWEAPON2])
                    bIInSlot[INVTYPE_2HWEAPON2] = pProto;
                else
                {
                    ItemPrototype const *tItem = BestItemBetween(bIInSlot[pProto->InventoryType], bIInSlot[INVTYPE_2HWEAPON2], true);
                    if (tItem == bIInSlot[pProto->InventoryType])
                        bIInSlot[INVTYPE_2HWEAPON2] = BestItemBetween(bIInSlot[INVTYPE_2HWEAPON2], pProto, true);
                    else
                        bIInSlot[pProto->InventoryType] = BestItemBetween(bIInSlot[pProto->InventoryType], pProto, true);
                }
                break;
            }
            default :
                break;
        }
    }

    for (uint8 i = INVTYPE_NON_EQUIP; i < MAX_INVTYPE+4; ++i)
        bIFromSet = CheckItemSet(bIInSlot[i], bIFromSet);

    if (bIFromSet && bIFromSet->ItemSet > 0)
    {
        ItemSetMap const & mItemSet = sObjectMgr.GetItemSetMap();
        for (ItemSetMap::const_iterator itr = mItemSet.begin(); itr != mItemSet.end(); ++itr)
        {
            if (itr->second.ItemSet != bIFromSet->ItemSet)
                continue;

            if (itr->second.ItemLevel > 155 && itr->second.ItemLevel != bIFromSet->ItemLevel)
                continue;

            if (itr->second.RequiredLevel > getLevel())
            {
                bIFromSet = NULL;
                break;
            }
        }
    }

    if (bIFromSet && bIFromSet->ItemSet > 0)
    {
        ItemSetMap const & mItemSet = sObjectMgr.GetItemSetMap();
        for (ItemSetMap::const_iterator itr = mItemSet.begin(); itr != mItemSet.end(); ++itr)
        {
            if (itr->second.ItemSet != bIFromSet->ItemSet)
                continue;

            if (itr->second.ItemLevel > 155 && itr->second.ItemLevel != bIFromSet->ItemLevel)
                continue;

            ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(itr->second.ItemId);
            if (!pProto)
                continue;

            bIInSlot[pProto->InventoryType] = pProto;
        }
    }

    for (uint8 i = INVTYPE_NON_EQUIP; i < MAX_INVTYPE+4; ++i)
    {
        switch (i)
        {
            case INVTYPE_CHEST:
            case INVTYPE_ROBE:
            case INVTYPE_RANGED:
            case INVTYPE_RANGEDRIGHT:
            case INVTYPE_WEAPON:
            case INVTYPE_SHIELD:
            case INVTYPE_2HWEAPON:
            case INVTYPE_WEAPONMAINHAND:
            case INVTYPE_WEAPONOFFHAND:
            case INVTYPE_WEAPON2:
            case INVTYPE_2HWEAPON2:
            case INVTYPE_FINGER:
            case INVTYPE_FINGER2:
            case INVTYPE_TRINKET:
            case INVTYPE_HOLDABLE:
            case INVTYPE_TRINKET2:
            {
                break;
            }
            default:
            {
                StoreNewItemInBestSlots(bIInSlot[i]);
                break;
            }
        }

    }
    StoreNewItemInBestSlots(BestItemBetween(bIInSlot[INVTYPE_CHEST], bIInSlot[INVTYPE_ROBE], false));
    StoreNewItemInBestSlots(BestItemBetween(bIInSlot[INVTYPE_RANGED], bIInSlot[INVTYPE_RANGEDRIGHT], false));

    ItemPrototype const *mItem = NULL;
    ItemPrototype const *sItem = NULL;

    switch(getRole())
    {
        case MageFire:
        case MageArcane:
        case MageFrost:
        case PriestHoly:
        case DruidRestoration:
        case WarlockDestruction:
        case WarlockCurses:
        case WarlockSummoning:
        {
            mItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONMAINHAND], true);
            mItem = BestItemBetween(mItem, bIInSlot[INVTYPE_WEAPON2], true);
            if (!mItem)
                mItem = BestItemBetween(bIInSlot[INVTYPE_2HWEAPON], bIInSlot[INVTYPE_2HWEAPON2], true);
            else
                sItem = bIInSlot[INVTYPE_HOLDABLE];
            break;
        }
        case WarriorArms:
        case ShamanEnhancement:
        {
            mItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONMAINHAND], true);
            mItem = BestItemBetween(mItem, bIInSlot[INVTYPE_WEAPON2], true);
            if (CanDualWield())
            {
                if (mItem == bIInSlot[INVTYPE_WEAPON])
                    sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON2], bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                else if (mItem == bIInSlot[INVTYPE_WEAPON2])
                    sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                else
                {
                    sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPON2], true);
                    sItem = BestItemBetween(sItem, bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                }
                if (!sItem)
                    sItem = bIInSlot[INVTYPE_SHIELD];
            }
            else
                sItem = bIInSlot[INVTYPE_SHIELD];
            break;
        }
        case WarriorProtection:
        {
            mItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONMAINHAND], true);
            mItem = BestItemBetween(mItem, bIInSlot[INVTYPE_WEAPON2], true);
            sItem = bIInSlot[INVTYPE_SHIELD];
            if (!sItem && CanDualWield())
            {
                if (mItem == bIInSlot[INVTYPE_WEAPON])
                    sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON2], bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                else if (mItem == bIInSlot[INVTYPE_WEAPON2])
                    sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                else
                {
                    sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPON2], true);
                    sItem = BestItemBetween(sItem, bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                }
            }
            break;
        }
        case WarriorFury:
        {
            if (CanTitanGrip())
            {
                mItem = BestItemBetween(bIInSlot[INVTYPE_2HWEAPON], bIInSlot[INVTYPE_2HWEAPON2], true);
                if (mItem == bIInSlot[INVTYPE_2HWEAPON])
                    sItem = bIInSlot[INVTYPE_2HWEAPON2];
                else
                    sItem = bIInSlot[INVTYPE_2HWEAPON];
                if (!sItem)
                    sItem = bIInSlot[INVTYPE_SHIELD];
            }
            else
            {
                mItem = BestItemBetween(bIInSlot[INVTYPE_2HWEAPON], bIInSlot[INVTYPE_2HWEAPON2], true);
                if (!mItem)
                {
                    mItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONMAINHAND], true);
                    mItem = BestItemBetween(mItem, bIInSlot[INVTYPE_WEAPON2], true);
                    if (CanDualWield())
                    {
                        if (mItem == bIInSlot[INVTYPE_WEAPON])
                            sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON2], bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                        else if (mItem == bIInSlot[INVTYPE_WEAPON2])
                            sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                        else
                        {
                            sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPON2], true);
                            sItem = BestItemBetween(sItem, bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                        }
                        if (!sItem)
                            sItem = bIInSlot[INVTYPE_SHIELD];
                    }
                    else
                        sItem = bIInSlot[INVTYPE_SHIELD];
                }
            }
            break;
        }
        case RogueCombat:
        case RogueAssassination:
        case RogueSubtlety:
        {
            mItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONMAINHAND], true);
            mItem = BestItemBetween(mItem, bIInSlot[INVTYPE_WEAPON2], true);
            if (mItem == bIInSlot[INVTYPE_WEAPON])
                sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON2], bIInSlot[INVTYPE_WEAPONOFFHAND], true);
            else if (mItem == bIInSlot[INVTYPE_WEAPON2])
                sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONOFFHAND], true);
            else
            {
                sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPON2], true);
                sItem = BestItemBetween(sItem, bIInSlot[INVTYPE_WEAPONOFFHAND], true);
            }
            break;
        }
        case PriestDiscipline:
        case PriestShadow:
        case ShamanElementalCombat:
        case DruidFeralCombat:
        case DruidBalance:
        {
            mItem = BestItemBetween(bIInSlot[INVTYPE_2HWEAPON], bIInSlot[INVTYPE_2HWEAPON2], true);
            if (!mItem)
            {
                mItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONMAINHAND], true);
                mItem = BestItemBetween(mItem, bIInSlot[INVTYPE_WEAPON2], true);
                if (!sItem)
                    sItem = bIInSlot[INVTYPE_HOLDABLE];
            }
            break;
        }
        case PaladinCombat:
        {
            mItem = BestItemBetween(bIInSlot[INVTYPE_2HWEAPON], bIInSlot[INVTYPE_2HWEAPON2], true);
            if (!mItem)
            {
                mItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONMAINHAND], true);
                mItem = BestItemBetween(mItem, bIInSlot[INVTYPE_WEAPON2], true);
                if (!sItem)
                    sItem = bIInSlot[INVTYPE_SHIELD];
            }
            break;
        }
        case ShamanRestoration:
        case DeathKnightFrost:
        case PaladinHoly:
        case PaladinProtection:
        {
            mItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONMAINHAND], true);
            mItem = BestItemBetween(mItem, bIInSlot[INVTYPE_WEAPON2], true);
            sItem = bIInSlot[INVTYPE_SHIELD];
            break;
        }
        case HunterBeastMastery:
        case HunterSurvival:
        case HunterMarksmanship:
        case DeathKnightBlood:
        case DeathKnightUnholy:
        {
            mItem = BestItemBetween(bIInSlot[INVTYPE_2HWEAPON], bIInSlot[INVTYPE_2HWEAPON2], true);
            if (!mItem)
            {
                mItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONMAINHAND], true);
                mItem = BestItemBetween(mItem, bIInSlot[INVTYPE_WEAPON2], true);
                if (CanDualWield())
                {
                    if (mItem == bIInSlot[INVTYPE_WEAPON])
                        sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON2], bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                    else if (mItem == bIInSlot[INVTYPE_WEAPON2])
                        sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                    else
                    {
                        sItem = BestItemBetween(bIInSlot[INVTYPE_WEAPON], bIInSlot[INVTYPE_WEAPON2], true);
                        sItem = BestItemBetween(sItem, bIInSlot[INVTYPE_WEAPONOFFHAND], true);
                    }
                    if (!sItem)
                        sItem = bIInSlot[INVTYPE_SHIELD];
                }
                else
                    sItem = bIInSlot[INVTYPE_SHIELD];
            }
            break;
        }
    }

    StoreNewItemInBestSlots(mItem);
    if (sItem && (sItem->InventoryType==INVTYPE_SHIELD || sItem->InventoryType==INVTYPE_HOLDABLE))
        StoreNewItemInBestSlots(sItem);
    else if (!StoreNewItemInBestSlots(mItem))
        StoreNewItemInBestSlots(sItem);

    ItemPrototype const *bIInSlotAmmo[3];
    for (uint8 i = 0; i < 3; ++i)
        bIInSlotAmmo[i] = NULL;

    for (uint32 id = 0; id < sItemStorage.MaxEntry; id++)
    {
        ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(id);
        if(!pProto)
            continue;

        if(!IsForMyClass(pProto) || !IsNotAllowedItem(pProto))
            continue;

        switch( pProto->InventoryType )
        {
            case INVTYPE_AMMO:
            {
                if (CanUseAmmo(id)!=EQUIP_ERR_OK)
                    break;
                if(!bIInSlotAmmo[pProto->SubClass - 1])
                    bIInSlotAmmo[pProto->SubClass - 1] = pProto;
                else
                    bIInSlotAmmo[pProto->SubClass - 1] = BestItemBetween(bIInSlotAmmo[pProto->SubClass - 1], pProto, true);
                break;
            }
            case INVTYPE_THROWN:
            {
                uint16 eDest;
                if (CanEquipNewItem(NULL_SLOT, eDest, id, false)!=EQUIP_ERR_OK)
                    break;
                if(!bIInSlotAmmo[0])
                    bIInSlotAmmo[0] = pProto;
                else
                    bIInSlotAmmo[0] = BestItemBetween(bIInSlotAmmo[0], pProto, true);
                break;
            }
            default:
                break;
        }
    }
    Item* it = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
    if (it)
    {
        if (bIInSlotAmmo[1] && it->GetProto()->AmmoType == bIInSlotAmmo[1]->SubClass)
        {
            StoreNewItemInBestSlots(bIInSlotAmmo[1]->ItemId, 200);
            SetAmmo(bIInSlotAmmo[1]->ItemId);
        }
        else if (bIInSlotAmmo[2] && it->GetProto()->AmmoType == bIInSlotAmmo[2]->SubClass)
        {
            StoreNewItemInBestSlots(bIInSlotAmmo[2]->ItemId, 200);
            SetAmmo(bIInSlotAmmo[2]->ItemId);
        }
    }
    else if (bIInSlotAmmo[0])
    {
        StoreNewItemInBestSlots(bIInSlotAmmo[0]);
        Item* i = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
        if (i && i->GetMaxStackCount()!=1) i->SetCount(200);
    }

    StoreNewItemInBestSlots(bIInSlot[INVTYPE_FINGER]);
    if (!StoreNewItemInBestSlots(bIInSlot[INVTYPE_FINGER]))
        StoreNewItemInBestSlots(bIInSlot[INVTYPE_FINGER2]);

    StoreNewItemInBestSlots(bIInSlot[INVTYPE_TRINKET]);
    if (!StoreNewItemInBestSlots(bIInSlot[INVTYPE_TRINKET]))
        StoreNewItemInBestSlots(bIInSlot[INVTYPE_TRINKET2]);

    bIFromSet = NULL;
}

bool Player::OtherItemsInSetAreAllowedForMe(ItemPrototype const* pProto)
{
    ItemSetMap const & mItemSet = sObjectMgr.GetItemSetMap();
    for (ItemSetMap::const_iterator itr = mItemSet.begin(); itr != mItemSet.end(); ++itr)
    {
        if (itr->second.ItemSet != pProto->ItemSet)
            continue;

        if (itr->second.ItemLevel > 155 && itr->second.ItemLevel != pProto->ItemLevel)
            continue;

        uint16 eDest;
        if (CanEquipNewItem(NULL_SLOT, eDest, itr->second.ItemId, false)!=EQUIP_ERR_OK)
            return false;

        ItemPrototype const *oProto = sObjectMgr.GetItemPrototype(itr->second.ItemId);
        if (!oProto)
            continue;

        if(!IsForMyClass(oProto) || !IsNotAllowedItem(oProto))
            return false;
    }
    return true;
}

ItemPrototype const* Player::CheckItemSet(ItemPrototype const* bIInSlot, ItemPrototype const* bIFromSet)
{
    if (!bIInSlot)
    {
        if (bIFromSet && bIFromSet->ItemSet > 0)
            return bIFromSet;
        else
            return NULL;
    }

    if (!bIFromSet)
    {
        if (bIInSlot && bIInSlot->ItemSet > 0)
            return bIInSlot;
        else
            return NULL;
    }

    if (bIFromSet->ItemSet == 0 && bIInSlot->ItemSet == 0)
        return NULL;
    else if (bIFromSet->ItemSet > 0 && bIInSlot->ItemSet == 0)
        return bIFromSet;
    else if (bIFromSet->ItemSet == 0 && bIInSlot->ItemSet > 0)
        return bIInSlot;
    else
    {
        if (bIInSlot->Quality > bIFromSet->Quality)
            return bIInSlot;
        else if (bIInSlot->Quality < bIFromSet->Quality)
            return bIFromSet;
        else if (bIInSlot->ItemLevel > bIFromSet->ItemLevel)
            return bIInSlot;
        else
            return bIFromSet;
    }
    return NULL;
}

bool Player::StoreNewItemInBestSlots(ItemPrototype const* bIInSlot)
{
    if (!bIInSlot)
        return false;

    uint16 eDest;
    uint8 msg = CanEquipNewItem( NULL_SLOT, eDest, bIInSlot->ItemId, false );
    if( msg != EQUIP_ERR_OK )
        return false;

    EquipNewItem( eDest, bIInSlot->ItemId, true);
    AutoUnequipOffhandIfNeed();

    return true;
}

void Player::RemoveMyEquipement(bool destroy)
{
    if (GetUInt32Value(PLAYER_AMMO_ID))
    {
        uint32 ammo = GetUInt32Value(PLAYER_AMMO_ID);
        RemoveAmmo();
        if (destroy)
        {
            Item* i = GetItemByEntry(ammo);
            if (i)
            {
                uint32 count = i->GetCount();
                DestroyItemCount(i, count, true);
            }
        }
    }

    for (int slot=EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
    {
        if (slot==EQUIPMENT_SLOT_BODY || slot==EQUIPMENT_SLOT_TABARD)
            continue;

        Item* pItem = GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        if (destroy && pItem->GetProto()->InventoryType == INVTYPE_THROWN)
        {
            if(pItem->GetMaxStackCount()==1)
            {
                uint32 count = 1;
                DestroyItemCount( pItem, count, true);
            }
            else
            {
                uint32 count = pItem->GetCount();
                DestroyItemCount( pItem, count, true);
            }
            continue;
        }

        ItemPosCountVec sDest;
        if(CanStoreItem( NULL_BAG, NULL_SLOT, sDest, pItem, false )!=EQUIP_ERR_OK)
            continue;

        RemoveItem(INVENTORY_SLOT_BAG_0, slot,true);
        pItem = StoreItem( sDest, pItem, true);

        if (destroy && pItem)
            DestroyItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
    }
}

bool Player::IsForMyClass(ItemPrototype const* pProto)
{
    uint16 role = getRole();
    switch (pProto->Class)
    {
        case ITEM_CLASS_WEAPON:
            switch (role)
            {
                case MageFire:
                case MageArcane:
                case MageFrost:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_SWORD:    return HasSpell(201);
                        case ITEM_SUBCLASS_WEAPON_STAFF:    return HasSpell(227);
                        case ITEM_SUBCLASS_WEAPON_DAGGER:   return HasSpell(1180);
                        case ITEM_SUBCLASS_WEAPON_WAND:     return HasSpell(5009);
                        default: return false;
                    }
                    break;
                case WarriorArms:
                case WarriorProtection:
                case WarriorFury:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_AXE:      return HasSpell(196);
                        case ITEM_SUBCLASS_WEAPON_AXE2:     return HasSpell(197);
                        case ITEM_SUBCLASS_WEAPON_BOW:      return HasSpell(264);
                        case ITEM_SUBCLASS_WEAPON_GUN:      return HasSpell(266);
                        case ITEM_SUBCLASS_WEAPON_MACE:     return HasSpell(198);
                        case ITEM_SUBCLASS_WEAPON_MACE2:    return HasSpell(199);
                        case ITEM_SUBCLASS_WEAPON_SWORD:    return HasSpell(201);
                        case ITEM_SUBCLASS_WEAPON_SWORD2:   return HasSpell(202);
                        case ITEM_SUBCLASS_WEAPON_CROSSBOW: return HasSpell(5011);
                        default: return false;
                    }
                    break;
                case RogueCombat:
                case RogueAssassination:
                case RogueSubtlety:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_BOW:      return HasSpell(264);
                        case ITEM_SUBCLASS_WEAPON_GUN:      return HasSpell(266);
                        case ITEM_SUBCLASS_WEAPON_CROSSBOW: return HasSpell(5011);
                        case ITEM_SUBCLASS_WEAPON_DAGGER:   return HasSpell(1180);
                        case ITEM_SUBCLASS_WEAPON_THROWN:   return HasSpell(2567);
                        default: return false;
                    }
                    break;
                case PriestDiscipline:
                case PriestHoly:
                case PriestShadow:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_MACE:     return HasSpell(198);
                        case ITEM_SUBCLASS_WEAPON_WAND:     return HasSpell(5009);
                        default: return false;
                    }
                    break;
                case ShamanElementalCombat:
                case ShamanRestoration:
                case ShamanEnhancement:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_MACE:     return HasSpell(198);
                        default: return false;
                    }
                    break;
                case DruidFeralCombat:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_MACE2:    return HasSpell(199);
                        case ITEM_SUBCLASS_WEAPON_STAFF:    return HasSpell(227);
                        default: return false;
                    }
                    break;
                case DruidRestoration:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_MACE:     return HasSpell(198);
                        default: return false;
                    }
                    break;
                case DruidBalance:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_STAFF:    return HasSpell(227);
                        default: return false;
                    }
                    break;
                case WarlockDestruction:
                case WarlockCurses:
                case WarlockSummoning:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_SWORD:    return HasSpell(201);
                        case ITEM_SUBCLASS_WEAPON_STAFF:    return HasSpell(227);
                        case ITEM_SUBCLASS_WEAPON_DAGGER:   return HasSpell(1180);
                        case ITEM_SUBCLASS_WEAPON_WAND:     return HasSpell(5009);
                        default: return false;
                    }
                    break;
                case HunterBeastMastery:
                case HunterSurvival:
                case HunterMarksmanship:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_AXE:      return HasSpell(196);
                        case ITEM_SUBCLASS_WEAPON_AXE2:     return HasSpell(197);
                        case ITEM_SUBCLASS_WEAPON_BOW:      return HasSpell(264);
                        case ITEM_SUBCLASS_WEAPON_GUN:      return HasSpell(266);
                        case ITEM_SUBCLASS_WEAPON_SWORD:    return HasSpell(201);
                        case ITEM_SUBCLASS_WEAPON_SWORD2:   return HasSpell(202);
                        case ITEM_SUBCLASS_WEAPON_CROSSBOW: return HasSpell(5011);
                        default: return false;
                    }
                    break;
                case PaladinCombat:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_MACE:     return HasSpell(198);
                        case ITEM_SUBCLASS_WEAPON_MACE2:    return HasSpell(199);
                        case ITEM_SUBCLASS_WEAPON_SWORD:    return HasSpell(201);
                        case ITEM_SUBCLASS_WEAPON_SWORD2:   return HasSpell(202);
                        default: return false;
                    }
                    break;
                case PaladinHoly:
                case PaladinProtection:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_MACE:     return HasSpell(198);
                        case ITEM_SUBCLASS_WEAPON_SWORD:    return HasSpell(201);
                        default: return false;
                    }
                    break;
                case DeathKnightBlood:
                case DeathKnightFrost:
                case DeathKnightUnholy:
                    switch (pProto->SubClass)
                    {
                        case ITEM_SUBCLASS_WEAPON_AXE:      return HasSpell(196);
                        case ITEM_SUBCLASS_WEAPON_AXE2:     return HasSpell(197);
                        case ITEM_SUBCLASS_WEAPON_SWORD:    return HasSpell(201);
                        case ITEM_SUBCLASS_WEAPON_SWORD2:   return HasSpell(202);
                        default: return false;
                    }
                    break;
            }
        case ITEM_CLASS_ARMOR:
            switch(pProto->SubClass)
            {
                case ITEM_SUBCLASS_ARMOR_CLOTH:
                {
                    if (!HasSpell(9078))
                        return false;

                    switch(getClass())
                    {
                        case CLASS_PRIEST:
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                            return true;
                        default:
                            return false;
                    }
                }
                case ITEM_SUBCLASS_ARMOR_LEATHER:
                {
                    if (!HasSpell(9077))
                        return false;

                    switch(getClass())
                    {

                        case CLASS_HUNTER:
                        case CLASS_DRUID:
                        case CLASS_ROGUE:
                            return true;
                        case CLASS_SHAMAN:
                            if (getLevel()<40)
                                return true;
                            else
                                return false;
                        default:
                            return false;
                    }
                }
                case ITEM_SUBCLASS_ARMOR_MAIL:
                {
                    if (!HasSpell(8737))
                        return false;

                    switch(getClass())
                    {
                        case CLASS_WARRIOR:
                        case CLASS_SHAMAN:
                        case CLASS_HUNTER:
                            return true;
                        case CLASS_PALADIN:
                            if (getLevel()<40)
                                return true;
                            else
                                return false;
                        default:
                            return false;
                    }
                }
                case ITEM_SUBCLASS_ARMOR_PLATE:
                {
                    if (!HasSpell(750))
                        return false;

                    switch(getClass())
                    {
                        case CLASS_WARRIOR:
                        case CLASS_PALADIN:
                        case CLASS_DEATH_KNIGHT:
                            return true;
                        default:
                            return false;
                    }
                }
                case ITEM_SUBCLASS_ARMOR_SHIELD:
                    return HasSpell(9116);
                default:
                    return true;
            }
    }
    return true;
}

bool Player::IsNotAllowedItem(ItemPrototype const* pProto)
{
    if((pProto->RequiredLevel == 0) && (pProto->ItemLevel > 1))
        return false;

    if ((pProto->MaxCount > 0) && (uint32(pProto->MaxCount) <= GetItemCount(pProto->ItemId)))
        return false;

    if(pProto->Bonding == BIND_QUEST_ITEM)
        return false;

    switch (pProto->InventoryType)
    {
        case INVTYPE_SHOULDERS:
        case INVTYPE_FINGER:
            if (getLevel() < 20)
                return false;
            return true;
        case INVTYPE_HEAD:
        case INVTYPE_TRINKET:
        case INVTYPE_NECK:
            if (getLevel() < 30)
                return false;
            return true;
        default:
            break;
    }

    if((pProto->Quality == ITEM_QUALITY_UNCOMMON) && (getLevel() < 15))
        return false;

    if((pProto->Quality == ITEM_QUALITY_RARE) && (getLevel() < 25))
        return false;

    if((pProto->Quality == ITEM_QUALITY_EPIC) && (getLevel() < 35))
        return false;

    if(pProto->Quality == ITEM_QUALITY_LEGENDARY)
        return false;

    if(pProto->Quality == ITEM_QUALITY_ARTIFACT)
        return false;

    if(pProto->Quality == ITEM_QUALITY_HEIRLOOM)
        return false;

    return true;
}

ItemPrototype const* Player::CompareItem(ItemPrototype const* pProto1, ItemPrototype const* pProto2, ItemModType pType)
{
    for (uint8 i = 0; i < pProto1->StatsCount; i++)
    {
        if (pProto1->ItemStat[i].ItemStatType != pType)
            continue;

        for (uint8 j = 0; j < pProto2->StatsCount; j++)
        {
            if (pProto2->ItemStat[j].ItemStatType != pType)
                continue;

            if (pProto1->ItemStat[i].ItemStatValue == pProto2->ItemStat[j].ItemStatValue)
                return NULL;

            if (pProto1->ItemStat[i].ItemStatValue > pProto2->ItemStat[j].ItemStatValue)
                return pProto1;
            else
                return pProto2;
        }

        return pProto1;
    }

    for (uint8 j = 0; j < pProto2->StatsCount; j++)
    {
        if (pProto2->ItemStat[j].ItemStatType != pType)
           continue;
        else
            return pProto2;
    }

    return NULL;
}

ItemPrototype const* Player::BestItemBetween(ItemPrototype const* pProto1, ItemPrototype const* pProto2, bool DPS)
{
    if (!pProto1 && !pProto2)
        return NULL;
    if (!pProto1)
        return pProto2;
    if (!pProto2)
        return pProto1;

    if (bIFromSet)
    {
        if (bIFromSet->ItemSet == pProto1->ItemSet)
            return pProto1;

        if (bIFromSet->ItemSet == pProto2->ItemSet)
            return pProto2;
    }

    uint16 role = getRole();
    ItemPrototype const *pProto = NULL;

    switch (role)
    {
        case MageArcane:
        case MageFrost:
        case MageFire:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_INTELLECT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPELL_POWER);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_SPELL_RATING);
            if(pProto)
                return pProto;
            break;
        case WarriorArms:
        case WarriorFury:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STRENGTH);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_MELEE_RATING);
            if(pProto)
                return pProto;
            break;
        case WarriorProtection:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STAMINA);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_DEFENSE_SKILL_RATING);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_BLOCK_RATING);
            if(pProto)
                return pProto;
            break;
        case RogueCombat:
        case RogueAssassination:
        case RogueSubtlety:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_AGILITY);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_MELEE_RATING);
            if(pProto)
                return pProto;
            break;
        case PriestDiscipline:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_INTELLECT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPELL_POWER);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_SPELL_RATING);
            if(pProto)
                return pProto;
            break;
        case PriestShadow:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_INTELLECT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPELL_POWER);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_HIT_SPELL_RATING);
            if(pProto)
                return pProto;
            break;
        case PriestHoly:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPIRIT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_INTELLECT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPELL_POWER);
            if(pProto)
                return pProto;
            break;
        case ShamanElementalCombat:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_INTELLECT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPELL_POWER);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_SPELL_RATING);
            if(pProto)
                return pProto;
            break;
        case ShamanRestoration:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_INTELLECT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPELL_POWER);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPIRIT);
            if(pProto)
                return pProto;
            break;
        case ShamanEnhancement:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STRENGTH);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_MELEE_RATING);
            if(pProto)
                return pProto;
            break;
        case DruidFeralCombat:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STAMINA);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STRENGTH);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_AGILITY);
            if(pProto)
                return pProto;
            break;
        case DruidRestoration:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_INTELLECT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPIRIT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPELL_POWER);
            if(pProto)
                return pProto;
            break;
        case DruidBalance:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_INTELLECT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPELL_POWER);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_SPELL_RATING);
            if(pProto)
                return pProto;
            break;
        case WarlockDestruction:
        case WarlockCurses:
        case WarlockSummoning:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STAMINA);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPELL_POWER);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_SPELL_RATING);
            if(pProto)
                return pProto;
            break;
        case HunterBeastMastery:
        case HunterMarksmanship:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_AGILITY);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STAMINA);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_RANGED_RATING);
            if(pProto)
                return pProto;
            break;
        case HunterSurvival:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STAMINA);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_AGILITY);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_RANGED_RATING);
            if(pProto)
                return pProto;
            break;
        case PaladinCombat:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STRENGTH);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_MELEE_RATING);
            if(pProto)
                return pProto;
            break;
        case PaladinHoly:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_INTELLECT);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_SPELL_POWER);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_SPELL_RATING);
            if(pProto)
                return pProto;
            break;
        case PaladinProtection:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STAMINA);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_DEFENSE_SKILL_RATING);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_BLOCK_RATING);
            if(pProto)
                return pProto;
            break;
        case DeathKnightBlood:
        case DeathKnightFrost:
        case DeathKnightUnholy:
            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STAMINA);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_STRENGTH);
            if(pProto)
                return pProto;

            pProto = CompareItem(pProto1, pProto2, ITEM_MOD_CRIT_MELEE_RATING);
            if(pProto)
                return pProto;
            break;
    }

    if (DPS)
    {
        if (pProto1->getDPS() > pProto2->getDPS())
            return pProto1;
        else
            return pProto2;
    }
    else
    {
        if (pProto1->Armor > pProto2->Armor)
            return pProto1;
        else
            return pProto2;
    }
}

void Player::setRole(uint16 role)
{
    PlayerbotAI* ai = GetPlayerbotAI();
    if(!ai)
        return;

    ai->setRole(role);
}

uint16 Player::getRole()
{
    PlayerbotAI* ai = GetPlayerbotAI();
    if(!ai)
    {
        switch(getClass())
        {
            case CLASS_WARRIOR:
                return WarriorProtection;
            case CLASS_PALADIN:
                return PaladinHoly;
            case CLASS_HUNTER:
                return HunterBeastMastery;
            case CLASS_ROGUE:
                return RogueSubtlety;
            case CLASS_PRIEST:
                return PriestHoly;
            case CLASS_DEATH_KNIGHT:
                return DeathKnightUnholy;
            case CLASS_SHAMAN:
                return ShamanElementalCombat;
            case CLASS_MAGE:
                return MageFrost;
            case CLASS_WARLOCK:
                return WarlockDestruction;
            case CLASS_DRUID:
                return DruidFeralCombat;
        }
    }
    else
    {
        switch(getClass())
        {
            case CLASS_WARRIOR:
                return ai->getRole();
            case CLASS_PALADIN:
                return ai->getRole();
            case CLASS_HUNTER:
                return ai->getRole();
            case CLASS_ROGUE:
                return ai->getRole();
            case CLASS_PRIEST:
                return ai->getRole();
            case CLASS_DEATH_KNIGHT:
                return ai->getRole();
            case CLASS_SHAMAN:
                return ai->getRole();
            case CLASS_MAGE:
                return ai->getRole();
            case CLASS_WARLOCK:
                return ai->getRole();
            case CLASS_DRUID:
                return ai->getRole();
        }
    }
    return 0;
}

bool Player::isWtfSpell(uint32 spell_id)
{
    switch(getClass())
    {
        case CLASS_WARRIOR:
        {
            uint32 blackListedSpells[] = {25266, 20559, 20560, 7400, 7402, 25225, 7405, 8380,
                11596, 11597, 23892, 23893, 23894, 30335, 25251, 6554, 6552, 25212, 1672, 1671,
                29704, 7887, 11584, 11585, 20616, 20617, 25272, 25275, 7373, 7372};
            uint32 max = sizeof(blackListedSpells)/sizeof(uint32);
            for(uint32 i = 0; i < max; ++i)
                if (spell_id == blackListedSpells[i])
                    return true;
            break;
        }
        case CLASS_PALADIN:
        {
            uint32 blackListedSpells[] = {51640, 53720, 2567, 43940};
            uint32 max = sizeof(blackListedSpells)/sizeof(uint32);
            for(uint32 i = 0; i < max; ++i)
                if (spell_id == blackListedSpells[i])
                    return true;
            break;
        }
        case CLASS_HUNTER:
        {
            uint32 blackListedSpells[] = {23034, 23035, 23539, 23538, 31333, 23356, 23357};
            uint32 max = sizeof(blackListedSpells)/sizeof(uint32);
            for(uint32 i = 0; i < max; ++i)
                if (spell_id == blackListedSpells[i])
                    return true;
            break;
        }
        case CLASS_ROGUE:
        {
            uint32 blackListedSpells[] = {1785, 1786, 1787, 9991, 8649, 8650, 11197, 11198,
                11279, 48669, 23726, 26866};
            uint32 max = sizeof(blackListedSpells)/sizeof(uint32);
            for(uint32 i = 0; i < max; ++i)
                if (spell_id == blackListedSpells[i])
                    return true;
            break;
        }
        case CLASS_PRIEST:
        {
            uint32 blackListedSpells[] = {16053, 35460, 39219, 38915, 38482, 36314, 34630, 2567,
                43939};
            uint32 max = sizeof(blackListedSpells)/sizeof(uint32);
            for(uint32 i = 0; i < max; ++i)
                if (spell_id == blackListedSpells[i])
                    return true;
            break;
        }
        case CLASS_DEATH_KNIGHT:
        {
            uint32 blackListedSpells[] = {49916, 49913, 49914, 49915, 51428, 51429, 51426, 51427, 2567};
            uint32 max = sizeof(blackListedSpells)/sizeof(uint32);
            for(uint32 i = 0; i < max; ++i)
                if (spell_id == blackListedSpells[i])
                    return true;
            break;
        }
        case CLASS_SHAMAN:
        {
            uint32 blackListedSpells[] = {2567};
            uint32 max = sizeof(blackListedSpells)/sizeof(uint32);
            for(uint32 i = 0; i < max; ++i)
                if (spell_id == blackListedSpells[i])
                    return true;
            break;
        }
        case CLASS_MAGE:
        {
            uint32 blackListedSpells[] = {26373, 29443, 57680, 73324, 10052, 10057, 10058, 27103, 42987,
                12510, 41234, 2567, 47700, 26448};
            uint32 max = sizeof(blackListedSpells)/sizeof(uint32);
            for(uint32 i = 0; i < max; ++i)
                if (spell_id == blackListedSpells[i])
                    return true;
            break;
        }
        case CLASS_WARLOCK:
        {
            uint32 blackListedSpells[] = {265, 5720, 5723, 6263, 6262, 11732, 23468, 23469, 23470, 23471, 23472,
                23473, 23474, 23475, 23476, 23477, 27235, 27236, 27237, 47874, 47873, 47872, 47875, 47876, 47877,
                55153, 55152, 55154, 2567};
            uint32 max = sizeof(blackListedSpells)/sizeof(uint32);
            for(uint32 i = 0; i < max; ++i)
                if (spell_id == blackListedSpells[i])
                    return true;
            break;
        }
        case CLASS_DRUID:
        {
            uint32 blackListedSpells[] = {21163, 43816, 6783, 9913, 2567};
            uint32 max = sizeof(blackListedSpells)/sizeof(uint32);
            for(uint32 i = 0; i < max; ++i)
                if (spell_id == blackListedSpells[i])
                    return true;
            break;
        }
    }
    return false;
}

void Player::choseMount(uint32 *Mount, uint32 max)
{
    bool learn = true;
    for (uint32 i = 0; i < max; ++i)
    {
        ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(Mount[i]);
        if (!pProto)
            continue;

        if (HasSpell(pProto->Spells[1].SpellId))
        {
            learn = false;
            break;
        }
    }

    if (learn)
    {
        ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(Mount[urand(0, max-1)]);
        if (!pProto)
            return;
        learnSpell(pProto->Spells[1].SpellId, false);
    }
}

void Player::FlyingMountsSpellsToItems()
{
    for (PlayerSpellMap::const_iterator itr = m_spells.begin(); itr != m_spells.end(); ++itr)
    {
        SpellEntry const *sEntry = sSpellStore.LookupEntry(itr->first);
        if(!sEntry)
            continue;

        if(! (isFlyingSpell(sEntry) || isFlyingFormSpell(sEntry)) )
            continue;

        uint32 itemId = 0;
        for (uint32 id = 0; id < sItemStorage.MaxEntry; id++)
        {
            ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(id);
            if(!pProto)
                continue;

            for(int i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
            {
                if(pProto->Spells[i].SpellId == itr->first)
                {
                    itemId = id;
                    break;
                }
            }
        }
        if(!HasItemCount(itemId, 1, false))
        {
            //Adding items
            uint32 noSpaceForCount = 0;

            // check space and find places
            ItemPosCountVec dest;
            uint8 msg = CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, itemId, 1, &noSpaceForCount );

            if(!dest.empty())                         // can't add any
            {
                Item* item = StoreNewItem( dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));
                SendNewItem(item, 1,false,false);
            }
        }

    }

    for(int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        Item *pItem = GetItemByPos( INVENTORY_SLOT_BAG_0, i );
        if(!pItem)
            continue;

        ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(pItem->GetEntry());
        if(!pProto)
            continue;

        for(int i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
        {
            SpellEntry const *sEntry = sSpellStore.LookupEntry(pProto->Spells[i].SpellId);
            if(!sEntry)
                continue;

            if(! (isFlyingSpell(sEntry) || isFlyingFormSpell(sEntry)) )
                continue;

            if(HasSpell(pProto->Spells[i].SpellId))
            {
                uint16 RindingSkill = GetSkillValue(SKILL_RIDING);
                removeSpell(pProto->Spells[i].SpellId, false, false);
                SetSkill(SKILL_RIDING, RindingSkill, 300, 300);
                break;
            }

        }
    }

    for(int i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
    {
        if(Bag* pBag = (Bag*)GetItemByPos( INVENTORY_SLOT_BAG_0, i ))
        {
            for(uint32 j = 0; j < pBag->GetBagSize(); ++j)
            {
                Item* pItem = GetItemByPos( i, j );
                if(!pItem)
                    continue;

                ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(pItem->GetEntry());
                if(!pProto)
                    continue;

                for(int i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
                {
                    SpellEntry const *sEntry = sSpellStore.LookupEntry(pProto->Spells[i].SpellId);
                    if(!sEntry)
                        continue;

                    if(! (isFlyingSpell(sEntry) || isFlyingFormSpell(sEntry)) )
                        continue;

                    if(HasSpell(pProto->Spells[i].SpellId))
                    {
                        uint16 RindingSkill = GetSkillValue(SKILL_RIDING);
                        removeSpell(pProto->Spells[i].SpellId, false, false);
                        SetSkill(SKILL_RIDING, RindingSkill, 300, 300);
                        break;
                    }
                }
            }
        }
    }
}

bool Player::CanUseFlyingMounts(SpellEntry const* sEntry)
{
    if(!GetFlyingMountTimer())
        return false;

    uint32 v_map = GetVirtualMapForMapAndZone(GetMapId(), GetZoneId());
    MapEntry const* mapEntry = sMapStore.LookupEntry(v_map);
    if(!getAttackers().empty())
    {
        WorldPacket data(SMSG_CAST_FAILED, (4+1+1));
        data << uint8(0);
        data << uint32(sEntry->Id);
        data << uint8(SPELL_FAILED_TARGET_IN_COMBAT);
        GetSession()->SendPacket(&data);
        return false;
    }
    if( (!mapEntry)/* || (mapEntry->Instanceable())*/ || (mapEntry->IsDungeon()) ||
        (mapEntry->IsRaid()) || (mapEntry->IsBattleArena()) || (mapEntry->IsBattleGround()) )
    {
        WorldPacket data(SMSG_CAST_FAILED, (4+1+1));
        data << uint8(0);
        data << uint32(sEntry->Id);
        data << uint8(SPELL_FAILED_NOT_HERE);
        GetSession()->SendPacket(&data);
        return false;
    }
    return true;
}