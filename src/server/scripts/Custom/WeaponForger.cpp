#include <ScriptedGossip.h>
#include <Player.h>
#include <WorldSession.h>
#include <ItemTemplate.h>
#include <iostream>
#include <Item.h>
#include <Chat.h>
#include <ObjectMgr.h>
#include <DatabaseEnv.h>
#include <Log.h>
#include <SpellMgr.h>

enum WEAPON_FORGER_MENU
{
    WEAPON_FORGER_GOSSIP_CREATE = 0,
    WEAPON_FORGER_GOSSIP_SAVE,
    WEAPON_FORGER_GOSSIP_RESET,
    WEAPON_FORGER_GOSSIP_BACK,
    WEAPON_FORGER_GOSSIP_EXIT,

    WEAPON_FORGER_GOSSIP_DETAILS,
    WEAPON_FORGER_GOSSIP_DETAILS_NAME,
    WEAPON_FORGER_GOSSIP_DETAILS_DISPLAY,

    WEAPON_FORGER_GOSSIP_DETAILS_QUALITY,
    WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_POOR,
    WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_COMMON,
    WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_UNCOMMON,
    WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_RARE,
    WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_EPIC,
    WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_LEGENDARY,

    WEAPON_FORGER_GOSSIP_DETAILS_TYPE,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_AXE,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_MACE,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_SWORD,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_DAGGER,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_FIST,

    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_AXE,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_MACE,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_SWORD,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_STAFF,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_POLEARM,

    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_BOW,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_CROSSBOW,
    WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_GUN,

    WEAPON_FORGER_GOSSIP_DETAILS_SHEATH,
    WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_2H,
    WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_STAFF,
    WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_1H,


    WEAPON_FORGER_GOSSIP_DETAILS_DESCRIPTION,


    WEAPON_FORGER_GOSSIP_STATS,
    WEAPON_FORGER_GOSSIP_STATS_1,
    WEAPON_FORGER_GOSSIP_STATS_2,
    WEAPON_FORGER_GOSSIP_STATS_3,
    WEAPON_FORGER_GOSSIP_STATS_4,
    WEAPON_FORGER_GOSSIP_STATS_5,
    WEAPON_FORGER_GOSSIP_STATS_DELAY,
    WEAPON_FORGER_GOSSIP_STATS_DELAY_1,
    WEAPON_FORGER_GOSSIP_STATS_DELAY_2,
    WEAPON_FORGER_GOSSIP_STATS_DELAY_3,
    WEAPON_FORGER_GOSSIP_STATS_DAMAGE,

    WEAPON_FORGER_GOSSIP_STATS_TYPE,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_NONE,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_AGILITY,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_STRENGTH,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_INTELLECT,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_SPIRIT,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_STAMINA,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_DEFENSE,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_DODGE,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_PARRY,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_BLOCK,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_HIT,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_CRIT,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_HASTE,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_ATTACKPOWER,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_SPELLPOWER,

    WEAPON_FORGER_GOSSIP_STATS_VALUE,


    WEAPON_FORGER_GOSSIP_MISC,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_1,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_2,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_3,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_NONE,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_RED,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_YELLOW,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_BLUE,

    WEAPON_FORGER_GOSSIP_MISC_SPELL,
    WEAPON_FORGER_GOSSIP_MISC_SPELL_1,
    WEAPON_FORGER_GOSSIP_MISC_SPELL_2,
    WEAPON_FORGER_GOSSIP_MISC_SPELL_3,
    WEAPON_FORGER_GOSSIP_MISC_SPELL_4,

};

class WeaponForgerCreature : public CreatureScript
{
public:
    WeaponForgerCreature() : CreatureScript("WeaponForgerCreature") {}

