#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include "../UpgradeSystem/UpgradeSystem.h"

constexpr uint32 ARTIFACT_ITEM_ID = 60002;
constexpr uint16 MAX_ARTIFACT_LEVEL = 1000;
constexpr uint8  MAX_NECK_LEVEL = 255;

struct AccountInfoItem
{
    uint32 AccountID;
    uint16 ArtifactLevel;
    uint32 ArtifactExperience;
    uint8 Buffs;
    bool AllowTextDetails;
    bool Updated;
};

struct CharacterInfoItem
{
    uint32 CharacterID;
    uint32 AccountID;
    std::string Name;
    uint8 Class;
    uint8 Race;
    uint8 Sex;
    uint8 NeckLevel;
    uint16 NeckExperience;
    uint8 Prestige;
    ObjectGuid UpgradeItemGUID;
    uint16 AchievementPoints;
    uint32 PreviousWeaponUpdate;
};

struct CustomExperienceItem
{
    uint16 Level;
    uint32 ArtifactExperience;
    uint16 NeckExperience;
};

struct WeaponDisplayID
{
    uint32 DisplayID;
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

    bool LoadAccountInfo(uint32 AccID);
    void SaveAccountInfo(uint32 AccID = 0);
    void SendSaveQuery(AccountInfoItem* Info);
    AccountInfoItem* GetAccountInfo(uint32 AccID);

    // Character Info

    bool LoadCharacterInfo(uint32 CharID);
    void DeleteCharacterInfo(uint32 CharID, uint32 AccID);
    void SaveCharacterInfo(uint32 CharID);
    CharacterInfoItem* GetCharacterInfo(uint32 CharID);
    void CreateCharacterInfo(Player* p);
    void RemoveCharacterInfo(uint32 CharID);

    // Other Info

    void LoadAllOnStart();
    void LoadCustomExperience();
    void LoadWeaponDisplayIDs();
    uint32 GetRequiredExperience(uint16 Level, bool bArtifact = true);
    void AddArtifactExperience(Player* p, uint32 Amt, bool bCommand = false);
    void AddNeckExperience(Player* p, uint32 Amt);
    bool CanUseDisplayID(uint32 display, uint8 type);
    ObjectGuid GetPlrUpgrade(uint32 CharID);
    void SetPlrUpgrade(uint32 CharID, ObjectGuid ItemGUID);

private:

    std::map<decltype(AccountInfoItem::AccountID), AccountInfoItem> m_AccountInfo;
    std::map<decltype(CharacterInfoItem::CharacterID), CharacterInfoItem> m_CharacterInfo;
    std::map<decltype(CustomExperienceItem::Level), CustomExperienceItem> m_CustomLevelExperience;
    std::map<decltype(WeaponDisplayID::DisplayID), WeaponDisplayID> m_WeaponDisplayIDs;
};

#define sPlayerInfo PlayerInfoSystem::instance()

#endif