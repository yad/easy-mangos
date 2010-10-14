#include "AuctionHouseBot.h"
#include "ProgressBar.h"
#include "../ObjectMgr.h"
#include "../AuctionHouseMgr.h"
#include "config.h"
#include "SystemConfig.h"

#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1( AuctionHouseBot );

AuctionHouseBot::AuctionHouseBot()
{
    // Define faction for our main class.
    m_AllianceConfig = AHBConfig(2);
    m_HordeConfig = AHBConfig(6);
    m_NeutralConfig = AHBConfig(7);

    // Define the last usable number for GUID will be used has AhBot GUID.
    m_FakeGuid.Set(std::numeric_limits< uint32 >::max());

    // Initialise m_ItemPool (list of items in database will be used to fill AH)
    m_ItemPool.resize(AHB_QUALITY_MAX, std::vector< std::vector< uint32 > >( MAX_ITEM_CLASS ));
}

AuctionHouseBot::~AuctionHouseBot()
{

}

// Set static of items on one AH faction.
// Fill ItemInfos object with real content of AH.
uint32 AuctionHouseBot::SetStat(AHBConfig& config)
{
    std::vector<std::vector<uint32> > ItemsInAH(AHB_QUALITY_MAX, std::vector< uint32 > (MAX_ITEM_CLASS));

    AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(config.GetAHID());
    AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);
    for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin();itr != auctionHouse->GetAuctionsEnd();++itr)
    {
        AuctionEntry *Aentry = itr->second;
        Item *item = sAuctionMgr.GetAItem(Aentry->item_guidlow);
        if (item)
        {
            ItemPrototype const *prototype = item->GetProto();
            if (prototype)
            {
                if ( Aentry->owner == GetAHBObjectGuid().GetRawValue()) // Add only ahbot items
                {
                    ++ItemsInAH[prototype->Quality][prototype->Class];
                }
            }
        }
    }
    uint32 count=0;
    for (uint32 j=0; j<AHB_QUALITY_MAX; ++j)
    {
        for (uint32 i=0;i<MAX_ITEM_CLASS;++i)
        {
            config.ItemInfos[j].ItemClassInfos[i].SetMissItems(ItemsInAH[j][i]);
            count+=config.ItemInfos[j].ItemClassInfos[i].GetMissItems();
        }
    }

    if (m_debug_Seller)
    {
        sLog.outString("Missed Item\tGrey\tWhite\tGreen\tBlue\tPurple\tOrange\tYellow");
        for (uint32 i=0; i<MAX_ITEM_CLASS;++i)
        {
            sLog.outString("%-11s\t%u\t%u\t%u\t%u\t%u\t%u\t%u",config.ItemInfos[0].ItemClassInfos[i].GetName().c_str(),config.ItemInfos[0].ItemClassInfos[i].GetMissItems(),config.ItemInfos[1].ItemClassInfos[i].GetMissItems(),config.ItemInfos[2].ItemClassInfos[i].GetMissItems(),
                config.ItemInfos[3].ItemClassInfos[i].GetMissItems(),config.ItemInfos[4].ItemClassInfos[i].GetMissItems(),config.ItemInfos[5].ItemClassInfos[i].GetMissItems(),config.ItemInfos[6].ItemClassInfos[i].GetMissItems());
        }
    }
    config.LastMissedItem = count;
    return count;
}


// Set static of items on one AH faction for buyer.
// Fill m_BuyableEntry object with real content of AH.
uint32 AuctionHouseBot::GetBuyableEntry(AHBConfig& config)
{
    config.m_SameItemInfo.clear();
    bool MinTimePassed = (m_LastBuyableEntryChecked - time(NULL)) > 300; // todo : add config to this value
    uint32 count=0;
    time_t Now=time(NULL);
    AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(config.GetAHID());
    AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);

    for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin();itr != auctionHouse->GetAuctionsEnd();++itr)
    {
        AuctionEntry *Aentry = itr->second;
        Item *item = sAuctionMgr.GetAItem(Aentry->item_guidlow);
        if (item)
        {
            ItemPrototype const *prototype = item->GetProto();
            if (prototype)
            {
                ++config.m_SameItemInfo[item->GetEntry()].ItemCount;    // Structure constructor will make sure Element are correctly initialised if entry is created here.
                config.m_SameItemInfo[item->GetEntry()].BuyPrice =config.m_SameItemInfo[item->GetEntry()].BuyPrice + (itr->second->buyout/item->GetCount());
                config.m_SameItemInfo[item->GetEntry()].BidPrice =config.m_SameItemInfo[item->GetEntry()].BidPrice + (itr->second->startbid/item->GetCount());
                if (itr->second->buyout != 0)
                {
                    if (itr->second->buyout/item->GetCount() < config.m_SameItemInfo[item->GetEntry()].MinBuyPrice)
                        config.m_SameItemInfo[item->GetEntry()].MinBuyPrice = itr->second->buyout/item->GetCount();
                    else if (config.m_SameItemInfo[item->GetEntry()].MinBuyPrice == 0)
                        config.m_SameItemInfo[item->GetEntry()].MinBuyPrice = itr->second->buyout/item->GetCount();
                }
                if (itr->second->startbid/item->GetCount() < config.m_SameItemInfo[item->GetEntry()].MinBidPrice)
                    config.m_SameItemInfo[item->GetEntry()].MinBidPrice = itr->second->startbid/item->GetCount();
                else if (config.m_SameItemInfo[item->GetEntry()].MinBidPrice == 0)
                    config.m_SameItemInfo[item->GetEntry()].MinBidPrice = itr->second->startbid/item->GetCount();

                //if (item->GetEntry() == 41604) sLog.outString(">items 41604 owner=%u, bid=%u, bidder=%u",Aentry->owner, Aentry->bid, Aentry->bidder);

                if ( Aentry->owner == GetAHBObjectGuid().GetRawValue())
                {

                    if ( (Aentry->bid!=0) && (Aentry->bidder!=GetAHBObjectGuid().GetRawValue()) ) // Add bided by player
                    {
                        config.m_CheckedEntry[Aentry->Id].LastExist=Now;
                        config.m_CheckedEntry[Aentry->Id].item_guidlow=item->GetGUIDLow();
                        config.m_CheckedEntry[Aentry->Id].AhEntry=Aentry->Id;
                        ++count;
                    }
                }
                else
                {
                    if (Aentry->bid!=0)
                    {
                        if  (Aentry->bidder!=GetAHBObjectGuid().GetRawValue())
                        {
                            config.m_CheckedEntry[Aentry->Id].LastExist=Now;
                            config.m_CheckedEntry[Aentry->Id].item_guidlow=item->GetGUIDLow();
                            config.m_CheckedEntry[Aentry->Id].AhEntry=Aentry->Id;
                            ++count;
                        }
                    }
                    else
                    {
                        config.m_CheckedEntry[Aentry->Id].LastExist=Now;
                        config.m_CheckedEntry[Aentry->Id].item_guidlow=item->GetGUIDLow();
                        config.m_CheckedEntry[Aentry->Id].AhEntry=Aentry->Id;
                        ++count;
                    }
                }
            }
        }
    }

    if (m_debug_Buyer)
    {
        sLog.outString("AHBot > %u items added to buyable vector for ah %u",count, config.GetAHID());
        sLog.outString("SameItemInfo size = %u", config.m_SameItemInfo.size());
    }
    return count;
}

// getRandomArray is used to make aviable the possibility to add any of missed item in place of first one to last one.
bool AuctionHouseBot::getRandomArray( AHBConfig& config, std::vector<s_randomArray>& ra, const std::vector<std::vector<uint32> >& addedItem  )
{
    ra.clear();
    s_randomArray miss_item;
    bool Ok=false;

    for (uint32 j=0; j<AHB_QUALITY_MAX; ++j)
    {
        for (uint32 i=0; i<MAX_ITEM_CLASS; ++i)
            if ((config.ItemInfos[j].ItemClassInfos[i].GetMissItems()   > addedItem[j][i]) && m_ItemPool[j][i].size() >0)
            {
                miss_item.color=j;
                miss_item.itemclass=i;
                ra.push_back(miss_item);
                Ok=true;
            }
    }

    return Ok;
}