    struct WeaponForgerCreature_AI : public ScriptedAI
    {
        WeaponForgerCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        void SendUpdatePacket(Player* p, uint32 entry)
        {
            ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(entry);

            if (!pProto)
                return;

            std::string Name = pProto->Name1;
            std::string Description = pProto->Description;

            LocaleConstant loc_idx = p->GetSession()->GetSessionDbLocaleIndex();
            if (ItemLocale const* il = sObjectMgr->GetItemLocale(pProto->ItemId))
            {
                ObjectMgr::GetLocaleString(il->Name, loc_idx, Name);
                ObjectMgr::GetLocaleString(il->Description, loc_idx, Description);
            }

            WorldPacket data(SMSG_ITEM_QUERY_SINGLE_RESPONSE, 600);
            data << pProto->ItemId;
            data << pProto->Class;
            data << pProto->SubClass;
            data << pProto->SoundOverrideSubclass;
            data << Name;
            data << uint8(0x00);                                //pProto->Name2; // blizz not send name there, just uint8(0x00); <-- \0 = empty string = empty name...
            data << uint8(0x00);                                //pProto->Name3; // blizz not send name there, just uint8(0x00);
            data << uint8(0x00);                                //pProto->Name4; // blizz not send name there, just uint8(0x00);
            data << pProto->DisplayInfoID;
            data << pProto->Quality;
            data << pProto->Flags;
            data << pProto->Flags2;
            data << pProto->BuyPrice;
            data << pProto->SellPrice;
            data << pProto->InventoryType;
            data << pProto->AllowableClass;
            data << pProto->AllowableRace;
            data << pProto->ItemLevel;
            data << pProto->RequiredLevel;
            data << pProto->RequiredSkill;
            data << pProto->RequiredSkillRank;
            data << pProto->RequiredSpell;
            data << pProto->RequiredHonorRank;
            data << pProto->RequiredCityRank;
            data << pProto->RequiredReputationFaction;
            data << pProto->RequiredReputationRank;
            data << int32(pProto->MaxCount);
            data << int32(pProto->Stackable);
            data << pProto->ContainerSlots;
            data << pProto->StatsCount;
            for (uint32 i = 0; i < pProto->StatsCount; ++i)
            {
                data << pProto->ItemStat[i].ItemStatType;
                data << pProto->ItemStat[i].ItemStatValue;
            }
            data << pProto->ScalingStatDistribution;            // scaling stats distribution
            data << pProto->ScalingStatValue;                   // some kind of flags used to determine stat values column
            for (int i = 0; i < MAX_ITEM_PROTO_DAMAGES; ++i)
            {
                data << pProto->Damage[i].DamageMin;
                data << pProto->Damage[i].DamageMax;
                data << pProto->Damage[i].DamageType;
            }

            // resistances (7)
            data << pProto->Armor;
            data << pProto->HolyRes;
            data << pProto->FireRes;
            data << pProto->NatureRes;
            data << pProto->FrostRes;
            data << pProto->ShadowRes;
            data << pProto->ArcaneRes;

            data << pProto->Delay;
            data << pProto->AmmoType;
            data << pProto->RangedModRange;

            for (int s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
            {
                // send DBC data for cooldowns in same way as it used in Spell::SendSpellCooldown
                    // use `item_template` or if not set then only use spell cooldowns
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(pProto->Spells[s].SpellId);
                if (spell)
                {
                    bool db_data = pProto->Spells[s].SpellCooldown >= 0 || pProto->Spells[s].SpellCategoryCooldown >= 0;

                    data << pProto->Spells[s].SpellId;
                    data << pProto->Spells[s].SpellTrigger;
                    data << uint32(-abs(pProto->Spells[s].SpellCharges));

                    if (db_data)
                    {
                        data << uint32(pProto->Spells[s].SpellCooldown);
                        data << uint32(pProto->Spells[s].SpellCategory);
                        data << uint32(pProto->Spells[s].SpellCategoryCooldown);
                    }
                    else
                    {
                        data << uint32(spell->RecoveryTime);
                        data << uint32(spell->GetCategory());
                        data << uint32(spell->CategoryRecoveryTime);
                    }
                }
                else
                {
                    data << uint32(0);
                    data << uint32(0);
                    data << uint32(0);
                    data << uint32(-1);
                    data << uint32(0);
                    data << uint32(-1);
                }
            }
            data << pProto->Bonding;
            data << Description;
            data << pProto->PageText;
            data << pProto->LanguageID;
            data << pProto->PageMaterial;
            data << pProto->StartQuest;
            data << pProto->LockID;
            data << int32(pProto->Material);
            data << pProto->Sheath;
            data << pProto->RandomProperty;
            data << pProto->RandomSuffix;
            data << pProto->Block;
            data << pProto->ItemSet;
            data << pProto->MaxDurability;
            data << pProto->Area;
            data << pProto->Map;                                // Added in 1.12.x & 2.0.1 client branch
            data << pProto->BagFamily;
            data << pProto->TotemCategory;
            for (int s = 0; s < MAX_ITEM_PROTO_SOCKETS; ++s)
            {
                data << pProto->Socket[s].Color;
                data << pProto->Socket[s].Content;
            }
            data << pProto->socketBonus;
            data << pProto->GemProperties;
            data << pProto->RequiredDisenchantSkill;
            data << pProto->ArmorDamageModifier;
            data << pProto->Duration;                           // added in 2.4.2.8209, duration (seconds)
            data << pProto->ItemLimitCategory;                  // WotLK, ItemLimitCategory
            data << pProto->HolidayId;                          // Holiday.dbc?
            p->GetSession()->SendPacket(&data);
        }

        void SaveCustomWeapon(Player* p)
        {
            auto& item = p->CustomWeapon;
            bool bCreateNewWeapon = false;
            if (item->ItemId == 0)
                bCreateNewWeapon = true;

            if (bCreateNewWeapon)
            {
                // Check for Required Forge Item
                //if (!p->HasItemCount(0, 1))
                //    return ChatHandler(p->GetSession()).PSendSysMessage("You need [%s]x1 to Forge this weapon.", sObjectMgr->GetItemTemplate(0)->Name1);
                uint32 GoldCost = CalculateWeaponCost(p, bCreateNewWeapon) * 10000;
                if (!p->HasEnoughMoney(GoldCost))
                    return ChatHandler(p->GetSession()).PSendSysMessage("You need %u Gold to Forge this weapon.", GoldCost);

                // Check make sure all stats are correct

                //p->ModifyMoney(GoldCost * -1);

                std::stringstream InsertString;
                InsertString << "INSERT INTO `item_template_custom` (`subclass`, `name`, `displayid`, `Quality`, `InventoryType`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `dmg_min1`, `dmg_max1`, `delay`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `description`, `sheath`, `socketColor_1`, `socketColor_2`, `socketColor_3`, `AccountID`, `CharacterID`) VALUES (";
                InsertString << item->SubClass << ",'" << item->Name1 << "'," << item->DisplayInfoID << "," << item->Quality << "," << item->InventoryType << "," << item->ItemStat[0].ItemStatType << "," << item->ItemStat[0].ItemStatValue << "," << item->ItemStat[1].ItemStatType << "," << item->ItemStat[1].ItemStatValue << "," << item->ItemStat[2].ItemStatType << "," << item->ItemStat[2].ItemStatValue << "," << item->ItemStat[3].ItemStatType << "," << item->ItemStat[3].ItemStatValue << "," << item->ItemStat[5].ItemStatType << "," << item->ItemStat[5].ItemStatValue << "," << item->Damage[0].DamageMin << "," << item->Damage[0].DamageMax << "," << item->Delay << "," << item->RangedModRange << "," << item->Spells[0].SpellId << "," << item->Spells[0].SpellTrigger << "," << item->Spells[0].SpellCharges << "," << item->Spells[0].SpellPPMRate << "," << item->Spells[0].SpellCooldown << "," << item->Spells[0].SpellCategory << "," << item->Spells[0].SpellCategoryCooldown << ",'" << item->Description << "'," << item->Sheath << "," << item->Socket[0].Color << "," << item->Socket[1].Color << "," << item->Socket[2].Color << "," << p->GetSession()->GetAccountId() << "," << p->GetGUID() << "); ";
                QueryResult InsertQuery = WorldDatabase.PQuery("%s", InsertString.str());
                QueryResult result = WorldDatabase.PQuery("SELECT `entry` FROM `item_template_custom` WHERE CharacterID = %u;", p->GetGUID());

                if (!result)
                {
                    p->CustomWeapon->ItemId = 1;
                    TC_LOG_ERROR("custom.insert", ">> Error creating custom weapon for (Account: %u, Character: %s).", p->GetSession()->GetAccountId(), p->GetName());
                    ChatHandler(p->GetSession()).SendSysMessage("There was an issue with creating your custom weapon, please contact an administrator.");
                    return;
                }
                Field* fields = result->Fetch();
                uint32 entry = fields[0].GetUInt32();
                item->ItemId = entry;

                sObjectMgr->UpdateCustomItemTemplate(*item, entry);
                sObjectMgr->InitializeQueriesData(QUERY_DATA_ITEMS);

                SendUpdatePacket(p, entry);
                p->AddItem(entry, 1);
            }
            else
            {
                Item* itemInfo = p->GetItemByEntry(item->ItemId);

                if (itemInfo)
                    if (itemInfo->IsEquipped())
                        p->_ApplyItemMods(itemInfo, itemInfo->GetSlot(), false);

                sObjectMgr->UpdateCustomItemTemplate(*item, item->ItemId);
                sObjectMgr->InitializeQueriesData(QUERY_DATA_ITEMS);

                std::stringstream InsertString;
                InsertString << "UPDATE `item_template_custom` SET `subclass` = " << item->SubClass << ", `name` = '" << item->Name1 << "', `displayid` = " << item->DisplayInfoID << ", `Quality` = " << item->Quality << ", `InventoryType` = " << item->InventoryType << ", `stat_type1` = " << item->ItemStat[0].ItemStatType << ", `stat_value1` = " << item->ItemStat[0].ItemStatValue << ", `stat_type2` = " << item->ItemStat[1].ItemStatType << ", `stat_value2` = " << item->ItemStat[1].ItemStatValue << ", `stat_type3` = " << item->ItemStat[2].ItemStatType << ", `stat_value3` = " << item->ItemStat[2].ItemStatValue << ", `stat_type4` = " << item->ItemStat[3].ItemStatType << ", `stat_value4` = " << item->ItemStat[3].ItemStatValue << ", `stat_type5` = " << item->ItemStat[4].ItemStatType << ", `stat_value5` = " << item->ItemStat[4].ItemStatValue << ", `dmg_min1` = " << uint32(item->Damage[0].DamageMin) << ", `dmg_max1` = " << uint32(item->Damage[0].DamageMax) << ", `delay` = " << item->Delay << ", `RangedModRange` = " << item->RangedModRange << ", `spellid_1` = " << item->Spells[0].SpellId << ", `spelltrigger_1` = " << item->Spells[0].SpellTrigger << ", `spellcharges_1` = " << item->Spells[0].SpellCharges << ", `spellppmRate_1` = " << item->Spells[0].SpellPPMRate << ", `spellcooldown_1` = " << item->Spells[0].SpellCooldown << ", `spellcategory_1` = " << item->Spells[0].SpellCategory << ", `spellcategorycooldown_1` = " << item->Spells[0].SpellCategoryCooldown << ", `description` = '" << item->Description << "', `sheath` = " << item->Sheath << ", `socketColor_1` = " << item->Socket[0].Color << ", `socketColor_2` = " << item->Socket[1].Color << ", `socketColor_3` = " << item->Socket[2].Color << " WHERE CharacterID = " << p->GetGUID() << ";";
                QueryResult InsertQuery = WorldDatabase.PQuery("%s", InsertString.str());

                SendUpdatePacket(p, item->ItemId);

                if (itemInfo)
                    if (itemInfo->IsEquipped())
                        p->_ApplyItemMods(itemInfo, itemInfo->GetSlot(), true);
            }
                        
            return;
        }

        std::string GetStatName(uint16 value)
        {
            switch (value)
            {
            case ITEM_MOD_AGILITY: return "Agility"; break;
            case ITEM_MOD_STRENGTH: return "Strength"; break;
            case ITEM_MOD_INTELLECT: return "Intellect"; break;
            case ITEM_MOD_SPIRIT: return "Spirit"; break;
            case ITEM_MOD_STAMINA: return "Stamina"; break;
            case ITEM_MOD_DEFENSE_SKILL_RATING: return "Defense Rating"; break;
            case ITEM_MOD_DODGE_RATING: return "Dodge Rating"; break;
            case ITEM_MOD_PARRY_RATING: return "Parry Rating"; break;
            case ITEM_MOD_BLOCK_RATING: return "Block Rating"; break;
            case ITEM_MOD_HIT_RATING: return "Hit Rating"; break;
            case ITEM_MOD_CRIT_RATING: return "Crit Rating"; break;
            case ITEM_MOD_HASTE_RATING: return "Haste Rating"; break;
            case ITEM_MOD_ATTACK_POWER: return "Attack Power"; break;
            case ITEM_MOD_SPELL_POWER: return "Spell Power"; break;
            default: return "<No Stat>"; break;
            }
        }

        std::string GetSocketColor(uint32 value)
        {
            switch (value)
            {
            case SocketColor::SOCKET_COLOR_RED: return "Red"; break;
            case SocketColor::SOCKET_COLOR_YELLOW: return "Yellow"; break;
            case SocketColor::SOCKET_COLOR_BLUE: return "Blue"; break;
            default: return "<No Socket>"; break;
            }
        }

        uint32 GetAttackSpeed(uint16 inventoryType, uint16 optionID)
        {
            switch (inventoryType)
            {
            case InventoryType::INVTYPE_2HWEAPON:
            {

                switch (optionID)
                {
                case 0: return 3200; break;
                case 1: return 3700; break;
                case 2: return 4000; break;
                }
            } break;
            case InventoryType::INVTYPE_WEAPON:
            {
                switch (optionID)
                {
                case 0: return 1600; break;
                case 1: return 1800; break;
                case 2: return 2600; break;
                }
            } break;
            case InventoryType::INVTYPE_RANGED:
            case InventoryType::INVTYPE_RANGEDRIGHT:
            {
                switch (optionID)
                {
                case 0: return 2400; break;
                case 1: return 2800; break;
                case 2: return 3200; break;
                }
            } break;
            }

            return 0;
        }

        void SetStatValue(Player* p, uint32 value)
        {
            auto& StatInfo = p->CustomWeapon->ItemStat[nOptionNumber];
            bool b2Hander = (p->CustomWeapon->InventoryType == 15 || p->CustomWeapon->InventoryType == 17 || p->CustomWeapon->InventoryType == 26);

            if (StatInfo.ItemStatType == ITEM_MOD_MANA)
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to set Stat Value, when no Stat Type is selected.");
                return;
            }

            bool bFailFlags = false;
            std::string sFailString;

            switch (StatInfo.ItemStatType)
            {

            case ITEM_MOD_AGILITY:
            {
                if (value > uint32(1000 * uint32(1 + b2Hander)))
                {
                    bFailFlags = true;
                    sFailString = "Stat Value for Agility is to high. The highest possible value is " + std::to_string(1000 * (1 + b2Hander));
                }
            } break;

            case ITEM_MOD_STRENGTH:
            {
                if (value > (2000 * uint32(1 + b2Hander)))
                {
                    bFailFlags = true;
                    sFailString = "Stat Value for Strength is to high. The highest possible value is " + std::to_string(2000 * (1 + b2Hander));
                }
            } break;

            case ITEM_MOD_INTELLECT:
            {
                if (value > (2000 * uint32(1 + b2Hander)))
                {
                    bFailFlags = true;
                    sFailString = "Stat Value for Intellect is to high. The highest possible value is " + std::to_string(2000 * (1 + b2Hander));
                }
            } break;

            case ITEM_MOD_SPIRIT:
            {
                if (value > (2000 * uint32(1 + b2Hander)))
                {
                    bFailFlags = true;
                    sFailString = "Stat Value for Spirit is to high. The highest possible value is " + std::to_string(2000 * (1 + b2Hander));
                }
            } break;

            case ITEM_MOD_STAMINA:
            {
                if (value > (5000 * uint32(1 + b2Hander)))
                {
                    bFailFlags = true;
                    sFailString = "Stat Value for Agility is to high. The highest possible value is " + std::to_string(5000 * (1 + b2Hander));
                }
            } break;
            
                case ITEM_MOD_DEFENSE_SKILL_RATING:
                {
                    if (value > (200 * uint32(1 + b2Hander)))
                    {
                        bFailFlags = true;
                        sFailString = "Stat Value for Defense Rating is to high. The highest possible value is " + std::to_string(200 * (1 + b2Hander));
                    }
                } break;

                case ITEM_MOD_DODGE_RATING:
                {
                    if (value > (200 * uint32(1 + b2Hander)))
                    {
                        bFailFlags = true;
                        sFailString = "Stat Value for Dodge Rating is to high. The highest possible value is " + std::to_string(200 * (1 + b2Hander));
                    }
                } break;

                case ITEM_MOD_PARRY_RATING:
                {
                    if (value > (200 * uint32(1 + b2Hander)))
                    {
                        bFailFlags = true;
                        sFailString = "Stat Value for Parry Rating is to high. The highest possible value is " + std::to_string(200 * (1 + b2Hander));
                    }
                } break;

                case ITEM_MOD_BLOCK_RATING:
                {
                    if (value > (200 * uint32(1 + b2Hander)))
                    {
                        bFailFlags = true;
                        sFailString = "Stat Value for Block Rating is to high. The highest possible value is " + std::to_string(200 * (1 + b2Hander));
                    }
                } break;

                case ITEM_MOD_HIT_RATING:
                {
                    if (value > (200 * uint32(1 + b2Hander)))
                    {
                        bFailFlags = true;
                        sFailString = "Stat Value for Hit Rating is to high. The highest possible value is " + std::to_string(200 * (1 + b2Hander));
                    }
                } break;

                case ITEM_MOD_CRIT_RATING:
                {
                    if (value > (200 * uint32(1 + b2Hander)))
                    {
                        bFailFlags = true;
                        sFailString = "Stat Value for Crit Rating is to high. The highest possible value is " + std::to_string(200 * (1 + b2Hander));
                    }
                } break;

                case ITEM_MOD_HASTE_RATING:
                {
                    if (value > (600 * uint32(1 + b2Hander)))
                    {
                        bFailFlags = true;
                        sFailString = "Stat Value for Haste Rating is to high. The highest possible value is " + std::to_string(600 * (1 + b2Hander));
                    }
                } break;

                case ITEM_MOD_ATTACK_POWER:
                {
                    if (value > (4000 * uint32(1 + b2Hander)))
                    {
                        bFailFlags = true;
                        sFailString = "Stat Value for Attack Power is to high. The highest possible value is " + std::to_string(4000 * (1 + b2Hander));
                    }
                } break;

                case ITEM_MOD_SPELL_POWER:
                {
                    if (value > (3000 * uint32(1 + b2Hander)))
                    {
                        bFailFlags = true;
                        sFailString = "Stat Value for Spell Power is to high. The highest possible value is " + std::to_string(3000 * (1 + b2Hander));
                    }
                } break;

            }

            if (bFailFlags)
                return ChatHandler(p->GetSession()).SendSysMessage(sFailString);

            p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = value;
        }

