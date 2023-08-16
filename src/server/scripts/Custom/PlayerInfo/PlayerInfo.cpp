#include <Chat.h>
#include <DatabaseEnv.h>
#include <Item.h>
#include <Log.h>
#include <Player.h>
#include <ScriptPCH.h>
#include <World.h>
#include <WorldSession.h>

#include "PlayerInfo.h"

void PlayerInfoSystem::Update(uint32 diff)
{
    UpdateTimer += diff;

    if (UpdateTimer < 300000)
        return;

    for (auto& i : m_AccountInfo)
        if (!sWorld->FindSession(i.first))
        {
            if (i.second.Updated)
                SaveAccountInfo(i.first);

            m_AccountInfo.erase(i.first);
        }

    UpdateTimer = 0;
}

bool PlayerInfoSystem::LoadAccountInfo(uint32 AccID)
{
    if (GetAccountInfo(AccID))
        return false;

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CUSTOM_ACCOUNT);
    stmt->setUInt32(0, AccID);
    PreparedQueryResult res = LoginDatabase.Query(stmt);
    if (res)
    {
        Field* pField = res->Fetch();
        AccountInfoItem Info = { 0 };

        Info.AccountID = pField[0].GetUInt32();
        Info.ArtifactLevel = pField[1].GetUInt16();
        Info.ArtifactExperience = pField[2].GetUInt32();
        Info.AnnounceExp = pField[3].GetBool();
        Info.Buffs = pField[4].GetUInt8();
        Info.AllowTextDetails = pField[5].GetBool();
        Info.Updated = false;

        m_AccountInfo.emplace(Info.AccountID, std::move(Info));
        return true;
    }
    else
        return false;
}

void PlayerInfoSystem::SaveAccountInfo(uint32 AccID)
{
    if (AccID > 0)
        if (AccountInfoItem* Info = GetAccountInfo(AccID))
            if (Info->Updated)
            {
                SendSaveQuery(Info);
                return;
            }

    for (auto& i : m_AccountInfo)
    {
        if (i.second.Updated)
            SendSaveQuery(&i.second);
    }
}

void PlayerInfoSystem::SendSaveQuery(AccountInfoItem* Info)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_CUSTOM_ACCOUNT);
    stmt->setUInt16(0, Info->ArtifactLevel);
    stmt->setUInt32(1, Info->ArtifactExperience);
    stmt->setBool(2, Info->AnnounceExp);
    stmt->setUInt8(3, Info->Buffs);
    stmt->setUInt32(4, Info->AccountID);

    LoginDatabase.Execute(stmt);
    Info->Updated = false;
}

AccountInfoItem* PlayerInfoSystem::GetAccountInfo(uint32 AccID)
{
    if (m_AccountInfo.find(AccID) != m_AccountInfo.end())
        return &m_AccountInfo[AccID];

    return nullptr;
}

bool PlayerInfoSystem::LoadCharacterInfo(uint32 CharID)
{
    if (GetCharacterInfo(CharID))
        return false;

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CUSTOM_CHARACTER);
    stmt->setUInt32(0, CharID);
    PreparedQueryResult res = LoginDatabase.Query(stmt);
    if (res)
    {
        Field* pField = res->Fetch();
        CharacterInfoItem Info = { 0 };

        Info.CharacterID          = pField[0].GetUInt32();
        Info.AccountID            = pField[1].GetUInt32();
        Info.Name                 = pField[2].GetString();
        Info.Class                = pField[3].GetUInt8();
        Info.Race                 = pField[4].GetUInt8();
        Info.Sex                  = pField[5].GetUInt8();
        Info.NeckLevel            = pField[6].GetUInt8();
        Info.NeckExperience       = pField[7].GetUInt16();
        Info.AnnounceExp          = pField[8].GetBool();
        Info.Prestige             = pField[9].GetUInt8();
        Info.AchievementPoints    = pField[10].GetUInt16();
        Info.PreviousWeaponUpdate = pField[11].GetUInt32();
        Info.UpgradeItemGUID      = { };

        m_CharacterInfo.emplace(Info.CharacterID, std::move(Info));
        return true;
    }
    else
        return false;
}

void PlayerInfoSystem::DeleteCharacterInfo(uint32 CharID, uint32 AccID)
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

