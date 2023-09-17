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
    };

    bool OnUse(Player* p, Item* item, SpellCastTargets const& /*targets*/) override
    {
        ClearGossipMenuFor(p);
        if (p->IsInCombat())
            return false;

        if (AccountInfoItem* Info = sPlayerInfo.GetAccountInfo(p->GetSession()->GetAccountId()))
        {
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Level: " + std::to_string(Info->ArtifactLevel), 0, ARTIFACT_GOSSIP_LEVEL);
            if (Info->ArtifactLevel < MAX_ARTIFACT_LEVEL)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Experience: " + std::to_string(Info->ArtifactExperience), 0, ARTIFACT_GOSSIP_EXP);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Announce Experience: " + std::string(Info->AnnounceExp ? "Enabled" : "Disabled"), 0, ARTIFACT_GOSSIP_ANNOUNCE);

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

        case ARTIFACT_GOSSIP_LEVEL:
        case ARTIFACT_GOSSIP_EXP:
        {
            if (AccountInfoItem* Info = sPlayerInfo.GetAccountInfo(p->GetSession()->GetAccountId()))
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Level: " + std::to_string(Info->ArtifactLevel), 0, ARTIFACT_GOSSIP_LEVEL);
                if (Info->ArtifactLevel < MAX_ARTIFACT_LEVEL)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Experience: " + std::to_string(Info->ArtifactExperience), 0, ARTIFACT_GOSSIP_EXP);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Announce Experience: " + std::string(Info->AnnounceExp ? "Enabled" : "Disabled"), 0, ARTIFACT_GOSSIP_ANNOUNCE);

                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
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

        }

        return;
    }
};

void AddSC_CustomArtifactItem()
{
    new CustomArtifactItem();
}
