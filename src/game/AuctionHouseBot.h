#ifndef AUCTION_HOUSE_BOT_H
#define AUCTION_HOUSE_BOT_H

#include "World.h"
#include "Config/Config.h"
#include "ace/Vector_T.h"
#include "ObjectGuid.h"
#include "Item.h"

#define AHB_GREY        0
#define AHB_WHITE       1
#define AHB_GREEN       2
#define AHB_BLUE        3
#define AHB_PURPLE      4
#define AHB_ORANGE      5
#define AHB_YELLOW      6
#define AHB_GREY_TG     0
#define AHB_WHITE_TG    1
#define AHB_GREEN_TG    2
#define AHB_BLUE_TG     3
#define AHB_PURPLE_TG   4
#define AHB_ORANGE_TG   5
#define AHB_YELLOW_TG   6
#define AHB_GREY_I      7
#define AHB_WHITE_I     8
#define AHB_GREEN_I     9
#define AHB_BLUE_I      10
#define AHB_PURPLE_I    11
#define AHB_ORANGE_I    12
#define AHB_YELLOW_I    13


enum e_ahb_quality{
    E_GREY,
    E_WHITE,
    E_GREEN,
    E_BLUE,
    E_PURPLE,
    E_ORANGE,
    E_YELLOW
};

class AHBConfig
{
private:
    uint32 AHID;
    uint32 AHFID;
    uint32 minTime;
    uint32 maxTime;

    uint32 buyerPriceGrey;
    uint32 buyerPriceWhite;
    uint32 buyerPriceGreen;
    uint32 buyerPriceBlue;
    uint32 buyerPricePurple;
    uint32 buyerPriceOrange;
    uint32 buyerPriceYellow;
    uint32 buyerBiddingInterval;
    uint32 buyerBidsPerInterval;

    std::vector<uint32> m_AmountOfItems;
    std::vector<uint32> m_MaxItemsPrice;
    std::vector<uint32> m_MinItemsPrice;
    std::vector<uint32> m_MinBidPrice;
    std::vector<uint32> m_MaxBidPrice;
    std::vector<uint32> m_MaxStack;
    std::vector<uint32> m_MissItems;
public:
    AHBConfig(uint32 ahid)
    {
        AHID = ahid;
        switch(ahid)
        {
        case 2:
            AHFID = 55;
            break;
        case 6:
            AHFID = 29;
            break;
        case 7:
            AHFID = 120;
            break;
        default:
            AHFID = 120;
            break;
        }
        m_AmountOfItems.resize(7);
        m_MaxItemsPrice.resize(7);
        m_MinItemsPrice.resize(7);
        m_MinBidPrice.resize(7);
        m_MaxBidPrice.resize(7);
        m_MaxStack.resize(7);
        m_MissItems.resize(7);

    }
    AHBConfig()
    {
        AHFID = 120;
        m_AmountOfItems.resize(7);
        m_MaxItemsPrice.resize(7);
        m_MinItemsPrice.resize(7);
        m_MinBidPrice.resize(7);
        m_MaxBidPrice.resize(7);
        m_MaxStack.resize(7);
        m_MissItems.resize(7);
    }
    uint32 GetAHID()
    {
        return AHID;
    }
    uint32 GetAHFID()
    {
        return AHFID;
    }
    void SetMinTime(uint32 value)
    {
        minTime = value;
    }
    uint32 GetMinTime()
    {
        if (minTime < 1)
            return 1;
        else if ((maxTime) && (minTime > maxTime))
            return maxTime;
        else
            return minTime;
    }
    void SetMaxTime(uint32 value)
    {
        maxTime = value;
    }
    uint32 GetMaxTime()
    {
        return maxTime;
    }
    void SetAmountOfItems(uint32 cnt, e_ahb_quality AHB_ITEMS)
    {
        m_AmountOfItems[AHB_ITEMS]=cnt;
    }
    uint32 GetAmountOfItems(e_ahb_quality AHB_ITEMS)
    {
        return m_AmountOfItems[AHB_ITEMS];
    }
    uint32 GetPercentages(uint32 color)
    {
            return 0;
    }
    void SetMinPrice(uint32 value, e_ahb_quality AHB_ITEMS)
    {
        m_MinItemsPrice[AHB_ITEMS]=value;
    }
    uint32 GetMinPrice(e_ahb_quality AHB_ITEMS)
    {
        return (m_MinItemsPrice[AHB_ITEMS]==0) ? 100 + (((uint32) AHB_ITEMS)*50) :
               (m_MinItemsPrice[AHB_ITEMS]>m_MaxItemsPrice[AHB_ITEMS]) ? m_MaxItemsPrice[AHB_ITEMS] :
                m_MinItemsPrice[AHB_ITEMS];
    }