// Set items price. All important value are passed by address. This method set olso stack value.
void AuctionHouseBot::SetPricesOfItem(const Item *item,AHBConfig& config, uint32& buyp, uint32& bidp, uint32& stackcnt, e_ahb_quality AHB_ITEMS)
{
    if (config.ItemInfos[AHB_ITEMS].GetMaxStack() != 0)
    {
        stackcnt = urand(1, minValue(item->GetMaxStackCount(), config.ItemInfos[AHB_ITEMS].GetMaxStack()));
    }
    double temp_buyp = buyp * config.ItemInfos[AHB_ITEMS].GetPriceRatio() *stackcnt;
    double randrange = temp_buyp * 0.4;
    buyp = (urand(temp_buyp-randrange, temp_buyp+randrange)/100)+1;
    double urandrange=buyp*40;
    double temp_bidp = buyp*50;
    bidp = (urand(temp_bidp-urandrange, temp_bidp+urandrange)/100)+1;
}

// Add new auction to one of the factions.
// Faction and setting assossiated is defined passed argument ( config )
void AuctionHouseBot::addNewAuctions(AHBConfig& config)
{
    uint32 items;

    // If there is large amount of items missed we can use boost value to get fast filled AH
    if (config.LastMissedItem > m_ItemsPerCycleBoost)
    {
        items=m_ItemsPerCycleBoost;
        sLog.outString("AHBot> Boost value used to fill AH! (if this happens often adjust both ItemsPerCycle in mangosd.conf)");
    }
    else items=m_ItemsPerCycleNormal;


    AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(config.GetAHID());
    AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);

    std::vector<s_randomArray> RandArray;
    std::vector<std::vector<uint32> > ItemsAdded(AHB_QUALITY_MAX,std::vector<uint32> (MAX_ITEM_CLASS));
    // Main loop
    // getRandomArray will give what categories of items should be added (return true if there is at least 1 items missed)
    while (getRandomArray(config,RandArray, ItemsAdded) && (items>0))
    {
        --items;
        uint32 itemID = 0;
        // Select random position from missed items table
        uint32 pos =  (urand(0,RandArray.size()-1));

        // Set itemID with random item ID for selected categories and color, from m_ItemPool table
        itemID = m_ItemPool[RandArray[pos].color][RandArray[pos].itemclass][urand(0,m_ItemPool[RandArray[pos].color][RandArray[pos].itemclass].size()-1)];
        ++ ItemsAdded[RandArray[pos].color][RandArray[pos].itemclass]; // Helper table to avoid rescan from DB in this loop. (has we add item in random orders)


        if (itemID == 0)
        {
            if (m_debug_Seller)
                sLog.outString("AHBot> Item::CreateItem() - Unable to find item");
            continue;
        }

        ItemPrototype const* prototype = sObjectMgr.GetItemPrototype(itemID);
        if (prototype == NULL)
        {
            sLog.outString("AHBot> Huh?!?! prototype == NULL");
            continue;
        }

        Item* item = Item::CreateItem(itemID, 1);
        if (item == NULL)
        {
            sLog.outString("AHBot> Item::CreateItem() returned NULL");
            return;
        }
        // Some items need to set random property (agility or intellect for ex)
        uint32 randomPropertyId = Item::GenerateItemRandomPropertyId(itemID);
        if (randomPropertyId != 0)
            item->SetItemRandomPropertiesNoUpdate(randomPropertyId);

        uint32 buyoutPrice;
        uint32 bidPrice = 0;
        uint32 stackCount = urand(1, item->GetMaxStackCount());
        // Not sure if i will keep the next test
        if(getConfig(CONFIG_BOOL_AHBOT_BUYPRICE_SELLER))
            buyoutPrice  = prototype->BuyPrice * item->GetCount();
        else
            buyoutPrice  = prototype->SellPrice * item->GetCount();
        // Price of items are set here
        SetPricesOfItem(item, config, buyoutPrice, bidPrice, stackCount, ((e_ahb_quality) prototype->Quality));

        item->SetCount(stackCount);
        item->SetOwnerGUID(GetAHBObjectGuid().GetRawValue());
        // Add Auction now on the AH
        AuctionEntry* auctionEntry = new AuctionEntry;
        auctionEntry->Id = sObjectMgr.GenerateAuctionID();
        auctionEntry->item_guidlow = item->GetGUIDLow();
        auctionEntry->item_template = item->GetEntry();
        auctionEntry->owner =((uint32) GetAHBObjectGuid().GetRawValue());
        auctionEntry->startbid = bidPrice;
        auctionEntry->buyout = buyoutPrice;
        auctionEntry->bidder = 0;
        auctionEntry->bid = 0;
        auctionEntry->deposit = 0;
        auctionEntry->expire_time = (time_t) (urand(config.GetMinTime(), config.GetMaxTime()) * 60 * 60 + time(NULL));
        auctionEntry->auctionHouseEntry = ahEntry;
        //item->SaveToDB();
        sAuctionMgr.AddAItem(item);
        auctionHouse->AddAuction(auctionEntry);
        //auctionEntry->SaveToDB();
    }
}

void AuctionHouseBot::PrepareListOfEntry(AHBConfig& config)
{
    AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(config.GetAHID());
    AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);

    time_t Now=time(NULL)-5;

    for (AHBConfig::t_checkEntryMap::iterator itr=config.m_CheckedEntry.begin();itr != config.m_CheckedEntry.end(); itr++)
    {
        if (itr->second.LastExist  < Now-5)
        {
            config.m_CheckedEntry.erase(itr);
        }
    }

    if (m_debug_Buyer)
    {
        sLog.outString("CheckedEntry size = %u",config.m_CheckedEntry.size());
    }
}

bool AuctionHouseBot::IsBuyableEntry(uint32 buyoutPrice, double InGame_BuyPrice, double MaxBuyablePrice, uint32 MinBuyPrice, uint32 MaxChance, uint32 ChanceRatio)
{
    double ratio=0;
    uint32 Chance=0;

    if (buyoutPrice <= MinBuyPrice)
    {
        if (buyoutPrice <= MaxBuyablePrice)
        {
            Chance=MaxChance;
        }
        else
        {

            if ((buyoutPrice > 0) && (MaxBuyablePrice > 0))
            {
                    ratio = buyoutPrice / MaxBuyablePrice;
                    if (ratio < 10)
                        Chance=MaxChance - (ratio*(MaxChance/10));
                    else Chance=1;
            }
        }
    }
    else if (buyoutPrice <= InGame_BuyPrice)
    {
        if (buyoutPrice <= MaxBuyablePrice)
        {
            Chance=MaxChance/5;
        }
        else
        {

            if ((buyoutPrice > 0) && (MaxBuyablePrice > 0))
            {
                    ratio = buyoutPrice / MaxBuyablePrice;
                    if (ratio < 10)
                        Chance=(MaxChance/5) - (ratio*(MaxChance/50));
                    else Chance=1;
            }
        }
    }
    else if (buyoutPrice <= MaxBuyablePrice)
    {
        Chance = MaxChance/10;
    }
    else
    {
        if ((buyoutPrice > 0) && (MaxBuyablePrice > 0))
        {
            ratio = buyoutPrice / MaxBuyablePrice;
            if (ratio < 10)
                Chance=(MaxChance/5) - (ratio*(MaxChance/50));
            else Chance=0;
         }
        else Chance = 0;
    }
    uint32 RandNum = urand(1,ChanceRatio);
    if (RandNum<=Chance)
    {
        if (m_debug_Buyer)
        {
            sLog.outString("> WIN BUY! Chance = %u, num = %u.",Chance, RandNum);
            sLog.outString("---------------------------------------------------------------------------");
        }
        return true;
    }
    else
    {
        if (m_debug_Buyer)
        {
            sLog.outString("> LOOSE BUY! Chance = %u, num = %u.",Chance, RandNum);
            sLog.outString("---------------------------------------------------------------------------");
        }
        return false;
    }
}

