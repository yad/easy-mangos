#include "AuctionHouseBot.h"
#include "ProgressBar.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1( AuctionHouseBot );

using namespace std;

AuctionHouseBot::AuctionHouseBot()
{
    AllianceConfig = AHBConfig(2);
    HordeConfig = AHBConfig(6);
    NeutralConfig = AHBConfig(7);
    m_FakeGuid.Set(std::numeric_limits< uint32 >::max());
}

AuctionHouseBot::~AuctionHouseBot()
{

}

//Set statisque of items on one faction.
void AuctionHouseBot::SetStat(AHBConfig& config)
{
    std::vector<uint32> ItemsInAH(7);

    AuctionHouseEntry const* ahEntry = sAuctionMgr.GetAuctionHouseEntryByFaction(config.GetAHFID());
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
                if ( Aentry->owner == GetAHBObjectGuid().GetRawValue()) 
                    ++ItemsInAH[prototype->Quality];
            }
        }
    }

    for (uint32 i=0;i<7;++i) config.SetMissItems(ItemsInAH[((e_ahb_quality) i)],((e_ahb_quality) i)); 

    if (debug_Out)
    {
        sLog.outString("Missed grey=%u, white=%u, green=%u, blue=%u, purple=%u, orange=%u, yellow=%u in AH=%u",
            config.GetMissItems(E_GREY),config.GetMissItems(E_WHITE),config.GetMissItems(E_GREEN),config.GetMissItems(E_BLUE),config.GetMissItems(E_PURPLE),config.GetMissItems(E_ORANGE),config.GetMissItems(E_YELLOW),
            config.GetAHID());
    }
}

bool AuctionHouseBot::getRandomArray( AHBConfig& config, std::vector<uint32>& ra, const std::vector<uint32>& addedItem  )
{
    ra.clear();
    bool Ok=false;
    if ((config.GetMissItems(E_GREY)     >   addedItem[E_GREY])      && (greyItemsBin.size()>0))         {ra.push_back(E_GREY); Ok=true;}
    if ((config.GetMissItems(E_WHITE)    >   addedItem[E_WHITE])     && (whiteItemsBin.size()>0))        {ra.push_back(E_WHITE); Ok=true;}
    if ((config.GetMissItems(E_GREEN)    >   addedItem[E_GREEN])     && (greenItemsBin.size()>0))        {ra.push_back(E_GREEN); Ok=true;}
    if ((config.GetMissItems(E_BLUE)     >   addedItem[E_BLUE])      && (blueItemsBin.size()>0))         {ra.push_back(E_BLUE); Ok=true;}
    if ((config.GetMissItems(E_PURPLE)   >   addedItem[E_PURPLE])    && (purpleItemsBin.size()>0))       {ra.push_back(E_PURPLE); Ok=true;}
    if ((config.GetMissItems(E_ORANGE)   >   addedItem[E_ORANGE])    && (orangeItemsBin.size()>0))       {ra.push_back(E_ORANGE); Ok=true;}
    if ((config.GetMissItems(E_YELLOW)   >   addedItem[E_YELLOW])    && (yellowItemsBin.size()>0))       {ra.push_back(E_YELLOW); Ok=true;}
    return Ok;
}

void AuctionHouseBot::SetPricesOfItem(const Item *item,AHBConfig& config, uint32& buyp, uint32& bidp, uint32& stackcnt, e_ahb_quality AHB_ITEMS)
{
    if (config.GetMaxStack(AHB_ITEMS) != 0)
    {
        stackcnt = urand(1, minValue(item->GetMaxStackCount(), config.GetMaxStack(AHB_ITEMS)));
    }
    buyp *= urand(config.GetMinPrice(AHB_ITEMS), config.GetMaxPrice(AHB_ITEMS)) * stackcnt;
    buyp /= 100;
    bidp = buyp * urand(config.GetMinBidPrice(AHB_ITEMS), config.GetMaxBidPrice(AHB_ITEMS));
    bidp /= 100;
}

