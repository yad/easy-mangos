#ifndef AUCTION_HOUSE_BOT_H
#define AUCTION_HOUSE_BOT_H

#include "../World.h"
#include "Config/Config.h"
#include "ace/Vector_T.h"
#include "../ObjectGuid.h"
#include "../AuctionHouseMgr.h"
#include "../Item.h"

enum e_ahb_quality
{
    E_GREY = 0,
    E_WHITE,
    E_GREEN,
    E_BLUE,
    E_PURPLE,
    E_ORANGE,
    E_YELLOW,
    AHB_QUALITY_MAX
};

enum e_AHBOTConfigUInt32Values
{
    CONFIG_UINT32_AHBOT_MAXTIME,
    CONFIG_UINT32_AHBOT_MINTIME,
    CONFIG_UINT32_AHBOT_ITEMS_PER_CYCLE_BOOST,
    CONFIG_UINT32_AHBOT_ITEMS_PER_CYCLE_NORMAL,
    CONFIG_UINT32_AHBOT_ALLIANCE_RATIO,
    CONFIG_UINT32_AHBOT_HORDE_RATIO,
    CONFIG_UINT32_AHBOT_NEUTRAL_RATIO,
    CONFIG_UINT32_AHBOT_ITEM_MIN_ITEM_LEVEL,
    CONFIG_UINT32_AHBOT_ITEM_MAX_ITEM_LEVEL,
    CONFIG_UINT32_AHBOT_ITEM_MIN_REQ_LEVEL,
    CONFIG_UINT32_AHBOT_ITEM_MAX_REQ_LEVEL,
    CONFIG_UINT32_AHBOT_ITEM_MIN_SKILL_RANK,
    CONFIG_UINT32_AHBOT_ITEM_MAX_SKILL_RANK,
    CONFIG_UINT32_AHBOT_ITEM_GREY_AMOUNT,
    CONFIG_UINT32_AHBOT_ITEM_WHITE_AMOUNT,
    CONFIG_UINT32_AHBOT_ITEM_GREEN_AMOUNT,
    CONFIG_UINT32_AHBOT_ITEM_BLUE_AMOUNT,
    CONFIG_UINT32_AHBOT_ITEM_PURPLE_AMOUNT,
    CONFIG_UINT32_AHBOT_ITEM_ORANGE_AMOUNT,
    CONFIG_UINT32_AHBOT_ITEM_YELLOW_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_CONSUMABLE_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_CONTAINER_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_WEAPON_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_GEM_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_ARMOR_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_REAGENT_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_PROJECTILE_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_TRADEGOOD_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_GENERIC_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_RECIPE_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_QUIVER_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_QUEST_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_KEY_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_MISC_AMOUNT,
    CONFIG_UINT32_AHBOT_CLASS_GLYPH_AMOUNT,
    CONFIG_UINT32_AHBOT_ALLIANCE_PRICE_RATIO,
    CONFIG_UINT32_AHBOT_HORDE_PRICE_RATIO,
    CONFIG_UINT32_AHBOT_NEUTRAL_PRICE_RATIO,
    CONFIG_UINT32_AHBOT_BUYER_CHANCE_RATIO_ALLIANCE,
    CONFIG_UINT32_AHBOT_BUYER_CHANCE_RATIO_HORDE,
    CONFIG_UINT32_AHBOT_BUYER_CHANCE_RATIO_NEUTRAL,
    CONFIG_UINT32_AHBOT_BUYER_RECHECK_INTERVAL,
    CONFIG_UINT32_AHBOT_UINT32_COUNT
};

enum e_AHBOTConfigBoolValues
{
    CONFIG_BOOL_AHBOT_BUYER_ALLIANCE_ENABLED,
    CONFIG_BOOL_AHBOT_BUYER_HORDE_ENABLED,
    CONFIG_BOOL_AHBOT_BUYER_NEUTRAL_ENABLED,
    CONFIG_BOOL_AHBOT_ITEMS_VENDOR,
    CONFIG_BOOL_AHBOT_ITEMS_LOOT,
    CONFIG_BOOL_AHBOT_ITEMS_MISC,
    CONFIG_BOOL_AHBOT_BIND_NO,
    CONFIG_BOOL_AHBOT_BIND_PICKUP,
    CONFIG_BOOL_AHBOT_BIND_EQUIP,
    CONFIG_BOOL_AHBOT_BIND_USE,
    CONFIG_BOOL_AHBOT_BIND_QUEST,
    CONFIG_BOOL_AHBOT_BUYPRICE_SELLER,
    CONFIG_BOOL_AHBOT_BUYPRICE_BUYER,
    CONFIG_BOOL_AHBOT_DEBUG_SELLER,
    CONFIG_BOOL_AHBOT_DEBUG_BUYER,
    CONFIG_BOOL_AHBOT_SELLER_ENABLED,
    CONFIG_BOOL_AHBOT_BUYER_ENABLED,
    CONFIG_BOOL_AHBOT_LOCKBOX_ENABLED,
    CONFIG_UINT32_AHBOT_BOOL_COUNT
};

