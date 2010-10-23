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

class c_ItemClassInfos
{
public:
    c_ItemClassInfos(uint32 indice=0)
    {
        m_class=(indice % MAX_ITEM_CLASS);

        switch (m_class)
        {
        case 0:     m_className = "Consumable"; break;
        case 1:     m_className = "Container"; break;
        case 2:     m_className = "Weapon"; break;
        case 3:     m_className = "Gem"; break;
        case 4:     m_className = "Armor"; break;
        case 5:     m_className = "Reageant"; break;
        case 6:     m_className = "Projectile"; break;
        case 7:     m_className = "Trade_Goods"; break;
        case 8:     m_className = "Generic"; break;
        case 9:     m_className = "Recipe"; break;
        case 10:    m_className = "Money"; break;
        case 11:    m_className = "Quiver"; break;
        case 12:    m_className = "Quest"; break;
        case 13:    m_className = "Key"; break;
        case 14:    m_className = "Permanant"; break;
        case 15:    m_className = "Misc"; break;
        case 16:    m_className = "Glyph"; break;
        }
        m_AmountOfItems=0;
        m_MissItems=0;
        m_Quantity=0;
    }

    const std::string GetName()
    {
        return m_className;
    }
    void SetAmountOfItems(uint32 indice)
    {
        m_AmountOfItems = indice*m_Quantity;
    }
    uint32 GetAmountOfItems()
    {
        return m_AmountOfItems;
    }
    void SetQuantityOfItems(uint32 quantity)
    {
        m_Quantity = quantity;
    }
    uint32 GetQuantityOfItems()
    {
        return m_Quantity;
    }
    void SetMissItems(uint32 value)
    {
        (m_AmountOfItems > value ) ? m_MissItems=m_AmountOfItems-value: m_MissItems = 0;
    }
    uint32 GetMissItems()
    {
        return m_MissItems;
    }

private:
    uint32 m_AmountOfItems;
    uint32 m_MissItems;
    uint32 m_Quantity;
    uint32 m_class;

    std::string m_className;
};

class c_ItemInfos
{
public:

    std::vector<c_ItemClassInfos> ItemClassInfos;

    c_ItemInfos(uint32 indice=0)
    {
        for (int i=0; i<MAX_ITEM_CLASS; ++i)
        {
            ItemClassInfos.push_back(c_ItemClassInfos(i));
        }

        m_indice=(indice % AHB_QUALITY_MAX);

        switch (m_indice)
        {
        case 0: m_color = "grey"; break;
        case 1: m_color = "white"; break;
        case 2: m_color = "green"; break;
        case 3: m_color = "blue"; break;
        case 4: m_color = "purple"; break;
        case 5: m_color = "orange"; break;
        case 6: m_color = "yellow"; break;
        }
    }
    const std::string GetColor()
    {
        return m_color;
    }
    void SetAmountOfItems(uint32 cnt)
    {
        m_AmountOfItems=cnt;
    }
    uint32 GetAmountOfItems()
    {
        return m_AmountOfItems;
    }
    void SetPriceRatio(uint32 value)
    {
        m_PriceRatio=value;
    }
    uint32 GetPriceRatio()
    {
        return m_PriceRatio;
    }
    void SetMaxStack(uint32 value)
    {
        m_MaxStack=value;
    }
    uint32 GetMaxStack()
    {
        return m_MaxStack;
    }
    void SetBuyerPrice(uint32 value)
    {
        m_buyerPrice = value;
     }
    uint32 GetBuyerPrice()
    {
        return m_buyerPrice;
    }

private:
    uint32 m_AmountOfItems;
    uint32 m_MaxItemsPrice;
    uint32 m_MinItemsPrice;
    uint32 m_MinBidPrice;
    uint32 m_MaxBidPrice;
    uint32 m_MaxStack;
    uint32 m_MissItems;
    uint32 m_buyerPrice;
    uint32 m_PriceRatio;

    uint32 m_indice;

    std::string m_color;
};

struct s_itemEval
{
    uint32  AhEntry;
    uint32  item_guidlow;
    time_t  lastchecked;
    time_t  LastExist;
    s_itemEval()
    {
        lastchecked=0;
    }
};

