#include <Item.h>
#include <Chat.h>
#include <WorldSession.h>
#include <ScriptedGossip.h>

#include "PlayerInfo.h"

class CustomArtifactItem : public ItemScript
{
public:
    CustomArtifactItem() : ItemScript("CustomArtifactItem") {}

    enum Neck_Gossip_Menu
    {
        ARTIFACT_GOSSIP_LEVEL = 0,
        ARTIFACT_GOSSIP_EXP,
        ARTIFACT_GOSSIP_ANNOUNCE,
        ARTIFACT_GOSSIP_REWARDS,
        ARTIFACT_GOSSIP_REWARD_STAMINA,
        ARTIFACT_GOSSIP_REWARD_CRIT,
        ARTIFACT_GOSSIP_REWARD_MOVESPEED,
        ARTIFACT_GOSSIP_REWARD_CRAFTSPEED,
        ARTIFACT_GOSSIP_REWARD_LOOTCHANCE,
        ARTIFACT_GOSSIP_BACK,
        ARTIFACT_GOSSIP_CLOSE
    };

    bool OnUse(Player* p, Item* item, SpellCastTargets const& /*targets*/) override
    {
        ClearGossipMenuFor(p);

        if (AccountInfoItem* Info = sPlayerInfo.GetAccountInfo(p->GetSession()->GetAccountId()))
        {
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Level: " + std::to_string(Info->ArtifactLevel), 0, ARTIFACT_GOSSIP_LEVEL);
            if (Info->ArtifactLevel < MAX_ARTIFACT_LEVEL)AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Experience: " + std::to_string(Info->ArtifactExperience) + " / " + std::to_string(sPlayerInfo.GetRequiredExperience(Info->ArtifactLevel)), 0, ARTIFACT_GOSSIP_EXP);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Announce Experience: " + std::string(Info->AnnounceExp ? "Enabled" : "Disabled"), 0, ARTIFACT_GOSSIP_ANNOUNCE);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Rewards", 0, ARTIFACT_GOSSIP_REWARDS);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Close", 0, ARTIFACT_GOSSIP_CLOSE);

            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
        }

        return false;
    }

    void OnGossipSelect(Player* p, Item* item, uint32 uiSender, uint32 uiAction) override
    {
        ClearGossipMenuFor(p);

        switch (uiAction)
        {

        case ARTIFACT_GOSSIP_LEVEL:
        case ARTIFACT_GOSSIP_EXP:
        case ARTIFACT_GOSSIP_BACK:
        {
            if (AccountInfoItem* Info = sPlayerInfo.GetAccountInfo(p->GetSession()->GetAccountId()))
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Level: " + std::to_string(Info->ArtifactLevel), 0, ARTIFACT_GOSSIP_LEVEL);
                if (Info->ArtifactLevel < MAX_ARTIFACT_LEVEL)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Experience: " + std::to_string(Info->ArtifactExperience) + " / " + std::to_string(sPlayerInfo.GetRequiredExperience(Info->ArtifactLevel)), 0, ARTIFACT_GOSSIP_EXP);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Announce Experience: " + std::string(Info->AnnounceExp ? "Enabled" : "Disabled"), 0, ARTIFACT_GOSSIP_ANNOUNCE);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Rewards", 0, ARTIFACT_GOSSIP_REWARDS);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Close", 0, ARTIFACT_GOSSIP_CLOSE);

                SendGossipMenuFor(p, 16777216, item->GetGUID());
            }
            else
                CloseGossipMenuFor(p);
        } break;

        case ARTIFACT_GOSSIP_ANNOUNCE:
        {
            AccountInfoItem& Info = *sPlayerInfo.GetAccountInfo(p->GetSession()->GetAccountId());
            Info.AnnounceExp = !Info.AnnounceExp;
            OnGossipSelect(p, item, uiSender, ARTIFACT_GOSSIP_LEVEL);

        } break;

        case ARTIFACT_GOSSIP_REWARDS:
        case ARTIFACT_GOSSIP_REWARD_STAMINA:
        case ARTIFACT_GOSSIP_REWARD_CRIT:
        case ARTIFACT_GOSSIP_REWARD_MOVESPEED:
        case ARTIFACT_GOSSIP_REWARD_CRAFTSPEED:
        case ARTIFACT_GOSSIP_REWARD_LOOTCHANCE:
        {
            AccountInfoItem& Info = *sPlayerInfo.GetAccountInfo(p->GetSession()->GetAccountId());
            AddGossipItemFor(p, GOSSIP_ICON_DOT, "50 Stamina Every 5 Levels. - " + std::to_string(Info.ArtifactLevel / 5 * 50), 0, ARTIFACT_GOSSIP_REWARD_STAMINA);
            AddGossipItemFor(p, GOSSIP_ICON_DOT, "1% Crit Rating Every 333 Levels. - " + std::to_string(Info.ArtifactLevel / 333) + "%", 0, ARTIFACT_GOSSIP_REWARD_CRIT);
            AddGossipItemFor(p, GOSSIP_ICON_DOT, "1% Movement Speed Every 100 Levels. - " + std::to_string(Info.ArtifactLevel / 100) + "%", 0, ARTIFACT_GOSSIP_REWARD_MOVESPEED);
            AddGossipItemFor(p, GOSSIP_ICON_DOT, "5% Reduced Crafting Time Every 200 Levels. - " + std::to_string(Info.ArtifactLevel / 200) + "%", 0, ARTIFACT_GOSSIP_REWARD_CRAFTSPEED);
            AddGossipItemFor(p, GOSSIP_ICON_DOT, "1% Increased Loot Chance Every 40 Levels. - " + std::to_string(Info.ArtifactLevel / 40) + "%", 0, ARTIFACT_GOSSIP_REWARD_LOOTCHANCE);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", 0, ARTIFACT_GOSSIP_BACK);
            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
        } break;

        case ARTIFACT_GOSSIP_CLOSE:
        {
            CloseGossipMenuFor(p);
        } break;

        }

        return;
    }
};

void AddSC_CustomArtifactItem()
{
    new CustomArtifactItem();
}