void PlayerInfoSystem::SaveCharacterInfo(uint32 CharID)
{
    if (CharacterInfoItem* Info = GetCharacterInfo(CharID))
    {
        // Save Info
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_CUSTOM_CHARACTER);
        stmt->setUInt8(0, Info->NeckLevel);
        stmt->setUInt16(1, Info->NeckExperience);
        stmt->setBool(2, Info->AnnounceExp);
        stmt->setUInt8(3, Info->Prestige);
        stmt->setUInt8(4, Info->AchievementPoints);
        stmt->setUInt32(5, Info->PreviousWeaponUpdate);
        stmt->setUInt32(6, Info->CharacterID);
        stmt->setUInt32(7, Info->AccountID);

        LoginDatabase.Execute(stmt);
    }
}

CharacterInfoItem* PlayerInfoSystem::GetCharacterInfo(uint32 CharID)
{
    if (m_CharacterInfo.find(CharID) != m_CharacterInfo.end())
        return &m_CharacterInfo[CharID];
    return nullptr;
}

void PlayerInfoSystem::CreateCharacterInfo(Player* p)
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

void PlayerInfoSystem::RemoveCharacterInfo(uint32 CharID)
{
    if (m_CharacterInfo.find(CharID) != m_CharacterInfo.end())
    {
        SaveCharacterInfo(CharID);
        m_CharacterInfo.erase(CharID);
    }
}

void PlayerInfoSystem::LoadAllOnStart()
{
    LoadCustomExperience();
    LoadCustomLoot();
    LoadWeaponDisplayIDs();
}

void PlayerInfoSystem::LoadWeaponDisplayIDs()
{
    TC_LOG_INFO("server.loading", "Loading Weapon Display IDs...");
    m_WeaponDisplayIDs.clear();
    uint32 msStartTime = getMSTime();
    int nCounter = 0;

    QueryResult res = LoginDatabase.Query("SELECT DisplayID, WeaponType FROM custom.weapon_display_ids");
    if (res)
        do
        {
            Field* pField = res->Fetch();
            WeaponDisplayID Info = { 0 };

            Info.DisplayID = pField[0].GetUInt32();
            Info.WeaponType = pField[1].GetUInt8();

            m_WeaponDisplayIDs.emplace(Info.DisplayID, std::move(Info));
            ++nCounter;
        } while (res->NextRow());
        TC_LOG_INFO("server.loading", "Loaded Weapon Display IDs (%d entries) in %ums", nCounter, GetMSTimeDiffToNow(msStartTime));
}

void PlayerInfoSystem::LoadCustomExperience()
{
    TC_LOG_INFO("server.loading", "Loading Custom Experience...");
    m_CustomLevelExperience.clear();
    uint32 msStartTime = getMSTime();
    int nCounter = 0;

    QueryResult res = LoginDatabase.Query("SELECT Level, ArtifactExp, NeckExp FROM custom.custom_experience ORDER BY Level");
    if (res)
    {
        do
        {
            Field* pField = res->Fetch();
            CustomExperienceItem item = { 0 };

            item.Level = pField[0].GetUInt16();
            item.ArtifactExperience = pField[1].GetUInt32();
            item.NeckExperience = pField[2].GetUInt16();

            m_CustomLevelExperience.emplace(item.Level, std::move(item));
            ++nCounter;
        } while (res->NextRow());
    }
    TC_LOG_INFO("server.loading", "Loaded Custom Experience (%d entries) in %ums", nCounter, GetMSTimeDiffToNow(msStartTime));
}