struct s_itemInfo
{
    uint32  ItemCount;
    double  BuyPrice;
    double  BidPrice;
    uint32  MinBuyPrice;
    uint32  MinBidPrice;

    s_itemInfo()
    {
        ItemCount=0;
        BuyPrice=0;
        BidPrice=0;
        MinBuyPrice=0;
        MinBidPrice=0;
    }
};

class AHB_Seller_Config
{
public:

    std::vector<c_ItemInfos> ItemInfos;
    uint32 LastMissedItem;

    AHB_Seller_Config(uint32 ahid=7)
    {
        m_AHID = ahid;

        for (int i=0; i<AHB_QUALITY_MAX; ++i)
        {
            ItemInfos.push_back(c_ItemInfos(i));
        }
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
    void SetMaxTime(uint32 value)
    {
        m_maxTime = value;
    }
    uint32 GetMaxTime()
    {
        return m_maxTime;
    }

    ~AHB_Seller_Config()
    {
    }

private:
    uint32 m_AHID;
    uint32 m_minTime;
    uint32 m_maxTime;
};

class AHB_Buyer_Config
{
public:
    typedef std::map< uint32 , s_itemInfo > t_itemInfo;
    typedef std::map< uint32, s_itemEval > t_checkEntryMap;

    uint32 m_FactionChance;

    t_itemInfo m_SameItemInfo;
    t_checkEntryMap m_CheckedEntry;

    bool   BuyerEnabled;
    uint32 Buyer_Price_Ratio;

    AHB_Buyer_Config(uint32 ahid=7)
    {
        m_AHID = ahid;
    }

    uint32 GetAHID()
    {
        return m_AHID;
    }

    ~AHB_Buyer_Config()
    {
    }

private:
    uint32 m_AHID;
};

struct s_randomArray
{
    uint32 color;
    uint32 itemclass;
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
    AHB_Base*           m_BaseConfig;
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
    AHB_Base*            m_BaseConfig;
    AHB_Seller_Config   m_AllianceConfig;
    AHB_Seller_Config   m_HordeConfig;
    AHB_Seller_Config   m_NeutralConfig;
    bool                m_debug_Seller;

    t_ItemPool m_ItemPool;

    void        LoadSellerValues(AHB_Seller_Config& config);
    uint32      SetStat(AHB_Seller_Config& config);
    bool        getRandomArray( AHB_Seller_Config& config, std::vector<s_randomArray>& ra, const std::vector<std::vector<uint32> >& addedItem  );
    void        SetPricesOfItem(const Item *item, AHB_Seller_Config& config, uint32& buyp, uint32& bidp, uint32& stackcnt, e_ahb_quality AHB_ITEMS);
    void        LoadItemsQuantity(AHB_Seller_Config& config);
};

class AuctionHouseBot
{
public:
    AuctionHouseBot();
    ~AuctionHouseBot();

    std::vector < std::vector < uint32 > > AhBotInfos;
    uint32 AllianceItemsCount;
    uint32 HordeItemsCount;
    uint32 NeutralItemsCount;

    void        Update();
    void        Initialize();
    ObjectGuid  GetAHBObjectGuid() { return m_BaseConfig->GetAHBObjectGuid(); }
    const char* GetAHBotName() const { return m_BaseConfig->GetAHBotName(); }
    uint32      getConfig(e_AHBOTConfigUInt32Values index) const { return m_BaseConfig->getConfig(index); }
    void        SetItemsRatio(uint32* al, uint32* ho, uint32* ne);
    void        SetItemsAmount(uint32* grey_i, uint32* white_i, uint32* green_i, uint32* blue_i, uint32* purple_i, uint32* orange_i, uint32* yellow_i);
    bool        ReloadAllConfig();
    void        PrepStatusInfos();
    void        Rebuild(bool all);

private:
    AHB_Base*       m_BaseConfig;
    uint32          m_OperationSelector;
    time_t          m_LastBuyableEntryChecked;
    bool            m_BuyerEnabled;
    bool            m_SellerEnabled;
    AHB_Buyer*      m_Buyer;
    AHB_Seller*     m_Seller;
};

#define auctionbot MaNGOS::Singleton<AuctionHouseBot>::Instance()

#endif