void AuctionHouseBot::addNewAuctions(AHBConfig& config)
{
    if (!sWorld.getConfig(CONFIG_BOOL_AHBOT_SELLER_ENABLED))
        return;
    SetStat(config);
    AuctionHouseEntry const* ahEntry = sAuctionMgr.GetAuctionHouseEntryByFaction(config.GetAHFID());
    AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);

    std::vector<uint32> RandArray;
    std::vector<uint32> ItemsAdded(7);
    uint32 items=ItemsPerCycle;
    // only insert a few at a time, so as not to peg the processor
    while (getRandomArray(config,RandArray, ItemsAdded) && (items>0))
    {
        --items;
        uint32 itemID = 0;
        switch ((e_ahb_quality)(RandArray[urand(1,RandArray.size())-1]))
        {
            case E_GREY     : itemID = greyItemsBin[urand(1, greyItemsBin.size())-1]; ++ItemsAdded[E_GREY]; break;
            case E_WHITE    : itemID = whiteItemsBin[urand(1, whiteItemsBin.size())-1]; ++ItemsAdded[E_WHITE]; break;
            case E_GREEN    : itemID = greenItemsBin[urand(1, greenItemsBin.size())-1]; ++ItemsAdded[E_GREEN]; break;
            case E_BLUE     : itemID = blueItemsBin[urand(1, blueItemsBin.size())-1]; ++ItemsAdded[E_BLUE]; break;
            case E_PURPLE   : itemID = purpleItemsBin[urand(1, purpleItemsBin.size())-1]; ++ItemsAdded[E_PURPLE]; break;
            case E_ORANGE   : itemID = orangeItemsBin[urand(1, orangeItemsBin.size())-1]; ++ItemsAdded[E_ORANGE]; break;
            case E_YELLOW   : itemID = yellowItemsBin[urand(1, yellowItemsBin.size())-1]; ++ItemsAdded[E_YELLOW]; break;
            default:
                sLog.outString("AuctionHouseBot> itemID Switch - Default Reached");
            break;
        }

        if (itemID == 0)
        {
            if (debug_Out)
                sLog.outString("AuctionHouseBot> Item::CreateItem() - Unable to find item");
            continue;
        }

        ItemPrototype const* prototype = sObjectMgr.GetItemPrototype(itemID);
        if (prototype == NULL)
        {
            sLog.outString("AuctionHouseBot> Huh?!?! prototype == NULL");
            continue;
        }

        Item* item = Item::CreateItem(itemID, 1);
        if (item == NULL)
        {
            sLog.outString("AuctionHouseBot> Item::CreateItem() returned NULL");
            return;
        }

        uint32 randomPropertyId = Item::GenerateItemRandomPropertyId(itemID);
        if (randomPropertyId != 0)
            item->SetItemRandomPropertiesNoUpdate(randomPropertyId);

        uint32 buyoutPrice;
        uint32 bidPrice = 0;
        uint32 stackCount = urand(1, item->GetMaxStackCount());
        if(sWorld.getConfig(CONFIG_BOOL_AHBOT_BUYPRICE_SELLER))
            buyoutPrice  = prototype->BuyPrice * item->GetCount();
        else
            buyoutPrice  = prototype->SellPrice * item->GetCount();

        SetPricesOfItem(item, config, buyoutPrice, bidPrice, stackCount, ((e_ahb_quality) prototype->Quality));
        item->SetCount(stackCount);
        item->SetOwnerGUID(GetAHBObjectGuid().GetRawValue());
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
        item->SaveToDB();
        sAuctionMgr.AddAItem(item);
        auctionHouse->AddAuction(auctionEntry);
        auctionEntry->SaveToDB();
    }
}