bool AuctionHouseBot::IsBidableEntry(uint32 bidPrice, double InGame_BuyPrice, double MaxBidablePrice, uint32 MinBidPrice, uint32 MaxChance, uint32 ChanceRatio)
{
    double ratio=0;
    uint32 Chance=0;

    if (bidPrice <= MinBidPrice)
    {
        if ((InGame_BuyPrice != 0) && (bidPrice < (InGame_BuyPrice - (InGame_BuyPrice / 30))))
        {
            Chance=MaxChance;
        }
        else
        {
            if (bidPrice < MaxBidablePrice)
            {
                ratio = MaxBidablePrice / bidPrice;
                if (ratio < 3)
                    Chance = ((MaxChance/500)*ratio);
                else
                    Chance = (MaxChance/500);
            }
        }
    }
    else if (bidPrice < (InGame_BuyPrice - (InGame_BuyPrice / 30)))
    {
        Chance=(MaxChance/10);
    }
    else
    {
        if (bidPrice < MaxBidablePrice)
        {
            ratio = MaxBidablePrice / bidPrice;
            if (ratio < 4)
                Chance = ((MaxChance/1000)*ratio);
            else
                Chance = (MaxChance/1000);
        }
    }
    uint32 RandNum = urand(1,ChanceRatio);
    if (RandNum<=Chance)
    {
        if (m_debug_Buyer)
        {
            sLog.outString("> WIN BID! Chance = %u, num = %u.",Chance, RandNum);
            sLog.outString("---------------------------------------------------------------------------");
        }
        return true;
    } else
    {
        if (m_debug_Buyer)
        {
            sLog.outString("> LOOSE BID! Chance = %u, num = %u.",Chance, RandNum);
            sLog.outString("---------------------------------------------------------------------------");
        }
        return false;
    }
}

void AuctionHouseBot::PlaceBidToEntry(AuctionHouseObject* auctionHouse, AuctionEntry* auction, uint32 bidPrice)
{
    if (m_debug_Buyer) sLog.outString(">>>>>>>>>>>>>>> Bid placed to entry %u, %.2fg <<<<<<<<<<<<<<<",auction->Id, (float) (bidPrice/10000));
    if ((auction->bidder!=0)&&(auction->bidder != GetAHBObjectGuid().GetRawValue()))
    {
        // If Entry is already bidded send mail and money back.
        m_Session->SendAuctionOutbiddedMail(auction, bidPrice);
    }

    auction->bidder = GetAHBObjectGuid().GetRawValue();
    auction->bid = bidPrice;
    // Saving auction into database
    CharacterDatabase.PExecute("UPDATE auction SET buyguid = '%u',lastbid = '%u' WHERE id = '%u'", auction->bidder, auction->bid, auction->Id);
}

void AuctionHouseBot::BuyEntry(AuctionHouseObject* auctionHouse, AuctionEntry* auction)
{
    if (m_debug_Buyer) sLog.outString(">>>>>>>>>>>>>>> Entry %u buyed at %.2fg! <<<<<<<<<<<<<<<",auction->Id,(float) (auction->buyout/10000));
    if ((auction->bidder!=0)&&(auction->bidder != GetAHBObjectGuid().GetRawValue()))
    {
        // If Entry is already bidded send mail and money back.
        m_Session->SendAuctionOutbiddedMail(auction, auction->buyout);
    }
    auction->bidder = GetAHBObjectGuid().GetRawValue();
    auction->bid = auction->buyout;

    // Send mails to buyer & seller
    sAuctionMgr.SendAuctionSuccessfulMail(auction);
    sAuctionMgr.SendAuctionWonMail(auction);

    // Remove item from auctionhouse
    sAuctionMgr.RemoveAItem(auction->item_guidlow);
    // Remove auction
    auctionHouse->RemoveAuction(auction->Id);
    // Remove from database
    //auction->DeleteFromDB();
}

void AuctionHouseBot::addNewAuctionBuyerBotBid(AHBConfig& config)
{
    // Fetches content of selected AH
    AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(config.GetAHID());
    AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);

    PrepareListOfEntry(config);

    time_t Now = time(NULL);

    for (AHBConfig::t_checkEntryMap::iterator itr=config.m_CheckedEntry.begin();itr != config.m_CheckedEntry.end(); itr++)
    {
        AuctionEntry* auction = auctionHouse->GetAuction(itr->second.AhEntry);
        if (!auction)
        {
            if (m_debug_Buyer) sLog.outString("Entry doesn't exists, perhaps bought already?");
            config.m_CheckedEntry.erase(itr);
            continue;
        }
        if ((itr->second.lastchecked == 0) || ((Now - itr->second.lastchecked) > m_CheckInterval))
        {
            uint32 BasePrice;
            double MaxBuyablePrice;
            double MaxBidablePrice;
            double InGame_BuyPrice;
            double InGame_BidPrice;
            uint32 MaxChance=5000;
            uint32 Chance=0;
            AHBConfig::t_itemInfo::iterator sameitem_itr;
            Item *item = sAuctionMgr.GetAItem(itr->second.item_guidlow);
            if (item)
            {
                ItemPrototype const *prototype = item->GetProto();
                if (prototype)
                {
                    (getConfig(CONFIG_BOOL_AHBOT_BUYPRICE_BUYER)) ? BasePrice = prototype->BuyPrice : BasePrice = prototype->SellPrice;
                    BasePrice *= item->GetCount();
                    MaxBuyablePrice = ( BasePrice * config.Buyer_Price_Ratio )/100;
                    sameitem_itr = config.m_SameItemInfo.find(item->GetEntry());
                    uint32 buyoutPrice = auction->buyout/item->GetCount();
                    uint32 bidPrice;
                    uint32 bidPriceByItem;
                    if (auction->bid >= auction->startbid)
                    {
                        bidPrice = auction->GetAuctionOutBid();
                        bidPriceByItem = auction->bid/item->GetCount();
                    }
                    else
                    {
                        bidPrice = auction->startbid;
                        bidPriceByItem = auction->startbid/item->GetCount();
                    }
                    if (sameitem_itr==config.m_SameItemInfo.end())
                    {
                        InGame_BuyPrice=0;
                        InGame_BidPrice=0;
                    }
                    else
                    {
                        if (sameitem_itr->second.ItemCount == 1) MaxBuyablePrice = MaxBuyablePrice * 5; // if only one item exist can be buyed if the price is high too.
                        InGame_BuyPrice=sameitem_itr->second.BuyPrice/sameitem_itr->second.ItemCount;
                        InGame_BidPrice=sameitem_itr->second.BidPrice/sameitem_itr->second.ItemCount;
                    }
                    MaxBidablePrice = MaxBuyablePrice - ( MaxBuyablePrice / 30); // Max Bidable price defined to 70% of max buyable price
                    if (m_debug_Buyer)
                    {
                        sLog.outString("===========================================================================");
                        sLog.outString("MaxPrice of Entry %u is %.1fg.",itr->second.AhEntry,MaxBuyablePrice/10000);
                        sLog.outString("GamePrice buy=%.1fg, bid=%.1fg.",InGame_BuyPrice/10000, InGame_BidPrice/10000);
                        sLog.outString("Minimal price see in AH Buy=%ug, Bid=%ug.",sameitem_itr->second.MinBuyPrice/10000,sameitem_itr->second.MinBidPrice/10000);
                        sLog.outString("Actual Entry price,  Buy=%ug, Bid=%ug.",buyoutPrice/10000, bidPrice/10000);
                        sLog.outString("---------------------------------------------------------------------------");
                    }
                    if (auction->owner == GetAHBObjectGuid().GetRawValue()) // Original auction owner
                    {
                        MaxChance = MaxChance / 5; // if Owner is AHBot this mean player placed bid on this auction. We divide by 5 chance for AhBuyer to place bid on it. (This make more challenge than ignore entry)
                    }
                    if (auction->buyout != 0) // Is the item directly buyable?
                    {
                        if (IsBuyableEntry(buyoutPrice, InGame_BuyPrice, MaxBuyablePrice, sameitem_itr->second.MinBuyPrice, MaxChance, config.m_FactionChance))
                        {
                            if (IsBidableEntry(bidPriceByItem, InGame_BuyPrice, MaxBidablePrice, sameitem_itr->second.MinBidPrice, MaxChance/2, config.m_FactionChance))
                            {
                                if (urand(0,5)==0) PlaceBidToEntry(auctionHouse, auction,bidPrice); else BuyEntry(auctionHouse, auction);
                            }
                            else
                            {
                                BuyEntry(auctionHouse, auction);
                            }
                        }
                        else
                        {
                            if (IsBidableEntry(bidPriceByItem, InGame_BuyPrice, MaxBidablePrice, sameitem_itr->second.MinBidPrice,MaxChance/2, config.m_FactionChance))
                                PlaceBidToEntry(auctionHouse, auction,bidPrice);
                        }
                    }
                    else // buyout = 0 mean only bid are possible
                    {
                        if (IsBidableEntry(bidPriceByItem, InGame_BuyPrice, MaxBidablePrice, sameitem_itr->second.MinBidPrice,MaxChance, config.m_FactionChance))
                            PlaceBidToEntry(auctionHouse, auction, bidPrice);
                    }
                }
                else
                {
                    sLog.outError("AHBot - buyer > Item guidLow (%u) have no prototype (AH entry = %u)!",itr->second.item_guidlow,itr->second.AhEntry);
                }
            }
            else
            {
                if (m_debug_Buyer) sLog.outString("AHBot - buyer > Item in entry %u is not reachable!",itr->second.AhEntry);
                config.m_CheckedEntry.erase(itr);
            }

            itr->second.lastchecked=Now;
        }
        else if (m_debug_Buyer) sLog.outString("In time interval wait for entry %u!",auction->Id);
    }
}

