#include <WorldSession.h>
#include <ScriptPCH.h>
#include <Log.h>
#include <DatabaseEnv.h>
#include <Chat.h>
#include <World.h>
#include <Player.h>

#include "PlayerInfo.h"

void PlayerInfoSystem::Update(uint32 diff)
{
    UpdateTimer += diff;

    if (UpdateTimer < 300000)
        return;

    for (auto& i : m_AccountInfo)
    {
        if (!sWorld->FindSession(i.second.AccID))
            m_AccountInfo.erase(i.second.AccID);
    }

    UpdateTimer = 0;
}

bool PlayerInfoSystem::LoadAccInfo(uint32 AccID)
{
    if (m_AccountInfo.find(AccID) != m_AccountInfo.end())
        return false;

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CUSTOM_ACCOUNT);
    stmt->setUInt32(0, AccID);
    PreparedQueryResult res = LoginDatabase.Query(stmt);
    if (res)
    {
        Field* pField = res->Fetch();
        AccInfoItem item = { 0 };

        item.AccID = pField[0].GetUInt32();
        item.MagicLevel = pField[1].GetUInt16();
        item.Buffs = pField[3].GetUInt8();
        item.WeaponBanned = pField[4].GetBool();
        item.Updated = false;

        m_AccountInfo.emplace(item.AccID, std::move(item));
        return true;
    }
    else
        return false;
}

void PlayerInfoSystem::SaveAccInfo(uint32 AccID)
{
    if (AccID > 0)
        if (m_AccountInfo.find(AccID) != m_AccountInfo.end())
        {
            AccInfoItem& i = m_AccountInfo[AccID];
            if (i.Updated)
                SendSaveQuery(&i);
            return;
        }

    for (auto& i : m_AccountInfo)
    {
        if (i.second.Updated)
            SendSaveQuery(&i.second);
    }
}

void PlayerInfoSystem::SendSaveQuery(AccInfoItem* Info)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_CUSTOM_ACCOUNT);
    stmt->setUInt16(0, Info->MagicLevel);
    stmt->setUInt32(1, Info->MagicExperience);
    stmt->setUInt8(2, Info->Buffs);
    stmt->setUInt32(3, Info->AccID);

    LoginDatabase.Execute(stmt);
    Info->Updated = false;
}

AccInfoItem* PlayerInfoSystem::GetAccInfo(uint32 AccID)
{
    if (m_AccountInfo.find(AccID) != m_AccountInfo.end())
        return &m_AccountInfo[AccID];

    return nullptr;
}

bool PlayerInfoSystem::LoadCharInfo(uint32 CharID)
{
    if (m_CharInfo.find(CharID) != m_CharInfo.end())
        return false;

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CUSTOM_CHARACTER);
    stmt->setUInt32(0, CharID);
    PreparedQueryResult res = LoginDatabase.Query(stmt);
    if (res)
    {
        Field* pField = res->Fetch();
        CharInfoItem item = { 0 };

        item.CharID             = pField[0].GetUInt32();
        item.AccID              = pField[1].GetUInt32();
        item.Name               = pField[2].GetString();
        item.Class              = pField[3].GetUInt8();
        item.Race               = pField[4].GetUInt8();
        item.Sex                = pField[5].GetUInt8();
        item.AchievementPoints  = pField[6].GetUInt8();
        item.WeaponUpdated      = pField[7].GetUInt32();
        item.UpgradeItemGUID    = { };

        m_CharInfo.emplace(item.CharID, std::move(item));
        return true;
    }
    else
        return false;
}

void PlayerInfoSystem::DeleteCharInfo(uint32 CharID, uint32 AccID)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_CUSTOM_CHARACTER);
    stmt->setUInt32(0, CharID);
    stmt->setUInt32(1, AccID);
    LoginDatabase.Execute(stmt);

    stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_CUSTOM_WEAPON);
    stmt->setUInt32(0, CharID);
    stmt->setUInt32(1, AccID);
    LoginDatabase.Execute(stmt);
}

void PlayerInfoSystem::SaveCharInfo(uint32 CharID)
{
    CharInfoItem* Info = GetCharInfo(CharID);

    if (Info == nullptr)
        return;

    // Save Info
    std::cout << Info->WeaponUpdated << " " << Info->CharID << std::endl;

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_CUSTOM_CHARACTER);
    stmt->setUInt8(0, Info->AchievementPoints);
    stmt->setUInt32(1, Info->WeaponUpdated);
    stmt->setUInt32(2, Info->CharID);
    stmt->setUInt32(3, Info->AccID);

    LoginDatabase.Execute(stmt);
}

CharInfoItem* PlayerInfoSystem::GetCharInfo(uint32 CharID)
{
    if (m_CharInfo.find(CharID) != m_CharInfo.end())
        return &m_CharInfo[CharID];
    return nullptr;
}

