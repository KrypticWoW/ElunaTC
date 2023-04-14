#include <Item.h>
#include <ScriptedGossip.h>
#include <WorldSession.h>
#include <ObjectMgr.h>
#include <Log.h>
#include <SpellMgr.h>
#include <DatabaseEnv.h>
#include <Chat.h>

class AccountBoundItem : public ItemScript
{
public:
    AccountBoundItem() : ItemScript("AccountBoundSpell") {}

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (auto iTemplate = sObjectMgr->GetItemTemplate(item->GetEntry()))
        {
            auto Spells = iTemplate->Spells;
            if (Spells[0].SpellId != 0)
            {
                uint32 LearnedSpellId = (Spells[0].SpellId == 483 || Spells[0].SpellId == 55884) ? Spells[1].SpellId : Spells[0].SpellId;

                if (auto spellInfo = sSpellMgr->GetSpellInfo(LearnedSpellId))
                {
                    uint8 ReqRidingSkill = 0;
                    if (iTemplate->RequiredSkill == 762)
                        ReqRidingSkill = iTemplate->RequiredSkillRank;

                    if (!player->HasSpell(spellInfo->Id))
                    {
                        CharacterDatabase.PExecute("INSERT IGNORE INTO custom.`account_spells` VALUES (%u, %u, -1, -1, 0, 1);", player->GetSession()->GetAccountId(), spellInfo->Id, iTemplate->AllowableRace, iTemplate->AllowableClass, ReqRidingSkill);
                        player->AddSpell(spellInfo->Id, true, false, true, false, false);
                        player->DestroyItemCount(item->GetEntry(), 1, true);

                        WorldPacket data(SMSG_LEARNED_SPELL, 6);
                        data << uint32(spellInfo->Id);
                        data << uint16(0);
                        player->SendDirectMessage(&data);
                    }
                    else
                        ChatHandler(player->GetSession()).SendSysMessage("You have already learned this spell.");;
                }
                else
                    TC_LOG_WARN("server.loading", "AccountBoundSystem: Item with ID %u has invalid spell ID(%u).", item->GetEntry(), iTemplate->Spells[0].SpellId);
            }
            else
                TC_LOG_WARN("server.loading", "AccountBoundSystem: Item with ID %u has has invalid spell ID(0).", item->GetEntry());
        }
        else
            TC_LOG_WARN("server.loading", "AccountBoundSystem: Unable to find ItemTemplate for ID %u.", item->GetEntry());

        return false;
    }
};

void AddSC_AccountBoundItem()
{
    new AccountBoundItem();
}
