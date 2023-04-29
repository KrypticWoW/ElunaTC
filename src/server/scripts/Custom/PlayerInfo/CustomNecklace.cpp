#include <Item.h>
#include <Chat.h>
#include <WorldSession.h>
#include <ScriptedGossip.h>

#include "PlayerInfo.h"

uint16 const AllowedConversions[] = { 1059, 1106, 1488, 35, 1100 };

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
        NECK_GOSSIP_CONVERT,
        NECK_GOSSIP_CONVERT_ITEM
    };

    bool OnUse(Player* p, Item* item, SpellCastTargets const& /*targets*/) override
    {
        ClearGossipMenuFor(p);
        if (p->IsInCombat())
            return false;

        if (CharacterInfoItem* Info = sPlayerInfo.GetCharacterInfo(p->GetGUID()))
        {
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Level: " + std::to_string(Info->NeckLevel), 0, NECK_GOSSIP_LEVEL);
            if (Info->NeckLevel < MAX_NECK_LEVEL)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Experience: " + std::to_string(Info->NeckExperience), 0, NECK_GOSSIP_EXP);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Prestige Rank: " + std::to_string(Info->Prestige), 0, NECK_GOSSIP_PRESTIGE_RANK);

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

        if (p->IsInCombat())
        {
            CloseGossipMenuFor(p);
            return;
        }

        switch (uiAction)
        {

        case NECK_GOSSIP_LEVEL:
        case NECK_GOSSIP_EXP:
        case NECK_GOSSIP_PRESTIGE_RANK:
        {
            if (CharacterInfoItem* Info = sPlayerInfo.GetCharacterInfo(p->GetGUID()))
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Level: " + std::to_string(Info->NeckLevel), 0, NECK_GOSSIP_LEVEL);
                if (Info->NeckLevel < MAX_NECK_LEVEL)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Experience: " + std::to_string(Info->NeckExperience), 0, NECK_GOSSIP_EXP);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Prestige Rank: " + std::to_string(Info->Prestige), 0, NECK_GOSSIP_PRESTIGE_RANK);

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
            CharacterInfoItem& Info = *sPlayerInfo.GetCharacterInfo(p->GetGUID());

            if (Info.NeckLevel >= MAX_NECK_LEVEL)
            {
                // Add Prestige Token
                Info.NeckLevel = 1;
                Info.NeckExperience = 0;
                Info.Prestige++;

                if (Info.Prestige <= 4)
                {
                    item->SetEntry(item->GetEntry() + 5);
                    item->SetState(ITEM_CHANGED, p);
                    item->SendUpdateToPlayer(p);
                }
            }

            CloseGossipMenuFor(p);

        } break;

        case NECK_GOSSIP_CONVERT:
        {
            uint32 NeckRole = item->GetEntry() % 65000 % 5;

            if (AllowedConversions[0] & p->GetClassMask() && NeckRole != 0)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert to Tank Amulet", 0, NECK_GOSSIP_CONVERT_ITEM, "Are you sure?", ConversionCost, false);
            if (AllowedConversions[1] & p->GetClassMask() && NeckRole != 1)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert to Healer Amulet", 1, NECK_GOSSIP_CONVERT_ITEM, "Are you sure?", ConversionCost, false);
            if (AllowedConversions[2] & p->GetClassMask() && NeckRole != 2)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert to Caster Amulet", 2, NECK_GOSSIP_CONVERT_ITEM, "Are you sure?", ConversionCost, false);
            if (AllowedConversions[3] & p->GetClassMask() && NeckRole != 3)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert to Strength Dps Amulet", 3, NECK_GOSSIP_CONVERT_ITEM, "Are you sure?", ConversionCost, false);
            if (AllowedConversions[4] & p->GetClassMask() && NeckRole  != 4)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Convert to Agility Dps Amulet", 4, NECK_GOSSIP_CONVERT_ITEM, "Are you sure?", ConversionCost, false);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Nevermind", 4, NECK_GOSSIP_LEVEL);
            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
        } break;

        case NECK_GOSSIP_CONVERT_ITEM:
        {
            CloseGossipMenuFor(p);
            if (!p->HasEnoughMoney(ConversionCost))
                return ChatHandler(p->GetSession()).SendSysMessage("You don't have enough Money");

            p->ModifyMoney(-ConversionCost);
            item->SetEntry(item->GetEntry() + (uiSender - (item->GetEntry() % 65000 % 5)));
            item->SetState(ITEM_CHANGED, p);
            item->SendUpdateToPlayer(p);
        } break;

        }

        return;
    }
};

void AddSC_CustomNeckItem()
{
    new CustomNeckItem();
}