    void SetMaxPrice(uint32 value, e_ahb_quality AHB_ITEMS)
    {
        m_MaxItemsPrice[AHB_ITEMS]=value;
    }
    uint32 GetMaxPrice(e_ahb_quality AHB_ITEMS)
    {
        return (m_MaxItemsPrice[AHB_ITEMS]==0) ? 100 + (((uint32) AHB_ITEMS)*75) : m_MaxItemsPrice[AHB_ITEMS];
    }
    void SetMinBidPrice(uint32 value, e_ahb_quality AHB_ITEMS)
    {
        m_MinBidPrice[AHB_ITEMS]=value;
    }
    uint32 GetMinBidPrice(e_ahb_quality AHB_ITEMS)
    {
        return (m_MinBidPrice[AHB_ITEMS]>100) ? 100 : m_MinBidPrice[AHB_ITEMS];
    }
    void SetMaxBidPrice(uint32 value, e_ahb_quality AHB_ITEMS)
    {
        m_MaxBidPrice[AHB_ITEMS]=value;
    }
    uint32 GetMaxBidPrice(e_ahb_quality AHB_ITEMS)
    {
        return (m_MaxBidPrice[AHB_ITEMS]>100) ? 100 : m_MaxBidPrice[AHB_ITEMS];
    }
    void SetMaxStack(uint32 value, e_ahb_quality AHB_ITEMS)
    {
        m_MaxStack[AHB_ITEMS]=value;
    }
    uint32 GetMaxStack(e_ahb_quality AHB_ITEMS)
    {
        return m_MaxStack[AHB_ITEMS];
    }
    void SetBuyerPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            buyerPriceGrey = value;
            break;
        case AHB_WHITE:
            buyerPriceWhite = value;
            break;
        case AHB_GREEN:
            buyerPriceGreen = value;
            break;
        case AHB_BLUE:
            buyerPriceBlue = value;
            break;
        case AHB_PURPLE:
            buyerPricePurple = value;
            break;
        case AHB_ORANGE:
            buyerPriceOrange = value;
            break;
        case AHB_YELLOW:
            buyerPriceYellow = value;
            break;
        default:
            break;
        }
    }
    uint32 GetBuyerPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            return buyerPriceGrey;
            break;
        case AHB_WHITE:
            return buyerPriceWhite;
            break;
        case AHB_GREEN:
            return buyerPriceGreen;
            break;
        case AHB_BLUE:
            return buyerPriceBlue;
            break;
        case AHB_PURPLE:
            return buyerPricePurple;
            break;
        case AHB_ORANGE:
            return buyerPriceOrange;
            break;
        case AHB_YELLOW:
            return buyerPriceYellow;
            break;
        default:
            return 0;
            break;
        }
    }
    void SetBiddingInterval(uint32 value)
    {
        buyerBiddingInterval = value;
    }
    uint32 GetBiddingInterval()
    {
        return buyerBiddingInterval;
    }
    void CalculatePercents()
    {

    }
    uint32 GetPercents(uint32 color)
    {
 
            return 0;
     }
    void SetBidsPerInterval(uint32 value)
    {
        buyerBidsPerInterval = value;
    }
    uint32 GetBidsPerInterval()
    {
        return buyerBidsPerInterval;
    }
    void SetMissItems(uint32 value, e_ahb_quality AHB_ITEMS)
    {
        (m_AmountOfItems[AHB_ITEMS] > value ) ? m_MissItems[AHB_ITEMS]=m_AmountOfItems[AHB_ITEMS]-value: m_MissItems[AHB_ITEMS] = 0;
    }

    uint32 GetMissItems(e_ahb_quality AHB_ITEMS)
    {
        return m_MissItems[AHB_ITEMS];
    }
    ~AHBConfig()
    {
    }
};

class AuctionHouseBot
{
private:
    ACE_Vector<uint32> npcItems;
    ACE_Vector<uint32> lootItems;
    ACE_Vector<uint32> greyItemsBin;
    ACE_Vector<uint32> whiteItemsBin;
    ACE_Vector<uint32> greenItemsBin;
    ACE_Vector<uint32> blueItemsBin;
    ACE_Vector<uint32> purpleItemsBin;
    ACE_Vector<uint32> orangeItemsBin;
    ACE_Vector<uint32> yellowItemsBin;

    bool debug_Out;

    AHBConfig AllianceConfig;
    AHBConfig HordeConfig;
    AHBConfig NeutralConfig;

    time_t _lastrun_a;
    time_t _lastrun_h;
    time_t _lastrun_n;

    uint32 ItemsPerCycle;

    ObjectGuid m_FakeGuid;

    inline uint32 minValue(uint32 a, uint32 b) { return a <= b ? a : b; };
    void    addNewAuctions(AHBConfig& config);
    void    addNewAuctionBuyerBotBid(AHBConfig *config, WorldSession *session);
    void    SetStat(AHBConfig& config);
    bool    getRandomArray( AHBConfig& config, std::vector<uint32>& ra, const std::vector<uint32>& addedItem  );
    void    SetPricesOfItem(const Item *item, AHBConfig& config, uint32& buyp, uint32& bidp, uint32& stackcnt, e_ahb_quality AHB_ITEMS);
public:
    AuctionHouseBot();
    ~AuctionHouseBot();
    void Update();
    void Initialize();
    void LoadDbConfig();
    void LoadValues(AHBConfig& config);
    void Commands(uint32, uint32, uint32, char*);
    void debug();
    ObjectGuid GetAHBObjectGuid() { return m_FakeGuid; };
};

#define auctionbot MaNGOS::Singleton<AuctionHouseBot>::Instance()

#endif