void AuctionHouseBot::Update()
{
    time_t _newrun = time(NULL);
    if ((getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO)==0) && (getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO)==0) && (getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO)==0) &&
        (getConfig(CONFIG_BOOL_AHBOT_BUYER_ALLIANCE_ENABLED)!=true) && (getConfig(CONFIG_BOOL_AHBOT_BUYER_HORDE_ENABLED)!=true) && (getConfig(CONFIG_BOOL_AHBOT_BUYER_NEUTRAL_ENABLED)!=true))
         return;
    m_Session = &WorldSession(0, NULL, SEC_PLAYER, true, 0, LOCALE_enUS);
    if (m_OperationSelector == 0)
    {
        if (getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO)>0)
        {
            if (m_debug_Seller) sLog.outString(">> Alliance selling ...");
            if (SetStat(m_AllianceConfig)!= 0) addNewAuctions(m_AllianceConfig);
            ++m_OperationSelector;
            return;
        }
        else ++m_OperationSelector;
    }
    if (m_OperationSelector == 1)
    {
        if (getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO)>0)
        {
            if (m_debug_Seller) sLog.outString(">> Horde selling ...");
            if (SetStat(m_HordeConfig)!= 0) addNewAuctions(m_HordeConfig);
            ++m_OperationSelector;
            return;
        }
        else ++m_OperationSelector;
    }
    if (m_OperationSelector == 2)
    {
        if (getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO)>0)
        {
            if (m_debug_Seller) sLog.outString(">> Neutral selling ...");
            if (SetStat(m_NeutralConfig)!= 0) addNewAuctions(m_NeutralConfig);
            ++m_OperationSelector;
            return;
        }
        else ++m_OperationSelector;
    }
    if (m_OperationSelector == 3)
    {
        if (getConfig(CONFIG_BOOL_AHBOT_BUYER_ALLIANCE_ENABLED))
        {
            if (m_debug_Buyer) sLog.outString(">> Alliance buying ...");
            if (GetBuyableEntry(m_AllianceConfig) > 0)
            {
                addNewAuctionBuyerBotBid(m_AllianceConfig);
            }
            ++m_OperationSelector;
            return;
        }
        else ++m_OperationSelector;
    }
    if (m_OperationSelector == 4)
    {
        if (getConfig(CONFIG_BOOL_AHBOT_BUYER_HORDE_ENABLED))
        {
            if (m_debug_Buyer) sLog.outString(">> Horde buying ...");
            if (GetBuyableEntry(m_HordeConfig) > 0)
            {
                addNewAuctionBuyerBotBid(m_HordeConfig);
            }
            ++m_OperationSelector;
            return;
        }
        else ++m_OperationSelector;
    }
    if (m_OperationSelector == 5)
    {
        if (getConfig(CONFIG_BOOL_AHBOT_BUYER_NEUTRAL_ENABLED))
        {
            if (m_debug_Buyer) sLog.outString(">> Neutral buying ...");
            if (GetBuyableEntry(m_NeutralConfig) > 0)
            {
                addNewAuctionBuyerBotBid(m_NeutralConfig);
            }
        }
    }
    if (m_OperationSelector>=5) m_OperationSelector=0; else ++m_OperationSelector;
}

void AuctionHouseBot::LoadConfig()
{
    if (getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO)>0)             LoadSellerValues(m_AllianceConfig);
    if (getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO)>0)                LoadSellerValues(m_HordeConfig);
    if (getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO)>0)              LoadSellerValues(m_NeutralConfig);

    m_AllianceConfig.BuyerEnabled = getConfig(CONFIG_BOOL_AHBOT_BUYER_ALLIANCE_ENABLED);
    m_HordeConfig.BuyerEnabled = getConfig(CONFIG_BOOL_AHBOT_BUYER_HORDE_ENABLED);
    m_NeutralConfig.BuyerEnabled = getConfig(CONFIG_BOOL_AHBOT_BUYER_NEUTRAL_ENABLED);
    if (m_AllianceConfig.BuyerEnabled)   LoadBuyerValues(m_AllianceConfig);
    if (m_HordeConfig.BuyerEnabled)      LoadBuyerValues(m_HordeConfig);
    if (m_NeutralConfig.BuyerEnabled)    LoadBuyerValues(m_NeutralConfig);
}

void AuctionHouseBot::setConfig(e_AHBOTConfigUInt32Values index, char const* fieldname, uint32 defvalue)
{
    setConfig(index, m_AhBotCfg.GetIntDefault(fieldname,defvalue));
}

void AuctionHouseBot::setConfig(e_AHBOTConfigBoolValues index, char const* fieldname, bool defvalue)
{
    setConfig(index, m_AhBotCfg.GetBoolDefault(fieldname,defvalue));
}

