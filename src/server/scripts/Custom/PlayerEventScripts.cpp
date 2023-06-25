#include <Bag.h>
#include <Chat.h>
#include <DatabaseEnv.h>
#include <Group.h>
#include <Item.h>
#include <Log.h>
#include <ObjectMgr.h>
#include <WorldSession.h>

#include "SpellRegulator.h"
#include "PlayerInfo/PlayerInfo.h"
#include "TeleportSystem/TeleportSystem.h"
#include "UpgradeSystem/UpgradeSystem.h"
#include "EventSystem/EventSystem.h"

class CustomPlayerScripts : public PlayerScript
{
public:
    CustomPlayerScripts() : PlayerScript("CustomPlayerScripts") {}

    void OnCreatureKill(Player* player, Creature* killed) override
    {
        if (killed->IsPet())
            return;
        if (!killed->GetLootRecipient())
            return;

        uint32 ArtifactExp = killed->GetCreatureTemplate()->ArtifactExp;
        uint32 NeckExp = killed->GetCreatureTemplate()->NeckExp;

        Group* group = killed->GetLootRecipient()->GetGroup();
        if (group)
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* player = itr->GetSource();
	
                if (!player || !player->GetSession())
                    continue;
	
                if (player->IsAtGroupRewardDistance(killed))
                {
                    if (ArtifactExp > 0)
                        sPlayerInfo.AddArtifactExperience(player, killed->GetCreatureTemplate()->ArtifactExp);
                    if (NeckExp > 0)
                        sPlayerInfo.AddNeckExperience(player, killed->GetCreatureTemplate()->NeckExp);
                }
            }
            return;
        }

        if (ArtifactExp > 0)
            sPlayerInfo.AddArtifactExperience(player, killed->GetCreatureTemplate()->ArtifactExp);
        if (NeckExp > 0)
            sPlayerInfo.AddNeckExperience(player, killed->GetCreatureTemplate()->NeckExp);
    }

    void OnCreate(Player* player) override
    {
        sPlayerInfo.CreateCharacterInfo(player);
    }

    void OnDelete(ObjectGuid guid, uint32 accountId)
    {
        sPlayerInfo.DeleteCharacterInfo(guid, accountId);
    }

    void OnLogin(Player* player, bool firstLogin) override
    {
        sPlayerInfo.LoadCharacterInfo(player->GetGUID());
    }

    void OnLogout(Player* player) override
    {
        sPlayerInfo.RemoveCharacterInfo(player->GetGUID());
        sEventSystem.KickPlayer(player, false);
    }
};

class CustomAccountScripts : public AccountScript
{
public:
    CustomAccountScripts() : AccountScript("CustomAccountScripts") {}

    void OnAccountLogin(uint32 accountId) override
    {
        sPlayerInfo.LoadAccountInfo(accountId);
    }
};

class CustomWorldScripts : public WorldScript
{
public:
    CustomWorldScripts() : WorldScript("CustomWorldScripts") {}

    void OnUpdate(uint32 diff) override
    {
        sPlayerInfo.Update(diff);
        sEventSystem.Update(diff);
    }
    void OnShutdown() override
    {
        sPlayerInfo.SaveAccountInfo();
    }

    void OnStartup() override
    {
        // Loads Upgrade System
        sUpgradeSystem.Load();

        // Loads Teleport System
        sTeleSystem.Load();

        // Loads Spell Modifier
        sSpellModifier.Load();

        // Loads Magic Experience
        sPlayerInfo.LoadAllOnStart();
    }
};

class CustomCommandScripts : public CommandScript
{
public:
    CustomCommandScripts() : CommandScript("CustomCommandScripts") {}

    struct ArtifactBuffs
    {
        uint32 Buff;
        uint16 Level;
    };