void PlayerInfoSystem::CreateCharInfo(Player* p)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_CUSTOM_CHARACTER);
    stmt->setUInt32(0, p->GetGUID());
    stmt->setUInt32(1, p->GetSession()->GetAccountId());
    stmt->setString(2, p->GetName());
    stmt->setUInt8(3, p->GetClass());
    stmt->setUInt8(4, p->GetRace());
    stmt->setUInt8(5, p->GetGender());

    LoginDatabase.Execute(stmt);
}

void PlayerInfoSystem::RemoveCharInfo(uint32 CharID)
{
    if (m_CharInfo.find(CharID) != m_CharInfo.end())
        m_CharInfo.erase(CharID);
}

void PlayerInfoSystem::LoadAllOnStart()
{
    LoadMagicExp();
    LoadLeaderboard();
    LoadWeaponDisplayIds();
}

void PlayerInfoSystem::LoadWeaponDisplayIds()
{
    TC_LOG_INFO("server.loading", "Loading Magic Experience...");
    m_WeaponDisplayIds.clear();
    uint32 msStartTime = getMSTime();
    int nCounter = 0;

    QueryResult res = LoginDatabase.Query("SELECT DisplayID, WeaponType FROM custom.weapon_display_ids");
    if (res)
        do
        {
            Field* pField = res->Fetch();
            WeaponDisplayId item = { 0 };

            item.DisplayId = pField[0].GetUInt32();
            item.WeaponType = pField[1].GetUInt8();

            m_WeaponDisplayIds.emplace(item.DisplayId, std::move(item));
            ++nCounter;
        } while (res->NextRow());
        TC_LOG_INFO("server.loading", "Loaded Weapon DisplayIds (%d entries) in %ums", nCounter, GetMSTimeDiffToNow(msStartTime));
}

void PlayerInfoSystem::LoadMagicExp()
{
    TC_LOG_INFO("server.loading", "Loading Magic Experience...");
    m_MagicLevelExp.clear();
    uint32 msStartTime = getMSTime();
    int nCounter = 0;

    QueryResult res = LoginDatabase.Query("SELECT Level, Experience FROM custom.magic_experience ORDER BY Level");
    if (res)
    {
        do
        {
            Field* pField = res->Fetch();
            MagicExpItem item = { 0 };

            item.Level = pField[0].GetUInt16();
            item.Experience = pField[1].GetUInt32();

            m_MagicLevelExp.emplace(item.Level, std::move(item));
            ++nCounter;
        } while (res->NextRow());
    }
    TC_LOG_INFO("server.loading", "Loaded Magic Experience (%d entries) in %ums", nCounter, GetMSTimeDiffToNow(msStartTime));
}

void PlayerInfoSystem::LoadLeaderboard()
{
    //LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CUSTOM_LEADERBOARD);
    //PreparedQueryResult res = LoginDatabase.Query(stmt);
    //if (res)
    //{
    //    int classID = 1;
    //    int ctr = 0;
    //
    //    do
    //    {
    //        Field* pField = res->Fetch();
    //
    //        CharInfoItem item = { 0 };
    //        item.CharID = pField[0].GetUInt32();
    //        item.AccID = pField[1].GetUInt32();
    //        item.Name = pField[2].GetString();
    //        item.Class = pField[3].GetUInt8();
    //        item.Race = pField[4].GetUInt8();
    //        item.Sex = pField[5].GetUInt8();
    //        item.PrestigeTime = pField[6].GetUInt32();
    //        item.CurrTimer = pField[7].GetUInt32();
    //        item.PrestigeCount = pField[8].GetUInt16();
    //
    //        if (classID != item.Class)
    //        {
    //            classID = item.Class;
    //            ctr = 0;
    //        }
    //
    //        if (item.Class == CLASS_DRUID)
    //            item.Class -= 1;
    //
    //        Leaderboard[item.Class - 1][ctr] = item;
    //
    //    } while (res->NextRow());
    //}
    //stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CUSTOM_PRESTIGE);
    //res = LoginDatabase.Query(stmt);
    //if (res)
    //{
    //    int ctr = 0;
    //    do
    //    {
    //        Field* pField = res->Fetch();
    //
    //        CharInfoItem item = { 0 };
    //        item.CharID = pField[0].GetUInt32();
    //        item.AccID = pField[1].GetUInt32();
    //        item.Name = pField[2].GetString();
    //        item.Class = pField[3].GetUInt8();
    //        item.Race = pField[4].GetUInt8();
    //        item.Sex = pField[5].GetUInt8();
    //        item.PrestigeTime = pField[6].GetUInt32();
    //        item.CurrTimer = pField[7].GetUInt32();
    //        item.PrestigeCount = pField[8].GetUInt16();
    //        item.AchievementPoints = pField[9].GetUInt8();
    //
    //        if (item.Class == CLASS_DRUID)
    //            item.Class -= 1;
    //
    //        PrestigeCount[ctr] = item;
    //        ctr++;
    //    } while (res->NextRow());
    //
    //    for (; ctr < MAX_LEADER_PRESTIGE_COUNT; ctr++)
    //        PrestigeCount[ctr] = { 0 };
    //}
    //stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CUSTOM_ACHIEVE);
    //res = LoginDatabase.Query(stmt);
    //if (res)
    //{
    //    int ctr = 0;
    //    do
    //    {
    //        Field* pField = res->Fetch();
    //
    //        CharInfoItem item = { 0 };
    //        item.CharID = pField[0].GetUInt32();
    //        item.AccID = pField[1].GetUInt32();
    //        item.Name = pField[2].GetString();
    //        item.Class = pField[3].GetUInt8();
    //        item.Race = pField[4].GetUInt8();
    //        item.Sex = pField[5].GetUInt8();
    //        item.PrestigeTime = pField[6].GetUInt32();
    //        item.CurrTimer = pField[7].GetUInt32();
    //        item.PrestigeCount = pField[8].GetUInt16();
    //        item.AchievementPoints = pField[9].GetUInt8();
    //
    //        if (item.Class == CLASS_DRUID)
    //            item.Class -= 1;
    //
    //        AchieveCount[ctr] = item;
    //        ctr++;
    //    } while (res->NextRow());
    //
    //    for (; ctr < MAX_LEADER_ACHIEVE_COUNT; ctr++)
    //        AchieveCount[ctr] = { 0 };
    //}
}