struct s_BuyerItemEval
{
    uint32  AhEntry;
    uint32  item_guidlow;
    time_t  lastchecked;
    time_t  LastExist;
    s_BuyerItemEval()
    {
        lastchecked=0;
    }
};

struct s_BuyerItemInfo
{
    uint32  ItemCount;
    double  BuyPrice;
    double  BidPrice;
    uint32  MinBuyPrice;
    uint32  MinBidPrice;

    s_BuyerItemInfo()
    {
        ItemCount=0;
        BuyPrice=0;
        BidPrice=0;
        MinBuyPrice=0;
        MinBidPrice=0;
    }
};

typedef std::map< uint32 , s_BuyerItemInfo > t_itemInfo;
typedef std::map< uint32, s_BuyerItemEval > t_checkEntryMap;

struct AHB_Buyer_Config
{
    AHB_Buyer_Config(uint32 ahid=7) { AHID = ahid; }

    uint32          AHID;
    t_itemInfo      SameItemInfo;
    t_checkEntryMap CheckedEntry;
    uint32          FactionChance;
    bool            BuyerEnabled;
    uint32          Buyer_Price_Ratio;
};

struct s_randomArray
{
    uint32 color;
    uint32 itemclass;
};

struct s_SellerItemClassInfo
{
    uint32 AmountOfItems;
    uint32 MissItems;
    uint32 Quantity;

    s_SellerItemClassInfo()
    {
        AmountOfItems=0;
        MissItems=0;
        Quantity=0;
    }
};

struct s_SellerItemInfo
{
    uint32 AmountOfItems;
    uint32 MaxItemsPrice;
    uint32 MinItemsPrice;
    uint32 MinBidPrice;
    uint32 MaxBidPrice;
    uint32 MaxStack;
    uint32 MissItems;
    uint32 buyerPrice;
    uint32 PriceRatio;

    std::vector<s_SellerItemClassInfo> ItemClassInfos;

    s_SellerItemInfo()
    {
        ItemClassInfos.resize(MAX_ITEM_CLASS);
        AmountOfItems=0;
        MaxItemsPrice=0;
        MinItemsPrice=0;
        MinBidPrice=0;
        MaxBidPrice=0;
        MaxStack=0;
        MissItems=0;
        buyerPrice=0;
        PriceRatio=0;
    }
};

class AHB_Seller_Config
{
public:

    uint32 LastMissedItem;

    AHB_Seller_Config(uint32 ahid=7)
    {
        m_AHID = ahid;
        m_ItemInfo.resize(AHB_QUALITY_MAX);
    }

    uint32 GetAHID()
    {
        return m_AHID;
    }
        void SetMinTime(uint32 value)
    {
        m_minTime = value;
    }
    uint32 GetMinTime()
    {
        if (m_minTime < 1)
            return 1;
        else if ((m_maxTime) && (m_minTime > m_maxTime))
            return m_maxTime;
        else
            return m_minTime;
    }

    void        SetMaxTime(uint32 value) { m_maxTime = value; }
    uint32      GetMaxTime() { return m_maxTime; }
    // Data access classified by item class and item quality //
    void        SetItemsAmountPerClass(e_ahb_quality quality, ItemClass itemclass, uint32 amount) { m_ItemInfo[quality].ItemClassInfos[itemclass].AmountOfItems=amount * m_ItemInfo[quality].ItemClassInfos[itemclass].Quantity; }
    uint32      GetItemsAmountPerClass(e_ahb_quality quality, ItemClass itemclass) { return m_ItemInfo[quality].ItemClassInfos[itemclass].AmountOfItems; }
    void        SetItemsQuantityPerClass(e_ahb_quality quality, ItemClass itemclass, uint32 qty) { m_ItemInfo[quality].ItemClassInfos[itemclass].Quantity=qty; }
    uint32      GetItemsQuantityPerClass(e_ahb_quality quality, ItemClass itemclass) { return m_ItemInfo[quality].ItemClassInfos[itemclass].Quantity; }
    void        SetMissedItemsPerClass(e_ahb_quality quality, ItemClass itemclass, uint32 found)
    {
        if (m_ItemInfo[quality].ItemClassInfos[itemclass].AmountOfItems > found )
            m_ItemInfo[quality].ItemClassInfos[itemclass].MissItems=m_ItemInfo[quality].ItemClassInfos[itemclass].AmountOfItems - found;
        else
            m_ItemInfo[quality].ItemClassInfos[itemclass].MissItems = 0;
    }
    uint32      GetMissedItemsPerClass(e_ahb_quality quality, ItemClass itemclass) { return m_ItemInfo[quality].ItemClassInfos[itemclass].MissItems; }