void PlayerInfoSystem::LoadCustomLoot()
{
    TC_LOG_INFO("server.loading", "Loading Custom Creature Loot...");
    m_CustomCreatureLoot.clear();
    uint32 msStartTime = getMSTime();
    int nCounter = 0;

    QueryResult res = LoginDatabase.Query("SELECT CreatureID, ItemID_1, MinItemCount_1, MaxItemCount_1, ItemID_2, MinItemCount_2, MaxItemCount_2, ItemID_3, MinItemCount_3, MaxItemCount_3, MinMoney, MaxMoney FROM custom.creature_loot");
    if (res)
    {
        do
        {
            Field* pField = res->Fetch();
            CustomLootItem item = { 0 };

            item.Creature       = pField[0].GetUInt32();
            item.ItemID_1       = pField[1].GetUInt32();
            item.MinItemCount_1 = pField[2].GetUInt8();
            item.MaxItemCount_1 = pField[3].GetUInt8();
            item.ItemID_2       = pField[4].GetUInt32();
            item.MinItemCount_2 = pField[5].GetUInt8();
            item.MaxItemCount_2 = pField[6].GetUInt8();
            item.ItemID_3       = pField[7].GetUInt32();
            item.MinItemCount_3 = pField[8].GetUInt8();
            item.MaxItemCount_3 = pField[9].GetUInt8();
            item.MinMoney       = pField[10].GetUInt32();
            item.MaxMoney       = pField[11].GetUInt32();

            m_CustomCreatureLoot.emplace(item.Creature, std::move(item));
            ++nCounter;
        } while (res->NextRow());
    }
    TC_LOG_INFO("server.loading", "Loaded Custom Creature Loot (%d entries) in %ums", nCounter, GetMSTimeDiffToNow(msStartTime));
}

uint32 PlayerInfoSystem::GetRequiredExperience(uint16 Level, bool bArtifact)
{
    if (m_CustomLevelExperience.find(Level) != m_CustomLevelExperience.end())
        if (bArtifact)
            return m_CustomLevelExperience[Level].ArtifactExperience;
        else
            return m_CustomLevelExperience[Level].NeckExperience;

    return 0;
}

void PlayerInfoSystem::AddArtifactExperience(Player* p, uint32 Amt, bool bCommand)
{
    if (!Amt)
        return;
    if (!p->HasItemCount(ARTIFACT_ITEM_ID))
        return;
    if (AccountInfoItem* Info = GetAccountInfo(p->GetSession()->GetAccountId()))
    {
        if (Info->ArtifactLevel >= MAX_ARTIFACT_LEVEL)
            return;
        if (!GetRequiredExperience(Info->ArtifactLevel))
            return;

        Info->ArtifactExperience += Amt;// (Amt += (Amt / 100) * item.ArtifactLevel * 10);
        if (Info->AnnounceExp)
            ChatHandler(p->GetSession()).SendNotify("You recieved %d Artifact Experience", Amt);

        if (!Info->Updated)
            Info->Updated = true;

        bool bLeveled = false;

        while (Info->ArtifactExperience >= GetRequiredExperience(Info->ArtifactLevel))
        {
            bLeveled = true;
            Info->ArtifactExperience -= GetRequiredExperience(Info->ArtifactLevel);
            Info->ArtifactLevel++;

            if (!GetRequiredExperience(Info->ArtifactLevel))
            {
                p->SendPlaySpellVisual(13906);
                ChatHandler(p->GetSession()).PSendSysMessage("|cfffda025[Artifact System]:|r Congratulations, Your Artifact is now max level.");
                Info->ArtifactExperience = 0;
                return;
            }
        }

        if (bLeveled)
        {
            p->SendPlaySpellVisual(13906);
            ChatHandler(p->GetSession()).PSendSysMessage("|cfffda025[Artifact System]:|r Your Artifact is now level %d", Info->ArtifactLevel);
        }
    }
}