        float GetStatCost(uint16 stat, uint32 value, bool b2hand)
        {
            float Cost = -1.0f;
            switch (stat)
            {
            case ITEM_MOD_AGILITY: Cost = 10.0f; break;
            case ITEM_MOD_STRENGTH: Cost = 5.0f; break;
            case ITEM_MOD_INTELLECT: Cost = 5.0f; break;
            case ITEM_MOD_SPIRIT: Cost = 5.0f; break;
            case ITEM_MOD_STAMINA: Cost = 2.0f; break;
            case ITEM_MOD_DEFENSE_SKILL_RATING: Cost = 10.0f; break;
            case ITEM_MOD_DODGE_RATING: Cost = 10.0f; break;
            case ITEM_MOD_PARRY_RATING: Cost = 10.0f; break;
            case ITEM_MOD_BLOCK_RATING: Cost = 10.0f; break;
            case ITEM_MOD_HIT_RATING: Cost = 10.0f; break;
            case ITEM_MOD_CRIT_RATING: Cost = 10.0f; break;
            case ITEM_MOD_HASTE_RATING: Cost = 3.33f; break;
            case ITEM_MOD_ATTACK_POWER: Cost = 2.5f; break;
            case ITEM_MOD_SPELL_POWER: Cost = 3.33f; break;
            }

            Cost *= value;

            if (b2hand && Cost > 0.0f)
                Cost *= 0.5f;

            return Cost;
        }