    // Data for every quality of item //
    void        SetItemsAmountPerQuality(e_ahb_quality quality, uint32 cnt) { m_ItemInfo[quality].AmountOfItems=cnt; }
    uint32      GetItemsAmountPerQuality(e_ahb_quality quality) { return m_ItemInfo[quality].AmountOfItems; }
    void        SetPriceRatioPerQuality(e_ahb_quality quality, uint32 value) { m_ItemInfo[quality].PriceRatio=value; }
    uint32      GetPriceRatioPerQuality(e_ahb_quality quality) { return m_ItemInfo[quality].PriceRatio; }
    void        SetMaxStackPerQuality(e_ahb_quality quality, uint32 value) { m_ItemInfo[quality].MaxStack=value; }
    uint32      GetMaxStackPerQuality(e_ahb_quality quality) { return m_ItemInfo[quality].MaxStack; }
    void        SetBuyerPricePerQuality(e_ahb_quality quality, uint32 value) { m_ItemInfo[quality].buyerPrice = value; }
    uint32      GetBuyerPricePerQuality(e_ahb_quality quality) { return m_ItemInfo[quality].buyerPrice; }

    ~AHB_Seller_Config() { }

private:
    uint32 m_AHID;
    uint32 m_minTime;
    uint32 m_maxTime;
    std::vector<s_SellerItemInfo> m_ItemInfo;

    // std::vector<c_ItemClassInfos> m_ItemClassInfo;
};

class AHB_Base
{
public :
    AHB_Base()
    {
        // Define the last usable number for GUID will be used has AhBot GUID.
        m_FakeGuid.Set(std::numeric_limits< uint32 >::max());
    }

    bool        Initialize();
    ObjectGuid  GetAHBObjectGuid() { return m_FakeGuid; };
    const char* GetAHBotName() const { return m_AHBotName.c_str(); }
    uint32      getConfig(e_AHBOTConfigUInt32Values index) const { return m_configUint32Values[index]; }
    bool        getConfig(e_AHBOTConfigBoolValues index) const { return m_configBoolValues[index]; }
    void        setConfig(e_AHBOTConfigBoolValues index, bool value) { m_configBoolValues[index]=value; }
    void        setConfig(e_AHBOTConfigUInt32Values index, uint32 value) { m_configUint32Values[index]=value; }
    uint32      GetItemPerCycleBoost()  { return m_ItemsPerCycleBoost; }
    uint32      GetItemPerCycleNormal() { return m_ItemsPerCycleNormal; }
    bool        Reload();
    const std::string GetItemClassName(ItemClass itemclass);

private :
    ObjectGuid      m_FakeGuid;
    std::string     m_AHBotName;
    Config          m_AhBotCfg;
    uint32          m_ItemsPerCycleBoost;
    uint32          m_ItemsPerCycleNormal;
    WorldSession*   m_Session;

    uint32      m_configUint32Values[CONFIG_UINT32_AHBOT_UINT32_COUNT];
    bool        m_configBoolValues[CONFIG_UINT32_AHBOT_BOOL_COUNT];

    void        SetAHBotName(const std::string& AHBotName) { if (AHBotName.size()>0) m_AHBotName = AHBotName; else m_AHBotName="AHBot"; }

    void        setConfig(e_AHBOTConfigUInt32Values index, char const* fieldname, uint32 defvalue);
    void        setConfig(e_AHBOTConfigBoolValues index, char const* fieldname, bool defvalue);
    void        GetConfigFromFile();
};

#define sAHB_BaseConfig MaNGOS::Singleton<AHB_Base>::Instance()

class AHB_Buyer
{
public:
    AHB_Buyer();
    ~AHB_Buyer();
    bool        Initialize();
    void        LoadConfig();
    void        addNewAuctionBuyerBotBid(AHB_Buyer_Config& config);
    bool        Update(uint32 operationSelector);

private:
    uint32              m_OperationSelector;
    WorldSession*       m_Session;
    bool                m_debug_Buyer;
    uint32              m_CheckInterval;
    AHB_Buyer_Config    m_AllianceConfig;
    AHB_Buyer_Config    m_HordeConfig;
    AHB_Buyer_Config    m_NeutralConfig;