//Get AuctionHousebot configuration file
void AuctionHouseBot::GetConfigFromFile()
{
    //Check config file version
    if (m_AhBotCfg.GetIntDefault("ConfVersion", 0) != AUCTIONHOUSEBOT_CONF_VERSION)
        sLog.outError("AHBot> Configuration file version doesn't match expected version. Some config variables may be wrong or missing.");

    setConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO            , "AuctionHouseBot.Alliance.Items.Amount.Ratio"  , 0);
    setConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO               , "AuctionHouseBot.Horde.Items.Amount.Ratio"  , 0);
    setConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO             , "AuctionHouseBot.Neutral.Items.Amount.Ratio"  , 0);

    SetAHBotName( m_AhBotCfg.GetStringDefault("AuctionHouseBot.Name", "AHBot" ) );

    setConfig(CONFIG_BOOL_AHBOT_BUYER_ALLIANCE_ENABLED      , "AuctionHouseBot.Buyer.Alliance.Enabled"   , false);
    setConfig(CONFIG_BOOL_AHBOT_BUYER_HORDE_ENABLED         , "AuctionHouseBot.Buyer.Horde.Enabled"   , false);
    setConfig(CONFIG_BOOL_AHBOT_BUYER_NEUTRAL_ENABLED       , "AuctionHouseBot.Buyer.Neutral.Enabled"   , false);

    setConfig(CONFIG_BOOL_AHBOT_ITEMS_VENDOR                , "AuctionHouseBot.Items.Vendor"    , false);
    setConfig(CONFIG_BOOL_AHBOT_ITEMS_LOOT                  , "AuctionHouseBot.Items.Loot"      , true);
    setConfig(CONFIG_BOOL_AHBOT_ITEMS_MISC                  , "AuctionHouseBot.Items.Misc"      , false);

    setConfig(CONFIG_BOOL_AHBOT_BIND_NO                     , "AuctionHouseBot.Bind.No"         , true);
    setConfig(CONFIG_BOOL_AHBOT_BIND_PICKUP                 , "AuctionHouseBot.Bind.Pickup"     , false);
    setConfig(CONFIG_BOOL_AHBOT_BIND_EQUIP                  , "AuctionHouseBot.Bind.Equip"      , true);
    setConfig(CONFIG_BOOL_AHBOT_BIND_USE                    , "AuctionHouseBot.Bind.Use"        , true);
    setConfig(CONFIG_BOOL_AHBOT_BIND_QUEST                  , "AuctionHouseBot.Bind.Quest"      , false);

    setConfig(CONFIG_BOOL_AHBOT_BUYPRICE_SELLER             , "AuctionHouseBot.BuyPrice.Seller" , false);
    setConfig(CONFIG_BOOL_AHBOT_BUYPRICE_BUYER              , "AuctionHouseBot.BuyPrice.Buyer"  , false);

    setConfig(CONFIG_UINT32_AHBOT_ITEMS_PER_CYCLE_BOOST     , "AuctionHouseBot.ItemsPerCycle.Boost"   , 1000);
    setConfig(CONFIG_UINT32_AHBOT_ITEMS_PER_CYCLE_NORMAL    , "AuctionHouseBot.ItemsPerCycle.Normal"   , 10);

    setConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_ITEM_LEVEL       , "AuctionHouseBot.Items.ItemLevel.Min"         , 0);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_ITEM_LEVEL       , "AuctionHouseBot.Items.ItemLevel.Max"         , 0);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_REQ_LEVEL        , "AuctionHouseBot.Items.ReqLevel.Min"          , 0);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_REQ_LEVEL        , "AuctionHouseBot.Items.ReqLevel.Max"          , 0);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_SKILL_RANK       , "AuctionHouseBot.Items.ReqSkill.Min"          , 0);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_SKILL_RANK       , "AuctionHouseBot.Items.ReqSkill.Max"          , 0);

    setConfig(CONFIG_UINT32_AHBOT_ITEM_GREY_AMOUNT          , "AuctionHouseBot.Items.Amount.Grey"           , 0);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_WHITE_AMOUNT         , "AuctionHouseBot.Items.Amount.White"          , 2000);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_GREEN_AMOUNT         , "AuctionHouseBot.Items.Amount.Green"          , 2500);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_BLUE_AMOUNT          , "AuctionHouseBot.Items.Amount.Blue"           , 1500);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_PURPLE_AMOUNT        , "AuctionHouseBot.Items.Amount.Purple"         , 500);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_ORANGE_AMOUNT        , "AuctionHouseBot.Items.Amount.Orange"         , 0);
    setConfig(CONFIG_UINT32_AHBOT_ITEM_YELLOW_AMOUNT        , "AuctionHouseBot.Items.Amount.Yellow"         , 0);

    setConfig(CONFIG_UINT32_AHBOT_CLASS_CONSUMABLE_AMOUNT   , "AuctionHouseBot.Class.Consumable"            , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_CONTAINER_AMOUNT    , "AuctionHouseBot.Class.Container"             , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT       , "AuctionHouseBot.Class.Weapon"                , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_GEM_AMOUNT          , "AuctionHouseBot.Class.Gem"                   , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT        , "AuctionHouseBot.Class.Armor"                 , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_REAGENT_AMOUNT      , "AuctionHouseBot.Class.Reagent"               , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_PROJECTILE_AMOUNT   , "AuctionHouseBot.Class.Projectile"            , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT    , "AuctionHouseBot.Class.TradeGood"             , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_GENERIC_AMOUNT      , "AuctionHouseBot.Class.Generic"               , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_RECIPE_AMOUNT       , "AuctionHouseBot.Class.Recipe"                , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_QUIVER_AMOUNT       , "AuctionHouseBot.Class.Quiver"                , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_QUEST_AMOUNT        , "AuctionHouseBot.Class.Quest"                 , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_KEY_AMOUNT          , "AuctionHouseBot.Class.Key"                   , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_MISC_AMOUNT         , "AuctionHouseBot.Class.Misc"                  , 10);
    setConfig(CONFIG_UINT32_AHBOT_CLASS_GLYPH_AMOUNT        , "AuctionHouseBot.Class.Glyph"                 , 10);

    setConfig(CONFIG_UINT32_AHBOT_ALLIANCE_PRICE_RATIO      , "AuctionHouseBot.Alliance.Price.Ratio"        , 100);
    setConfig(CONFIG_UINT32_AHBOT_HORDE_PRICE_RATIO         , "AuctionHouseBot.Horde.Price.Ratio"           , 100);
    setConfig(CONFIG_UINT32_AHBOT_NEUTRAL_PRICE_RATIO       , "AuctionHouseBot.Neutral.Price.Ratio"         , 100);

    setConfig(CONFIG_UINT32_AHBOT_MINTIME                   , "AuctionHouseBot.MinTime"                     , 1);
    setConfig(CONFIG_UINT32_AHBOT_MAXTIME                   , "AuctionHouseBot.MaxTime"                     , 72);

    setConfig(CONFIG_UINT32_AHBOT_BUYER_CHANCE_RATIO_ALLIANCE, "AuctionHouseBot.Buyer.Alliance.Chance.Ratio" , 3);
    setConfig(CONFIG_UINT32_AHBOT_BUYER_CHANCE_RATIO_HORDE  , "AuctionHouseBot.Buyer.Horde.Chance.Ratio"    , 3);
    setConfig(CONFIG_UINT32_AHBOT_BUYER_CHANCE_RATIO_NEUTRAL, "AuctionHouseBot.Buyer.Neutral.Chance.Ratio"  , 3);
    setConfig(CONFIG_UINT32_AHBOT_BUYER_RECHECK_INTERVAL    , "AuctionHouseBot.Buyer.Recheck.Interval"      , 20);

    m_debug_Seller = m_AhBotCfg.GetIntDefault("AuctionHouseBot.DEBUG.Seller", 0);
    m_debug_Buyer = m_AhBotCfg.GetIntDefault("AuctionHouseBot.DEBUG.Buyer", 0);
}