        uint32 ConvertStat(Player *p, uint16 oldStat, uint16 newStat, uint32 amt, uint16 newInvType = 0)
        {
            bool bNew2Hander = (newInvType == 15 || newInvType == 17 || newInvType == 26);
            bool b2Hander = (p->CustomWeapon->InventoryType == 15 || p->CustomWeapon->InventoryType == 17 || p->CustomWeapon->InventoryType == 26);
            return uint32(GetStatCost(oldStat, amt, b2Hander) / GetStatCost(newStat, 1, newInvType == 0 ? b2Hander : bNew2Hander));
        }

        uint32 ConvertDamage(Player* p, uint32 newSpeed, uint16 newInvType = 0)
        {
            bool bNew2hander = (newInvType == 15 || newInvType == 17 || newInvType == 26);
            bool b2Hander = (p->CustomWeapon->InventoryType == 15 || p->CustomWeapon->InventoryType == 17 || p->CustomWeapon->InventoryType == 26);
            float Dps = ((p->CustomWeapon->Damage[0].DamageMin + p->CustomWeapon->Damage[0].DamageMax) * 0.5f / (p->CustomWeapon->Delay * 0.001f));

            if (newInvType != 0)
                if (bNew2hander != b2Hander)
                    if (bNew2hander)
                        Dps *= (3500.0f / 2650.0f);
                    else
                        Dps *= (2650.0f / 3500.0f);

            return uint32(Dps * (newSpeed * 0.001f) * 2.0f / 1.9f);
        }

