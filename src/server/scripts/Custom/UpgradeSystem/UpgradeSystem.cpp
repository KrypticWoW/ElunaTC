#include <ArenaTeamMgr.h>
#include <Chat.h>
#include <DatabaseEnv.h>
#include <DBCStores.h>
#include <GossipDef.h>
#include <Item.h>
#include <Log.h>
#include <ObjectMgr.h>
#include <ScriptedGossip.h>
#include <WorldSession.h>

#include "UpgradeSystem.h"
#include "../PlayerInfo/PlayerInfo.h"

std::string GetItemLink(uint32 entry, Player* p)
{
    const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
    std::ostringstream oss;
    oss << "|c" << std::hex << ItemQualityColors[temp->Quality] << std::dec << "|Hitem:" << entry << ":0:0:0:0:0:0:0:0:0|h[" << temp->Name1 << "]|h|r";

    return oss.str();
}

class DynamicUpgradeItem : public ItemScript
{
public:
    DynamicUpgradeItem() : ItemScript("DynamicUpgradeItem") {}

public:

    bool OnUse(Player* p, Item* item, SpellCastTargets const& targets) override
    {
        if (!targets.GetItemTarget())
            return false;

        UpgradeItem *upgradeInfo = sUpgradeSystem.GetUpgradeInfo(targets.GetItemTargetEntry());
        if (!upgradeInfo)
        {
            ChatHandler(p->GetSession()).SendNotify("Item is not upgradable");
            return false;
        }

        if (upgradeInfo->Entry != targets.GetItemTargetEntry())
        {
            ChatHandler(p->GetSession()).SendNotify("Item is not upgradable");
            return false;
        }

        sPlayerInfo.SetPlrUpgrade(p->GetGUID(), targets.GetItemTargetGUID());

        std::ostringstream details;

        if (sObjectMgr->GetItemTemplate(upgradeInfo->Entry)->GetMaxStackSize() <= 1)
        {
            if (p->HasItemCount(upgradeInfo->Entry))
                details << "|CFF009900+|r ";
            else
                details << "|CFFFF0000+|r ";

            details << "Requires: " << GetItemLink(upgradeInfo->Entry, p) << " x1\n";
        }

        for (int i = 0; i < MAX_REQ_ITEM_COUNT; i++)
        {
            if (upgradeInfo->ReqItemID[i] > 0)
            {
                if (p->HasItemCount(upgradeInfo->ReqItemID[i], upgradeInfo->ReqItemCount[i]))
                    details << "|CFF009900+|r ";
                else
                    details << "|CFFFF0000+|r ";

                details << "Requires: " << GetItemLink(upgradeInfo->ReqItemID[i], p) << " x" << upgradeInfo->ReqItemCount[i] << "\n";
            }
        }

        if (upgradeInfo->ReqGold > 0)
        {
            if (p->HasEnoughMoney(upgradeInfo->ReqGold * 10000))
                details << "|CFF009900+|r ";
            else
                details << "|CFFFF0000+|r ";

            details << "Requires: " << upgradeInfo->ReqGold << " Gold\n\n";
        }

        details << "|CFF009900+|r Upgrade Chance: " << (int)upgradeInfo->UpgradeChance << "%\n";

        std::string questTitle = "Upgrade Formula";

        std::string questDetails = details.str().c_str();
        std::string questObjectives = "Click |CFF009900*Accept*|r to upgrade!";
        std::string questAreaDescription = "";

        WorldPacket questData(SMSG_QUEST_GIVER_QUEST_DETAILS, 100);

        questData << uint64(item->GetGUID());
        questData << uint64(0);
        questData << uint32(999999); // Need to make an entry in quest_template
        questData << questTitle;
        questData << questDetails;
        questData << questObjectives;
        questData << uint8(0);
        questData << uint32(0);
        questData << uint32(0);
        questData << uint8(0);
        questData << uint32(1);
        questData << uint32(upgradeInfo->UpgradeEntry);
        questData << uint32(1);
        questData << uint32(sObjectMgr->GetItemTemplate(upgradeInfo->UpgradeEntry)->DisplayInfoID);
        questData << uint32(0);
        questData << uint32(0);
        questData << uint32(0);
        questData << uint32(0);
        questData << float(0.0f);
        questData << uint32(0);
        questData << int32(0);
        questData << uint32(0);
        questData << uint32(0);
        questData << uint32(0);
        questData << uint32(0);
        for (uint32 i = 0; i < QUEST_REPUTATIONS_COUNT; ++i)
            questData << uint32(0);
        for (uint32 i = 0; i < QUEST_REPUTATIONS_COUNT; ++i)
            questData << int32(0);
        for (uint32 i = 0; i < QUEST_REPUTATIONS_COUNT; ++i)
            questData << int32(0);
        questData << uint32(QUEST_EMOTE_COUNT);
        for (uint32 i = 0; i < QUEST_EMOTE_COUNT; ++i)
        {
            questData << uint32(0);
            questData << uint32(0);
        }
        p->GetSession()->SendPacket(&questData);

        return false;
    }
};