    Trinity::ChatCommands::ChatCommandTable GetCommands() const override
    {
        static Trinity::ChatCommands::ChatCommandTable GameEventCommandTable =
        {
            { "join",       HandleGameEventJoinCommand,         rbac::RBAC_ROLE_PLAYER,     Trinity::ChatCommands::Console::No },
            { "leave",      HandleGameEventLeaveCommand,        rbac::RBAC_ROLE_PLAYER,     Trinity::ChatCommands::Console::No },
            { "info",       HandleGameEventInfoCommand,         rbac::RBAC_ROLE_PLAYER,     Trinity::ChatCommands::Console::No },
            { "vote",       HandleGameEventVoteCommand,         rbac::RBAC_ROLE_PLAYER,     Trinity::ChatCommands::Console::No },
            { "disable",    HandleGameEventDisableCommand,      rbac::RBAC_ROLE_GAMEMASTER, Trinity::ChatCommands::Console::No },
            { "enable",     HandleGameEventEnableCommand,       rbac::RBAC_ROLE_GAMEMASTER, Trinity::ChatCommands::Console::No },
            { "terminate",  HandleGameEventTerminateCommand,    rbac::RBAC_ROLE_GAMEMASTER, Trinity::ChatCommands::Console::No },
            //{ "respawn",    HandleGameEventRespawnCommand,  rbac::RBAC_ROLE_PLAYER, Trinity::ChatCommands::Console::No },
            //{ "teleport",   HandleGameEventTeleportCommand, rbac::RBAC_ROLE_PLAYER, Trinity::ChatCommands::Console::No },
        };
        static Trinity::ChatCommands::ChatCommandTable ListCommandTable =
        {
            { "inventory",          HandleListInventoryCommand,         rbac::RBAC_ROLE_GAMEMASTER,         Trinity::ChatCommands::Console::Yes },
        };
        static Trinity::ChatCommands::ChatCommandTable MagicCommandTable =
        {
            { "addexp",             HandleMagicAddExpCommand,           rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "info",               HandleMagicInfoCommand,             rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "setlevel",           HandleMagicSetLevelCommand,         rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
        };
        static Trinity::ChatCommands::ChatCommandTable ReloadCommandTable =
        {
            { "spell_modifier",     HandleReloadSpellModifierCommand,   rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "teleport_system",    HandleReloadTeleportSystemCommand,  rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "upgrade_system",     HandleReloadUpgradeSystemCommand,   rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "weapon_display_ids", HandleReloadWeaponDisplaysCommand,  rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
        };
        static std::vector<ChatCommand> CustomCommandTable =
        {
            { "buff",               HandleBuffCommand,                  rbac::RBAC_ROLE_PLAYER,                Trinity::ChatCommands::Console::No },
            { "abcd",               HandleAbcdCommand,                  rbac::RBAC_ROLE_PLAYER,                Trinity::ChatCommands::Console::No },
            { "gameevent",          GameEventCommandTable },
            { "list",               ListCommandTable },
            { "magic",              MagicCommandTable },
            { "reload",             ReloadCommandTable },
		};
        return CustomCommandTable;
    }

    // Buff Command
	
    static bool HandleBuffCommand(ChatHandler* handler, const char* /**/)
    {
        Player* p = handler->GetSession()->GetPlayer();
        if (p->InBattleground() || p->InArena())
        {
            ChatHandler(p->GetSession()).SendNotify("You can't use this command in pvp zones.");
            return true;
        }
	
        uint8 AccBuffs = sPlayerInfo.GetAccountInfo(p->GetSession()->GetAccountId())->Buffs;
        uint16 ArtifactLevel = sPlayerInfo.GetAccountInfo(p->GetSession()->GetAccountId())->ArtifactLevel;
	
        for (auto& buff : m_Auras)
        {
            if (ArtifactLevel >= buff.Level)
                p->AddAura(buff.Buff, p);
        }
	
        for (uint16 i = 1, ctr = 0; i < 256; i *= 2, ctr++)
        {
            if (AccBuffs & i)
                p->AddAura(m_AccBuffs[ctr], p);
        }
	
        handler->SendNotify("|cffB400B4You have been buffed, enjoy!");
        return true;
    }


    static bool HandleAbcdCommand(ChatHandler* handler, const char* /**/)
    {
        Player* p = handler->GetSession()->GetPlayer();

        p->MorphOnUpdate = !p->MorphOnUpdate;

        return true;
    }

    // GameEvent Commands

    static bool HandleGameEventJoinCommand(ChatHandler* handler, const char* /**/)
    {
        if (handler && handler->GetPlayer())
            sEventSystem.InvitePlayer(handler->GetPlayer());

        return true;
    }

    static bool HandleGameEventLeaveCommand(ChatHandler* handler, const char* /**/)
    {
        if (handler && handler->GetPlayer())
            sEventSystem.KickPlayer(handler->GetPlayer(), true);

        return true;
    }

    static bool HandleGameEventInfoCommand(ChatHandler* handler, const char* /**/)
    {
        if (handler && handler->GetPlayer())
            sEventSystem.SendGameEventInfo(handler->GetPlayer());;

        return true;
    }

    static bool HandleGameEventVoteCommand(ChatHandler* handler, Optional<uint16> voteID)
    {
        if (handler && handler->GetPlayer())
            if (!voteID)
                handler->SendSysMessage("Incorrect vote ID.");
            else
                sEventSystem.HandlePlayerVote(handler->GetPlayer(), *voteID);

        return true;
    }

    static bool HandleGameEventDisableCommand(ChatHandler* handler, const char* /**/)
    {
        if (sEventSystem.IsActive())
        {
            handler->SendSysMessage("Unable to disable Event System while a current event is active. Type .GameEvent Terminate to force end an event.");
            return true;
        }


        if (sEventSystem.IsEnabled())
        {
            handler->SendSysMessage("Event System has been disabled.");
            sEventSystem.UpdateAllowEventCreation(false);
        }
        else
            handler->SendSysMessage("Event System is already disabled.");

        return true;
    }

    static bool HandleGameEventEnableCommand(ChatHandler* handler, const char* /**/)
    {
        if (sEventSystem.IsEnabled())
            handler->SendSysMessage("Event System is already enabled.");
        else
        {
            sEventSystem.UpdateAllowEventCreation(true);
            handler->SendSysMessage("Event System has been enabled.");
        }
        return true;
    }

    static bool HandleGameEventTerminateCommand(ChatHandler* handler, const char* /**/)
    {
        sEventSystem.GlobalAnnounce("Event has forcefully been terminated.", MEMBER_ANNOUNCE);
        sEventSystem.EndEvent();
        handler->SendSysMessage("Event has been terminated.");
        return true;
    }

    // List Commands

    static bool HandleListInventoryCommand(ChatHandler* handler, Optional<Trinity::ChatCommands::PlayerIdentifier> targetIdentifier)
    {
        // Checks for target, or <name> 
        if (!targetIdentifier)
            targetIdentifier = Trinity::ChatCommands::PlayerIdentifier::FromTarget(handler);
        if (!targetIdentifier)
        {
            handler->PSendSysMessage("Syntax: .list inventory [$playername]|nOutputs a list of character with $playername (or selected if name not provided) Inventory.");
            return true;
        }

        Player* target = targetIdentifier->GetConnectedPlayer();

        // If targeted player is online
        if (target)
        {
            std::string Output = "(Account: " + std::to_string(target->GetSession()->GetAccountId()) + " Guid: " + std::to_string(target->GetGUID()) + "): " + target->GetSession()->GetPlayerName() + " has ";

            for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; i++)
                if (Item* pItem = target->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    handler->PSendSysMessage("%s%sx%u - %s", Output, (handler->IsConsole() ? "[" + std::to_string(pItem->GetEntry()) + "] [" + pItem->GetTemplate()->Name1 + "]" : pItem->GetItemLink()), pItem->GetCount(), i >= INVENTORY_SLOT_BAG_END ? "Bag" : "Equipped");

            for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
                for (uint8 j = 0; j < MAX_BAG_SIZE; j++)
                    if (Item* pItem = target->GetItemByPos(((i << 8) | j)))
                        handler->PSendSysMessage("%s%sx%u - Bag", Output, (handler->IsConsole() ? "[" + std::to_string(pItem->GetEntry()) + "] [" + pItem->GetTemplate()->Name1 + "]" : pItem->GetItemLink()), pItem->GetCount());

            for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++)
                if (Item* pItem = target->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    handler->PSendSysMessage("%s%sx%u - Bank", Output, (handler->IsConsole() ? "[" + std::to_string(pItem->GetEntry()) + "] [" + pItem->GetTemplate()->Name1 + "]" : pItem->GetItemLink()), pItem->GetCount());

            for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
                if (Item* pItem = target->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                {
                    handler->PSendSysMessage("%s%sx%u - Bank", Output, (handler->IsConsole() ? "[" + std::to_string(pItem->GetEntry()) + "] [" + pItem->GetTemplate()->Name1 + "]" : pItem->GetItemLink()), pItem->GetCount());

                    for (uint8 j = 0; j < MAX_BAG_SIZE; j++)
                        if (Item* pItem2 = target->GetItemByPos(i, j))
                            handler->PSendSysMessage("%s%sx%u - Bank", Output, (handler->IsConsole() ? "[" + std::to_string(pItem->GetEntry()) + "] [" + pItem2->GetTemplate()->Name1 + "]" : pItem2->GetItemLink()), pItem2->GetCount());
                }

            for (uint8 i = CURRENCYTOKEN_SLOT_START; i < CURRENCYTOKEN_SLOT_END; i++)
                if (Item* pItem = target->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    handler->PSendSysMessage("%s%sx%u - Currency", Output, (handler->IsConsole() ? "[" + std::to_string(pItem->GetEntry()) + "] [" + pItem->GetTemplate()->Name1 + "]" : pItem->GetItemLink()), pItem->GetCount());

            return true;
        }

        // if target player is offline
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CUSTOM_CHAR_SEL_INVENTORY);
        stmt->setString(0, targetIdentifier->GetName());
        PreparedQueryResult queryResult = CharacterDatabase.Query(stmt);
        if (queryResult)
        {
            do
            {
                Field* fields = queryResult->Fetch();

                uint32 item_entry = fields[0].GetUInt32();
                std::string item_name = fields[1].GetString();
                uint32 item_count = fields[2].GetUInt32();
                std::string char_name = fields[3].GetString();
                std::string item_enchants = ":" + fields[4].GetString();
                uint32 acc_guid = fields[5].GetUInt32();
                uint32 char_guid = fields[6].GetUInt32();

                for (auto& s : item_enchants)
                {
                    if (s == ' ')
                        s = ':';
                }

                std::ostringstream ItemLink;
                if (!handler->IsConsole())
                    ItemLink << "|c" << std::hex << ItemQualityColors[sObjectMgr->GetItemTemplate(item_entry)->Quality] << std::dec << "|Hitem:" << item_entry << item_enchants << "|h[" << item_name << "]|h|r";
                else
                    ItemLink << "[" << item_entry << "] [" << item_name << "]";

                handler->PSendSysMessage("(Account: %u Guid: %u): %s has %sx%u", acc_guid, char_guid, char_name, ItemLink.str(), item_count);
            } while (queryResult->NextRow());
        }
        else
            handler->PSendSysMessage("Player %s not found, or doesn't have any items.", targetIdentifier->GetName());

        return true;
    }
	
    // Magic Commands
	
    static bool HandleMagicAddExpCommand(ChatHandler* handler, Optional<Trinity::ChatCommands::PlayerIdentifier> player, uint32 amt)
    {
        if (!player)
            player = Trinity::ChatCommands::PlayerIdentifier::FromTargetOrSelf(handler);
        if (!player)
            return true;
	
        if (Player* target = player->GetConnectedPlayer())
        {
            sPlayerInfo.AddArtifactExperience(target, amt);
	
            if (handler->IsConsole())
                ChatHandler(target->GetSession()).PSendSysMessage("|CFFFF0000[Console]|r added %u experience to your Artifact.", amt);
            else
                if (target != handler->GetSession()->GetPlayer())
                    ChatHandler(target->GetSession()).PSendSysMessage("%s added %u experience to your Artifact.", handler->playerLink(handler->GetPlayer()->GetName()), amt);
        }
        else
        {
            handler->PSendSysMessage("Player is not online or doesn't exist.");
            return true;
        }
	
        handler->PSendSysMessage("You gave %s %u Magic Experience.", handler->playerLink(*player).c_str(), amt);
        return true;
    }
	
    static bool HandleMagicInfoCommand(ChatHandler* handler, Optional<Trinity::ChatCommands::PlayerIdentifier> player)
    {
        if (!player)
            player = Trinity::ChatCommands::PlayerIdentifier::FromTargetOrSelf(handler);
        if (!player)
            return true;
	
        if (Player* target = player->GetConnectedPlayer())
        {
            AccountInfoItem* Info = sPlayerInfo.GetAccountInfo(target->GetSession()->GetAccountId());
            handler->PSendSysMessage("%s Artifact Level is %i, and has %i experience.", handler->playerLink(*player).c_str(), Info->ArtifactLevel, Info->ArtifactExperience);
            return true;
        }
        else
        {
            handler->PSendSysMessage("Player is not online or doesn't exist.");
            return true;
        }
    }
	
    static bool HandleMagicSetLevelCommand(ChatHandler* handler, Optional<Trinity::ChatCommands::PlayerIdentifier> player, uint16 level)
    {
        if (!player)
            player = Trinity::ChatCommands::PlayerIdentifier::FromTargetOrSelf(handler);
        if (!player)
            return true;
	
        if (level > MAX_ARTIFACT_LEVEL)
            level = MAX_ARTIFACT_LEVEL;
        if (level == 0)
            level = 1;
	
        if (Player* target = player->GetConnectedPlayer())
        {
            AccountInfoItem& Info = *sPlayerInfo.GetAccountInfo(target->GetSession()->GetAccountId());
            Info.ArtifactLevel = level;
            Info.ArtifactExperience = 0;
            Info.Updated = true;
	
            if (handler->IsConsole())
                ChatHandler(target->GetSession()).PSendSysMessage("|CFFFF0000[Console]|r set your Artifact level to %i", level);
            else
                if (target != handler->GetSession()->GetPlayer())
                    ChatHandler(target->GetSession()).PSendSysMessage("%s set your Artifact level to %i", handler->playerLink(handler->GetPlayer()->GetName()), level);
        }
        else
        {
            handler->PSendSysMessage("Player is not online or doesn't exist.");
            return true;
        }
	
        handler->PSendSysMessage("You set %s Artifact level to %i.", handler->playerLink(*player).c_str(), level);
        return true;
    }
	
    // Reload Commands

    static bool HandleReloadSpellModifierCommand(ChatHandler* handler, char const* /*args*/)
    {
        sSpellModifier.Load();
        handler->SendGlobalGMSysMessage("Spell Modifier data reloaded.");
        return true;
    }
	
    static bool HandleReloadTeleportSystemCommand(ChatHandler* handler, const char* /**/)
    {
        TC_LOG_INFO("misc", "Reloading teleport_locations tables...");
        sTeleSystem.Load();
        handler->SendGlobalGMSysMessage("Teleport System reloaded.");
        return true;
    }

    static bool HandleReloadUpgradeSystemCommand(ChatHandler* handler, const char* /**/)
    {
        TC_LOG_INFO("misc", "Reloading item_upgrades tables...");
        sUpgradeSystem.Load();
        handler->SendGlobalGMSysMessage("Upgrade System reloaded.");
        return true;
    }

    static bool HandleReloadWeaponDisplaysCommand(ChatHandler* handler, const char* /**/)
    {
        TC_LOG_INFO("misc", "Reloading weapon_display_ids tables...");
        sPlayerInfo.LoadWeaponDisplayIDs();
        handler->SendGlobalGMSysMessage("Weapon Display Ids reloaded.");
        return true;
    }

private:
    static constexpr std::array<ArtifactBuffs, 7> m_Auras = { ArtifactBuffs {48162, 100}, {48074, 200}, {48170, 300}, {43223, 400}, {36880, 500}, {467, 600}, {48479, 700} };
    static constexpr std::array<uint32, 8> m_AccBuffs = { 23736, 23767, 23768, 23766, 23769, 23738, 23737, 23735 };
};

void AddSC_CustomEventScripts()
{
    new CustomPlayerScripts();
    new CustomAccountScripts();
    new CustomWorldScripts();
    new CustomCommandScripts();
}