uint32 PlayerInfoSystem::GetRequiredExp(uint16 Level)
{
    if (m_MagicLevelExp.find(Level) != m_MagicLevelExp.end())
        return m_MagicLevelExp[Level].Experience;

    return 0;
}

void PlayerInfoSystem::AddArtifactExp(Player* p, uint32 Amt)
{
    AccInfoItem& item = m_AccountInfo[p->GetSession()->GetAccountId()];

    if (m_MagicLevelExp.find(item.MagicLevel) == m_MagicLevelExp.end())
    {
        TC_LOG_ERROR("custom.artifact", "Account: %u Info Doesn't Exist.", p->GetSession()->GetAccountId());
        return;
    }

    if (!p->HasItemCount(ARTIFACT_ITEM_ID))
        return;
    //(Info->MagicLevel / 200 * 100)

    item.MagicExperience += (Amt += (Amt / 100) * item.MagicLevel * 10);
    ChatHandler(p->GetSession()).SendNotify("You recieved %d Magic Experience", Amt);
    if (!item.Updated)
        item.Updated = true;

    bool bLeveled = false;

    while (item.MagicExperience >= GetRequiredExp(item.MagicLevel))
    {
        bLeveled = true;
        item.MagicExperience -= GetRequiredExp(item.MagicLevel);
        item.MagicLevel++;

        if (m_MagicLevelExp.find(item.MagicLevel) == m_MagicLevelExp.end())
        {
            item.MagicExperience = 0;
            break;
        }
    }

    if (bLeveled)
    {
        p->SendPlaySpellVisual(13906);
        ChatHandler(p->GetSession()).PSendSysMessage("|cfffda025[Artifact System]:|r Your Magic is now level %d", item.MagicLevel);
        p->UpdateStats(Stats::STAT_STAMINA);
    }
}

bool PlayerInfoSystem::CanUseDisplayID(uint32 display, uint8 type)
{
    if (m_WeaponDisplayIds.find(display) == m_WeaponDisplayIds.end())
        return false;

    return (m_WeaponDisplayIds[display].WeaponType & type);
}

ObjectGuid PlayerInfoSystem::GetPlrUpgrade(uint32 CharID)
{
    if (GetCharInfo(CharID))
        return GetCharInfo(CharID)->UpgradeItemGUID;

    return { };
}

void PlayerInfoSystem::SetPlrUpgrade(uint32 CharID, ObjectGuid ItemGUID)
{
    if (GetCharInfo(CharID))
        GetCharInfo(CharID)->UpgradeItemGUID = ItemGUID;
}

CharInfoItem PlayerInfoSystem::GetLeaderboardInfo(uint8 Class, uint8 Rank)
{
    if (Class >= MAX_LEADER_CLASS || Rank >= MAX_LEADERBOARD)
    {
        TC_LOG_ERROR("server.error", "Searching for Incorrect Leaderboard Value, Class: %d - Rank: %d", Class, Rank);
        return { 0 };
    }

    return Leaderboard[Class][Rank];
}

CharInfoItem PlayerInfoSystem::GetPrestigeLeader(uint8 Rank)
{
    if (Rank >= MAX_LEADER_PRESTIGE_COUNT)
    {
        TC_LOG_ERROR("server.error", "Searching for Incorrect Prestige Leaderboard, Rank: %d", Rank);
        return { 0 };
    }

    return PrestigeCount[Rank];
}

CharInfoItem PlayerInfoSystem::GetAchieveLeader(uint8 Rank)
{
    if (Rank >= MAX_LEADER_PRESTIGE_COUNT)
    {
        TC_LOG_ERROR("server.error", "Searching for Incorrect Prestige Leaderboard, Rank: %d", Rank);
        return { 0 };
    }

    return AchieveCount[Rank];
}