void AuctionHouseBot::addNewAuctionBuyerBotBid(AHBConfig *config, WorldSession *session)
{
    if (!sWorld.getConfig(CONFIG_BOOL_AHBOT_BUYER_ENABLED))
        return;

    // Fetches content of selected AH
    AuctionHouseEntry const* ahEntry = sAuctionMgr.GetAuctionHouseEntryByFaction(config->GetAHFID());
    AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);
    vector<uint32> possibleBids;

    for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin();itr != auctionHouse->GetAuctionsEnd();++itr)
    {
        // Check if the auction is ours
        // if it is, we skip this iteration.
        if (itr->second->owner == std::numeric_limits< int >::max())
        {
            continue;
        }
        // Check that we haven't bidded in this auction already.
        if (itr->second->bidder != std::numeric_limits< int >::max())
        {
            uint32 tmpdata = itr->second->Id;
            possibleBids.push_back(tmpdata);
        }
    }

    for (uint32 count = 0;count < config->GetBidsPerInterval();++count)
    {

        // Do we have anything to bid? If not, stop here.
        if (possibleBids.empty())
        {
            count = config->GetBidsPerInterval();
            continue;
        }

        // Choose random auction from possible auctions
        uint32 vectorPos = urand(0, possibleBids.size() - 1);
        uint32 auctionID = possibleBids[vectorPos];

        // Erase the auction from the vector to prevent bidding on item in next iteration.
        vector<uint32>::iterator iter = possibleBids.begin();
        advance(iter, vectorPos);
        possibleBids.erase(iter);

        // from auctionhousehandler.cpp, creates auction pointer & player pointer
        AuctionEntry* auction = auctionHouse->GetAuction(auctionID);
        if (!auction)
        {
            sLog.outError("Item doesn't exists, perhaps bought already?");
            continue;
        }

        // get exact item information
        Item *pItem = sAuctionMgr.GetAItem(auction->item_guidlow);
        if (!pItem)
        {
            sLog.outError("Item doesn't exists, perhaps bought already?");
            continue;
        }

        // get item prototype
        ItemPrototype const* prototype = sObjectMgr.GetItemPrototype(auction->item_template);

        // check which price we have to use, startbid or if it is bidded already
        if (debug_Out)
        {
            sLog.outError("Auction Number: %u", auction->Id);
            sLog.outError("Item Template: %u", auction->item_template);
            sLog.outError("Buy Price: %u", prototype->BuyPrice);
            sLog.outError("Sell Price: %u", prototype->SellPrice);
            sLog.outError("Quality: %u", prototype->Quality);
        }
        uint32 currentprice;
        if (auction->bid)
        {
            currentprice = auction->bid;
            if (debug_Out)
                sLog.outError("Current Price: %u", auction->bid);
        }
        else
        {
            currentprice = auction->startbid;
            if (debug_Out)
                sLog.outError("Current Price: %u", auction->startbid);
        }
        uint32 bidprice;

        // Prepare portion from maximum bid
        uint32 tmprate2 = urand(0, 100);
        double tmprate = static_cast<double>(tmprate2);
        if (debug_Out)
            sLog.outError("tmprate: %f", tmprate);

        double bidrate = tmprate / 100;
        if (debug_Out)
            sLog.outError("bidrate: %f", bidrate);

        long double bidMax = 0;

        // check that bid has acceptable value and take bid based on vendorprice, stacksize and quality
        if(sWorld.getConfig(CONFIG_BOOL_AHBOT_BUYPRICE_BUYER))
        {
            switch (prototype->Quality)
            {
            case 0:
                if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREY))
                {
                    bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREY);
                }
                break;
            case 1:
                if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_WHITE))
                {
                    bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_WHITE);
                }
                break;
            case 2:
                if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREEN))
                {
                    bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREEN);
                }
                break;
            case 3:
                if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_BLUE))
                {
                    bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_BLUE);
                }
                break;
            case 4:
                if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_PURPLE))
                {
                    bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_PURPLE);
                }
                break;
            case 5:
                if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_ORANGE))
                {
                    bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_ORANGE);
                }
                break;
            case 6:
                if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_YELLOW))
                {
                    bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_YELLOW);
                }
                break;
            default:
                // quality is something it shouldn't be, let's get out of here
                if (debug_Out)
                    sLog.outError("bidMax(fail): %f", bidMax);
                continue;
                break;
            }
        }
        else
        {
            switch (prototype->Quality)
            {
            case 0:
                if (currentprice < prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREY))
                {
                    bidMax = prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREY);
                }
                break;
            case 1:
                if (currentprice < prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_WHITE))
                {
                    bidMax = prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_WHITE);
                }
                break;
            case 2:
                if (currentprice < prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREEN))
                {
                    bidMax = prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREEN);
                }
                break;
            case 3:
                if (currentprice < prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_BLUE))
                {
                    bidMax = prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_BLUE);
                }
                break;
            case 4:
                if (currentprice < prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_PURPLE))
                {
                    bidMax = prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_PURPLE);
                }
                break;
            case 5:
                if (currentprice < prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_ORANGE))
                {
                    bidMax = prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_ORANGE);
                }
                break;
            case 6:
                if (currentprice < prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_YELLOW))
                {
                    bidMax = prototype->SellPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_YELLOW);
                }
                break;
            default:
                // quality is something it shouldn't be, let's get out of here
                if (debug_Out)
                    sLog.outError("bidMax(fail): %f", bidMax);
                continue;
                break;
            }
        }

        if (debug_Out)
            sLog.outError("bidMax(succeed): %f", bidMax);

        // check some special items, and do recalculating to their prices
        switch (prototype->Class)
        {
            // ammo
        case 6:
            bidMax = 0;
            break;
        default:
            break;
        }

        if (bidMax == 0)
            continue;   // quality check failed to get bidmax, let's get out of here

        // Calculate our bid
        long double bidvalue = currentprice + ((bidMax - currentprice) * bidrate);
        if (debug_Out)
            sLog.outError("bidvalue: %f", bidvalue);

        // Convert to uint32
        bidprice = static_cast<uint32>(bidvalue);
        if (debug_Out)
            sLog.outError("bidprice: %u", bidprice);

        // Check our bid is high enough to be valid. If not, correct it to minimum.
        if ((currentprice + auction->GetAuctionOutBid()) > bidprice)
        {
            bidprice = currentprice + auction->GetAuctionOutBid();
            if (debug_Out)
                sLog.outError("bidprice(>): %u", bidprice);
        }

        // Check wether we do normal bid, or buyout
        if ((bidprice < auction->buyout) || (auction->buyout == 0))
        {

            if (auction->bidder > 0)
            {
                if (auction->bidder == GetAHBObjectGuid().GetRawValue())
                {
                    //pl->ModifyMoney(-int32(price - auction->bid));
                }
                else
                {
                    // mail to last bidder and return money
                    session->SendAuctionOutbiddedMail(auction , bidprice);
                    //pl->ModifyMoney(-int32(price));
                }
            }

            auction->bidder = GetAHBObjectGuid().GetRawValue();
            auction->bid = bidprice;

            // Saving auction into database
            CharacterDatabase.PExecute("UPDATE auction SET buyguid = '%u',lastbid = '%u' WHERE id = '%u'", auction->bidder, auction->bid, auction->Id);
        }
        else
        {
            //buyout
            if (GetAHBObjectGuid().GetRawValue() == auction->bidder)
            {
                //pl->ModifyMoney(-int32(auction->buyout - auction->bid));
            }
            else
            {
                //pl->ModifyMoney(-int32(auction->buyout));
                if (auction->bidder)
                {
                    session->SendAuctionOutbiddedMail(auction, auction->buyout);
                }
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
            auction->DeleteFromDB();
        }
    }
}

void AuctionHouseBot::Update()
{
    time_t _newrun = time(NULL);
    if ((!sWorld.getConfig(CONFIG_BOOL_AHBOT_SELLER_ENABLED)) && (!sWorld.getConfig(CONFIG_BOOL_AHBOT_BUYER_ENABLED)))
        return;

    WorldSession _session(0, NULL, SEC_PLAYER, true, 0, LOCALE_enUS);

    // Add New Bids
    if (!sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        addNewAuctions(AllianceConfig);
        if (((_newrun - _lastrun_a) > (AllianceConfig.GetBiddingInterval() * 60)) && (AllianceConfig.GetBidsPerInterval() > 0))
        {
            addNewAuctionBuyerBotBid(&AllianceConfig, &_session);
            _lastrun_a = _newrun;
        }

        addNewAuctions(HordeConfig);
        if (((_newrun - _lastrun_h) > (HordeConfig.GetBiddingInterval() *60)) && (HordeConfig.GetBidsPerInterval() > 0))
        {
            addNewAuctionBuyerBotBid(&HordeConfig, &_session);
            _lastrun_h = _newrun;
        }
    }
    addNewAuctions(NeutralConfig);
    if (((_newrun - _lastrun_n) > (NeutralConfig.GetBiddingInterval() * 60)) && (NeutralConfig.GetBidsPerInterval() > 0))
    {
        addNewAuctionBuyerBotBid(&NeutralConfig, &_session);
        _lastrun_n = _newrun;
    }
}