void AddSC_DynamicUpgradeItem()
{
    new DynamicUpgradeItem();
}

void UpgradeSystem::HandleUpgrade(Player* p)
{
    if (!p->GetItemByGuid(sPlayerInfo.GetPlrUpgrade(p->GetGUID())))
        return ChatHandler(p->GetSession()).PSendSysMessage("|CFF3B94A5[Upgrade System]|r: You are missing the item you want to upgrade.");

    UpgradeItem *upgradeInfo = sUpgradeSystem.GetUpgradeInfo(p->GetItemByGuid(sPlayerInfo.GetPlrUpgrade(p->GetGUID()))->GetEntry());
    if (!upgradeInfo)
        return ChatHandler(p->GetSession()).SendNotify("Item is not upgradable.");

    if (upgradeInfo->Entry <= 0)
        return;

    for (int i = 0; i < MAX_REQ_ITEM_COUNT; i++)
        if (upgradeInfo->ReqItemID[i] > 0 && upgradeInfo->ReqItemCount[i] > 0)
            if (!p->HasItemCount(upgradeInfo->ReqItemID[i], upgradeInfo->ReqItemCount[i] + (upgradeInfo->ReqItemID[i] == upgradeInfo->Entry)))
                return ChatHandler(p->GetSession()).PSendSysMessage(std::string("|CFF3B94A5[Upgrade System]|r: You are Missing " + GetItemLink(upgradeInfo->ReqItemID[i], p) + "x" + std::to_string(upgradeInfo->ReqItemCount[i] + (upgradeInfo->ReqItemID[i] == upgradeInfo->Entry) - p->GetItemCount(upgradeInfo->ReqItemID[i]))).c_str());//sObjectMgr->GetItemTemplate(upgradeInfo->ReqItemID[i])->Name1).c_str());

    if (upgradeInfo->ReqHonor)
        if (p->GetHonorPoints() < upgradeInfo->ReqHonor)
            return ChatHandler(p->GetSession()).PSendSysMessage("|CFF3B94A5[Upgrade System]|r: You don't have enough Honor Points.");

    if (upgradeInfo->ReqArenaPoints)
        if (p->GetArenaPoints() < upgradeInfo->ReqArenaPoints)
            return ChatHandler(p->GetSession()).PSendSysMessage("|CFF3B94A5[Upgrade System]|r: You don't have enough Arena Points.");

    if (upgradeInfo->ReqGold)
        if (!p->HasEnoughMoney(upgradeInfo->ReqGold * 10000))
            return ChatHandler(p->GetSession()).PSendSysMessage("|CFF3B94A5[Upgrade System]|r: You don't have enough Gold.");

    if (upgradeInfo->ReqArenaRating)
        if (p->GetMaxPersonalArenaRatingRequirement(0) < upgradeInfo->ReqArenaRating)
            return ChatHandler(p->GetSession()).PSendSysMessage("|CFF3B94A5[Upgrade System]|r: Your personal Arena Rating isn't high enough.");

    if (p->GetFreeInventorySpace() < 1)
        return ChatHandler(p->GetSession()).PSendSysMessage("|CFF3B94A5[Upgrade System]|r: You must have a free bag slot to upgrade.");

    if (rand() % 100 + 1 <= upgradeInfo->UpgradeChance)
    {
        Item* i = p->GetItemByGuid(sPlayerInfo.GetPlrUpgrade(p->GetGUID()));
        if (i->GetMaxStackCount() > 1)
        {
            p->DestroyItemCount(upgradeInfo->Entry, 1, true);
            p->AddItem(upgradeInfo->UpgradeEntry, 1);
        }
        else
        {
            i->SetEntry(upgradeInfo->UpgradeEntry);
            const ItemTemplate* itemTemplate = i->GetTemplate();
            const ItemTemplate* upgradeTemplate = sObjectMgr->GetItemTemplate(upgradeInfo->UpgradeEntry);
            p->PlayDirectSound(1204);

            for (uint32 enchant_slot = SOCK_ENCHANTMENT_SLOT; enchant_slot < SOCK_ENCHANTMENT_SLOT + MAX_GEM_SOCKETS; ++enchant_slot)
            {
                //if (itemTemplate->Socket[enchant_slot - SOCK_ENCHANTMENT_SLOT].Color == SOCKET_COLOR_META || upgradeTemplate->Socket[enchant_slot - SOCK_ENCHANTMENT_SLOT].Color == SOCKET_COLOR_META)
                if (itemTemplate->Socket[static_cast<std::array<_Socket, 3Ui64>::size_type>(enchant_slot) - SOCK_ENCHANTMENT_SLOT].Color & SOCKET_COLOR_META || upgradeTemplate->Socket[enchant_slot - SOCK_ENCHANTMENT_SLOT].Color & SOCKET_COLOR_META)
                    if (itemTemplate->Socket[enchant_slot - SOCK_ENCHANTMENT_SLOT].Color != upgradeTemplate->Socket[enchant_slot - SOCK_ENCHANTMENT_SLOT].Color)
                        i->ClearEnchantment(EnchantmentSlot(enchant_slot));
            }
            p->_RemoveAllItemMods();
            p->_ApplyAllItemMods();
        }

        p->ItemAddedQuestCheck(i->GetEntry(), 1);

        ChatHandler(p->GetSession()).SendNotify("Upgrade was successful.");
    }
    else
        ChatHandler(p->GetSession()).SendNotify("Upgrade Failed.");

    for (int i = 0; i < MAX_REQ_ITEM_COUNT; i++)
        if (upgradeInfo->ReqItemID[i] > 0)
            p->DestroyItemCount(upgradeInfo->ReqItemID[i], upgradeInfo->ReqItemCount[i], true);
    if (upgradeInfo->ReqGold > 0)
        p->ModifyMoney(upgradeInfo->ReqGold * -10000);
    if (upgradeInfo->ReqHonor > 0)
        p->ModifyHonorPoints(upgradeInfo->ReqHonor * -1);
    if (upgradeInfo->ReqArenaPoints > 0)
        p->ModifyArenaPoints(upgradeInfo->ReqArenaPoints * -1);

    p->PlayerTalkClass->ClearMenus();
    p->PlayerTalkClass->SendCloseGossip();

    return;
}

