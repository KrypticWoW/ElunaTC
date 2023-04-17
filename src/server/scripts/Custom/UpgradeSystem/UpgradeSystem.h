#ifndef UPGRADESYSTEM_H
#define UPGRADESYSTEM_H

constexpr auto UPGRADE_QUEST_ID = 999999;
constexpr auto MAX_REQ_ITEM_COUNT = 4;

struct UpgradeItem
{
    uint32 Entry;
    uint32 UpgradeEntry;
    uint32 ReqGold;
    int32 ReqItemID[MAX_REQ_ITEM_COUNT];
    uint16 ReqItemCount[MAX_REQ_ITEM_COUNT];
    uint32 ReqHonor;
    uint16 ReqArenaPoints;
    uint16 ReqArenaRating;
    uint8 UpgradeChance;
};

class UpgradeSystem
{
    UpgradeSystem() { }

public:
    static UpgradeSystem& instance()
    {
        static UpgradeSystem upsys;
        return upsys;
    }

    UpgradeItem *GetUpgradeInfo(uint32 itemEntry)
    {
        if (m_Upgrades.find(itemEntry) != m_Upgrades.end())
            return &m_Upgrades[itemEntry];
        return nullptr;
    }

    auto& GetUpgrades()
    {
        return m_Upgrades;
    }

    void HandleUpgrade(Player* p);

    void Load();

private:
    std::map<decltype(UpgradeItem::Entry), UpgradeItem> m_Upgrades;
};

#define sUpgradeSystem UpgradeSystem::instance()

#endif
