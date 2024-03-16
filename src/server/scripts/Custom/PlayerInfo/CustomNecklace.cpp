#include <Item.h>
#include <Chat.h>
#include <WorldSession.h>
#include <ScriptedGossip.h>

#include "PlayerInfo.h"

class CustomNeckItem : public ItemScript
{
public:
    CustomNeckItem() : ItemScript("CustomNeckItem") {}

    int32 ConversionCost = 1000000;
    enum Neck_Gossip_Menu
    {
        NECK_GOSSIP_LEVEL = 0,
        NECK_GOSSIP_EXP,
        NECK_GOSSIP_PRESTIGE_RANK,
        NECK_GOSSIP_PRESTIGE,
        NECK_GOSSIP_ANNOUNCE,
        NECK_GOSSIP_CONVERT,
        NECK_GOSSIP_CONVERT_ITEM
    };

    bool OnUse(Player* p, Item* item, SpellCastTargets const& /*targets*/) override
    {
        ClearGossipMenuFor(p);

        if (CharacterInfoItem* Info = sPlayerInfo.GetCharacterInfo(p->GetGUID()))
        {
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Level: " + std::to_string(Info->NeckLevel), 0, NECK_GOSSIP_LEVEL);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Prestige Rank: " + std::to_string(Info->Prestige), 0, NECK_GOSSIP_PRESTIGE_RANK);
            if (Info->NeckLevel < MAX_NECK_LEVEL)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Experience: " + std::to_string(Info->NeckExperience) + " / 100", 0, NECK_GOSSIP_EXP);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Announce Experience: " + std::string(Info->AnnounceExp ? "Enabled" : "Disabled"), 0, NECK_GOSSIP_ANNOUNCE);

            if (Info->NeckLevel >= MAX_NECK_LEVEL)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Prestige Necklace", 0, NECK_GOSSIP_PRESTIGE, "Are you sure? This will reset yoru necklace back to level 1.", 0, false);

            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert Necklace", 0, NECK_GOSSIP_CONVERT);

            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
        }

        return false;
    }

    void OnGossipSelect(Player* p, Item* item, uint32 uiSender, uint32 uiAction) override
    {
        ClearGossipMenuFor(p);

        switch (uiAction)
        {

        case NECK_GOSSIP_LEVEL:
        case NECK_GOSSIP_EXP:
        case NECK_GOSSIP_PRESTIGE_RANK:
        {
            if (CharacterInfoItem* Info = sPlayerInfo.GetCharacterInfo(p->GetGUID()))
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Level: " + std::to_string(Info->NeckLevel), 0, NECK_GOSSIP_LEVEL);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Prestige Rank: " + std::to_string(Info->Prestige), 0, NECK_GOSSIP_PRESTIGE_RANK);
                if (Info->NeckLevel < MAX_NECK_LEVEL)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Experience: " + std::to_string(Info->NeckExperience) + " / 100", 0, NECK_GOSSIP_EXP);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Announce Experience: " + std::string(Info->AnnounceExp ? "Enabled" : "Disabled"), 0, NECK_GOSSIP_ANNOUNCE);

                if (Info->NeckLevel >= MAX_NECK_LEVEL)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Prestige Necklace", 0, NECK_GOSSIP_PRESTIGE, "Are you sure? This will reset yoru necklace back to level 1.", 0, false);

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert Necklace", 0, NECK_GOSSIP_CONVERT);

                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
            }
            else
                CloseGossipMenuFor(p);
        } break;

        case NECK_GOSSIP_PRESTIGE:
        {
            if (p->IsInCombat())
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to prestige necklace while in combat.");
                OnGossipSelect(p, item, uiSender, NECK_GOSSIP_LEVEL);
            }
            CharacterInfoItem& Info = *sPlayerInfo.GetCharacterInfo(p->GetGUID());

            if (Info.NeckLevel >= MAX_NECK_LEVEL)
            {
                // Add Prestige Token
                Info.NeckLevel = 1;
                Info.NeckExperience = 0;
                Info.Prestige++;
            }

            CloseGossipMenuFor(p);

        } break;

        case NECK_GOSSIP_ANNOUNCE:
        {
            CharacterInfoItem& Info = *sPlayerInfo.GetCharacterInfo(p->GetGUID());
            Info.AnnounceExp = !Info.AnnounceExp;
            OnGossipSelect(p, item, uiSender, NECK_GOSSIP_LEVEL);

        } break;

        case NECK_GOSSIP_CONVERT:
        {
            uint32 NeckRole = item->GetEntry() - 68000;
            if (NeckRole != 0)
                NeckRole /= 255;

            if (NeckRole != 0)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert to Tank Amulet", 0, NECK_GOSSIP_CONVERT_ITEM, "Are you sure?", ConversionCost, false);
            if (NeckRole != 1)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert to Healer Amulet", 1, NECK_GOSSIP_CONVERT_ITEM, "Are you sure?", ConversionCost, false);
            if (NeckRole != 2)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert to Caster Amulet", 2, NECK_GOSSIP_CONVERT_ITEM, "Are you sure?", ConversionCost, false);
            if (NeckRole != 3)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert to Strength Dps Amulet", 3, NECK_GOSSIP_CONVERT_ITEM, "Are you sure?", ConversionCost, false);
            if (NeckRole != 4)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert to Agility Dps Amulet", 4, NECK_GOSSIP_CONVERT_ITEM, "Are you sure?", ConversionCost, false);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Nevermind", 4, NECK_GOSSIP_LEVEL);
            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
        } break;

        case NECK_GOSSIP_CONVERT_ITEM:
        {
            CloseGossipMenuFor(p);
            if (!p->HasEnoughMoney(ConversionCost))
                return ChatHandler(p->GetSession()).SendSysMessage("You don't have enough Money");

            if (CharacterInfoItem* Info = sPlayerInfo.GetCharacterInfo(p->GetGUID()))
            {
                p->_ApplyItemMods(item, item->GetSlot(), false);
                uint32 NewItemID = 68000 + (255 * uiSender) + (Info->NeckLevel - 1);
                item->SetEntry(NewItemID);
                item->SetState(ITEM_CHANGED, p);
                item->SendUpdateToPlayer(p);
                p->_ApplyItemMods(item, item->GetSlot(), true);
                p->ModifyMoney(-ConversionCost);
            }
            else
                return ChatHandler(p->GetSession()).SendSysMessage("Internal error, please report to a staff member");
        } break;

        }

        return;
    }
};

void AddSC_CustomNeckItem()
{
    new CustomNeckItem();
}