void AuctionHouseBot::LoadDbConfig()
{
    if (!sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        LoadValues(AllianceConfig);
        LoadValues(HordeConfig);
    }
    LoadValues(NeutralConfig);
}

void AuctionHouseBot::Initialize()
{
    debug_Out = sConfig.GetIntDefault("AuctionHouseBot.DEBUG", 0);
    ItemsPerCycle = sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEMS_CYCLE);
    LoadDbConfig();
    if (sWorld.getConfig(CONFIG_BOOL_AHBOT_SELLER_ENABLED))
    {
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
            sLog.outString("AuctionHouseBot> \"%s\" failed", npcQuery);
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

        sLog.outString(">> Loading Items...");
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
            sLog.outString("AuctionHouseBot> \"%s\" failed", lootQuery);
            return;
        }
        sLog.outString(">> Sorting and cleaning Items bases...");

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
                if (!sWorld.getConfig(CONFIG_BOOL_AHBOT_BIND_NO))
                    continue;
                break;
            case 1:
                if (!sWorld.getConfig(CONFIG_BOOL_AHBOT_BIND_PICKUP))
                    continue;
                break;
            case 2:
                if (!sWorld.getConfig(CONFIG_BOOL_AHBOT_BIND_EQUIP))
                    continue;
                break;
            case 3:
                if (!sWorld.getConfig(CONFIG_BOOL_AHBOT_BIND_USE))
                    continue;
                break;
            case 4:
                if (!sWorld.getConfig(CONFIG_BOOL_AHBOT_BIND_QUEST))
                    continue;
                break;
            default:
                continue;
                break;
            }

            if(sWorld.getConfig(CONFIG_BOOL_AHBOT_BUYPRICE_SELLER))
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

            if (!sWorld.getConfig(CONFIG_BOOL_AHBOT_ITEMS_VENDOR))
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

            if (!sWorld.getConfig(CONFIG_BOOL_AHBOT_ITEMS_LOOT))
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

            if (!sWorld.getConfig(CONFIG_BOOL_AHBOT_ITEMS_MISC))
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

            // Disable Items with an item level lower than X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_ITEM_LEVEL)) && (prototype->Class != ITEM_CLASS_TRADE_GOODS) && (prototype->ItemLevel < sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_ITEM_LEVEL)))
                continue;

            // Disable Items with an item level higher than X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_ITEM_LEVEL)) && (prototype->Class != ITEM_CLASS_TRADE_GOODS) && (prototype->ItemLevel > sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_ITEM_LEVEL)))
                continue;

            // Disable trade goods with an item level lower than X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MIN_ITEM_LEVEL)) && (prototype->Class == ITEM_CLASS_TRADE_GOODS) && (prototype->ItemLevel < sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MIN_ITEM_LEVEL)))
                continue;

            // Disable trade goods with an item level higher than X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MAX_ITEM_LEVEL)) && (prototype->Class == ITEM_CLASS_TRADE_GOODS) && (prototype->ItemLevel > sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MAX_ITEM_LEVEL)))
                continue;

            // Disable items for level lower than X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_REQ_LEVEL)) && (prototype->Class != ITEM_CLASS_TRADE_GOODS) && (prototype->RequiredLevel < sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_REQ_LEVEL)))
                continue;

            // Disable items for level higher than X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_REQ_LEVEL)) && (prototype->Class != ITEM_CLASS_TRADE_GOODS) && (prototype->RequiredLevel > sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_REQ_LEVEL)))
                continue;

            // Disable trade goods for level lower than X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MIN_REQ_LEVEL)) && (prototype->Class == ITEM_CLASS_TRADE_GOODS) && (prototype->RequiredLevel < sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MIN_REQ_LEVEL)))
                continue;

            // Disable trade goods for level higher than X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MAX_REQ_LEVEL)) && (prototype->Class == ITEM_CLASS_TRADE_GOODS) && (prototype->RequiredLevel > sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MAX_REQ_LEVEL)))
                continue;

            // Disable items with skill requirement lower then X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_SKILL_RANK)) && (prototype->Class != ITEM_CLASS_TRADE_GOODS) && (prototype->RequiredSkillRank < sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MIN_SKILL_RANK)))
                continue;

            // Disable items with skill requirement higher then X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_SKILL_RANK)) && (prototype->Class != ITEM_CLASS_TRADE_GOODS) && (prototype->RequiredSkillRank < sWorld.getConfig(CONFIG_UINT32_AHBOT_ITEM_MAX_SKILL_RANK)))
                continue;

            // Disable trade goods with skill requirement lower then X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MIN_SKILL_RANK)) && (prototype->Class == ITEM_CLASS_TRADE_GOODS) && (prototype->RequiredSkillRank < sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MIN_SKILL_RANK)))
                continue;

            // Disable trade goods with skill requirement higher then X
            if ((sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MAX_SKILL_RANK)) && (prototype->Class == ITEM_CLASS_TRADE_GOODS) && (prototype->RequiredSkillRank < sWorld.getConfig(CONFIG_UINT32_AHBOT_TG_MAX_SKILL_RANK)))
                continue;

            switch (prototype->Quality)
            {
            case 0: greyItemsBin.push_back(itemID);     break;
            case 1: whiteItemsBin.push_back(itemID);    break;
            case 2: greenItemsBin.push_back(itemID);    break;
            case 3: blueItemsBin.push_back(itemID);     break;
            case 4: purpleItemsBin.push_back(itemID);   break;
            case 5: orangeItemsBin.push_back(itemID);   break;
            case 6: yellowItemsBin.push_back(itemID);   break;
            }
        }

        if (
            (greyItemsBin.size() == 0) &&
            (whiteItemsBin.size() == 0) &&
            (greenItemsBin.size() == 0) &&
            (blueItemsBin.size() == 0) &&
            (purpleItemsBin.size() == 0) &&
            (orangeItemsBin.size() == 0) &&
            (yellowItemsBin.size() == 0)
            )
        {
            sLog.outString("AuctionHouseBot> Error, no items from xxxx_loot_template tables.");
            sLog.outString("AuctionHouseBot> AHBot is disabled!");
            sWorld.setConfig(CONFIG_BOOL_AHBOT_SELLER_ENABLED, false);
            return;
        }
        sLog.outString("-------------------------------");
        sLog.outString("------- AuctionHouseBot -------");
        sLog.outString("-------------------------------");
        sLog.outString("loaded %d grey items", greyItemsBin.size());
        sLog.outString("loaded %d white items", whiteItemsBin.size());
        sLog.outString("loaded %d green items", greenItemsBin.size());
        sLog.outString("loaded %d blue items", blueItemsBin.size());
        sLog.outString("loaded %d purple items", purpleItemsBin.size());
        sLog.outString("loaded %d orange items", orangeItemsBin.size());
        sLog.outString("loaded %d yellow items", yellowItemsBin.size());
    }
    sLog.outString("AuctionHouseBot> [AHBot-x004] is now loaded");
    sLog.outString("AuctionHouseBot> updated by Cyberium from Xeross git");
    sLog.outString("AuctionHouseBot> (Original by Naicisum, ChrisK, Paradox)");
    sLog.outString("AuctionHouseBot> Includes AHBuyer by Kerbe and Paradox");
}