    void        LoadBuyerValues(AHB_Buyer_Config& config);
    bool        IsBuyableEntry(uint32 buyoutPrice, double InGame_BuyPrice, double MaxBuyablePrice, uint32 MinBuyPrice, uint32 MaxChance, uint32 ChanceRatio);
    bool        IsBidableEntry(uint32 bidPrice, double InGame_BuyPrice, double MaxBidablePrice, uint32 MinBidPrice, uint32 MaxChance, uint32 ChanceRatio);
    void        PlaceBidToEntry(AuctionHouseObject* auctionHouse, AuctionEntry* auction, uint32 bidPrice);
    void        BuyEntry(AuctionHouseObject* auctionHouse, AuctionEntry* auction);
    void        PrepareListOfEntry(AHB_Buyer_Config& config);
    uint32      GetBuyableEntry( AHB_Buyer_Config& config );
};

class AHB_Seller
{
public:
    typedef std::vector<std::vector<std::vector<uint32> > > t_ItemPool;

    AHB_Seller();
    ~AHB_Seller();
    bool        Initialize();
    void        addNewAuctions(AHB_Seller_Config& config);
    bool        Update(uint32 operationSelector);
    void        SetItemsRatio(uint32* al, uint32* ho, uint32* ne);
    void        SetItemsAmount(uint32* grey_i, uint32* white_i, uint32* green_i, uint32* blue_i, uint32* purple_i, uint32* orange_i, uint32* yellow_i);
    void        LoadConfig();

private:
    AHB_Seller_Config   m_AllianceConfig;
    AHB_Seller_Config   m_HordeConfig;
    AHB_Seller_Config   m_NeutralConfig;
    bool                m_debug_Seller;

    t_ItemPool m_ItemPool;

    void        LoadSellerValues(AHB_Seller_Config& config);
    uint32      SetStat(AHB_Seller_Config& config);
    bool        getRandomArray( AHB_Seller_Config& config, std::vector<s_randomArray>& ra, const std::vector<std::vector<uint32> >& addedItem  );
    void        SetPricesOfItem(const Item *item, AHB_Seller_Config& config, uint32& buyp, uint32& bidp, uint32& stackcnt, e_ahb_quality itemQuality);
    void        LoadItemsQuantity(AHB_Seller_Config& config);
};

class AuctionHouseBot
{
public:
    typedef std::vector < std::vector < uint32 > > t_AhBotInfos;

    AuctionHouseBot();
    ~AuctionHouseBot();

    void        Update();
    void        Initialize();
    ObjectGuid  GetAHBObjectGuid() { return sAHB_BaseConfig.GetAHBObjectGuid(); }
    const char* GetAHBotName() const { return sAHB_BaseConfig.GetAHBotName(); }

    // Followed method is mainly used by level3.cpp for ingame/console command
    uint32      getConfig(e_AHBOTConfigUInt32Values index) const { return sAHB_BaseConfig.getConfig(index); }
    void        SetItemsRatio(uint32* al, uint32* ho, uint32* ne);
    void        SetItemsAmount(uint32* grey_i, uint32* white_i, uint32* green_i, uint32* blue_i, uint32* purple_i, uint32* orange_i, uint32* yellow_i);
    bool        ReloadAllConfig();
    void        PrepStatusInfos();
    void        Rebuild(bool all);
    uint32      GetAllianceItemsCount() { return m_AllianceItemsCount; }
    uint32      GetHordeItemsCount() { return m_HordeItemsCount; }
    uint32      GetNeutralItemsCount() { return m_NeutralItemsCount; }
    uint32      GetAHBotItemInfos(uint32 FactionNum, e_ahb_quality Quality) { return m_AhBotInfos[FactionNum][Quality]; }

private:
    AHB_Buyer*      m_Buyer;
    AHB_Seller*     m_Seller;
    uint32          m_OperationSelector;
    time_t          m_LastBuyableEntryChecked;
    bool            m_BuyerEnabled;
    bool            m_SellerEnabled;
    uint32          m_AllianceItemsCount;
    uint32          m_HordeItemsCount;
    uint32          m_NeutralItemsCount;
    t_AhBotInfos    m_AhBotInfos;
};

#define auctionbot MaNGOS::Singleton<AuctionHouseBot>::Instance()

#endif