void AuctionHouseBot::Initialize()
{
    sLog.outString("");
    sLog.outString("-------------------------------");
    sLog.outString("------- AuctionHouseBot -------");
    sLog.outString("-------------------------------");
    sLog.outString("");
    sLog.outString("AHBot> New CORE by Cyberium (Original by Xeross, Naicisum, ChrisK, Paradox)");
    sLog.outString("AHBot> Includes AHBuyer by Kerbe and Paradox (beta)");
    char const* cfg_file = _AUCTIONHOUSEBOT_CONFIG;
    if (!m_AhBotCfg.SetSource(cfg_file))
    {
        sLog.outError("AHBot> Unable to open configuration file(%s). AHBOT is Disabled.",_AUCTIONHOUSEBOT_CONFIG);
        setConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO, 0);
        setConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO, 0);
        setConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO, 0);
        return;
    }
    else
        sLog.outString("AHBot> Using configuration file %s",_AUCTIONHOUSEBOT_CONFIG);
    GetConfigFromFile();

    if ((getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO)==0) && (getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO)==0) && (getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO)==0) &&
        (getConfig(CONFIG_BOOL_AHBOT_BUYER_ALLIANCE_ENABLED)!=true) && (getConfig(CONFIG_BOOL_AHBOT_BUYER_HORDE_ENABLED)!=true) && (getConfig(CONFIG_BOOL_AHBOT_BUYER_NEUTRAL_ENABLED)!=true))
    {
        sLog.outString("All feature of AuctionHouseBot are disabled! (If you want to use it please set config in 'mangos.conf')");
        return;
    }
    if ((getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO)==0) && (getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO)==0) && (getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO)==0))
    {
        sLog.outString("AuctionHouseBot SELLER is disabled! (If you want to use it please set config in 'mangos.conf')");
        return;
    }
    if ((getConfig(CONFIG_BOOL_AHBOT_BUYER_ALLIANCE_ENABLED)!=true) && (getConfig(CONFIG_BOOL_AHBOT_BUYER_HORDE_ENABLED)!=true) && (getConfig(CONFIG_BOOL_AHBOT_BUYER_NEUTRAL_ENABLED)!=true))
    {
        sLog.outString("AuctionHouseBot BUYER is disabled! (If you want to use it please set config in 'mangos.conf')");
    }
        //load bidding interval
    m_CheckInterval=getConfig(CONFIG_UINT32_AHBOT_BUYER_RECHECK_INTERVAL)*60;
    if (m_CheckInterval<60) m_CheckInterval=60;
    if (m_CheckInterval>86400) m_CheckInterval=86400;
    if (m_debug_Buyer)
    {
        sLog.outString("Interval Between 2 Check = %u", m_CheckInterval);
    }

    m_ItemsPerCycleBoost = getConfig(CONFIG_UINT32_AHBOT_ITEMS_PER_CYCLE_BOOST);
    m_ItemsPerCycleNormal = getConfig(CONFIG_UINT32_AHBOT_ITEMS_PER_CYCLE_NORMAL);

    m_OperationSelector = 0;

    std::vector<uint32> npcItems;
    std::vector<uint32> lootItems;

    bool ItemAdded=false;
    QueryResult* results = (QueryResult*) NULL;
    char npcQuery[] = "SELECT distinct `item` FROM `npc_vendor`";
    sLog.outString(">> Loading npc...");
    results = WorldDatabase.PQuery(npcQuery);
    if (results != NULL)
    {
        barGoLink bar((int)results->GetRowCount());
        do
        {
            bar.step();
            Field* fields = results->Fetch();
            npcItems.push_back(fields[0].GetUInt32());

        } while (results->NextRow());

        delete results;
    }
    else
    {
        sLog.outString("AHBot> \"%s\" failed", npcQuery);
        return;
    }
    char lootQuery[] = "SELECT `item` FROM `creature_loot_template` UNION "
        "SELECT `item` FROM `disenchant_loot_template` UNION "
        "SELECT `item` FROM `fishing_loot_template` UNION "
        "SELECT `item` FROM `gameobject_loot_template` UNION "
        "SELECT `item` FROM `item_loot_template` UNION "
        "SELECT `item` FROM `milling_loot_template` UNION "
        "SELECT `item` FROM `pickpocketing_loot_template` UNION "
        "SELECT `item` FROM `prospecting_loot_template` UNION "
        "SELECT `item` FROM `skinning_loot_template`";

    sLog.outString("\n>> Loading Items...");
    results = WorldDatabase.PQuery(lootQuery);
    if (results != NULL)
    {

        barGoLink bar((int)results->GetRowCount());
        do
        {
            bar.step();
            Field* fields = results->Fetch();
            lootItems.push_back(fields[0].GetUInt32());
        } while (results->NextRow());
        delete results;
    }
    else
    {
        sLog.outString("AHBot> \"%s\" failed", lootQuery);
        return;
    }
    sLog.outString("\n>> Sorting and cleaning Items bases...");

    barGoLink bar(sItemStorage.MaxEntry);
    for (uint32 itemID = 0; itemID < sItemStorage.MaxEntry; itemID++)
    {
        ItemPrototype const* prototype = sObjectMgr.GetItemPrototype(itemID);
        bar.step();
        if (prototype == NULL)
            continue;

        switch (prototype->Bonding)
        {
        case 0:
            if (!getConfig(CONFIG_BOOL_AHBOT_BIND_NO))
                continue;
            break;
        case 1:
            if (!getConfig(CONFIG_BOOL_AHBOT_BIND_PICKUP))
                continue;
            break;
        case 2:
            if (!getConfig(CONFIG_BOOL_AHBOT_BIND_EQUIP))
                continue;
            break;
        case 3:
            if (!getConfig(CONFIG_BOOL_AHBOT_BIND_USE))
                continue;
            break;
        case 4:
            if (!getConfig(CONFIG_BOOL_AHBOT_BIND_QUEST))
                continue;
            break;
        default:
            continue;
            break;
        }

        if(getConfig(CONFIG_BOOL_AHBOT_BUYPRICE_SELLER))
        {
            if(prototype->BuyPrice == 0)
                continue;
        }
        else
        {
            if (prototype->SellPrice == 0)
                continue;
        }

        if ((prototype->Quality < 0) || (prototype->Quality > 6))
            continue;

        if (!getConfig(CONFIG_BOOL_AHBOT_ITEMS_VENDOR))
        {
            bool isVendorItem = false;

            for (unsigned int i = 0; (i < npcItems.size()) && (!isVendorItem); i++)
            {
                if (itemID == npcItems[i])
                    isVendorItem = true;
            }

            if (isVendorItem)
                continue;
        }

        if (!getConfig(CONFIG_BOOL_AHBOT_ITEMS_LOOT))
        {
            bool isLootItem = false;

            for (unsigned int i = 0; (i < lootItems.size()) && (!isLootItem); i++)
            {
                if (itemID == lootItems[i])
                    isLootItem = true;
            }

            if (isLootItem)
                continue;
        }

        if (!getConfig(CONFIG_BOOL_AHBOT_ITEMS_MISC))
        {
            bool isVendorItem = false;
            bool isLootItem = false;

            for (unsigned int i = 0; (i < npcItems.size()) && (!isVendorItem); i++)
            {
                if (itemID == npcItems[i])
                    isVendorItem = true;
            }
            for (unsigned int i = 0; (i < lootItems.size()) && (!isLootItem); i++)
            {
                if (itemID == lootItems[i])
                    isLootItem = true;
            }
            if ((!isLootItem) && (!isVendorItem))
                continue;
        }

        if ((prototype->Class==ITEM_CLASS_ARMOR)||(prototype->Class==ITEM_CLASS_WEAPON)||(prototype->Class==ITEM_CLASS_ARMOR))
        {
            if (((getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_ITEM_LEVEL)) > 0) && (prototype->ItemLevel < getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_ITEM_LEVEL))) continue;
            if (((getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_ITEM_LEVEL)) > 0) && (prototype->ItemLevel > getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_ITEM_LEVEL))) continue;
            if (((getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_REQ_LEVEL)) > 0) && (prototype->RequiredLevel < getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_REQ_LEVEL))) continue;
            if (((getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_REQ_LEVEL)) > 0) && (prototype->RequiredLevel > getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_REQ_LEVEL))) continue;
            if (((getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_SKILL_RANK)) > 0) && (prototype->RequiredSkill < getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_SKILL_RANK))) continue;
            if (((getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_SKILL_RANK)) > 0) && (prototype->RequiredSkill > getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_SKILL_RANK))) continue;
        }

        if ((prototype->Class==ITEM_CLASS_RECIPE)||(prototype->Class==ITEM_CLASS_CONSUMABLE)||(prototype->Class==ITEM_CLASS_PROJECTILE))
        {
            if (((getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_REQ_LEVEL)) > 0) && (prototype->RequiredLevel < getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_REQ_LEVEL))) continue;
            if (((getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_REQ_LEVEL)) > 0) && (prototype->RequiredLevel > getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_REQ_LEVEL))) continue;
            if (((getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_SKILL_RANK)) > 0) && (prototype->RequiredSkill < getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_SKILL_RANK))) continue;
            if (((getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_SKILL_RANK)) > 0) && (prototype->RequiredSkill > getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_SKILL_RANK))) continue;
        }

        m_ItemPool[prototype->Quality][prototype->Class].push_back(itemID);
        ItemAdded = true;

    }
    LoadConfig();
    if (!ItemAdded)
    {
        sLog.outString("\nAuctionHouseBot> Error, no items from xxxx_loot_template tables.");
        sLog.outString("AuctionHouseBot> AHBot is disabled!");
        setConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO, 0);
        setConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO, 0);
        setConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO, 0);
        return;
    }
    sLog.outString("\nItems loaded\tGrey\tWhite\tGreen\tBlue\tPurple\tOrange\tYellow");
    for (uint32 i=0; i<MAX_ITEM_CLASS;++i)
    {
        sLog.outString("%-11s\t%u\t%u\t%u\t%u\t%u\t%u\t%u",m_AllianceConfig.ItemInfos[0].ItemClassInfos[i].GetName().c_str(), m_ItemPool[0][i].size(),m_ItemPool[1][i].size(),m_ItemPool[2][i].size(),m_ItemPool[3][i].size(),m_ItemPool[4][i].size(),m_ItemPool[5][i].size(),m_ItemPool[6][i].size());
    }
    sLog.outString("\nAHBot> [AHBot-beta] is now loaded");
}