void AuctionHouseBot::Commands(uint32 command, uint32 ahMapID, uint32 col, char* args)
{
    AHBConfig *config;
    switch (ahMapID)
    {
    case 2:
        config = &AllianceConfig;
        break;
    case 6:
        config = &HordeConfig;
        break;
    case 7:
        config = &NeutralConfig;
        break;
    }
    std::string color;
    switch (col)
    {
    case AHB_GREY:
        color = "grey";
        break;
    case AHB_WHITE:
        color = "white";
        break;
    case AHB_GREEN:
        color = "green";
        break;
    case AHB_BLUE:
        color = "blue";
        break;
    case AHB_PURPLE:
        color = "purple";
        break;
    case AHB_ORANGE:
        color = "orange";
        break;
    case AHB_YELLOW:
        color = "yellow";
        break;
    default:
        break;
    }
    switch (command)
    {
    case 0:     //ahexpire
        {
            AuctionHouseEntry const* ahEntry = sAuctionMgr.GetAuctionHouseEntryByFaction(config->GetAHFID());
            AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);

            AuctionHouseObject::AuctionEntryMap::iterator itr;
            itr = auctionHouse->GetAuctionsBegin();

            while (itr != auctionHouse->GetAuctionsEnd())
            {
                if (itr->second->owner == GetAHBObjectGuid().GetRawValue())
                    itr->second->expire_time = sWorld.GetGameTime();

                ++itr;
            }
        }break;
    case 1:     //min items
        {

        }break;
    case 2:     //max items
        {

        }break;
    case 3:     //min time
        {
            char * param1 = strtok(args, " ");
            uint32 minTime = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET mintime = '%u' WHERE auctionhouse = '%u'", minTime, ahMapID);
            config->SetMinTime(minTime);
        }break;
    case 4:     //max time
        {
            char * param1 = strtok(args, " ");
            uint32 maxTime = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET maxtime = '%u' WHERE auctionhouse = '%u'", maxTime, ahMapID);
            config->SetMaxTime(maxTime);
        }break;
    case 5:     //percentages
        {
            char * param1 = strtok(args, " ");
            char * param2 = strtok(NULL, " ");
            char * param3 = strtok(NULL, " ");
            char * param4 = strtok(NULL, " ");
            char * param5 = strtok(NULL, " ");
            char * param6 = strtok(NULL, " ");
            char * param7 = strtok(NULL, " ");

            uint32 greyi = (uint32) strtoul(param1, NULL, 0);
            uint32 whitei = (uint32) strtoul(param2, NULL, 0);
            uint32 greeni = (uint32) strtoul(param3, NULL, 0);
            uint32 bluei = (uint32) strtoul(param4, NULL, 0);
            uint32 purplei = (uint32) strtoul(param5, NULL, 0);
            uint32 orangei = (uint32) strtoul(param6, NULL, 0);
            uint32 yellowi = (uint32) strtoul(param7, NULL, 0);

            CharacterDatabase.BeginTransaction();
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET percentgreyitems = '%u' WHERE auctionhouse = '%u'", greyi, ahMapID);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET percentwhiteitems = '%u' WHERE auctionhouse = '%u'", whitei, ahMapID);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET percentgreenitems = '%u' WHERE auctionhouse = '%u'", greeni, ahMapID);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET percentblueitems = '%u' WHERE auctionhouse = '%u'", bluei, ahMapID);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET percentpurpleitems = '%u' WHERE auctionhouse = '%u'", purplei, ahMapID);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET percentorangeitems = '%u' WHERE auctionhouse = '%u'", orangei, ahMapID);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET percentyellowitems = '%u' WHERE auctionhouse = '%u'", yellowi, ahMapID);
            CharacterDatabase.CommitTransaction();
            //config->SetPercentages(greytg, whitetg, greentg, bluetg, purpletg, orangetg, yellowtg, greyi, whitei, greeni, bluei, purplei, orangei, yellowi);
        }break;
    case 6:     //min prices
        {
            char * param1 = strtok(args, " ");
            uint32 minPrice = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET minprice%s = '%u' WHERE auctionhouse = '%u'",color.c_str(), minPrice, ahMapID);
            //config->SetMinPrice(col, minPrice);
        }break;
    case 7:     //max prices
        {
            char * param1 = strtok(args, " ");
            uint32 maxPrice = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET maxprice%s = '%u' WHERE auctionhouse = '%u'",color.c_str(), maxPrice, ahMapID);
            //config->SetMaxPrice(col, maxPrice);
        }break;
    case 8:     //min bid price
        {
            char * param1 = strtok(args, " ");
            uint32 minBidPrice = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET minbidprice%s = '%u' WHERE auctionhouse = '%u'",color.c_str(), minBidPrice, ahMapID);
            //config->SetMinBidPrice(col, minBidPrice);
        }break;
    case 9:     //max bid price
        {
            char * param1 = strtok(args, " ");
            uint32 maxBidPrice = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET maxbidprice%s = '%u' WHERE auctionhouse = '%u'",color.c_str(), maxBidPrice, ahMapID);
            //config->SetMaxBidPrice(col, maxBidPrice);
        }break;
    case 10:        //max stacks
        {
            char * param1 = strtok(args, " ");
            uint32 maxStack = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET maxstack%s = '%u' WHERE auctionhouse = '%u'",color.c_str(), maxStack, ahMapID);
            //config->SetMaxStack(col, maxStack);
        }break;
    case 11:        //buyer bid prices
        {
            char * param1 = strtok(args, " ");
            uint32 buyerPrice = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET buyerprice%s = '%u' WHERE auctionhouse = '%u'",color.c_str(), buyerPrice, ahMapID);
            config->SetBuyerPrice(col, buyerPrice);
        }break;
    case 12:        //buyer bidding interval
        {
            char * param1 = strtok(args, " ");
            uint32 bidInterval = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET buyerbiddinginterval = '%u' WHERE auctionhouse = '%u'", bidInterval, ahMapID);
            config->SetBiddingInterval(bidInterval);
        }break;
    case 13:        //buyer bids per interval
        {
            char * param1 = strtok(args, " ");
            uint32 bidsPerInterval = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auction_house_bot_config SET buyerbidsperinterval = '%u' WHERE auctionhouse = '%u'", bidsPerInterval, ahMapID);
            config->SetBidsPerInterval(bidsPerInterval);
        }break;
    default:
        break;
    }
}