        uint32 CalculateWeaponCost(Player* p, bool bNew)
        {
            bool b2Hander = (p->CustomWeapon->InventoryType == 15 || p->CustomWeapon->InventoryType == 17 || p->CustomWeapon->InventoryType == 26);
            uint32 totalCost = 1000;

            if (bNew)
            {
                totalCost = 20000;

                for (int i = 0; i < 5; i++)
                {
                    float statCost = GetStatCost(p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, b2Hander);

                    if (statCost > 0.0f)
                        totalCost += statCost;
                }

                for (int i = 0; i < MAX_ITEM_PROTO_SOCKETS; i++)
                    if (p->CustomWeapon->Socket[i].Color > 0)
                        totalCost += 200;

                if (b2Hander)
                    totalCost += uint32(p->CustomWeapon->getDPS() * 5.71428571f); //3500
                else
                    totalCost += uint32(p->CustomWeapon->getDPS() * 7.54716981f); //2650

                return totalCost;
            }

            auto& iTemplate = sObjectMgr->GetItemTemplateStore().at(p->CustomWeapon->ItemId);

            for (int i = 0; i < 5; i++)
            {
                uint32 updatedValue = 0;
                if (p->CustomWeapon->ItemStat[i].ItemStatType != iTemplate.ItemStat[i].ItemStatType)
                    updatedValue = ConvertStat(p, iTemplate.ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, iTemplate.ItemStat[i].ItemStatValue);
                else
                    updatedValue = p->CustomWeapon->ItemStat[i].ItemStatValue - iTemplate.ItemStat[i].ItemStatValue;

                if (updatedValue == 0)
                    continue;

                float statCost = GetStatCost(p->CustomWeapon->ItemStat[i].ItemStatType, (p->CustomWeapon->ItemStat[i].ItemStatValue - updatedValue), b2Hander);

                if (statCost > 0.0f)
                    totalCost += statCost;
            }

            for (int i = 0; i < MAX_ITEM_PROTO_SOCKETS; i++)
                if (iTemplate.Socket[i].Color != p->CustomWeapon->Socket[i].Color)
                    if (iTemplate.Socket[i].Color == 0)
                        totalCost += 200;
                    else
                        totalCost += 20;

            float dpsDiff;

            if (iTemplate.Delay != p->CustomWeapon->Delay)
            {
                bool bNew2hander = (p->CustomWeapon->InventoryType == 15 || p->CustomWeapon->InventoryType == 17 || p->CustomWeapon->InventoryType == 26);
                bool b2Hander = (iTemplate.InventoryType == 15 || iTemplate.InventoryType == 17 || iTemplate.InventoryType == 26);
                float Dps = iTemplate.getDPS();

                if (bNew2hander != b2Hander)
                {
                    if (bNew2hander)
                        Dps *= (3500.0f / 2650.0f);
                    else
                        Dps *= (2650.0f / 3500.0f);

                    dpsDiff = Dps - p->CustomWeapon->getDPS();
                }
                else
                    dpsDiff = p->CustomWeapon->getDPS() - iTemplate.getDPS();
            }
            else
                dpsDiff = p->CustomWeapon->getDPS() - iTemplate.getDPS();

            if (dpsDiff > 0.0f)

                if (b2Hander)
                    totalCost += uint32(dpsDiff * 5.71428571f); //3500
                else
                    totalCost += uint32(dpsDiff * 7.54716981f); //2650

            return totalCost;
        }

        bool OnGossipHello(Player* p) override
        {
            p->PlayerTalkClass->ClearMenus();

            if (p->CustomWeapon)
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Details", WEAPON_FORGER_GOSSIP_DETAILS, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Stats", WEAPON_FORGER_GOSSIP_STATS, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Misc", WEAPON_FORGER_GOSSIP_MISC, 0);
                bool bNew = true;

                if (p->CustomWeapon->ItemId > 0)
                {
                    bNew = false;
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Reset Weapon Changes", WEAPON_FORGER_GOSSIP_RESET, 0);
                }
                
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Save Weapon", WEAPON_FORGER_GOSSIP_SAVE, 0, "Gold Cost " + std::to_string(CalculateWeaponCost(p, bNew)), 0, false);
            }
            else
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Forge Weapon", WEAPON_FORGER_GOSSIP_CREATE, 0);

            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Exit", WEAPON_FORGER_GOSSIP_EXIT, 0);
            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            return true;
        }