void UpgradeSystem::Load()
{
    TC_LOG_INFO("server.loading", "Loading UpgradeSystem...");
    m_Upgrades.clear();
    uint32 msStartTime = getMSTime();
    int nCounter = 0;

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_UPGRADE_INFORMATION);
    PreparedQueryResult res = WorldDatabase.Query(stmt);
    if (res)
    {
        std::set<UpgradeItem*> lookupTable;
        do
        {
            Field* pField = res->Fetch();
            UpgradeItem item = { 0 };

            item.Entry = pField[0].GetUInt32();
            item.UpgradeEntry = pField[1].GetUInt32();
            item.ReqGold = pField[2].GetUInt32();
            for (int i = 0; i < MAX_REQ_ITEM_COUNT; i++)
            {
                item.ReqItemID[i] = pField[3 + (i * 2)].GetInt32();
                item.ReqItemCount[i] = pField[4 + (i * 2)].GetUInt16();
            }
            item.ReqHonor = pField[11].GetUInt32();
            item.ReqArenaPoints = pField[12].GetUInt16();
            item.ReqArenaRating = pField[13].GetUInt16();
            item.UpgradeChance = pField[14].GetUInt8();

            m_Upgrades.emplace(item.Entry, std::move(item));

            ++nCounter;
        } while (res->NextRow());
    }
    TC_LOG_INFO("server.loading", "Loaded UpgradeSystem (%d entries) in %ums", nCounter, GetMSTimeDiffToNow(msStartTime));
}
