#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include "../UpgradeSystem/UpgradeSystem.h"

constexpr auto ARTIFACT_ITEM_ID = 60002;
constexpr auto MAX_LEADER_CLASS = 10;
constexpr auto MAX_LEADERBOARD = 3;
constexpr auto MAX_LEADER_PRESTIGE_COUNT = 10;
constexpr auto MAX_LEADER_ACHIEVE_COUNT = 10;

struct AccInfoItem
{
    uint32 AccID;
    uint16 MagicLevel;
    uint32 MagicExperience;
    uint8 Buffs;
    bool WeaponBanned;
    bool Updated;
};

struct CharInfoItem
{
    uint32 CharID;
    uint32 AccID;
    std::string Name;
    uint8 Class;
    uint8 Race;
    uint8 Sex;
    ObjectGuid UpgradeItemGUID;
    uint8 AchievementPoints;
    uint32 WeaponUpdated;
};

struct MagicExpItem
{
    uint16 Level;
    uint32 Experience;
};

struct WeaponDisplayId
{
    uint32 DisplayId;
    uint8  WeaponType;
};

class PlayerInfoSystem
{
    PlayerInfoSystem() {}

public:
    static PlayerInfoSystem& instance()
    {
        static PlayerInfoSystem PlrInfoSys;
        return PlrInfoSys;
    }
    uint32 UpdateTimer = 0;

    // General

    void Update(uint32 diff);

    // Account Info

    bool LoadAccInfo(uint32 AccID);
    void SaveAccInfo(uint32 AccID = 0);
    void SendSaveQuery(AccInfoItem* Info);
    AccInfoItem* GetAccInfo(uint32 AccID);

    // Character Info

    bool LoadCharInfo(uint32 CharID);
    void DeleteCharInfo(uint32 CharID, uint32 AccID);
    void SaveCharInfo(uint32 CharID);
    CharInfoItem* GetCharInfo(uint32 CharID);
    void CreateCharInfo(Player* p);
    void RemoveCharInfo(uint32 CharID);

    // Other Info

    void LoadAllOnStart();
    void LoadMagicExp();
    void LoadLeaderboard();
    void LoadWeaponDisplayIds();
    uint32 GetRequiredExp(uint16 Level);
    void AddArtifactExp(Player* p, uint32 Amt);
    bool CanUseDisplayID(uint32 display, uint8 type);
    ObjectGuid GetPlrUpgrade(uint32 CharID);
    void SetPlrUpgrade(uint32 CharID, ObjectGuid ItemGUID);
    CharInfoItem GetLeaderboardInfo(uint8 Class, uint8 Rank);
    CharInfoItem GetPrestigeLeader(uint8 Rank);
    CharInfoItem GetAchieveLeader(uint8 Rank);

private:

    CharInfoItem Leaderboard[MAX_LEADER_CLASS][MAX_LEADERBOARD] = { 0 };
    CharInfoItem PrestigeCount[MAX_LEADER_PRESTIGE_COUNT] = { 0 };
    CharInfoItem AchieveCount[MAX_LEADER_ACHIEVE_COUNT] = { 0 };

    std::map<decltype(AccInfoItem::AccID), AccInfoItem> m_AccountInfo;
    std::map<decltype(CharInfoItem::CharID), CharInfoItem> m_CharInfo;
    std::map<decltype(MagicExpItem::Level), MagicExpItem> m_MagicLevelExp;
    std::map<decltype(WeaponDisplayId::DisplayId), WeaponDisplayId> m_WeaponDisplayIds;
};

#define sPlayerInfo PlayerInfoSystem::instance()

#endif