        bool OnGossipSelect(Player* p, uint32 menu_id, uint32 gossipListId) override
        {
            uint32 sender = gossipListId == 999 ? menu_id : p->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            ClearGossipMenuFor(p);

            switch (sender)
            {

            case WEAPON_FORGER_GOSSIP_CREATE:
            {
                if (true) // Check for Item Requirements
                {
                    auto& item = p->CustomWeapon;
                    item = new ItemTemplate();
                    item->Class = 2;
                    item->SoundOverrideSubclass = -1;
                    item->Name1 = "Default Weapon Name";
                    item->Flags = 67633184;
                    item->BuyCount = 1;
                    item->AllowableClass = -1;
                    item->AllowableRace = -1;
                    item->InventoryType = 13;
                    item->ItemLevel = 300;
                    item->RequiredLevel = 80;
                    item->Stackable = 1;
                    item->StatsCount = 5;
                    item->Damage[0].DamageMin = 450;
                    item->Damage[0].DamageMax = 500;
                    item->Delay = GetAttackSpeed(item->InventoryType, 0);
                    item->Bonding = 1;
                    item->Material = 1;
                    item->Sheath = 3;
                    item->RequiredDisenchantSkill = -1;

                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Details", WEAPON_FORGER_GOSSIP_DETAILS, 0);
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Stats", WEAPON_FORGER_GOSSIP_STATS, 0);
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Misc", WEAPON_FORGER_GOSSIP_MISC, 0);
                    bool bNew = (p->CustomWeapon->ItemId == 0);

                    if (bNew)
                        AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Reset Weapon Changes", WEAPON_FORGER_GOSSIP_RESET, 0);

                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Save Weapon", WEAPON_FORGER_GOSSIP_SAVE, 0, "Gold Cost " + std::to_string(CalculateWeaponCost(p, bNew)), 0, false);
                }
                else
                {
                    ChatHandler(p->GetSession()).SendSysMessage("Missing Required Items.");
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Forge Weapon", WEAPON_FORGER_GOSSIP_CREATE, 0);
                }

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Exit", WEAPON_FORGER_GOSSIP_EXIT, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_SAVE:
            {
                SaveCustomWeapon(p);
                p->PlayerTalkClass->SendCloseGossip();
            } break;

            case WEAPON_FORGER_GOSSIP_RESET:
            {
                *p->CustomWeapon = sObjectMgr->GetItemTemplateStore().at(p->CustomWeapon->ItemId);
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_BACK, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_BACK:
            {
                bool bNew = (p->CustomWeapon->ItemId == 0);

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Details", WEAPON_FORGER_GOSSIP_DETAILS, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Stats", WEAPON_FORGER_GOSSIP_STATS, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Misc", WEAPON_FORGER_GOSSIP_MISC, 0);

                if (bNew)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Reset Weapon Changes", WEAPON_FORGER_GOSSIP_RESET, 0);

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Save Weapon", WEAPON_FORGER_GOSSIP_SAVE, 0, "Gold Cost " + std::to_string(CalculateWeaponCost(p, bNew)), 0, false);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Exit", WEAPON_FORGER_GOSSIP_EXIT, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_EXIT:
            {
                p->PlayerTalkClass->SendCloseGossip();
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS:
            {
                std::stringstream  WeaponQuality;
                WeaponQuality << "|c" << std::hex << ItemQualityColors[p->CustomWeapon->Quality] << std::dec;
                switch (p->CustomWeapon->Quality)
                {
                case ITEM_QUALITY_POOR:         WeaponQuality << "Poor|r";       break;
                case ITEM_QUALITY_NORMAL:       WeaponQuality << "Common|r";     break;
                case ITEM_QUALITY_UNCOMMON:     WeaponQuality << "Uncommon|r";   break;
                case ITEM_QUALITY_RARE:         WeaponQuality << "Rare|r";       break;
                case ITEM_QUALITY_EPIC:         WeaponQuality << "Epic|r";       break;
                case ITEM_QUALITY_LEGENDARY:    WeaponQuality << "Legendary|r";  break;
                }
                std::string WeaponType;
                switch (p->CustomWeapon->SubClass)
                {
                case 0: WeaponType = "One Handed Axe"; break;
                case 1: WeaponType = "Two Handed Axe"; break;
                case 2: WeaponType = "Bow"; break;
                case 3: WeaponType = "Gun"; break;
                case 4: WeaponType = "One Handed Mace"; break;
                case 5: WeaponType = "Two Handed Mace"; break;
                case 6: WeaponType = "Polearm"; break;
                case 7: WeaponType = "One Handed Sword"; break;
                case 8: WeaponType = "Two Handed Sword"; break;
                case 10: WeaponType = "Staff"; break;
                case 13: WeaponType = "Fist Weapon"; break;
                case 15: WeaponType = "Dagger"; break;
                case 18: WeaponType = "Crossbow"; break;
                default: WeaponType = "Not Selected"; break;
                }
                std::string WeaponSheath;
                switch (p->CustomWeapon->Sheath)
                {
                case 1: WeaponSheath = "Two Handed Weapon"; break;
                case 2: WeaponSheath = "Staff"; break;
                case 3: WeaponSheath = "One Handed Weapon"; break;
                }

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Name: " + p->CustomWeapon->Name1, WEAPON_FORGER_GOSSIP_DETAILS_NAME, 0, "Input Name", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Display ID: " + std::to_string(p->CustomWeapon->DisplayInfoID), WEAPON_FORGER_GOSSIP_DETAILS_DISPLAY, 0, "Input Display ID", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Quality: " + WeaponQuality.str(), WEAPON_FORGER_GOSSIP_DETAILS_QUALITY, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Type: " + WeaponType, WEAPON_FORGER_GOSSIP_DETAILS_TYPE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Sheath: " + WeaponSheath, WEAPON_FORGER_GOSSIP_DETAILS_SHEATH, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Description: ", WEAPON_FORGER_GOSSIP_DETAILS_DESCRIPTION, 0, "Input Description", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, p->CustomWeapon->Description.size() > 0 ? p->CustomWeapon->Description : "<No Description>", WEAPON_FORGER_GOSSIP_DETAILS, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_BACK, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_QUALITY:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Quality: |CFF9D9D9DPoor|r", WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_POOR, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Quality: |CFFFFFFFFCommon|r", WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_COMMON, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Quality: |CFF1EFF00Uncommon|r", WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_UNCOMMON, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Quality: |CFF0070DDRare|r", WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_RARE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Quality: |CFFA335EEEpic|r", WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_EPIC, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Quality: |CFFFF8000Legendary|r", WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_LEGENDARY, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_DETAILS, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_POOR:
            case WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_COMMON:
            case WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_UNCOMMON:
            case WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_RARE:
            case WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_EPIC:
            case WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_LEGENDARY:
            {
                p->CustomWeapon->Quality = sender - WEAPON_FORGER_GOSSIP_DETAILS_QUALITY_POOR;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "One Handed Weapons", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Two Handed Weapons", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Other Weapons", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_DETAILS, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "One Handed Axe", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_AXE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "One Handed Mace", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_MACE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "One Handed Sword", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_SWORD, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Fist Weapon", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_FIST, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Dagger", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_DAGGER, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_DETAILS_TYPE, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_AXE:
            {
                p->CustomWeapon->SubClass = 0;
                if (p->CustomWeapon->InventoryType != 13)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 13);

                    if (p->CustomWeapon->Delay > 0)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(13, i), 13);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(13, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(13, 0);
                    p->CustomWeapon->InventoryType = 13;
                }
                if (p->CustomWeapon->RangedModRange > 0)
                    p->CustomWeapon->RangedModRange = 0;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_MACE:
            {
                p->CustomWeapon->SubClass = 4;
                if (p->CustomWeapon->InventoryType != 13)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 13);

                    if (p->CustomWeapon->Delay > 0)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(13, i), 13);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(13, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(13, 0);
                    p->CustomWeapon->InventoryType = 13;
                }
                if (p->CustomWeapon->RangedModRange > 0)
                    p->CustomWeapon->RangedModRange = 0;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_SWORD:
            {
                p->CustomWeapon->SubClass = 7;
                if (p->CustomWeapon->InventoryType != 13)
                {

                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 13);

                    if (p->CustomWeapon->Delay > 0)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(13, i), 13);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(13, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(13, 0);
                    p->CustomWeapon->InventoryType = 13;
                }
                if (p->CustomWeapon->RangedModRange > 0)
                    p->CustomWeapon->RangedModRange = 0;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_DAGGER:
            {
                p->CustomWeapon->SubClass = 15;
                if (p->CustomWeapon->InventoryType != 13)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 13);

                    if (p->CustomWeapon->Delay > 0)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(13, i), 13);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(13, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(13, 0);
                    p->CustomWeapon->InventoryType = 13;
                }
                if (p->CustomWeapon->RangedModRange > 0)
                    p->CustomWeapon->RangedModRange = 0;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_FIST:
            {
                p->CustomWeapon->SubClass = 13;
                if (p->CustomWeapon->InventoryType != 13)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 13);

                    if (p->CustomWeapon->Delay > 0)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(13, i), 13);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(13, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(13, 0);
                    p->CustomWeapon->InventoryType = 13;
                }
                if (p->CustomWeapon->RangedModRange > 0)
                    p->CustomWeapon->RangedModRange = 0;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Two Handed Axe", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_AXE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Two Handed Mace", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_MACE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Two Handed Sword", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_SWORD, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Polearm", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_POLEARM, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Staff", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_STAFF, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_DETAILS_TYPE, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_AXE:
            {
                p->CustomWeapon->SubClass = 1;
                if (p->CustomWeapon->InventoryType != 17)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 17);

                    if (p->CustomWeapon->Delay > 0)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(17, i), 17);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(17, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(17, 0);
                    p->CustomWeapon->InventoryType = 17;
                }
                if (p->CustomWeapon->RangedModRange > 0)
                    p->CustomWeapon->RangedModRange = 0;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_MACE:
            {
                p->CustomWeapon->SubClass = 5;
                if (p->CustomWeapon->InventoryType != 17)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 17);

                    if (p->CustomWeapon->Delay > 0)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(17, i), 17);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(17, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(17, 0);
                    p->CustomWeapon->InventoryType = 17;
                }
                if (p->CustomWeapon->RangedModRange > 0)
                    p->CustomWeapon->RangedModRange = 0;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_SWORD:
            {
                p->CustomWeapon->SubClass = 8;
                if (p->CustomWeapon->InventoryType != 17)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 17);

                    if (p->CustomWeapon->Delay > 0)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(17, i), 17);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(17, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(17, 0);
                    p->CustomWeapon->InventoryType = 17;
                }
                if (p->CustomWeapon->RangedModRange > 0)
                    p->CustomWeapon->RangedModRange = 0;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_STAFF:
            {
                p->CustomWeapon->SubClass = 10;
                if (p->CustomWeapon->InventoryType != 17)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 17);

                    if (p->CustomWeapon->Delay > 0)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(17, i), 17);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(17, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(17, 0);
                    p->CustomWeapon->InventoryType = 17;
                }
                if (p->CustomWeapon->RangedModRange > 0)
                    p->CustomWeapon->RangedModRange = 0;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_POLEARM:
            {
                p->CustomWeapon->SubClass = 6;
                if (p->CustomWeapon->InventoryType != 17)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 17);

