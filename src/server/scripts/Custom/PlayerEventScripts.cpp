#include <Bag.h>
#include <Chat.h>
#include <DatabaseEnv.h>
#include <Group.h>
#include <Item.h>
#include <Log.h>
#include <ObjectMgr.h>
#include <WorldSession.h>
#include <ScriptedGossip.h>

#include "SpellRegulator.h"
#include "PlayerInfo/PlayerInfo.h"
#include "TeleportSystem/TeleportSystem.h"
#include "UpgradeSystem/UpgradeSystem.h"
#include "EventSystem/EventSystem.h"
#include "MallArena/MallArena.h"

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
        float RNG = frand(0.0f, 1.0f);

        Group* group = killed->GetLootRecipient()->GetGroup();
        if (group)
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* member = itr->GetSource();
	
                if (!member || !member->GetSession())
                    continue;
	
                if (member->IsAtGroupRewardDistance(killed))
                {
                    if (ArtifactExp > 0)
                        sPlayerInfo.AddArtifactExperience(member, killed->GetCreatureTemplate()->ArtifactExp);
                    if (NeckExp > 0)
                        sPlayerInfo.AddNeckExperience(member, killed->GetCreatureTemplate()->NeckExp);

                    sPlayerInfo.AddCreatureLoot(member, killed->GetCreatureTemplate()->Entry, RNG);
                }
            }
            return;
        }

        if (ArtifactExp > 0)
            sPlayerInfo.AddArtifactExperience(player, killed->GetCreatureTemplate()->ArtifactExp);
        if (NeckExp > 0)
            sPlayerInfo.AddNeckExperience(player, killed->GetCreatureTemplate()->NeckExp);
        sPlayerInfo.AddCreatureLoot(player, killed->GetCreatureTemplate()->Entry, RNG);
    }

    void OnCreate(Player* player) override
    {
        sPlayerInfo.CreateCharacterInfo(player);
    }

    void OnDelete(ObjectGuid guid, uint32 accountId)
    {
        sPlayerInfo.DeleteCharacterInfo(guid, accountId);
    }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        sPlayerInfo.LoadCharacterInfo(player->GetGUID());
    }

    void OnLogout(Player* player) override
    {
        sPlayerInfo.RemoveCharacterInfo(player->GetGUID());
        sPlayerInfo.SaveAccountInfo(player->GetSession()->GetAccountId());
        sEventSystem.KickPlayer(player, false);
        sMAS.HandleDisconnect(player);
    }

    void OnGossipSelect(Player* p, uint32 menu, uint32 sender, uint32 /*action*/)
    {
        if (menu != ArenaChallengeMenu)
            return;

        ClearGossipMenuFor(p);
        switch (sender)
        {
        case 3:
            sMAS.AcceptChallenge();
            break;
        case 4:
            sMAS.DeclineChallenge();
            break;
        }
        CloseGossipMenuFor(p);
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

        // Loads Artifact Experience
        sPlayerInfo.LoadAllOnStart();

        // Loads GameEvent Informations
        sEventSystem.Load();
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
            { "respawn",    HandleGameEventRespawnCommand,      rbac::RBAC_ROLE_PLAYER,     Trinity::ChatCommands::Console::No },
            { "teleport",   HandleGameEventTeleportCommand,     rbac::RBAC_ROLE_PLAYER,     Trinity::ChatCommands::Console::No },
        };
        static Trinity::ChatCommands::ChatCommandTable ListCommandTable =
        {
            { "inventory",          HandleListInventoryCommand,         rbac::RBAC_ROLE_GAMEMASTER,         Trinity::ChatCommands::Console::Yes },
        };
        static Trinity::ChatCommands::ChatCommandTable ArtifactCommandTable =
        {
            { "addexp",             HandleArtifactAddExpCommand,           rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "info",               HandleArtifactInfoCommand,             rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "setlevel",           HandleArtifactSetLevelCommand,         rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
        };
        static Trinity::ChatCommands::ChatCommandTable NpcSetCommandTable =
        {
            { "scale",              HandleNpcSetScaleCommand,           rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::No },
            { "name",               HandleNpcSetNameCommand,            rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::No },
            { "emote",              HandleNpcSetEmoteCommand,           rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::No },
        };
        static Trinity::ChatCommands::ChatCommandTable NpcCommandTable =
        {
            { "set",                NpcSetCommandTable },
        };
        static Trinity::ChatCommands::ChatCommandTable RandomizeCommandTable =
        {
            { "morph",                RandmozieMorphCommand,            rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::No },
            { "spell",                RandmozieSpellCommand,            rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::No },
        };
        static Trinity::ChatCommands::ChatCommandTable ReloadCommandTable =
        {
            { "spell_modifier",     HandleReloadSpellModifierCommand,   rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "teleport_system",    HandleReloadTeleportSystemCommand,  rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "upgrade_system",     HandleReloadUpgradeSystemCommand,   rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "weapon_display_ids", HandleReloadWeaponDisplaysCommand,  rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
            { "gameevent_system",   HandleReloadGameEventCommand,       rbac::RBAC_ROLE_ADMINISTRATOR,      Trinity::ChatCommands::Console::Yes },
        };
        static Trinity::ChatCommands::ChatCommandTable WorldCommandTable =
        {
            { "disable",     HandleDisableWorldChatCommand,          rbac::RBAC_ROLE_PLAYER,             Trinity::ChatCommands::Console::No },
            { "enable",      HandleEnableWorldChatCommand,         rbac::RBAC_ROLE_PLAYER,             Trinity::ChatCommands::Console::No },
        };
        static std::vector<ChatCommand> CustomCommandTable =
        {
            { "buff",               HandleBuffCommand,                  rbac::RBAC_ROLE_PLAYER,             Trinity::ChatCommands::Console::No },
            { "UpdateTele",         HandleUpdateTeleCommand,            rbac::RBAC_ROLE_GAMEMASTER,         Trinity::ChatCommands::Console::No },
            { "display",            HandleDisplayCommand,               rbac::RBAC_ROLE_GAMEMASTER,         Trinity::ChatCommands::Console::No },
            { "gameevent",          GameEventCommandTable },
            { "list",               ListCommandTable },
            { "artifact",           ArtifactCommandTable },
            { "npc",                NpcCommandTable },
            { "randomize",          RandomizeCommandTable },
            { "reload",             ReloadCommandTable },
            { "world",              WorldCommandTable },
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

    // Randomize Commands

    static bool RandmozieMorphCommand(ChatHandler* handler, const char* /**/)
    {
        Player* p = handler->GetSession()->GetPlayer();
        p->RandomizeMorph = !p->RandomizeMorph;

        return true;
    }

    static bool RandmozieSpellCommand(ChatHandler* handler, const char* /**/)
    {
        Player* p = handler->GetSession()->GetPlayer();
        p->RandomizeSpell = !p->RandomizeSpell;

        return true;
    }

    static bool HandleUpdateTeleCommand(ChatHandler* handler, Optional<uint16> voteID)
    {
        if (handler && handler->GetPlayer())
        {
            std::string QUERY = "UPDATE custom.`teleport_locations` SET `Map_ID` = " + std::to_string(handler->GetPlayer()->GetMapId());
            QUERY += ", `Pos_X` = " + std::to_string(handler->GetPlayer()->GetPositionX());
            QUERY += ", `Pos_Y` = " + std::to_string(handler->GetPlayer()->GetPositionY());
            QUERY += ", `Pos_Z` = " + std::to_string(handler->GetPlayer()->GetPositionZ());
            QUERY += ", `Pos_O` = " + std::to_string(handler->GetPlayer()->GetOrientation());
            if (voteID)
            {
                QUERY += " WHERE `ID` = " + std::to_string(voteID.value());
                WorldDatabase.PQuery("{}", QUERY);                
            }
            else
                handler->SendSysMessage("Incorrect ID.");
        }

        return true;
    }

    // DisplayID Command

    static bool HandleDisplayCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 itemId = 0;

        if (args[0] == '[')                                        // [name] manual form
        {
            char const* itemNameStr = strtok((char*)args, "]");

            if (itemNameStr && itemNameStr[0])
            {
                std::string itemName = itemNameStr + 1;
                WorldDatabase.EscapeString(itemName);

                WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_ITEM_TEMPLATE_BY_NAME);
                stmt->setString(0, itemName);
                PreparedQueryResult result = WorldDatabase.Query(stmt);

                if (!result)
                {
                    handler->PSendSysMessage(LANG_COMMAND_COULDNOTFIND, itemNameStr + 1);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                itemId = result->Fetch()->GetUInt32();
            }
            else
                return false;
        }
        else                                                    // item_id or [name] Shift-click form |color|Hitem:item_id:0:0:0|h[name]|h|r
        {
            char const* id = handler->extractKeyFromLink((char*)args, "Hitem");
            if (!id)
                return false;

            itemId = Trinity::StringTo<uint32>(id).value_or(0);
        }

        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(itemId);
        if (!itemTemplate)
        {
            handler->PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, itemId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage("[%s]: DisplayID = %u", itemTemplate->Name1, itemTemplate->DisplayInfoID);
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
        if (sEventSystem.IsActive())
        {
            sEventSystem.EndEvent();
            sEventSystem.GlobalAnnounce("Event has forcefully been terminated.", MEMBER_ANNOUNCE);
            handler->SendSysMessage("Event has been terminated.");
        }
        else
            handler->SendSysMessage("There isn't an active event.");

        return true;
    }

    static bool HandleGameEventRespawnCommand(ChatHandler* handler)
    {
        if (handler && handler->GetPlayer())
            sEventSystem.HandleEventRespawn(handler->GetPlayer());
        return true;
    }

    static bool HandleGameEventTeleportCommand(ChatHandler* handler)
    {
        if (handler && handler->GetPlayer())
            sEventSystem.HandleEventTeleport(handler->GetPlayer());
        return true;
    }

    // List Commands

    static bool HandleListInventoryCommand(ChatHandler* handler, Optional<Trinity::ChatCommands::PlayerIdentifier> targetIdentifier)
    {
        // Checks for target, or <name> 
        if (!targetIdentifier)
            targetIdentifier = Trinity::ChatCommands::PlayerIdentifier::FromTarget(handler);
        if (!targetIdentifier)
            return false;

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
                std::string item_enchants = fields[4].GetString();
                int16 RandomPropertyID = fields[5].GetInt16();
                uint16 CharLevel = fields[6].GetUInt16();
                uint32 acc_guid = fields[7].GetUInt32();
                uint32 char_guid = fields[8].GetUInt32();

                uint16 ctr = 0;
                std::ostringstream ssEnchants;
                std::string temp;
                std::string RandomSuffixFactor;

                for (auto& s : item_enchants)
                {
                    if (s != ' ')
                        temp += s;
                    else
                    {
                        switch (ctr)
                        {
                        case 0:
                        case 6:
                        case 9:
                        case 12:
                        case 15:
                            ssEnchants << ":" << temp;
                            break;
                        case 21:
                            RandomSuffixFactor = temp;
                            break;
                        }

                        temp.clear();
                        ctr++;
                    }
                }

                ssEnchants << ":" << RandomPropertyID << ":" << RandomSuffixFactor << ":" << CharLevel;

                std::ostringstream ItemLink;
                if (!handler->IsConsole())
                    ItemLink << "|c" << std::hex << ItemQualityColors[sObjectMgr->GetItemTemplate(item_entry)->Quality] << std::dec << "|Hitem:" << item_entry << ssEnchants.str() << "|h[" << item_name << "]|h|r";
                else
                    ItemLink << "[" << item_entry << "] [" << item_name << "]";

                handler->PSendSysMessage("(Account: %u Guid: %u): %s has %sx%u", acc_guid, char_guid, char_name, ItemLink.str(), item_count);
            } while (queryResult->NextRow());
        }
        else
            handler->PSendSysMessage("Player %s not found, or doesn't have any items.", targetIdentifier->GetName());

        return true;
    }
	
    // Artifact Commands
	
    static bool HandleArtifactAddExpCommand(ChatHandler* handler, Optional<Trinity::ChatCommands::PlayerIdentifier> player, uint32 amt)
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
	
        handler->PSendSysMessage("You gave %s %u Artifact Experience.", handler->playerLink(*player).c_str(), amt);
        return true;
    }
	
    static bool HandleArtifactInfoCommand(ChatHandler* handler, Optional<Trinity::ChatCommands::PlayerIdentifier> player)
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
	
    static bool HandleArtifactSetLevelCommand(ChatHandler* handler, Optional<Trinity::ChatCommands::PlayerIdentifier> player, uint16 level)
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

            target->UpdateStats(STAT_STAMINA);
            sPlayerInfo.UpdateArtifactAuras(target, &Info);
        }
        else
        {
            handler->PSendSysMessage("Player is not online or doesn't exist.");
            return true;
        }
	
        handler->PSendSysMessage("You set %s Artifact level to %i.", handler->playerLink(*player).c_str(), level);
        return true;
    }

    // Npc Set Commands

    static bool HandleNpcSetScaleCommand(ChatHandler* handler, float scale)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->PSendSysMessage("You must select a target.");
            return true;
        }

        if (!target->IsCreature())
        {
            handler->PSendSysMessage("You must select a creature.");
            return true;
        }

        std::string query = "UPDATE Creature_Template Set scale = " + std::to_string(scale) + " Where entry = " + std::to_string(target->GetEntry());

        WorldDatabase.PQuery("{}", query);
        target->SetObjectScale(scale);

        WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_CREATURE_TEMPLATE);
        stmt->setUInt32(0, target->GetEntry());
        PreparedQueryResult result = WorldDatabase.Query(stmt);

        if (!result)
            return true;

        CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(target->GetEntry());
        if (!cInfo)
            return true;

        Field* fields = result->Fetch();
        sObjectMgr->LoadCreatureTemplate(fields);
        sObjectMgr->CheckCreatureTemplate(cInfo);
        sObjectMgr->InitializeQueriesData(QUERY_DATA_CREATURES);

        return true;
    }

    static bool HandleNpcSetNameCommand(ChatHandler* handler, const char* name)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->PSendSysMessage("You must select a target.");
            return true;
        }

        if (!target->IsCreature())
        {
            handler->PSendSysMessage("You must select a creature.");
            return true;
        }

        std::string sName = name;
        std::string newName;

        for (auto& i : sName)
        {
            if (i == '\'')
                newName += '\\';

            newName += i;
        }

        std::string query = "UPDATE Creature_template set Name = '" + newName + "' WHERE entry = " + std::to_string(target->GetEntry());
        WorldDatabase.PQuery("{}", query);

        return true;
    }

    static bool HandleNpcSetEmoteCommand(ChatHandler* handler, uint16 emote)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->PSendSysMessage("You must select a target.");
            return true;
        }

        if (!target->IsCreature())
        {
            handler->PSendSysMessage("You must select a creature.");
            return true;
        }

        std::string query;
        if (target->ToCreature()->GetCreatureAddon())
            query = "UPDATE creature_addon SET emote = " + std::to_string(emote) + " WHERE guid = " + std::to_string(target->ToCreature()->GetSpawnId());
        else
            query = "INSERT INTO creature_addon (`guid`, `emote`) VALUES (" + std::to_string(target->ToCreature()->GetSpawnId()) + ", " + std::to_string(emote) + ");";

        std::cout << query << std::endl;

        WorldDatabase.PQuery("{}", query);
        target->SetEmoteState(static_cast<Emote>(emote));

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

    static bool HandleReloadGameEventCommand(ChatHandler* handler)
    {
        if (sEventSystem.IsActive())
            handler->SendSysMessage("Unable to reload while event is active.");
        else
        {
            TC_LOG_INFO("misc", "Reloading gameevent_information tables...");
            sEventSystem.Load();
            handler->SendGlobalGMSysMessage("GameEvent Information reloaded.");
        }
        return true;
    }

    static bool HandleDisableWorldChatCommand(ChatHandler* handler)
    {
        sPlayerInfo.UpdateWorldChat(handler->GetPlayer()->GetGUID(), false);
        handler->PSendSysMessage("You have disabled world chat.");
        return true;
    }

    static bool HandleEnableWorldChatCommand(ChatHandler* handler)
    {
        sPlayerInfo.UpdateWorldChat(handler->GetPlayer()->GetGUID(), true);
        handler->PSendSysMessage("You have enabled world chat.");
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