void AuctionHouseBot::LoadValues(AHBConfig& config)
{
    if (sWorld.getConfig(CONFIG_BOOL_AHBOT_SELLER_ENABLED))
    {
        sLog.outString("In Load Values...");
        config.SetAmountOfItems(CharacterDatabase.PQuery("SELECT greyitems FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(),E_GREY);
        config.SetAmountOfItems(CharacterDatabase.PQuery("SELECT whiteitems FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(),E_WHITE);
        config.SetAmountOfItems(CharacterDatabase.PQuery("SELECT greenitems FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(),E_GREEN);
        config.SetAmountOfItems(CharacterDatabase.PQuery("SELECT blueitems FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(),E_BLUE);
        config.SetAmountOfItems(CharacterDatabase.PQuery("SELECT purpleitems FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(),E_PURPLE);
        config.SetAmountOfItems(CharacterDatabase.PQuery("SELECT orangeitems FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(),E_ORANGE);
        config.SetAmountOfItems(CharacterDatabase.PQuery("SELECT yellowitems FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(),E_YELLOW);
        
        //load min and max auction times
        config.SetMinTime(CharacterDatabase.PQuery("SELECT mintime FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        config.SetMaxTime(CharacterDatabase.PQuery("SELECT maxtime FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        if (debug_Out)
        {
            sLog.outError("minTime = %u", config.GetMinTime());
            sLog.outError("maxTime = %u", config.GetMaxTime());
        }
        sLog.outString("Load percentages...");
        //load percentages

        if (debug_Out)
        {
            sLog.outError("GreyItems = %u", config.GetAmountOfItems(E_GREY));
            sLog.outError("WhiteItems = %u", config.GetAmountOfItems(E_WHITE));
            sLog.outError("GreenItems = %u", config.GetAmountOfItems(E_GREEN));
            sLog.outError("BlueItems = %u", config.GetAmountOfItems(E_BLUE));
            sLog.outError("PurpleItems = %u", config.GetAmountOfItems(E_PURPLE));
            sLog.outError("OrangeItems = %u", config.GetAmountOfItems(E_ORANGE));
            sLog.outError("YellowItems = %u", config.GetAmountOfItems(E_YELLOW));
        }
        //load min and max prices
        config.SetMinPrice( CharacterDatabase.PQuery("SELECT minpricegrey FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_GREY);
        config.SetMaxPrice( CharacterDatabase.PQuery("SELECT maxpricegrey FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_GREY);
        if (debug_Out)
        {
            sLog.outError("minPriceGrey = %u", config.GetMinPrice(E_GREY));
            sLog.outError("maxPriceGrey = %u", config.GetMaxPrice(E_GREY));
        }
        config.SetMinPrice( CharacterDatabase.PQuery("SELECT minpricewhite FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_WHITE);
        config.SetMaxPrice( CharacterDatabase.PQuery("SELECT maxpricewhite FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_WHITE);
        if (debug_Out)
        {
            sLog.outError("minPriceWhite = %u", config.GetMinPrice(E_WHITE));
            sLog.outError("maxPriceWhite = %u", config.GetMaxPrice(E_WHITE));
        }
        config.SetMinPrice( CharacterDatabase.PQuery("SELECT minpricegreen FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_GREEN);
        config.SetMaxPrice( CharacterDatabase.PQuery("SELECT maxpricegreen FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_GREEN);
        if (debug_Out)
        {
            sLog.outError("minPriceGreen = %u", config.GetMinPrice(E_GREEN));
            sLog.outError("maxPriceGreen = %u", config.GetMaxPrice(E_GREEN));
        }
        config.SetMinPrice( CharacterDatabase.PQuery("SELECT minpriceblue FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_BLUE);
        config.SetMaxPrice( CharacterDatabase.PQuery("SELECT maxpriceblue FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_BLUE);
        if (debug_Out)
        {
            sLog.outError("minPriceBlue = %u", config.GetMinPrice(E_BLUE));
            sLog.outError("maxPriceBlue = %u", config.GetMaxPrice(E_BLUE));
        }
        config.SetMinPrice( CharacterDatabase.PQuery("SELECT minpricepurple FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_PURPLE);
        config.SetMaxPrice( CharacterDatabase.PQuery("SELECT maxpricepurple FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_PURPLE);
        if (debug_Out)
        {
            sLog.outError("minPricePurple = %u", config.GetMinPrice(E_PURPLE));
            sLog.outError("maxPricePurple = %u", config.GetMaxPrice(E_PURPLE));
        }
        config.SetMinPrice( CharacterDatabase.PQuery("SELECT minpriceorange FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_ORANGE);
        config.SetMaxPrice( CharacterDatabase.PQuery("SELECT maxpriceorange FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_ORANGE);
        if (debug_Out)
        {
            sLog.outError("minPriceOrange = %u", config.GetMinPrice(E_ORANGE));
            sLog.outError("maxPriceOrange = %u", config.GetMaxPrice(E_ORANGE));
        }
        config.SetMinPrice( CharacterDatabase.PQuery("SELECT minpriceyellow FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_YELLOW);
        config.SetMaxPrice( CharacterDatabase.PQuery("SELECT maxpriceyellow FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_YELLOW);
        if (debug_Out)
        {
            sLog.outError("minPriceYellow = %u", config.GetMinPrice(E_YELLOW));
            sLog.outError("maxPriceYellow = %u", config.GetMaxPrice(E_YELLOW));
        }
        //load min and max bid prices
        config.SetMinBidPrice( CharacterDatabase.PQuery("SELECT minbidpricegrey FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_GREY);
        if (debug_Out)
        {
            sLog.outError("minBidPriceGrey = %u", config.GetMinBidPrice(E_GREY));
        }
        config.SetMaxBidPrice( CharacterDatabase.PQuery("SELECT maxbidpricegrey FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_GREY);
        if (debug_Out)
        {
            sLog.outError("maxBidPriceGrey = %u", config.GetMaxBidPrice(E_GREY));
        }
        config.SetMinBidPrice( CharacterDatabase.PQuery("SELECT minbidpricewhite FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_WHITE);
        if (debug_Out)
        {
            sLog.outError("minBidPriceWhite = %u", config.GetMinBidPrice(E_WHITE));
        }
        config.SetMaxBidPrice( CharacterDatabase.PQuery("SELECT maxbidpricewhite FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_WHITE);
        if (debug_Out)
        {
            sLog.outError("maxBidPriceWhite = %u", config.GetMaxBidPrice(E_WHITE));
        }
        config.SetMinBidPrice( CharacterDatabase.PQuery("SELECT minbidpricegreen FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_GREEN);
        if (debug_Out)
        {
            sLog.outError("minBidPriceGreen = %u", config.GetMinBidPrice(E_GREEN));
        }
        config.SetMaxBidPrice( CharacterDatabase.PQuery("SELECT maxbidpricegreen FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_GREEN);
        if (debug_Out)
        {
            sLog.outError("maxBidPriceGreen = %u", config.GetMaxBidPrice(E_GREEN));
        }
        config.SetMinBidPrice( CharacterDatabase.PQuery("SELECT minbidpriceblue FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_BLUE);
        if (debug_Out)
        {
            sLog.outError("minBidPriceBlue = %u", config.GetMinBidPrice(E_BLUE));
        }
        config.SetMaxBidPrice( CharacterDatabase.PQuery("SELECT maxbidpriceblue FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_BLUE);
        if (debug_Out)
        {
            sLog.outError("maxBidPriceBlue = %u", config.GetMinBidPrice(E_BLUE));
        }
        config.SetMinBidPrice( CharacterDatabase.PQuery("SELECT minbidpricepurple FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_PURPLE);
        if (debug_Out)
        {
            sLog.outError("minBidPricePurple = %u", config.GetMinBidPrice(E_PURPLE));
        }
        config.SetMaxBidPrice( CharacterDatabase.PQuery("SELECT maxbidpricepurple FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_PURPLE);
        if (debug_Out)
        {
            sLog.outError("maxBidPricePurple = %u", config.GetMaxBidPrice(E_PURPLE));
        }
        config.SetMinBidPrice( CharacterDatabase.PQuery("SELECT minbidpriceorange FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_ORANGE);
        if (debug_Out)
        {
            sLog.outError("minBidPriceOrange = %u", config.GetMinBidPrice(E_ORANGE));
        }
        config.SetMaxBidPrice( CharacterDatabase.PQuery("SELECT maxbidpriceorange FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_ORANGE);
        if (debug_Out)
        {
            sLog.outError("maxBidPriceOrange = %u", config.GetMaxBidPrice(E_ORANGE));
        }
        config.SetMinBidPrice( CharacterDatabase.PQuery("SELECT minbidpriceyellow FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_YELLOW);
        if (debug_Out)
        {
            sLog.outError("minBidPriceYellow = %u", config.GetMinBidPrice(E_YELLOW));
        }
        config.SetMaxBidPrice( CharacterDatabase.PQuery("SELECT maxbidpriceyellow FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_YELLOW);
        if (debug_Out)
        {sLog.outError("maxBidPriceYellow = %u", config.GetMaxBidPrice(E_YELLOW));
        }
        //load max stacks
        config.SetMaxStack( CharacterDatabase.PQuery("SELECT maxstackgrey FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_GREY);
        if (debug_Out)
        {
            sLog.outError("maxStackGrey = %u", config.GetMaxStack(E_GREY));
        }
        config.SetMaxStack( CharacterDatabase.PQuery("SELECT maxstackwhite FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_WHITE);
        if (debug_Out)
        {
            sLog.outError("maxStackWhite = %u", config.GetMaxStack(E_WHITE));
        }
        config.SetMaxStack( CharacterDatabase.PQuery("SELECT maxstackgreen FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_GREEN);
        if (debug_Out)
        {
            sLog.outError("maxStackGreen = %u", config.GetMaxStack(E_GREEN));
        }
        config.SetMaxStack( CharacterDatabase.PQuery("SELECT maxstackblue FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_BLUE);
        if (debug_Out)
        {
            sLog.outError("maxStackBlue = %u", config.GetMaxStack(E_BLUE));
        }
        config.SetMaxStack( CharacterDatabase.PQuery("SELECT maxstackpurple FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_PURPLE);
        if (debug_Out)
        {
            sLog.outError("maxStackPurple = %u", config.GetMaxStack(E_PURPLE));
        }
        config.SetMaxStack( CharacterDatabase.PQuery("SELECT maxstackorange FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_ORANGE);
        if (debug_Out)
        {
            sLog.outError("maxStackOrange = %u", config.GetMaxStack(E_ORANGE));
        }
        config.SetMaxStack( CharacterDatabase.PQuery("SELECT maxstackyellow FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32(), E_YELLOW);
        if (debug_Out)
        {
            sLog.outError("maxStackYellow = %u", config.GetMaxStack(E_YELLOW));
        }
    }
    if (sWorld.getConfig(CONFIG_BOOL_AHBOT_BUYER_ENABLED))
    {
        //load buyer bid prices
        config.SetBuyerPrice(AHB_GREY, CharacterDatabase.PQuery("SELECT buyerpricegrey FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        config.SetBuyerPrice(AHB_WHITE, CharacterDatabase.PQuery("SELECT buyerpricewhite FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        config.SetBuyerPrice(AHB_GREEN, CharacterDatabase.PQuery("SELECT buyerpricegreen FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        config.SetBuyerPrice(AHB_BLUE, CharacterDatabase.PQuery("SELECT buyerpriceblue FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        config.SetBuyerPrice(AHB_PURPLE, CharacterDatabase.PQuery("SELECT buyerpricepurple FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        config.SetBuyerPrice(AHB_ORANGE, CharacterDatabase.PQuery("SELECT buyerpriceorange FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        config.SetBuyerPrice(AHB_YELLOW, CharacterDatabase.PQuery("SELECT buyerpriceyellow FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        if (debug_Out)
        {
            sLog.outError("buyerPriceGrey = %u", config.GetBuyerPrice(AHB_GREY));
            sLog.outError("buyerPriceWhite = %u", config.GetBuyerPrice(AHB_WHITE));
            sLog.outError("buyerPriceGreen = %u", config.GetBuyerPrice(AHB_GREEN));
            sLog.outError("buyerPriceBlue = %u", config.GetBuyerPrice(AHB_BLUE));
            sLog.outError("buyerPricePurple = %u", config.GetBuyerPrice(AHB_PURPLE));
            sLog.outError("buyerPriceOrange = %u", config.GetBuyerPrice(AHB_ORANGE));
            sLog.outError("buyerPriceYellow = %u", config.GetBuyerPrice(AHB_YELLOW));
        }
        //load bidding interval
        config.SetBiddingInterval(CharacterDatabase.PQuery("SELECT buyerbiddinginterval FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        if (debug_Out)
        {
            sLog.outError("buyerBiddingInterval = %u", config.GetBiddingInterval());
        }
        //load bids per interval
        config.SetBidsPerInterval(CharacterDatabase.PQuery("SELECT buyerbidsperinterval FROM auction_house_bot_config WHERE auctionhouse = %u",config.GetAHID())->Fetch()->GetUInt32());
        if (debug_Out)
        {
            sLog.outError("buyerBidsPerInterval = %u", config.GetBidsPerInterval());
        }
    }
}

void AuctionHouseBot::debug()
{
}