void AuctionHouseBot::LoadItemsQuantity(AHBConfig& config)
{
    if (getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO)>10000) setConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO,10000);
    if (getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO)>10000) setConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO,10000);
    if (getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO)>10000) setConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO,10000);

    switch(config.GetAHID())
    {
    case 2:
        config.ItemInfos[E_GREY].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_GREY_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO))/100));
        config.ItemInfos[E_WHITE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_WHITE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO))/100));
        config.ItemInfos[E_GREEN].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_GREEN_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO))/100));
        config.ItemInfos[E_BLUE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_BLUE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO))/100));
        config.ItemInfos[E_PURPLE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_PURPLE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO))/100));
        config.ItemInfos[E_ORANGE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_ORANGE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO))/100));
        config.ItemInfos[E_YELLOW].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_YELLOW_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO))/100));
        break;
    case 6:
        config.ItemInfos[E_GREY].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_GREY_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO))/100));
        config.ItemInfos[E_WHITE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_WHITE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO))/100));
        config.ItemInfos[E_GREEN].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_GREEN_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO))/100));
        config.ItemInfos[E_BLUE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_BLUE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO))/100));
        config.ItemInfos[E_PURPLE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_PURPLE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO))/100));
        config.ItemInfos[E_ORANGE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_ORANGE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO))/100));
        config.ItemInfos[E_YELLOW].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_YELLOW_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO))/100));
        break;
    case 7:
        config.ItemInfos[E_GREY].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_GREY_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_WHITE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_WHITE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_GREEN].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_GREEN_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_BLUE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_BLUE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_PURPLE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_PURPLE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_ORANGE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_ORANGE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_YELLOW].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_YELLOW_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        break;
    default:
        config.ItemInfos[E_GREY].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_GREY_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_WHITE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_WHITE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_GREEN].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_GREEN_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_BLUE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_BLUE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_PURPLE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_PURPLE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_ORANGE].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_ORANGE_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        config.ItemInfos[E_YELLOW].SetAmountOfItems(((getConfig(CONFIG_UINT32_AHBOT_ITEM_YELLOW_AMOUNT)*getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO))/100));
        break;
    }

    // Limit to 0..10 class amount ratio
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_CONSUMABLE_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_CONSUMABLE_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_CONTAINER_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_CONTAINER_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_GEM_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_GEM_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_REAGENT_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_REAGENT_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_PROJECTILE_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_PROJECTILE_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_GENERIC_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_GENERIC_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_RECIPE_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_RECIPE_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_QUIVER_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_QUIVER_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_QUEST_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_QUEST_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_KEY_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_KEY_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_MISC_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_MISC_AMOUNT,10);
    if (getConfig(CONFIG_UINT32_AHBOT_CLASS_GLYPH_AMOUNT)>10) setConfig(CONFIG_UINT32_AHBOT_CLASS_GLYPH_AMOUNT,10);

    // Set quantity wanted but only on possible item color
    // This avoid any no-exist class-color items selection by random items create function
    // ============================================================================================
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_CONSUMABLE].SetQuantityOfItems(0);
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_CONTAINER].SetQuantityOfItems(0);
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_WEAPON].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT));
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_GEM].SetQuantityOfItems(0);
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_ARMOR].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT));
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_REAGENT].SetQuantityOfItems(0);
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_PROJECTILE].SetQuantityOfItems(0);
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_TRADE_GOODS].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT));
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_GENERIC].SetQuantityOfItems(0);
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_RECIPE].SetQuantityOfItems(0);
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_QUIVER].SetQuantityOfItems(0);
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_QUEST].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_QUEST_AMOUNT));
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_KEY].SetQuantityOfItems(0);
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_MISC].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_MISC_AMOUNT));
    config.ItemInfos[E_GREY].ItemClassInfos[ITEM_CLASS_GLYPH].SetQuantityOfItems(0);

    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_CONSUMABLE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_CONSUMABLE_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_CONTAINER].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_CONTAINER_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_WEAPON].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_GEM].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_GEM_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_ARMOR].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_REAGENT].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_REAGENT_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_PROJECTILE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_PROJECTILE_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_TRADE_GOODS].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_GENERIC].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_GENERIC_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_RECIPE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_RECIPE_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_QUIVER].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_QUIVER_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_QUEST].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_QUEST_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_KEY].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_KEY_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_MISC].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_MISC_AMOUNT));
    config.ItemInfos[E_WHITE].ItemClassInfos[ITEM_CLASS_GLYPH].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_GLYPH_AMOUNT));

    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_CONSUMABLE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_CONSUMABLE_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_CONTAINER].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_CONTAINER_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_WEAPON].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_GEM].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_GEM_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_ARMOR].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_REAGENT].SetQuantityOfItems(0);
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_PROJECTILE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_PROJECTILE_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_TRADE_GOODS].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_GENERIC].SetQuantityOfItems(0);
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_RECIPE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_RECIPE_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_QUIVER].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_QUIVER_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_QUEST].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_QUEST_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_KEY].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_KEY_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_MISC].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_MISC_AMOUNT));
    config.ItemInfos[E_GREEN].ItemClassInfos[ITEM_CLASS_GLYPH].SetQuantityOfItems(0);

    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_CONSUMABLE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_CONSUMABLE_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_CONTAINER].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_CONTAINER_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_WEAPON].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_GEM].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_GEM_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_ARMOR].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_REAGENT].SetQuantityOfItems(0);
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_PROJECTILE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_PROJECTILE_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_TRADE_GOODS].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_GENERIC].SetQuantityOfItems(0);
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_RECIPE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_RECIPE_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_QUIVER].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_QUIVER_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_QUEST].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_QUEST_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_KEY].SetQuantityOfItems(0);
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_MISC].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_MISC_AMOUNT));
    config.ItemInfos[E_BLUE].ItemClassInfos[ITEM_CLASS_GLYPH].SetQuantityOfItems(0);

    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_CONSUMABLE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_CONSUMABLE_AMOUNT));
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_CONTAINER].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_CONTAINER_AMOUNT));
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_WEAPON].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT));
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_GEM].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_GEM_AMOUNT));
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_ARMOR].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT));
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_REAGENT].SetQuantityOfItems(0);
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_PROJECTILE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_PROJECTILE_AMOUNT));
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_TRADE_GOODS].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT));
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_GENERIC].SetQuantityOfItems(0);
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_RECIPE].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_RECIPE_AMOUNT));
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_QUIVER].SetQuantityOfItems(0);
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_QUEST].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_QUEST_AMOUNT));
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_KEY].SetQuantityOfItems(0);
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_MISC].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_MISC_AMOUNT));
    config.ItemInfos[E_PURPLE].ItemClassInfos[ITEM_CLASS_GLYPH].SetQuantityOfItems(0);

    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_CONSUMABLE].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_CONTAINER].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_WEAPON].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT));
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_GEM].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_ARMOR].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT));
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_REAGENT].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_PROJECTILE].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_TRADE_GOODS].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT));
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_GENERIC].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_RECIPE].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_QUIVER].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_QUEST].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_KEY].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_MISC].SetQuantityOfItems(0);
    config.ItemInfos[E_ORANGE].ItemClassInfos[ITEM_CLASS_GLYPH].SetQuantityOfItems(0);

    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_CONSUMABLE].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_CONTAINER].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_WEAPON].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT));
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_GEM].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_ARMOR].SetQuantityOfItems(getConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT));
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_REAGENT].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_PROJECTILE].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_TRADE_GOODS].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_GENERIC].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_RECIPE].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_QUIVER].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_QUEST].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_KEY].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_MISC].SetQuantityOfItems(0);
    config.ItemInfos[E_YELLOW].ItemClassInfos[ITEM_CLASS_GLYPH].SetQuantityOfItems(0);
    // ============================================================================================

    // Set the best value to get nearest amount of items wanted
    for (uint32 j=0; j<AHB_QUALITY_MAX; ++j)
    {
        uint32 indice = config.ItemInfos[j].GetAmountOfItems()/
            (getConfig(CONFIG_UINT32_AHBOT_CLASS_CONSUMABLE_AMOUNT) + getConfig(CONFIG_UINT32_AHBOT_CLASS_CONTAINER_AMOUNT) + getConfig(CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT) +
            getConfig(CONFIG_UINT32_AHBOT_CLASS_GEM_AMOUNT) + getConfig(CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT) + getConfig(CONFIG_UINT32_AHBOT_CLASS_REAGENT_AMOUNT) +
            getConfig(CONFIG_UINT32_AHBOT_CLASS_PROJECTILE_AMOUNT) + getConfig(CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT) + getConfig(CONFIG_UINT32_AHBOT_CLASS_GENERIC_AMOUNT) +
            getConfig(CONFIG_UINT32_AHBOT_CLASS_RECIPE_AMOUNT) + getConfig(CONFIG_UINT32_AHBOT_CLASS_QUIVER_AMOUNT) + getConfig(CONFIG_UINT32_AHBOT_CLASS_QUEST_AMOUNT) +
            getConfig(CONFIG_UINT32_AHBOT_CLASS_KEY_AMOUNT) + getConfig(CONFIG_UINT32_AHBOT_CLASS_MISC_AMOUNT) + getConfig(CONFIG_UINT32_AHBOT_CLASS_GLYPH_AMOUNT));
        for (uint32 i=0;i<MAX_ITEM_CLASS;++i)
        {
            config.ItemInfos[j].ItemClassInfos[i].SetAmountOfItems(indice);
        }
    }
}
void AuctionHouseBot::LoadSellerValues(AHBConfig& config)
{
    LoadItemsQuantity(config);
    uint32 PriceRatio;
    switch(config.GetAHID())
    {
    case 2: PriceRatio = getConfig(CONFIG_UINT32_AHBOT_ALLIANCE_PRICE_RATIO); break;
    case 6: PriceRatio = getConfig(CONFIG_UINT32_AHBOT_HORDE_PRICE_RATIO); break;
    case 7: PriceRatio = getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_PRICE_RATIO); break;
    default:
        PriceRatio = getConfig(CONFIG_UINT32_AHBOT_NEUTRAL_PRICE_RATIO);
        break;
    }
    config.ItemInfos[E_GREY].SetPriceRatio(PriceRatio);
    config.ItemInfos[E_WHITE].SetPriceRatio(PriceRatio);
    config.ItemInfos[E_GREEN].SetPriceRatio(PriceRatio);
    config.ItemInfos[E_BLUE].SetPriceRatio(PriceRatio);
    config.ItemInfos[E_PURPLE].SetPriceRatio(PriceRatio);
    config.ItemInfos[E_ORANGE].SetPriceRatio(PriceRatio);
    config.ItemInfos[E_YELLOW].SetPriceRatio(PriceRatio);

    //load min and max auction times
    config.SetMinTime(getConfig(CONFIG_UINT32_AHBOT_MINTIME));
    config.SetMaxTime(getConfig(CONFIG_UINT32_AHBOT_MAXTIME));
    if (m_debug_Seller)
    {
        sLog.outString("minTime = %u", config.GetMinTime());
        sLog.outString("maxTime = %u", config.GetMaxTime());
    }
    //load percentages

    if (m_debug_Seller)
    {
        sLog.outString("\nFor AH number %u",config.GetAHID());
        sLog.outString("GreyItems = %u", config.ItemInfos[E_GREY].GetAmountOfItems());
        sLog.outString("WhiteItems = %u", config.ItemInfos[E_WHITE].GetAmountOfItems());
        sLog.outString("GreenItems = %u", config.ItemInfos[E_GREEN].GetAmountOfItems());
        sLog.outString("BlueItems = %u", config.ItemInfos[E_BLUE].GetAmountOfItems());
        sLog.outString("PurpleItems = %u", config.ItemInfos[E_PURPLE].GetAmountOfItems());
        sLog.outString("OrangeItems = %u", config.ItemInfos[E_ORANGE].GetAmountOfItems());
        sLog.outString("YellowItems = %u", config.ItemInfos[E_YELLOW].GetAmountOfItems());
    }

}