void PlayerInfoSystem::AddNeckExperience(Player* p, uint32 Amt)
{
    if (!Amt)
        return;
    if (CharacterInfoItem* Info = GetCharacterInfo(p->GetGUID()))
    {
        if (Info->NeckLevel >= MAX_NECK_LEVEL)
            return;

        uint8 OldLevel = Info->NeckLevel;

        if (Item* pItem = p->GetItemByPos(INVENTORY_SLOT_BAG_0, SLOT_NECK))
        {
            if (!pItem->GetTemplate()->HasFlag(ITEM_FLAGS_CU_CUSTOM_NECKLACE))
                return;

            if (!GetRequiredExperience(Info->NeckLevel, false))
                return;

            Info->NeckExperience += Amt;
            if (Info->AnnounceExp)
                ChatHandler(p->GetSession()).SendNotify("You recieved %d Cosmic Energy", Amt);

            bool bLeveled = false;

            while (Info->NeckExperience >= GetRequiredExperience(Info->NeckLevel, false))
            {
                bLeveled = true;
                Info->NeckExperience -= GetRequiredExperience(Info->NeckLevel, false);
                Info->NeckLevel++;

                if (!GetRequiredExperience(Info->NeckLevel, false))
                {
                    std::string Output = "|cffa335ee[Skycaller's Amulet]:|r Congratulations, your Amulet is now max level.";
                    Info->NeckExperience = 0;
                    p->SendPlaySpellVisual(13906);

                    if (Info->Prestige == 0)
                    {
                        pItem->SetEntry(pItem->GetEntry() + 5);
                        pItem->SetState(ITEM_CHANGED, p);
                        pItem->SendUpdateToPlayer(p);
                        Output += " Something is different about your Amulet...";
                    }

                    return ChatHandler(p->GetSession()).PSendSysMessage(Output.c_str());
                }
            }

            if (bLeveled)
            {
                std::string Output = "|cffa335ee[Skycaller's Amulet]:|r Your Amulet is now level " + std::to_string(Info->NeckLevel);
                p->SendPlaySpellVisual(13906);

                if (Info->Prestige == 0)
                {
                    for (int i = 1; i < 4; i++)
                        if (OldLevel <= i * 75 && Info->NeckLevel >= i * 75)
                        {
                            pItem->SetEntry(pItem->GetEntry() + 5);
                            pItem->SetState(ITEM_CHANGED, p);
                            pItem->SendUpdateToPlayer(p);
                            Output += " Something is different about your Amulet...";
                        }
                }

                return ChatHandler(p->GetSession()).PSendSysMessage(Output.c_str());
            }
        }
    }
}

void PlayerInfoSystem::AddCreatureLoot(Player* p, uint32 CreatureID, float rand)
{
    if (!CreatureID)
        return;

    if (m_CustomCreatureLoot.find(CreatureID) == m_CustomCreatureLoot.end())
        return;

    auto& Info = m_CustomCreatureLoot[CreatureID];

    if (Info.MinItemCount_1 > 0)
        p->AddItem(Info.ItemID_1, urand(Info.MinItemCount_1, Info.MaxItemCount_1));
    if (Info.MinItemCount_2 > 0)
        p->AddItem(Info.ItemID_2, urand(Info.MinItemCount_2, Info.MaxItemCount_2));
    if (Info.MinItemCount_3 > 0)
        p->AddItem(Info.ItemID_3, urand(Info.MinItemCount_3, Info.MaxItemCount_3));
    if (Info.MinMoney > 0 && Info.MaxMoney > 0)
    {
        uint32 LootedMoney = ((Info.MaxMoney - Info.MinMoney) * rand) + Info.MinMoney;
        if (p->ModifyMoney(LootedMoney, false))
        {
            std::string MoneyOutput = "";
            uint16 copper = LootedMoney % 100;
            LootedMoney = (LootedMoney - copper) / 100;
            uint16 silver = LootedMoney % 100;
            uint16 gold = (LootedMoney - silver) / 100;

            if (gold > 0)
                MoneyOutput += std::to_string(gold) + " Gold";

            if (silver > 0)
            {
                if (gold > 0)
                    MoneyOutput += ", ";

                MoneyOutput += std::to_string(silver) + " Silver";
            }

            if (copper > 0)
            {
                if (silver > 0 || gold > 0)
                    MoneyOutput += ", ";

                MoneyOutput += std::to_string(copper) + " Copper";
            }
            return ChatHandler(p->GetSession()).PSendSysMessage("You loot %s", MoneyOutput);
        }
    }
}

bool PlayerInfoSystem::CanUseDisplayID(uint32 display, uint8 type)
{
    if (m_WeaponDisplayIDs.find(display) == m_WeaponDisplayIDs.end())
        return false;

    return (m_WeaponDisplayIDs[display].WeaponType & type);
}

ObjectGuid PlayerInfoSystem::GetPlrUpgrade(uint32 CharID)
{
    if (CharacterInfoItem* Info = GetCharacterInfo(CharID))
        return Info->UpgradeItemGUID;

    return { };
}

void PlayerInfoSystem::SetPlrUpgrade(uint32 CharID, ObjectGuid ItemGUID)
{
    if (CharacterInfoItem* Info = GetCharacterInfo(CharID))
        Info->UpgradeItemGUID = ItemGUID;
}