                    if (p->CustomWeapon->Delay > 0)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(17, i), 17);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(17, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(17, 0);
                    p->CustomWeapon->InventoryType = 17;
                }
                if (p->CustomWeapon->RangedModRange > 0)
                    p->CustomWeapon->RangedModRange = 0;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Crossbow", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_CROSSBOW, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Bow", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_BOW, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Gun", WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_GUN, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_DETAILS_TYPE, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_CROSSBOW:
            {
                p->CustomWeapon->SubClass = 18;
                if (p->CustomWeapon->InventoryType != 15)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 15);

                    if (p->CustomWeapon->Delay > 0 && p->CustomWeapon->InventoryType != 26)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(15, i), 15);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(15, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(15, 0);
                    p->CustomWeapon->InventoryType = 15;
                }
                if (p->CustomWeapon->RangedModRange < 100)
                    p->CustomWeapon->RangedModRange = 100;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_BOW:
            {
                p->CustomWeapon->SubClass = 2;
                if (p->CustomWeapon->InventoryType != 15)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 15);

                    if (p->CustomWeapon->Delay > 0 && p->CustomWeapon->InventoryType != 26)
                    {
                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(15, i), 15);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(15, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(15, 0);
                    p->CustomWeapon->InventoryType = 15;
                }
                if (p->CustomWeapon->RangedModRange < 100)
                    p->CustomWeapon->RangedModRange = 100;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_GUN:
            {
                p->CustomWeapon->SubClass = 3;
                if (p->CustomWeapon->InventoryType != 26)
                {
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatType, p->CustomWeapon->ItemStat[i].ItemStatValue, 26);

                    if (p->CustomWeapon->Delay > 0 && p->CustomWeapon->InventoryType != 15)
                    {

                        for (int i = 0; i < 3; i++)
                            if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            {
                                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                                {
                                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(26, i), 26);
                                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                                }

                                p->CustomWeapon->Delay = GetAttackSpeed(26, i);
                            }
                    }
                    else
                        p->CustomWeapon->Delay = GetAttackSpeed(26, 0);
                    p->CustomWeapon->InventoryType = 26;
                }
                if (p->CustomWeapon->RangedModRange < 100)
                    p->CustomWeapon->RangedModRange = 100;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_SHEATH:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Staff (Diagonally across the back pointing upwards)", WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_STAFF, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Two Handed Weapon (On the left-hand side of the character's waist)", WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_2H, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "One Handed Weapon (On the middle of the character's back)", WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_1H, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_DETAILS, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_2H:
            case WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_STAFF:
            case WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_1H:
            {
                p->CustomWeapon->Sheath = sender - WEAPON_FORGER_GOSSIP_DETAILS_SHEATH;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS:
            {
                for (int i = 0; i < 5; i++)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Stat " + std::to_string(i + 1) + ": " + GetStatName(p->CustomWeapon->ItemStat[i].ItemStatType), WEAPON_FORGER_GOSSIP_STATS_1 + i, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Attack Speed: " + std::to_string(p->CustomWeapon->Delay), WEAPON_FORGER_GOSSIP_STATS_DELAY, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Damage: " + std::to_string(uint32(p->CustomWeapon->Damage[0].DamageMin)) + " - " + std::to_string(uint32(p->CustomWeapon->Damage[0].DamageMax)), WEAPON_FORGER_GOSSIP_STATS_DAMAGE, 0, "Input Damage Amount", false, true);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_BACK, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_1:
            case WEAPON_FORGER_GOSSIP_STATS_2:
            case WEAPON_FORGER_GOSSIP_STATS_3:
            case WEAPON_FORGER_GOSSIP_STATS_4:
            case WEAPON_FORGER_GOSSIP_STATS_5:
            {
                nOptionNumber = sender - WEAPON_FORGER_GOSSIP_STATS_1;
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Stat Type: " + GetStatName(p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType), WEAPON_FORGER_GOSSIP_STATS_TYPE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Stat Value: " + std::to_string(p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue), WEAPON_FORGER_GOSSIP_STATS_VALUE, 0, "Input Stat Value", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_STATS, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_DELAY:
            {
                for (int i = 0; i < 3; i++)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Attack Speed: " + std::to_string(GetAttackSpeed(p->CustomWeapon->InventoryType, i)), GOSSIP_SENDER_MAIN, WEAPON_FORGER_GOSSIP_STATS_DELAY + i + 1);

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, WEAPON_FORGER_GOSSIP_STATS);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_DELAY_1:
            case WEAPON_FORGER_GOSSIP_STATS_DELAY_2:
            case WEAPON_FORGER_GOSSIP_STATS_DELAY_3:
            {
                uint32 DelayID = sender - WEAPON_FORGER_GOSSIP_STATS_DELAY_1;

                if (p->CustomWeapon->Damage[0].DamageMax > 0)
                {
                    uint32 newDamage = ConvertDamage(p, GetAttackSpeed(p->CustomWeapon->InventoryType, DelayID));
                    p->CustomWeapon->Damage[0].DamageMax = newDamage;
                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                }

                p->CustomWeapon->Delay = GetAttackSpeed(p->CustomWeapon->InventoryType, DelayID);
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_MANA), WEAPON_FORGER_GOSSIP_STATS_TYPE_NONE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_AGILITY), WEAPON_FORGER_GOSSIP_STATS_TYPE_AGILITY, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_STRENGTH), WEAPON_FORGER_GOSSIP_STATS_TYPE_STRENGTH, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_INTELLECT), WEAPON_FORGER_GOSSIP_STATS_TYPE_INTELLECT, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_SPIRIT), WEAPON_FORGER_GOSSIP_STATS_TYPE_SPIRIT, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_STAMINA), WEAPON_FORGER_GOSSIP_STATS_TYPE_STAMINA, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_DEFENSE_SKILL_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_DEFENSE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_DODGE_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_DODGE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_PARRY_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_PARRY, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_BLOCK_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_BLOCK, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_HIT_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_HIT, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_CRIT_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_CRIT, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_HASTE_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_HASTE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_ATTACK_POWER), WEAPON_FORGER_GOSSIP_STATS_TYPE_ATTACKPOWER, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_SPELL_POWER), WEAPON_FORGER_GOSSIP_STATS_TYPE_SPELLPOWER, 0);

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_NONE:
            {
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = 0;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_MANA;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_AGILITY:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_AGILITY, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_AGILITY;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_STRENGTH:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_STRENGTH, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_STRENGTH;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_INTELLECT:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_INTELLECT, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_INTELLECT;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_SPIRIT:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_SPIRIT, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_SPIRIT;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_STAMINA:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_STAMINA, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_STAMINA;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_DEFENSE:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_DEFENSE_SKILL_RATING, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_DEFENSE_SKILL_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_DODGE:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_DODGE_RATING, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_DODGE_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_PARRY:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_PARRY_RATING, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_PARRY_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_BLOCK:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_BLOCK_RATING, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_BLOCK_RATING;
                OnGossipSelect(p, sender, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_HIT:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_HIT_RATING, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_HIT_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_CRIT:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_CRIT_RATING, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_CRIT_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_HASTE:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_HASTE_RATING, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_HASTE_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_ATTACKPOWER:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_ATTACK_POWER, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_ATTACK_POWER;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_SPELLPOWER:
            {
                if (p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue > 0)
                    p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = ConvertStat(p, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType, ITEM_MOD_SPELL_POWER, p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_SPELL_POWER;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;


            case WEAPON_FORGER_GOSSIP_MISC:
            {
                for (int i = 0; i < MAX_ITEM_PROTO_SOCKETS; i++)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Gem Socket " + std::to_string(i + 1) + ": " + GetSocketColor(p->CustomWeapon->Socket[i].Color), WEAPON_FORGER_GOSSIP_MISC_SOCKET_1 + i, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_BACK, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SOCKET_1:
            case WEAPON_FORGER_GOSSIP_MISC_SOCKET_2:
            case WEAPON_FORGER_GOSSIP_MISC_SOCKET_3:
            {
                nOptionNumber = sender - WEAPON_FORGER_GOSSIP_MISC_SOCKET_1;

                if (p->CustomWeapon->Socket[nOptionNumber].Color != 0)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Remove Gem Socket", WEAPON_FORGER_GOSSIP_MISC_SOCKET_NONE, 0);

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Red Socket", WEAPON_FORGER_GOSSIP_MISC_SOCKET_RED, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Yellow Socket", WEAPON_FORGER_GOSSIP_MISC_SOCKET_YELLOW, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Blue Socket", WEAPON_FORGER_GOSSIP_MISC_SOCKET_BLUE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_MISC, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SOCKET_NONE:
            {
                p->CustomWeapon->Socket[nOptionNumber].Color = 0;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SOCKET_RED:
            {
                p->CustomWeapon->Socket[nOptionNumber].Color = SocketColor::SOCKET_COLOR_RED;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SOCKET_YELLOW:
            {
                p->CustomWeapon->Socket[nOptionNumber].Color = SocketColor::SOCKET_COLOR_YELLOW;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SOCKET_BLUE:
            {
                p->CustomWeapon->Socket[nOptionNumber].Color = SocketColor::SOCKET_COLOR_BLUE;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SPELL:
            {

            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SPELL_1:
            {

            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SPELL_2:
            {

            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SPELL_3:
            {

            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SPELL_4:
            {

            } break;


            }

            return true;
        }

        bool OnGossipSelectCode(Player* p, uint32 menu_id, uint32 gossipListId, char const* code) override
        {
            uint32 sender = gossipListId == 999 ? menu_id : p->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            ClearGossipMenuFor(p);

            switch (sender)
            {

            case WEAPON_FORGER_GOSSIP_DETAILS_NAME:
            {
                // Check if AccInfo is banned from naming weapons
                if (true)
                {
                    if (std::string(code).size() <= 25)
                        p->CustomWeapon->Name1 = code;
                    else
                        ChatHandler(p->GetSession()).SendSysMessage("Invalid name, can only be 25 characters or less.");
                }
                else
                    ChatHandler(p->GetSession()).SendSysMessage("You are banned from personlizing the weapon name.");

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_DISPLAY:
            {
                for (auto& c : std::string(code))
                {
                    if (!std::isdigit(c))
                    {
                        ChatHandler(p->GetSession()).SendSysMessage("Invalid DisplayID, You didn't input a number.");
                        OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
                        return true;
                    }
                }

                if (true)
                    p->CustomWeapon->DisplayInfoID = std::atoi(code);
                else
                    ChatHandler(p->GetSession()).SendSysMessage("Unable to use that Display ID.");

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;


            case WEAPON_FORGER_GOSSIP_DETAILS_DESCRIPTION:
            {
                // Check if AccInfo is banned from naming weapons
                if (true)
                    if (std::string(code).size() <= 50)
                        p->CustomWeapon->Description = code;
                    else
                        ChatHandler(p->GetSession()).SendSysMessage("Invalid name, can only be 50 characters or less.");
                else
                    ChatHandler(p->GetSession()).SendSysMessage("You are banned from personlizing the weapon name.");

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_DAMAGE:
            {
                bool b2Hander = (p->CustomWeapon->InventoryType == 15 || p->CustomWeapon->InventoryType == 17 || p->CustomWeapon->InventoryType == 26);

                for (auto& c : std::string(code))
                {
                    if (!std::isdigit(c))
                    {
                        ChatHandler(p->GetSession()).SendSysMessage("Invalid Damage, You didn't input a valid number.");
                        OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
                        return true;
                    }
                }

                int32 Value = std::atoi(code);
                float dps = (Value * 1.9f * 0.5f) / (p->CustomWeapon->Delay * 0.001f);

                if (Value >= 500)
                {
                    bool bValidDps = true;
                    if (b2Hander)
                    {
                        if (dps > 3500.0f)
                            bValidDps = false;
                    }
                    else
                        if (dps > 2650.0f)
                            bValidDps = false;

                    if (bValidDps)
                    {
                        p->CustomWeapon->Damage[0].DamageMin = Value * 0.9f;
                        p->CustomWeapon->Damage[0].DamageMax = Value;
                    }
                    else
                        ChatHandler(p->GetSession()).PSendSysMessage("Invalid Dps Amount, must be less than or equal to %s.", b2Hander ? "3500" : "2650");
                }
                else
                    ChatHandler(p->GetSession()).SendSysMessage("Invalid Damage Amount.");

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_VALUE:
            {
                for (auto& c : std::string(code))
                {
                    if (!std::isdigit(c))
                    {
                        ChatHandler(p->GetSession()).SendSysMessage("Invalid Stat Value, You didn't input a number.");
                        OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
                        return true;
                    }
                }

                int32 Value = std::atoi(code);

                SetStatValue(p, Value);
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS_1 + nOptionNumber, 999);
            } break;

            }

            return true;
        }
        uint16 nOptionNumber = 0;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new WeaponForgerCreature_AI(creature);
    }
};

void AddSC_WeaponForgerCreature()
{
    new WeaponForgerCreature();
}
