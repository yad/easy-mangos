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
#include "Chat.h"
#include "ArenaTeam.h"
#include "SpellMgr.h"
#include "ObjectMgr.h"
#include "playerbot/PlayerbotAI.h"

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

        if(!IsForMyClass(pProto))
            continue;

        if (!IsNotAllowedItem(pProto))
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

    ItemPrototype const *bAmmoBag[2];
    for (uint8 i = 0; i < 2; ++i)
        bAmmoBag[i] = NULL;

    ItemPrototype const *bOtherBag[4];
    for (uint8 i = 0; i < 4; ++i)
        bOtherBag[i] = NULL;

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
            case INVTYPE_BAG:
            {
                if (pProto->Class == ITEM_CLASS_QUIVER)
                {
                    uint16 eDest;
                    if (CanEquipNewItem(NULL_SLOT, eDest, id, false)!=EQUIP_ERR_OK)
                        break;
                    if(!bAmmoBag[pProto->SubClass - 2])
                        bAmmoBag[pProto->SubClass - 2] = pProto;
                    else
                        bAmmoBag[pProto->SubClass - 2] = BestItemBetween(bAmmoBag[pProto->SubClass - 2], pProto, false /*use max slot if isBag()*/);
                }
                else if (pProto->SubClass == 0)
                {
                    if (id == 23162) //Foror's Crate of Endless Resist Gear Storage - 36
                        continue;

                    uint16 eDest;
                    if (CanEquipNewItem(NULL_SLOT, eDest, id, false)!=EQUIP_ERR_OK)
                        break;

                    for (uint8 i = 0; i < 4; ++i)
                    {
                        const ItemPrototype *ip = BestItemBetween(bOtherBag[i], pProto, false /*use max slot if isBag()*/);
                        if (ip == pProto && ip->MaxCount > 0)
                        {
                            for (uint8 j = 3; j > i; --j)
                                bOtherBag[j] = bOtherBag[j-1];
                            bOtherBag[i] = ip;
                            break;
                        }
                        else
                        {
                            bOtherBag[i] = ip;
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    Item* it = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
    if (it)
    {
        bOtherBag[3] = NULL;

        if (bIInSlotAmmo[1] && it->GetProto()->AmmoType == bIInSlotAmmo[1]->SubClass)
        {
            StoreNewItemInBestSlots(bAmmoBag[0]);
            Item* bag = GetItemByEntry(bAmmoBag[0]->ItemId);
            if (bag && bag->IsBag())
            {
                uint8 msg = EQUIP_ERR_OK;
                do
                {
                    ItemPosCountVec sDest;
                    msg = CanStoreNewItem( NULL_BAG, NULL_SLOT, sDest, bIInSlotAmmo[1]->ItemId, bIInSlotAmmo[1]->Stackable );
                    if( msg == EQUIP_ERR_OK )
                        StoreNewItem( sDest, bIInSlotAmmo[1]->ItemId, true, Item::GenerateItemRandomPropertyId(bIInSlotAmmo[1]->ItemId) );
                }while(msg == EQUIP_ERR_OK && ((Bag*)bag)->GetFreeSlots()>0);
            }
            else
            {
                StoreNewItemInBestSlots(bIInSlotAmmo[1]->ItemId, bIInSlotAmmo[1]->Stackable);
            }
            SetAmmo(bIInSlotAmmo[1]->ItemId);
        }
        else if (bIInSlotAmmo[2] && it->GetProto()->AmmoType == bIInSlotAmmo[2]->SubClass)
        {
            StoreNewItemInBestSlots(bAmmoBag[1]);
            Item* bag = GetItemByEntry(bAmmoBag[1]->ItemId);
            if (bag && bag->IsBag())
            {
                uint8 msg = EQUIP_ERR_OK;
                do
                {
                    ItemPosCountVec sDest;
                    msg = CanStoreNewItem( NULL_BAG, NULL_SLOT, sDest, bIInSlotAmmo[2]->ItemId, bIInSlotAmmo[2]->Stackable );
                    if( msg == EQUIP_ERR_OK )
                        StoreNewItem( sDest, bIInSlotAmmo[2]->ItemId, true, Item::GenerateItemRandomPropertyId(bIInSlotAmmo[2]->ItemId) );
                }while(msg == EQUIP_ERR_OK && ((Bag*)bag)->GetFreeSlots()>0);
            }
            else
            {
                StoreNewItemInBestSlots(bIInSlotAmmo[2]->ItemId, bIInSlotAmmo[2]->Stackable);
            }
            SetAmmo(bIInSlotAmmo[2]->ItemId);
        }
    }
    else if (bIInSlotAmmo[0])
    {
        StoreNewItemInBestSlots(bIInSlotAmmo[0]);
        Item* i = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
        if (i && i->GetMaxStackCount()!=1) i->SetCount(bIInSlotAmmo[0]->Stackable);
    }

    for (uint8 i = 0; i < 4; ++i)
        StoreNewItemInBestSlots(bOtherBag[i]);

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

    if (IsBot())
    {
        for (int slot=INVENTORY_SLOT_BAG_START; slot < INVENTORY_SLOT_BAG_END; ++slot)
        {
            Item* pItem = GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            if (!pItem || !pItem->IsBag())
                continue;

            Bag* pBag = (Bag*)pItem;
            if (!pBag)
                continue;

            DestroyItem(INVENTORY_SLOT_BAG_0, pItem->GetSlot(), true);
        }
    }

    Item* pItem = NULL;
    for (int slot=INVENTORY_SLOT_BAG_START; slot < INVENTORY_SLOT_BAG_END; ++slot)
    {
        pItem = GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem || !pItem->IsBag())
            continue;

        Bag* pBag = (Bag*)pItem;
        if (!pBag)
            continue;

        if (pBag->GetProto()->Class == ITEM_CLASS_QUIVER)
            break;
    }

    if (!pItem)
        pItem = GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_SLOT_BAG_END-1);
    if (pItem)
        DestroyItem(INVENTORY_SLOT_BAG_0, pItem->GetSlot(), true);
}

bool Player::IsForMyClass(ItemPrototype const* pProto)
{
    if (pProto->InventoryType==INVTYPE_CLOAK)
        return true;

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
                        case CLASS_DRUID:
                        case CLASS_ROGUE:
                            return true;
                        case CLASS_HUNTER:
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
    if (pProto->InventoryType!=INVTYPE_BAG)
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

    if (pProto1->InventoryType == INVTYPE_BAG)
    {
        if (pProto1->ContainerSlots > pProto2->ContainerSlots)
            return pProto1;
        else
            return pProto2;
    }
    else if (DPS)
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

void Player::GMStartup()
{
    uint32 level = getLevel();

    if (IsBot())
    {
        RemoveAllAuras(AURA_REMOVE_BY_DELETE);
        resetSpells();
    }
    RemoveMyEquipement(true);

    LearnAllMyTalentsForMyLevel();
    LearnAllMySpellsForMyLevel();
    UpdateSkillsToMaxSkillsForLevel();

    switch (getRace())
    {
        case RACE_HUMAN:
            if (!GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
                StoreNewItemInBestSlots(41255, 1);
            break;
        case RACE_ORC:
            if (!GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
                StoreNewItemInBestSlots(42369, 1);
            break;
        case RACE_DWARF:
            if (!GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
                StoreNewItemInBestSlots(42371, 1);
            break;
        case RACE_NIGHTELF:
            if (!GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
                StoreNewItemInBestSlots(42372, 1);
            break;
        case RACE_UNDEAD:
            if (!GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
                StoreNewItemInBestSlots(14617, 1);
            break;
        case RACE_TAUREN:
            if (!GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
                StoreNewItemInBestSlots(6125, 1);
            break;
        case RACE_GNOME:
            if (!GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
                StoreNewItemInBestSlots(42373, 1);
            break;
        case RACE_TROLL:
            if (!GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
                StoreNewItemInBestSlots(18231, 1);
            break;
        case RACE_BLOODELF:
            if (!GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
                StoreNewItemInBestSlots(42370, 1);
            break;
        case RACE_DRAENEI:
            if (!GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
                StoreNewItemInBestSlots(10054, 1);
            break;
        default:
            break;
    }

    switch(getClass())
    {
        case CLASS_SHAMAN:
        {
            if(!HasItemCount(5175, 1, false))
                StoreNewItemInBestSlots(5175, 1);
            if(!HasItemCount(5176, 1, false))
                StoreNewItemInBestSlots(5176, 1);
            if(!HasItemCount(5177, 1, false))
                StoreNewItemInBestSlots(5177, 1);
            if(!HasItemCount(5178, 1, false))
                StoreNewItemInBestSlots(5178, 1);
            break;
        }
        default:
            break;
    }
    switch (getRole())
    {
        case MageFire:
        case MageArcane:
        case MageFrost:
            break;
        case WarriorArms:
        case WarriorProtection:
        case WarriorFury:
            break;
        case RogueCombat:
        case RogueAssassination:
        case RogueSubtlety:
            break;
        case PriestDiscipline:
        case PriestHoly:
        case PriestShadow:
            break;
        case ShamanElementalCombat:
            if (level >= 85)
            {
                if(!HasItemCount(64674, 1, false))
                    StoreNewItemInBestSlots(64674, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50463, 1, false))
                    StoreNewItemInBestSlots(50463, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(38367, 1, false))
                    StoreNewItemInBestSlots(38367, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(35104, 1, false))
                    StoreNewItemInBestSlots(35104, 1);
            }
            else if (level >= 60)
            {
                if(!HasItemCount(23005, 1, false))
                    StoreNewItemInBestSlots(23005, 1);
            }
            else if (level >= 57)
            {
                if(!HasItemCount(22345, 1, false))
                    StoreNewItemInBestSlots(22345, 1);
            }
            else if (level >= 51)
            {
                if(!HasItemCount(22395, 1, false))
                    StoreNewItemInBestSlots(22395, 1);
            }
            else if (level >= 40)
            {
                if(!HasItemCount(23200, 1, false))
                    StoreNewItemInBestSlots(23200, 1);
            }
            break;
        case ShamanRestoration:
            if (level >= 85)
            {
                if(!HasItemCount(64673, 1, false))
                    StoreNewItemInBestSlots(64673, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50464, 1, false))
                    StoreNewItemInBestSlots(50464, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(38368, 1, false))
                    StoreNewItemInBestSlots(38368, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(35106, 1, false))
                    StoreNewItemInBestSlots(35106, 1);
            }
            else if (level >= 60)
            {
                if(!HasItemCount(23005, 1, false))
                    StoreNewItemInBestSlots(23005, 1);
            }
            else if (level >= 57)
            {
                if(!HasItemCount(22345, 1, false))
                    StoreNewItemInBestSlots(22345, 1);
            }
            else if (level >= 51)
            {
                if(!HasItemCount(22395, 1, false))
                    StoreNewItemInBestSlots(22395, 1);
            }
            else if (level >= 40)
            {
                if(!HasItemCount(23200, 1, false))
                    StoreNewItemInBestSlots(23200, 1);
            }
            break;
        case ShamanEnhancement:
            if (level >= 85)
            {
                if(!HasItemCount(64672, 1, false))
                    StoreNewItemInBestSlots(64672, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50458, 1, false))
                    StoreNewItemInBestSlots(50458, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(38361, 1, false))
                    StoreNewItemInBestSlots(38361, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(35105, 1, false))
                    StoreNewItemInBestSlots(35105, 1);
            }
            else if (level >= 60)
            {
                if(!HasItemCount(23005, 1, false))
                    StoreNewItemInBestSlots(23005, 1);
            }
            else if (level >= 57)
            {
                if(!HasItemCount(22345, 1, false))
                    StoreNewItemInBestSlots(22345, 1);
            }
            else if (level >= 51)
            {
                if(!HasItemCount(22395, 1, false))
                    StoreNewItemInBestSlots(22395, 1);
            }
            else if (level >= 40)
            {
                if(!HasItemCount(23200, 1, false))
                    StoreNewItemInBestSlots(23200, 1);
            }
            break;
        case DruidFeralCombat:
            if (level >= 85)
            {
                if(!HasItemCount(64676, 1, false))
                    StoreNewItemInBestSlots(64676, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50456, 1, false))
                    StoreNewItemInBestSlots(50456, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(38365, 1, false))
                    StoreNewItemInBestSlots(38365, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(33509, 1, false))
                    StoreNewItemInBestSlots(33509, 1);
            }
            else if (level >= 60)
            {
                if(!HasItemCount(23004, 1, false))
                    StoreNewItemInBestSlots(23004, 1);
            }
            else if (level >= 57)
            {
                if(!HasItemCount(22398, 1, false))
                    StoreNewItemInBestSlots(22398, 1);
            }
            else if (level >= 48)
            {
                if(!HasItemCount(23198, 1, false))
                    StoreNewItemInBestSlots(23198, 1);
            }
            break;
        case DruidRestoration:
            if (level >= 85)
            {
                if(!HasItemCount(64673, 1, false))
                    StoreNewItemInBestSlots(64673, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50454, 1, false))
                    StoreNewItemInBestSlots(50454, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(38366, 1, false))
                    StoreNewItemInBestSlots(38366, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(33508, 1, false))
                    StoreNewItemInBestSlots(33508, 1);
            }
            else if (level >= 60)
            {
                if(!HasItemCount(23004, 1, false))
                    StoreNewItemInBestSlots(23004, 1);
            }
            else if (level >= 57)
            {
                if(!HasItemCount(22398, 1, false))
                    StoreNewItemInBestSlots(22398, 1);
            }
            else if (level >= 48)
            {
                if(!HasItemCount(23198, 1, false))
                    StoreNewItemInBestSlots(23198, 1);
            }
            break;
        case DruidBalance:
            if (level >= 85)
            {
                if(!HasItemCount(64672, 1, false))
                    StoreNewItemInBestSlots(64672, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50457, 1, false))
                    StoreNewItemInBestSlots(50457, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(38360, 1, false))
                    StoreNewItemInBestSlots(38360, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(33510, 1, false))
                    StoreNewItemInBestSlots(33510, 1);
            }
            else if (level >= 60)
            {
                if(!HasItemCount(23004, 1, false))
                    StoreNewItemInBestSlots(23004, 1);
            }
            else if (level >= 57)
            {
                if(!HasItemCount(22398, 1, false))
                    StoreNewItemInBestSlots(22398, 1);
            }
            else if (level >= 48)
            {
                if(!HasItemCount(23198, 1, false))
                    StoreNewItemInBestSlots(23198, 1);
            }
            break;
        case WarlockDestruction:
        case WarlockCurses:
        case WarlockSummoning:
            break;
        case HunterBeastMastery:
        case HunterSurvival:
        case HunterMarksmanship:
            break;
        case PaladinCombat:
            if (level >= 85)
            {
                if(!HasItemCount(64674, 1, false))
                    StoreNewItemInBestSlots(64674, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50455, 1, false))
                    StoreNewItemInBestSlots(50455, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(38362, 1, false))
                    StoreNewItemInBestSlots(38362, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(33503, 1, false))
                    StoreNewItemInBestSlots(33503, 1);
            }
            else if (level >= 60)
            {
                if(!HasItemCount(23006, 1, false))
                    StoreNewItemInBestSlots(23006, 1);
            }
            else if (level >= 57)
            {
                if(!HasItemCount(22401, 1, false))
                    StoreNewItemInBestSlots(22401, 1);
            }
            else if (level >= 51)
            {
                if(!HasItemCount(22400, 1, false))
                    StoreNewItemInBestSlots(22400, 1);
            }
            else if (level >= 40)
            {
                if(!HasItemCount(23201, 1, false))
                    StoreNewItemInBestSlots(23201, 1);
            }
            break;
        case PaladinHoly:
            if (level >= 85)
            {
                if(!HasItemCount(64673, 1, false))
                    StoreNewItemInBestSlots(64673, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50460, 1, false))
                    StoreNewItemInBestSlots(50460, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(38364, 1, false))
                    StoreNewItemInBestSlots(38364, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(33502, 1, false))
                    StoreNewItemInBestSlots(33502, 1);
            }
            else if (level >= 60)
            {
                if(!HasItemCount(23006, 1, false))
                    StoreNewItemInBestSlots(23006, 1);
            }
            else if (level >= 57)
            {
                if(!HasItemCount(22401, 1, false))
                    StoreNewItemInBestSlots(22401, 1);
            }
            else if (level >= 51)
            {
                if(!HasItemCount(22400, 1, false))
                    StoreNewItemInBestSlots(22400, 1);
            }
            else if (level >= 40)
            {
                if(!HasItemCount(23201, 1, false))
                    StoreNewItemInBestSlots(23201, 1);
            }
            break;
        case PaladinProtection:
            if (level >= 85)
            {
                if(!HasItemCount(64676, 1, false))
                    StoreNewItemInBestSlots(64676, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50461, 1, false))
                    StoreNewItemInBestSlots(50461, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(38363, 1, false))
                    StoreNewItemInBestSlots(38363, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(33504, 1, false))
                    StoreNewItemInBestSlots(33504, 1);
            }
            else if (level >= 60)
            {
                if(!HasItemCount(23006, 1, false))
                    StoreNewItemInBestSlots(23006, 1);
            }
            else if (level >= 57)
            {
                if(!HasItemCount(22401, 1, false))
                    StoreNewItemInBestSlots(22401, 1);
            }
            else if (level >= 51)
            {
                if(!HasItemCount(22400, 1, false))
                    StoreNewItemInBestSlots(22400, 1);
            }
            else if (level >= 40)
            {
                if(!HasItemCount(23201, 1, false))
                    StoreNewItemInBestSlots(23201, 1);
            }
            break;
        case DeathKnightBlood:
            if (level >= 85)
            {
                if(!HasItemCount(64672, 1, false))
                    StoreNewItemInBestSlots(64672, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50459, 1, false))
                    StoreNewItemInBestSlots(50459, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(40867, 1, false))
                    StoreNewItemInBestSlots(40867, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(39208, 1, false))
                    StoreNewItemInBestSlots(39208, 1);
            }
            else if (level >= 51)
            {
                if(!HasItemCount(66050, 1, false))
                    StoreNewItemInBestSlots(66050, 1);
            }
            break;
        case DeathKnightFrost:
            if (level >= 85)
            {
                if(!HasItemCount(64674, 1, false))
                    StoreNewItemInBestSlots(64674, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50462, 1, false))
                    StoreNewItemInBestSlots(50462, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(40822, 1, false))
                    StoreNewItemInBestSlots(40822, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(39208, 1, false))
                    StoreNewItemInBestSlots(39208, 1);
            }
            else if (level >= 51)
            {
                if(!HasItemCount(66051, 1, false))
                    StoreNewItemInBestSlots(66051, 1);
            }
            break;
        case DeathKnightUnholy:
            if (level >= 85)
            {
                if(!HasItemCount(64672, 1, false))
                    StoreNewItemInBestSlots(64672, 1);
            }
            else if (level >= 80)
            {
                if(!HasItemCount(50459, 1, false))
                    StoreNewItemInBestSlots(50459, 1);
            }
            else if (level >= 74)
            {
                if(!HasItemCount(40875, 1, false))
                    StoreNewItemInBestSlots(40875, 1);
            }
            else if (level >= 70)
            {
                if(!HasItemCount(39208, 1, false))
                    StoreNewItemInBestSlots(39208, 1);
            }
            else if (level >= 51)
            {
                if(!HasItemCount(66049, 1, false))
                    StoreNewItemInBestSlots(66049, 1);
            }
            break;
    }

    GiveMeBestItemForMyLevel();

    SetHealth(GetMaxHealth());
    SetPower(getPowerType(), GetMaxPower(getPowerType()));
}

bool Player::LearnAllMySpellsForMyLevel()
{
    learnDefaultSpells();

    ChrClassesEntry const* clsEntry = sChrClassesStore.LookupEntry(getClass());
    if(!clsEntry)
        return false;

    uint32 family = clsEntry->spellfamily;

    for (uint32 id = 0; id< sCreatureStorage.MaxEntry; ++id)
    {
        CreatureInfo const *cinfo = sObjectMgr.GetCreatureTemplate(id);
        if(!cinfo)
            continue;

        if((cinfo->npcflag & UNIT_NPC_FLAG_TRAINER) || (cinfo->npcflag & UNIT_NPC_FLAG_TRAINER_CLASS)
            || (cinfo->npcflag & UNIT_NPC_FLAG_TRAINER_PROFESSION))
        {
            TrainerSpellData const* cSpells = sObjectMgr.GetNpcTrainerSpells(cinfo->Entry);
            TrainerSpellData const* tSpells = sObjectMgr.GetNpcTrainerTemplateSpells(cinfo->trainerId);

            if (!cSpells && !tSpells)
                continue;

            if (cSpells)
            {
                for(TrainerSpellMap::const_iterator itr = cSpells->spellList.begin(); itr != cSpells->spellList.end(); ++itr)
                {
                    TrainerSpell const* tSpell = &itr->second;

                    SpellEntry const* spellInfo1 = sSpellStore.LookupEntry(tSpell->spell);
                    if (!spellInfo1)
                        continue;

                    SpellEntry const* spellInfo2 = sSpellStore.LookupEntry(tSpell->learnedSpell);
                    if (!spellInfo2)
                        continue;

                    if (GetTrainerSpellState(tSpell, true) != TRAINER_SPELL_GREEN)
                        continue;

                    if (sSpellMgr.IsProfessionSpell(tSpell->learnedSpell))
                        continue;

                    if (spellInfo1->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_WEAPON) {}
                    else if (spellInfo1->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_PROFICIENCY) {}
                    else if(spellInfo1->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL
                        && spellInfo1->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SKILL) {}
                    else if (spellInfo1->SpellFamilyName != family || spellInfo2->SpellFamilyName != family)
                        continue;

                    if(tSpell->IsCastable())
                    {
                        if (!isWtfSpell(tSpell->learnedSpell) && tSpell->reqLevel <= getLevel())
                            learnSpell(tSpell->learnedSpell, false, true);
                    }
                    else
                    {
                        if (!isWtfSpell(tSpell->spell) && tSpell->reqLevel <= getLevel())
                            learnSpell(tSpell->spell, false, true);
                    }
                }
            }

            if (tSpells)
            {
                for(TrainerSpellMap::const_iterator itr = tSpells->spellList.begin(); itr != tSpells->spellList.end(); ++itr)
                {
                    TrainerSpell const* tSpell = &itr->second;

                    SpellEntry const* spellInfo1 = sSpellStore.LookupEntry(tSpell->spell);
                    if (!spellInfo1)
                        continue;

                    SpellEntry const* spellInfo2 = sSpellStore.LookupEntry(tSpell->learnedSpell);
                    if (!spellInfo2)
                        continue;

                    if (GetTrainerSpellState(tSpell, true) != TRAINER_SPELL_GREEN)
                        continue;

                    if (sSpellMgr.IsProfessionSpell(tSpell->learnedSpell))
                        continue;

                    if (spellInfo1->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_WEAPON) {}
                    else if (spellInfo1->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_PROFICIENCY) {}
                    else if(spellInfo1->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL
                        && spellInfo1->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SKILL) {}
                    else if (spellInfo1->SpellFamilyName != family || spellInfo2->SpellFamilyName != family)
                        continue;

                    if(tSpell->IsCastable())
                    {
                        if (!isWtfSpell(tSpell->learnedSpell) && tSpell->reqLevel <= getLevel())
                            learnSpell(tSpell->learnedSpell, false, true);
                    }
                    else
                    {
                        if (!isWtfSpell(tSpell->spell) && tSpell->reqLevel <= getLevel())
                            learnSpell(tSpell->spell, false, true);
                    }
                }
            }
        }

        if(cinfo->npcflag & UNIT_NPC_FLAG_QUESTGIVER)
        {
            QuestRelationsMapBounds bounds = sObjectMgr.GetCreatureQuestRelationsMapBounds(id);
            for(QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
            {
                Quest const *pQuest = sObjectMgr.GetQuestTemplate(itr->second);

                if (!pQuest || pQuest->GetMinLevel() > getLevel())
                    continue;

                SpellEntry const* spellInfo1 = sSpellStore.LookupEntry(pQuest->GetSrcSpell());
                if (spellInfo1)
                {
                    if (IsSpellFitByClassAndRace(spellInfo1->Id) && !isWtfSpell(spellInfo1->Id))
                    {
                        if ( (spellInfo1->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL && spellInfo1->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SKILL)
                            || (spellInfo1->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_TRADE_SKILL) ) {}
                        else if (spellInfo1->SpellFamilyName == family)
                            learnSpell(spellInfo1->Id, false, true);
                        else if  ( ((cinfo->npcflag & UNIT_NPC_FLAG_TRAINER) || (cinfo->npcflag & UNIT_NPC_FLAG_TRAINER_CLASS)
                            || (cinfo->npcflag & UNIT_NPC_FLAG_TRAINER_PROFESSION)) && (cinfo->trainer_class == getClass()) )
                            learnSpell(spellInfo1->Id, false, true);
                    }
                }

                SpellEntry const* spellInfo2 = sSpellStore.LookupEntry(pQuest->GetRewSpell());
                if (spellInfo2)
                {
                    if (IsSpellFitByClassAndRace(spellInfo2->Id) && !isWtfSpell(spellInfo2->Id))
                    {
                        if ( (spellInfo2->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL && spellInfo2->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SKILL)
                            || (spellInfo2->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_TRADE_SKILL) ) {}
                        else if (spellInfo2->SpellFamilyName == family)
                            learnSpell(spellInfo2->Id, false, true);
                        else if  ( ((cinfo->npcflag & UNIT_NPC_FLAG_TRAINER) || (cinfo->npcflag & UNIT_NPC_FLAG_TRAINER_CLASS)
                            || (cinfo->npcflag & UNIT_NPC_FLAG_TRAINER_PROFESSION)) && (cinfo->trainer_class == getClass()) )
                            learnSpell(spellInfo2->Id, false, true);
                    }
                }

                SpellEntry const* spellInfo3 = sSpellStore.LookupEntry(pQuest->GetRewSpellCast());
                if (spellInfo3)
                {
                    bool learn = false;
                    for(int j = 0; j < 3; j++)
                    {
                        if(spellInfo3->Effect[j] == SPELL_EFFECT_LEARN_SPELL)
                        {
                            learn = true;
                            SpellEntry const* spellInfo4 = sSpellStore.LookupEntry(spellInfo3->EffectTriggerSpell[j]);
                            if (spellInfo4)
                            {
                                if (IsSpellFitByClassAndRace(spellInfo4->Id) && !isWtfSpell(spellInfo4->Id))
                                {
                                    if ( (spellInfo4->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL && spellInfo4->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SKILL)
                                        || (spellInfo4->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_TRADE_SKILL) ) {}
                                    else if (spellInfo4->SpellFamilyName == family)
                                        learnSpell(spellInfo4->Id, false, true);
                                    else if  ( ((cinfo->npcflag & UNIT_NPC_FLAG_TRAINER) || (cinfo->npcflag & UNIT_NPC_FLAG_TRAINER_CLASS)
                                        || (cinfo->npcflag & UNIT_NPC_FLAG_TRAINER_PROFESSION)) && (cinfo->trainer_class == getClass()) )
                                        learnSpell(spellInfo4->Id, false, true);
                                }
                            }
                        }
                    }
                    if (!learn)
                    {
                        if (IsSpellFitByClassAndRace(spellInfo3->Id) && !isWtfSpell(spellInfo3->Id))
                        {
                            if ( (spellInfo3->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL && spellInfo3->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SKILL)
                                || (spellInfo3->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_TRADE_SKILL) ) {}
                            else if (spellInfo3->SpellFamilyName == family)
                                learnSpell(spellInfo3->Id, false, true);
                            else if  ( ((cinfo->npcflag & UNIT_NPC_FLAG_TRAINER) || (cinfo->npcflag & UNIT_NPC_FLAG_TRAINER_CLASS)
                                || (cinfo->npcflag & UNIT_NPC_FLAG_TRAINER_PROFESSION)) && (cinfo->trainer_class == getClass()) )
                                learnSpell(spellInfo3->Id, false, true);
                        }
                    }
                }
            }
        }
    }

    for (uint32 id = 0; id < sItemStorage.MaxEntry; ++id)
    {
        ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(id);
        if(!pProto || pProto->Stackable > 1 || pProto->Class==ITEM_CLASS_WEAPON || pProto->Class==ITEM_CLASS_ARMOR || pProto->RequiredLevel > getLevel())
            continue;

        for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            bool learn = false;
            SpellEntry const* spellInfo1 = sSpellStore.LookupEntry(pProto->Spells[i].SpellId);
            if (spellInfo1)
            {
                bool learn = false;
                for(int j = 0; j < 3; j++)
                {
                    if(spellInfo1->Effect[j] == SPELL_EFFECT_LEARN_SPELL)
                    {
                        learn = true;
                        SpellEntry const* spellInfo2 = sSpellStore.LookupEntry(spellInfo1->EffectTriggerSpell[j]);
                        if (spellInfo2)
                        {
                            if (IsSpellFitByClassAndRace(spellInfo2->Id) && !isWtfSpell(spellInfo2->Id))
                            {
                                if ( (spellInfo2->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL && spellInfo2->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SKILL)
                                    || (spellInfo2->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_TRADE_SKILL) ) {}
                                else if (spellInfo2->SpellFamilyName == family)
                                    learnSpell(spellInfo2->Id, false, true);
                            }
                        }
                    }
                }
                if (!learn)
                {
                    if (IsSpellFitByClassAndRace(spellInfo1->Id) && !isWtfSpell(spellInfo1->Id))
                    {
                        if ( (spellInfo1->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL && spellInfo1->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SKILL)
                            || (spellInfo1->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_TRADE_SKILL) ) {}
                        else if (spellInfo1->SpellFamilyName == family)
                            learnSpell(spellInfo1->Id, false, true);
                    }
                }
            }
        }
    }

    if(getLevel() > 19)
        learnSpell(33388, false);
    if(getLevel() > 39)
        learnSpell(33391, false);
    if(getLevel() > 59)
        learnSpell(34090, false);
    if(getLevel() > 69)
        learnSpell(34091, false);
    if(getLevel() > 76)
        learnSpell(54197, false);

    switch (getClass())
    {
        case CLASS_WARRIOR:
        case CLASS_PALADIN:
        case CLASS_DEATH_KNIGHT:
        {
            if(getLevel() > 39)
                learnSpell(750, false);
            break;
        }
        case CLASS_HUNTER:
        case CLASS_SHAMAN:
        {
            if(getLevel() > 39)
                learnSpell(8737, false);
            break;
        }
        default:
            break;
    }

    switch (getRace())
    {
        case RACE_HUMAN:
        {
            static uint32 Mounts20[] = {2414, 5655, 5656};
            static uint32 Mounts40[] = {18776, 18778, 18777};
            if (getLevel() >= 20)
                choseMount(Mounts20, sizeof(Mounts20)/sizeof(uint32));
            if (getLevel() >= 40)
                choseMount(Mounts40, sizeof(Mounts40)/sizeof(uint32));
            break;
        }
        case RACE_ORC:
        {
            static uint32 Mounts20[] = {5665, 46099, 1132, 5668};
            static uint32 Mounts40[] = {18798, 18797, 18796};
            if (getLevel() >= 20)
                choseMount(Mounts20, sizeof(Mounts20)/sizeof(uint32));
            if (getLevel() >= 40)
                choseMount(Mounts40, sizeof(Mounts40)/sizeof(uint32));
            break;
        }
        case RACE_DWARF:
        {
            static uint32 Mounts20[] = {5864, 5872, 5873};
            static uint32 Mounts40[] = {18787, 18786, 18785};
            if (getLevel() >= 20)
                choseMount(Mounts20, sizeof(Mounts20)/sizeof(uint32));
            if (getLevel() >= 40)
                choseMount(Mounts40, sizeof(Mounts40)/sizeof(uint32));
            break;
        }
        case RACE_NIGHTELF:
        {
            static uint32 Mounts20[] = {8629, 47100, 8632, 8631};
            static uint32 Mounts40[] = {18902, 18766, 18767};
            if (getLevel() >= 20)
                choseMount(Mounts20, sizeof(Mounts20)/sizeof(uint32));
            if (getLevel() >= 40)
                choseMount(Mounts40, sizeof(Mounts40)/sizeof(uint32));
            break;
        }
        case RACE_UNDEAD:
        {
            static uint32 Mounts20[] = {13331, 46308, 13332, 13333};
            static uint32 Mounts40[] = {18791, 13334, 47101};
            if (getLevel() >= 20)
                choseMount(Mounts20, sizeof(Mounts20)/sizeof(uint32));
            if (getLevel() >= 40)
                choseMount(Mounts40, sizeof(Mounts40)/sizeof(uint32));
            break;
        }
        case RACE_TAUREN:
        {
            static uint32 Mounts20[] = {15277, 15290, 46100};
            static uint32 Mounts40[] = {18795, 18794, 18793};
            if (getLevel() >= 20)
                choseMount(Mounts20, sizeof(Mounts20)/sizeof(uint32));
            if (getLevel() >= 40)
                choseMount(Mounts40, sizeof(Mounts40)/sizeof(uint32));
            break;
        }
        case RACE_GNOME:
        {
            static uint32 Mounts20[] = {13321, 8563, 13322, 8595};
            static uint32 Mounts40[] = {18772, 18774, 18773};
            if (getLevel() >= 20)
                choseMount(Mounts20, sizeof(Mounts20)/sizeof(uint32));
            if (getLevel() >= 40)
                choseMount(Mounts40, sizeof(Mounts40)/sizeof(uint32));
            break;
        }
        case RACE_TROLL:
        {
            static uint32 Mounts20[] = {8592, 8591, 8588};
            static uint32 Mounts40[] = {18789, 18790, 18788};
            if (getLevel() >= 20)
                choseMount(Mounts20, sizeof(Mounts20)/sizeof(uint32));
            if (getLevel() >= 40)
                choseMount(Mounts40, sizeof(Mounts40)/sizeof(uint32));
            break;
        }
        case RACE_BLOODELF:
        {
            static uint32 Mounts20[] = {29222, 28927, 29221, 29220};
            static uint32 Mounts40[] = {29224, 29223, 28936};
            if (getLevel() >= 20)
                choseMount(Mounts20, sizeof(Mounts20)/sizeof(uint32));
            if (getLevel() >= 40)
                choseMount(Mounts40, sizeof(Mounts40)/sizeof(uint32));
            break;
        }
        case RACE_DRAENEI:
        {
            static uint32 Mounts20[] = {29743, 29744, 28481};
            static uint32 Mounts40[] = {29747, 29746, 29745};
            if (getLevel() >= 20)
                choseMount(Mounts20, sizeof(Mounts20)/sizeof(uint32));
            if (getLevel() >= 40)
                choseMount(Mounts40, sizeof(Mounts40)/sizeof(uint32));
            break;
        }
    }

    switch(GetTeam())
    {
        case ALLIANCE:
        {
            static uint32 Mounts60[] = {25472, 25470, 25471};
            static uint32 Mounts70[] = {25529, 25528, 25527, 25473};
            if (getLevel() >= 60)
                choseMount(Mounts60, sizeof(Mounts60)/sizeof(uint32));
            if (getLevel() >= 70)
                choseMount(Mounts70, sizeof(Mounts70)/sizeof(uint32));
            break;
        }
        case HORDE:
        {
            static uint32 Mounts60[] = {25476, 25474, 25475};
            static uint32 Mounts70[] = {25533, 25531, 25477, 25532};
            if (getLevel() >= 60)
                choseMount(Mounts60, sizeof(Mounts60)/sizeof(uint32));
            if (getLevel() >= 70)
                choseMount(Mounts70, sizeof(Mounts70)/sizeof(uint32));
            break;
        }
    }
    return true;
}

bool Player::LearnAllMyTalentsForMyLevel()
{
    uint32 classMask = getClassMask();
    uint32 level = getLevel();

    for (uint32 i = 0; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const *talentInfo = sTalentStore.LookupEntry(i);
        if(!talentInfo)
            continue;

        TalentTabEntry const *talentTabInfo = sTalentTabStore.LookupEntry( talentInfo->TalentTab );
        if(!talentTabInfo)
            continue;

        if(!IsBot() && talentTabInfo->TalentTabID != getRole())
            continue;

        if( (classMask & talentTabInfo->ClassMask) == 0 )
            continue;

        if(talentInfo->Row < ((level-5) / 5))
            learnSpellHighRank(talentInfo->RankID[0]);
        else
        {
            for (int i = 0; i < MAX_TALENT_RANK; ++i)
                if (talentInfo->RankID[i] && HasSpell(talentInfo->RankID[i]))
                    removeSpell(talentInfo->RankID[i], false, false);
        }
    }

    SendTalentsInfoData(false);
    return true;
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

bool ChatHandler::HandleGMStartUpCommand(char* args)
{
    Player *player = m_session->GetPlayer();
    if (!player)
        return true;

    player->GMStartup();
    return true;
}

bool ChatHandler::HandleBotTank(char* args)
{
    Player* pl = m_session->GetPlayer();
    Unit* target = getSelectedUnit();

    if (!pl->GetSelectionGuid().IsEmpty() && target)
    {
        if (target->GetTypeId() == TYPEID_UNIT)
        {
            SendSysMessage(1);
            SetSentErrorMessage(true);
            return false;
        }

        Player *p_target = (Player *)target;
        Group *p_group = p_target->GetGroup();

        if (pl->GetGroup() != p_group)
        {
            PSendSysMessage("Vous devez etre en groupe avec un bot");
            SetSentErrorMessage(true);
            return false;
        }

        if (p_target->IsBot())
            p_group->SetGroupUniqueFlag(p_target->GetObjectGuid(), GROUP_ASSIGN_MAINTANK, 1);
    }

    return true;
}

bool ChatHandler::HandleBotAssist(char* args)
{
    Player* pl = m_session->GetPlayer();
    Unit* target = getSelectedUnit();

    if (!pl->GetSelectionGuid().IsEmpty() && target)
    {
        if (target->GetTypeId() == TYPEID_UNIT)
        {
            SendSysMessage(1);
            SetSentErrorMessage(true);
            return false;
        }

        Player *p_target = (Player *)target;
        Group *p_group = p_target->GetGroup();

        if (pl->GetGroup() != p_group)
        {
            PSendSysMessage("Vous devez etre en groupe avec un bot");
            SetSentErrorMessage(true);
            return false;
        }

        if (p_target->IsBot())
            p_group->SetGroupUniqueFlag(p_target->GetObjectGuid(), GROUP_ASSIGN_ASSISTANT, 1);
    }

    return true;
}

bool ChatHandler::HandleBotTankTarget(char* args)
{
    Player* pl = m_session->GetPlayer();
    Unit* target = getSelectedUnit();

    if (!pl->GetSelectionGuid().IsEmpty() && target)
    {
        if (target->GetTypeId() != TYPEID_UNIT)
        {
            SendSysMessage(2);
            SetSentErrorMessage(true);
            return false;
        }

        Group *gr = pl->GetGroup();

        if (!gr)
        {
            PSendSysMessage("Vous devez etre en groupe avec un bot");
            SetSentErrorMessage(true);
            return false;
        }

        gr->SetTankTarget(target);
    }

    return true;
}

bool ChatHandler::HandleBotAssistTarget(char* args)
{
    Player* pl = m_session->GetPlayer();
    Unit* target = getSelectedUnit();

    if (!pl->GetSelectionGuid().IsEmpty() && target)
    {
        if (target->GetTypeId() != TYPEID_UNIT)
        {
            SendSysMessage(2);
            SetSentErrorMessage(true);
            return false;
        }

        Group *gr = pl->GetGroup();

        if (!gr)
        {
            PSendSysMessage("Vous devez etre en groupe avec un bot");
            SetSentErrorMessage(true);
            return false;
        }

        gr->SetAssistTarget(target);
    }

    return true;
}

bool ChatHandler::HandleBotPull(char* args)
{
    Player* pl = m_session->GetPlayer();

    if (!pl->GetGroup())
    {
        PSendSysMessage("Vous devez etre en groupe avec un bot");
        SetSentErrorMessage(true);
        return false;
    }

    Group *m_group = pl->GetGroup();
    GroupReference *ref = (m_group) ? m_group->GetFirstMember() : NULL;

    do
    {
        Player *g_member = (ref) ? ref->getSource() : pl;

        if (!g_member->isAlive())
            continue;

        if (g_member->IsBot())
            g_member->GetPlayerbotAI()->Pull();

    }while(ref = (ref) ? ref->next() : NULL);

    return true;
}

bool ChatHandler::HandleBotInvite(char* args)
{
    if (!*args)
        return false;

    int16 role = (int16)atoi(args);
    if (role == 0)
        return false;

    Player *pl = m_session->GetPlayer();

    uint8 _class = 0;
    switch (role)
    {
        case MageFire:
        case MageArcane:
        case MageFrost:
            _class = CLASS_MAGE;
            break;
        case WarriorArms:
        case WarriorProtection:
        case WarriorFury:
            _class = CLASS_WARRIOR;
            break;
        case RogueCombat:
        case RogueAssassination:
        case RogueSubtlety:
            _class = CLASS_ROGUE;
            break;
        case PriestDiscipline:
        case PriestHoly:
        case PriestShadow:
            _class = CLASS_PRIEST;
            break;
        case ShamanElementalCombat:
        case ShamanRestoration:
        case ShamanEnhancement:
            _class = CLASS_SHAMAN;
            break;
        case DruidFeralCombat:
        case DruidRestoration:
        case DruidBalance:
            _class = CLASS_DRUID;
            break;
        case WarlockDestruction:
        case WarlockCurses:
        case WarlockSummoning:
            _class = CLASS_WARLOCK;
            break;
        case HunterBeastMastery:
        case HunterSurvival:
        case HunterMarksmanship:
            _class = CLASS_HUNTER;
            break;
        case PaladinCombat:
        case PaladinHoly:
        case PaladinProtection:
            _class = CLASS_PALADIN;
            break;
        case DeathKnightBlood:
        case DeathKnightFrost:
        case DeathKnightUnholy:
        {
            if (pl->getLevel() < 55)
            {
                PSendSysMessage("Impossible d'inviter un Chevalier de la Mort avant le niveau 55");
                return true;
            }
            _class = CLASS_DEATH_KNIGHT;
            break;
        }
    }
    if (_class == 0)
        return false;

    HashMapHolder<Player>::MapType& m = sObjectAccessor.GetPlayers();
    for(HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        Player* chr = itr->second;

        if(!chr || !chr->IsBot() || chr->GetGroup() || pl->GetTeam() != chr->GetTeam())
            continue;

        PlayerInfo const* info = sObjectMgr.GetPlayerInfo(chr->getRace(), _class);
        if (!info)
            continue;

        if (pl->GetGroup() && !pl->GetGroup()->isRaidGroup() && pl->GetGroup()->IsFull())
        {
            WorldPacket pk;
            m_session->HandleGroupRaidConvertOpcode(pk);
        }

        WorldPacket p(CMSG_GROUP_INVITE, 10);                // guess size
        p << chr->GetName();                                 // max len 48
        p << uint32(0);
        m_session->HandleGroupInviteOpcode(p);

        chr->setClass(_class);
        chr->setRole(role);
        chr->RemovePet(PET_SAVE_AS_DELETED);
        chr->SetByteValue(UNIT_FIELD_BYTES_0,1,_class);// class

        uint32 lvl = pl->getLevel();
        if (lvl == DEFAULT_MAX_LEVEL)
            chr->GiveLevel(pl->getLevel()-1);
        else
            chr->GiveLevel(pl->getLevel()+1);

        chr->RemoveAllAuras(AURA_REMOVE_BY_DELETE);
        ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(_class);
        if(cEntry && cEntry->powerType < MAX_POWERS && uint32(chr->getPowerType()) != cEntry->powerType)
            chr->setPowerType(Powers(cEntry->powerType));
        chr->InitRunes();
        break;
    }

    return true;
}

bool ChatHandler::HandleBotInviteArena(char* args)
{
    if (!*args)
        return false;

    int16 role = (int16)atoi(args);
    if (role == 0)
        return false;

    Player *pl = m_session->GetPlayer();

    uint8 _class = 0;
    switch (role)
    {
        case MageFire:
        case MageArcane:
        case MageFrost:
            _class = CLASS_MAGE;
            break;
        case WarriorArms:
        case WarriorProtection:
        case WarriorFury:
            _class = CLASS_WARRIOR;
            break;
        case RogueCombat:
        case RogueAssassination:
        case RogueSubtlety:
            _class = CLASS_ROGUE;
            break;
        case PriestDiscipline:
        case PriestHoly:
        case PriestShadow:
            _class = CLASS_PRIEST;
            break;
        case ShamanElementalCombat:
        case ShamanRestoration:
        case ShamanEnhancement:
            _class = CLASS_SHAMAN;
            break;
        case DruidFeralCombat:
        case DruidRestoration:
        case DruidBalance:
            _class = CLASS_DRUID;
            break;
        case WarlockDestruction:
        case WarlockCurses:
        case WarlockSummoning:
            _class = CLASS_WARLOCK;
            break;
        case HunterBeastMastery:
        case HunterSurvival:
        case HunterMarksmanship:
            _class = CLASS_HUNTER;
            break;
        case PaladinCombat:
        case PaladinHoly:
        case PaladinProtection:
            _class = CLASS_PALADIN;
            break;
        case DeathKnightBlood:
        case DeathKnightFrost:
        case DeathKnightUnholy:
        {
            if (pl->getLevel() < 55)
            {
                PSendSysMessage("Impossible d'inviter un Chevalier de la Mort avant le niveau 55");
                return true;
            }
            _class = CLASS_DEATH_KNIGHT;
            break;
        }
    }
    if (_class == 0)
        return false;

    HashMapHolder<Player>::MapType& m = sObjectAccessor.GetPlayers();
    for(HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        Player* chr = itr->second;

        if(!chr || !chr->IsBot() || chr->GetGroup() || pl->GetTeam() != chr->GetTeam())
            continue;

        PlayerInfo const* info = sObjectMgr.GetPlayerInfo(chr->getRace(), _class);
        if (!info)
            continue;

        WorldPacket pk1(CMSG_GROUP_INVITE, 10);                // guess size
        pk1 << chr->GetName();                                 // max len 48
        pk1 << uint32(0);
        m_session->HandleGroupInviteOpcode(pk1);
        for (uint8 i = 0; i < MAX_ARENA_SLOT; ++i)
        {
            if(uint32 a_id = pl->GetArenaTeamId(i))
            {
                if(ArenaTeam *at = sObjectMgr.GetArenaTeamById(a_id))
                {
                    chr->SetArenaTeamIdInvited(at->GetId());
                    WorldPacket pk2;
                    chr->GetSession()->HandleArenaTeamAcceptOpcode(pk2);
                    break;
                }
            }
        }

        chr->setClass(_class);
        chr->setRole(role);
        chr->RemovePet(PET_SAVE_AS_DELETED);
        chr->SetByteValue(UNIT_FIELD_BYTES_0,1,_class);// class

        uint32 lvl = pl->getLevel();
        if (lvl == DEFAULT_MAX_LEVEL)
            chr->GiveLevel(pl->getLevel()-1);
        else
            chr->GiveLevel(pl->getLevel()+1);

        chr->RemoveAllAuras(AURA_REMOVE_BY_DELETE);
        ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(_class);
        if(cEntry && cEntry->powerType < MAX_POWERS && uint32(chr->getPowerType()) != cEntry->powerType)
            chr->setPowerType(Powers(cEntry->powerType));
        chr->InitRunes();
        break;
    }

    return true;
}

bool ChatHandler::HandleBotAddPOI(char* args)
{
    uint32 id = 0;
    QueryResult *result = WorldDatabase.Query("SELECT MAX(id) FROM bot_info_position");
    if(!result)
    {
        id++;
    }
    else
    {
        Field *fields = result->Fetch();
        id = fields[0].GetUInt32()+1;
        delete result;
    }

    Player *pl = m_session->GetPlayer();

    float x, y , z;
    pl->GetPosition(x,y,z);
    uint32 mapid = pl->GetMapId();
    uint32 zoneid = pl->GetZoneId();

    BotInfoZone const* biz = sObjectMgr.GetBotInfoZone(zoneid);
    if (!biz)
        return true;

    uint32 minlevel = biz->minlevel;
    uint32 maxlevel = biz->maxlevel;
    uint8 territory = biz->territory;

    WorldDatabase.PExecute("INSERT INTO bot_info_position (id, x, y, z, mapid, zoneid, minlevel, maxlevel, territory) VALUES ('%u', '%f', '%f', '%f', '%u', '%u', '%u', '%u', '%u')",
        id, x, y, z, mapid, zoneid, minlevel, maxlevel, territory);
    return true;
}