void AuctionHouseBot::LoadBuyerValues(AHBConfig& config)
{
    uint32 FactionChance;
    switch(config.GetAHID())
    {
    case 2: config.Buyer_Price_Ratio = getConfig( CONFIG_UINT32_AHBOT_ALLIANCE_PRICE_RATIO )+50;
            FactionChance = getConfig( CONFIG_UINT32_AHBOT_BUYER_CHANCE_RATIO_ALLIANCE );
            break;
    case 6: config.Buyer_Price_Ratio = getConfig( CONFIG_UINT32_AHBOT_HORDE_PRICE_RATIO )+50;
            FactionChance = getConfig( CONFIG_UINT32_AHBOT_BUYER_CHANCE_RATIO_HORDE );
            break;
    case 7: config.Buyer_Price_Ratio = getConfig( CONFIG_UINT32_AHBOT_NEUTRAL_PRICE_RATIO )+50;
            FactionChance = getConfig( CONFIG_UINT32_AHBOT_BUYER_CHANCE_RATIO_NEUTRAL );
            break;
    default:
        sLog.outError("LoadBuyerValue()> Non identified faction!");
        config.Buyer_Price_Ratio = getConfig( CONFIG_UINT32_AHBOT_NEUTRAL_PRICE_RATIO )+50;
        FactionChance = getConfig( CONFIG_UINT32_AHBOT_BUYER_CHANCE_RATIO_NEUTRAL );
        break;
    }
    if (FactionChance == 0) FactionChance = 1;
    if (FactionChance > 100) FactionChance = 100;
    config.m_FactionChance=5000*FactionChance;
}

bool AuctionHouseBot::ReloadAllConfig()
{

    if (m_AhBotCfg.Reload())
    {
        GetConfigFromFile();
        LoadConfig();
        return true;
    }
    else
    {
        sLog.outError("Error while trying to reload config from file!");
        return false;
    }

}

void AuctionHouseBot::SetItemsRatio(uint32* al, uint32* ho, uint32* ne)
{
    if (al != NULL) setConfig(CONFIG_UINT32_AHBOT_ALLIANCE_RATIO, *al);
    if (ho != NULL) setConfig(CONFIG_UINT32_AHBOT_HORDE_RATIO, *ho);
    if (ne != NULL) setConfig(CONFIG_UINT32_AHBOT_NEUTRAL_RATIO, *ne);
    LoadItemsQuantity(m_AllianceConfig);
    LoadItemsQuantity(m_HordeConfig);
    LoadItemsQuantity(m_NeutralConfig);
}

void AuctionHouseBot::SetItemsAmount(uint32* grey_i, uint32* white_i, uint32* green_i, uint32* blue_i, uint32* purple_i, uint32* orange_i, uint32* yellow_i)
{
    if (grey_i != NULL) setConfig(CONFIG_UINT32_AHBOT_ITEM_GREY_AMOUNT,*grey_i);
    if (white_i != NULL) setConfig(CONFIG_UINT32_AHBOT_ITEM_WHITE_AMOUNT,*white_i);
    if (green_i != NULL) setConfig(CONFIG_UINT32_AHBOT_ITEM_GREEN_AMOUNT,*green_i);
    if (blue_i != NULL) setConfig(CONFIG_UINT32_AHBOT_ITEM_BLUE_AMOUNT,*blue_i);
    if (purple_i != NULL) setConfig(CONFIG_UINT32_AHBOT_ITEM_PURPLE_AMOUNT,*purple_i);
    if (orange_i != NULL) setConfig(CONFIG_UINT32_AHBOT_ITEM_ORANGE_AMOUNT,*orange_i);
    if (yellow_i != NULL) setConfig(CONFIG_UINT32_AHBOT_ITEM_YELLOW_AMOUNT,*yellow_i);
    LoadItemsQuantity(m_AllianceConfig);
    LoadItemsQuantity(m_HordeConfig);
    LoadItemsQuantity(m_NeutralConfig);
}

void AuctionHouseBot::PrepStatusInfos()
{

    AhBotInfos.clear();
    AhBotInfos.resize(3, std::vector < uint32 > (AHB_QUALITY_MAX));
    AllianceItemsCount=HordeItemsCount=NeutralItemsCount=0;
    for (uint32 i=0; i<3; i++)
    {
        uint32 faction;
        switch (i)
        {
        case 0 : faction = 2; break;
        case 1 : faction = 6; break;
        case 2 : faction = 7; break;
        default : faction = 7;
        }
        AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(faction);
        AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);
        for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin();itr != auctionHouse->GetAuctionsEnd();++itr)
        {
            AuctionEntry *Aentry = itr->second;
            Item *item = sAuctionMgr.GetAItem(Aentry->item_guidlow);
            if (item)
            {
                ItemPrototype const *prototype = item->GetProto();
                if (prototype)
                {
                    if ( Aentry->owner == GetAHBObjectGuid().GetRawValue()) // Add only ahbot items
                    {
                        ++AhBotInfos[i][prototype->Quality];
                        switch (i)
                        {
                        case 0 : ++AllianceItemsCount; break;
                        case 1 : ++HordeItemsCount; break;
                        case 2 : ++NeutralItemsCount; break;
                        }
                    }
                }
            }
        }
    }
}

void AuctionHouseBot::Rebuild(bool all)
{
    for (uint32 i=0; i<3; i++)
    {
        uint32 faction;
        switch (i)
        {
        case 0 : faction = 2; break;
        case 1 : faction = 6; break;
        case 2 : faction = 7; break;
        default : faction = 7;
        }
        AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(faction);
        if(!ahEntry) return;

        AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);
        if(!auctionHouse) return;

        for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin();itr != auctionHouse->GetAuctionsEnd();++itr)
        {
            if (itr->second->owner == GetAHBObjectGuid().GetRawValue())
            {
                if (all==true) itr->second->expire_time = sWorld.GetGameTime();
                else if (itr->second->bid == 0) itr->second->expire_time = sWorld.GetGameTime();
            }
        }
    }
}
