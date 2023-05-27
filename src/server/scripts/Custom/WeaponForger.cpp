#include <Chat.h>
#include <DatabaseEnv.h>
#include <Item.h>
#include <ItemTemplate.h>
#include <Log.h>
#include <ObjectMgr.h>
#include <Player.h>
#include <ScriptedGossip.h>
#include <SpellMgr.h>
#include <WorldSession.h>

#include "PlayerInfo/PlayerInfo.h"
#include <iostream>
#include <Mail.h>

enum WEAPON_FORGER_MENU
{
    WEAPON_FORGER_GOSSIP_CREATE = 0,
    WEAPON_FORGER_GOSSIP_UPGRADE,
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
    WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_RANGED,
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

    WEAPON_FORGER_GOSSIP_STATS_TYPE_NONE,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_AGILITY,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_STRENGTH,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_INTELLECT,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_SPIRIT,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_STAMINA,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_DEFENSE,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_DODGE,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_PARRY,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_HIT,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_CRIT,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_HASTE,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_EXPERTISE,  // NEW
    WEAPON_FORGER_GOSSIP_STATS_TYPE_ARP,        // NEW
    WEAPON_FORGER_GOSSIP_STATS_TYPE_ATTACKPOWER,
    WEAPON_FORGER_GOSSIP_STATS_TYPE_SPELLPOWER,


    WEAPON_FORGER_GOSSIP_MISC,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_1,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_2,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_3,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_NONE,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_RED,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_YELLOW,
    WEAPON_FORGER_GOSSIP_MISC_SOCKET_BLUE,

    WEAPON_FORGER_GOSSIP_MISC_SPELL,

    WEAPON_FORGER_GOSSIP_MISC_TANK_SPELLS,
    WEAPON_FORGER_GOSSIP_MISC_TANK_SPELLS_1,
    WEAPON_FORGER_GOSSIP_MISC_TANK_SPELLS_2,

    WEAPON_FORGER_GOSSIP_MISC_HEALER_SPELLS,
    WEAPON_FORGER_GOSSIP_MISC_HEALER_SPELLS_1,
    WEAPON_FORGER_GOSSIP_MISC_HEALER_SPELLS_2,

    WEAPON_FORGER_GOSSIP_MISC_CASTER_SPELLS,
    WEAPON_FORGER_GOSSIP_MISC_CASTER_SPELLS_1,
    WEAPON_FORGER_GOSSIP_MISC_CASTER_SPELLS_2,

    WEAPON_FORGER_GOSSIP_MISC_RANGED_SPELLS,
    WEAPON_FORGER_GOSSIP_MISC_RANGED_SPELLS_1,
    WEAPON_FORGER_GOSSIP_MISC_RANGED_SPELLS_2,

    WEAPON_FORGER_GOSSIP_MISC_MELEE_SPELLS,
    WEAPON_FORGER_GOSSIP_MISC_MELEE_SPELLS_1,
    WEAPON_FORGER_GOSSIP_MISC_MELEE_SPELLS_2,
};

class WeaponForgerCreature : public CreatureScript
{
public:
    WeaponForgerCreature() : CreatureScript("WeaponForgerCreature") {}

    struct WeaponForgerCreature_AI : public ScriptedAI
    {
        WeaponForgerCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        // NEW UPDATED NEED

        uint8 GetWeaponType(uint16 invType)
        {
            switch (invType)
            {
            case 13: return 1; break;
            case 17: return 2; break;
            case 15:
            case 26:
                return 4; break;
            default:
                return 0;
            }
        }

        uint32 GetStatAmount(uint32 type, Player* p)
        {
            bool b2Hander = (p->CustomWeapon->InventoryType == 15 || p->CustomWeapon->InventoryType == 17 || p->CustomWeapon->InventoryType == 26);
            float value = 0;

            switch (type)
            {
            case ITEM_MOD_AGILITY: value = 88.0f; break;
            case ITEM_MOD_STRENGTH: value = 121.0f; break;
            case ITEM_MOD_INTELLECT: value = 105.0f; break;
            case ITEM_MOD_SPIRIT: value = 82.5f; break;
            case ITEM_MOD_STAMINA: value = 121.0f; break;
            case ITEM_MOD_DEFENSE_SKILL_RATING: return 50.0f;
            case ITEM_MOD_DODGE_RATING: return 50.0f;
            case ITEM_MOD_PARRY_RATING:return 50.0f;
            case ITEM_MOD_HIT_RATING: return 50.0f * (b2Hander + 1);
            case ITEM_MOD_CRIT_RATING: return 50.0f * (b2Hander + 1);
            case ITEM_MOD_HASTE_RATING: return 75.0f * (b2Hander + 1);
            case ITEM_MOD_EXPERTISE_RATING: value = 55.0f; break;
            case ITEM_MOD_ARMOR_PENETRATION_RATING: value = 55.0f; break;
            case ITEM_MOD_ATTACK_POWER: value = 121.0f; break;
            case ITEM_MOD_SPELL_POWER: value = 450.0f; break;
            default: return 0; break;
            }

            if (b2Hander) value *= 2;
            return value * p->WeaponRank;
        }

        uint32 GetDmgAmount(Player* p)
        {
            bool b2Hander = (p->CustomWeapon->InventoryType == 15 || p->CustomWeapon->InventoryType == 17 || p->CustomWeapon->InventoryType == 26);
            float Dps = (b2Hander ? 175.0f : 132.5f) * p->WeaponRank;
            float AtkSpeed = p->CustomWeapon->Delay * 0.001f;

            //DPS = 1.9X / 2 / DELAY
            return uint32(std::ceil(Dps * AtkSpeed * 2 / 1.9f));
        }

        // OLD BUT NEED MAYBE UPDATE?

        std::string WeaponUpdateString(Player* p)
        {
            if (p->CustomWeapon->ItemId == 0)
                return "";

            auto& a = p->CustomWeapon;
            auto& b = *sObjectMgr->GetItemTemplate(a->ItemId);

            std::stringstream ss;

            if (a->SubClass != b.SubClass)  ss << "`subclass` = " << a->SubClass;
            if (a->Name1 != b.Name1)  ss << ", `name` = '" << a->Name1 << "'";
            if (a->DisplayInfoID != b.DisplayInfoID)  ss << ", `displayid` = " << a->DisplayInfoID;
            if (a->Quality != b.Quality)  ss << ", `Quality` = " << a->Quality;
            if (a->InventoryType != b.InventoryType)  ss << ", `InventoryType` = " << a->InventoryType;
            for (int i = 0; i < 5; i++)
            {
                if (a->ItemStat[i].ItemStatType != b.ItemStat[i].ItemStatType) ss << ", `stat_type" << i + 1 << "` = " << a->ItemStat[i].ItemStatType;
                if (a->ItemStat[i].ItemStatValue != b.ItemStat[i].ItemStatValue) ss << ", `stat_value" << i + 1 << "` = " << a->ItemStat[i].ItemStatValue;
            }
            if (a->Damage[0].DamageMin != b.Damage[0].DamageMin) ss << ", `dmg_min1` = " << a->Damage[0].DamageMin;
            if (a->Damage[0].DamageMax != b.Damage[0].DamageMax) ss << ", `dmg_max1` = " << a->Damage[0].DamageMax;
            if (a->Delay != b.Delay)  ss << ", `delay` = " << a->Delay;
            if (a->RangedModRange != b.RangedModRange)  ss << ", `RangedModRange` = " << a->RangedModRange;
            if (a->Spells[0].SpellId != b.Spells[0].SpellId)  ss << ", `spellid_1` = " << a->Spells[0].SpellId;
            if (a->Spells[0].SpellTrigger != b.Spells[0].SpellTrigger)  ss << ", `spelltrigger_1` = " << a->Spells[0].SpellTrigger;
            if (a->Spells[0].SpellCharges != b.Spells[0].SpellCharges)  ss << ", `spellcharges_1` = " << a->Spells[0].SpellCharges;
            if (a->Spells[0].SpellPPMRate != b.Spells[0].SpellPPMRate)  ss << ", `spellppmRate_1` = " << a->Spells[0].SpellPPMRate;
            if (a->Spells[0].SpellCooldown != b.Spells[0].SpellCooldown)  ss << ", `spellcooldown_1` = " << a->Spells[0].SpellCooldown;
            if (a->Spells[0].SpellCategory != b.Spells[0].SpellCategory)  ss << ", `spellcategory_1` = " << a->Spells[0].SpellCategory;
            if (a->Spells[0].SpellCategoryCooldown != b.Spells[0].SpellCategoryCooldown)  ss << ", `spellcategorycooldown_1` = " << a->Spells[0].SpellCategoryCooldown;
            if (a->Description != b.Description)  ss << ", `description` = '" << a->Description << "'";
            for (int i = 0; i < MAX_ITEM_PROTO_SOCKETS; i++)
                if (a->Socket[i].Color != b.Socket[i].Color) ss << ", `socketColor_" << i + 1 << "` = " << a->Socket[i].Color;
            ss << ", `Rank` = " << p->WeaponRank;

            std::string updateString = ss.str();
            while (updateString[0] == ',' || updateString[0] == ' ')
                updateString.erase(updateString.begin(), updateString.begin() + 1);

            return updateString;
        }

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

        bool SaveCustomWeapon(Player* p, bool bUpgrade = false)
        {
            auto& item = p->CustomWeapon;
            bool bCreateNewWeapon = false;
            if (item->ItemId == 0)
                bCreateNewWeapon = true;

            // Check for Required Forge Item
            //if (!p->HasItemCount(0, 1))
            // {
            //      ChatHandler(p->GetSession()).PSendSysMessage("You need [%s]x1 to Forge this weapon.", sObjectMgr->GetItemTemplate(0)->Name1);
            //      return false;
            // }

            // Make sure valid Display ID
            if (!sPlayerInfo.CanUseDisplayID(p->CustomWeapon->DisplayInfoID, GetWeaponType(p->CustomWeapon->InventoryType)))
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to Save weapon, invalid Display ID");
                return false;
            }

            // Make sure no Duplicate Stas
            for (int i = 0; i < 4; i++)
                for (int j = i + 1; j < 5; j++)
                    if (item->ItemStat[i].ItemStatType == item->ItemStat[j].ItemStatType && item->ItemStat[i].ItemStatType != ITEM_MOD_MANA)
                    {
                        ChatHandler(p->GetSession()).SendSysMessage("Unable to Save weapon, cannot have multiple of the same stat type.");
                        return false;
                    }

            // Make sure has enough money

            //uint32 GoldCost = CalculateWeaponCost(p, bCreateNewWeapon);
            //if (!p->HasEnoughMoney(GoldCost * 10000))
            //{
            //    ChatHandler(p->GetSession()).PSendSysMessage("You need %u Gold to Forge this weapon.", GoldCost);
            //    return false;
            //}

            // Check make sure all stats are correct

            //p->ModifyMoney(GoldCost * -10000);

            if (bCreateNewWeapon)
            {
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
                    return false;
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
                QueryResult InsertQuery = WorldDatabase.PQuery("UPDATE `item_template_custom` SET %s WHERE `CharacterID` = %u;", WeaponUpdateString(p), p->GetGUID().GetRawValue());

                if (itemInfo)
                    if (itemInfo->IsEquipped())
                        p->_ApplyItemMods(itemInfo, itemInfo->GetSlot(), false);

                sObjectMgr->UpdateCustomItemTemplate(*item, item->ItemId);
                if (!bUpgrade)
                    sObjectMgr->InitializeQueriesData(QUERY_DATA_ITEMS);
                SendUpdatePacket(p, item->ItemId);

                if (itemInfo)
                {
                    itemInfo->SetState(ITEM_CHANGED, p);

                    if (itemInfo->IsEquipped())
                    {
                        p->SetVisibleItemSlot(itemInfo->GetSlot(), itemInfo);
                        itemInfo->SendUpdateToPlayer(p);
                    }

                    p->_ApplyItemMods(itemInfo, itemInfo->GetSlot(), true);
                }
            }

            p->WeaponUpdated = false;
            return true;
        }

        std::string GetStatName(uint32 value)
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
            case ITEM_MOD_HIT_RATING: return "Hit Rating"; break;
            case ITEM_MOD_CRIT_RATING: return "Crit Rating"; break;
            case ITEM_MOD_HASTE_RATING: return "Haste Rating"; break;
            case ITEM_MOD_EXPERTISE_RATING: return "Expertise Rating"; break;
            case ITEM_MOD_ARMOR_PENETRATION_RATING: return "Armor Penetration Rating"; break;
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

        // UNSURE OLD MAYBE NEED

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

        uint32 CalculateWeaponCost(Player* p, bool bNew)
        {
            bool b2Hander = (p->CustomWeapon->InventoryType == 15 || p->CustomWeapon->InventoryType == 17 || p->CustomWeapon->InventoryType == 26);
            uint32 totalCost = 1000;

            if (bNew)
            {
                totalCost = 20000;

                for (int i = 0; i < 5; i++)
                    if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0)
                        totalCost += 500;

                for (int i = 0; i < MAX_ITEM_PROTO_SOCKETS; i++)
                    if (p->CustomWeapon->Socket[i].Color > 0)
                        totalCost += 200;

                return totalCost;
            }

            auto& iTemplate = sObjectMgr->GetItemTemplateStore().at(p->CustomWeapon->ItemId);

            for (int i = 0; i < 5; i++)
                if (p->CustomWeapon->ItemStat[i].ItemStatValue > 0 && iTemplate.ItemStat[i].ItemStatValue == 0)
                    totalCost += 500;

            for (int i = 0; i < MAX_ITEM_PROTO_SOCKETS; i++)
                if (iTemplate.Socket[i].Color != p->CustomWeapon->Socket[i].Color)
                    if (iTemplate.Socket[i].Color == 0)
                        totalCost += 200;
                    else
                        totalCost += 20;

            return totalCost;
        }

        bool OnGossipHello(Player* p) override
        {
            ClearGossipMenuFor(p);

            if (p->CustomWeapon)
            {
                bool bNew = p->CustomWeapon->ItemId == 0;
                bool bUpdated = p->WeaponUpdated;
                uint32 UpdateTimer = sPlayerInfo.GetCharacterInfo(p->GetGUID())->PreviousWeaponUpdate;

                if (std::time(0) > UpdateTimer)
                {

                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Details", WEAPON_FORGER_GOSSIP_DETAILS, 0);
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Stats", WEAPON_FORGER_GOSSIP_STATS, 0);
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Misc", WEAPON_FORGER_GOSSIP_MISC, 0);

                    if (!bUpdated && !bNew && p->WeaponRank < 20)
                        AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Upgrade Weapon to [Rank " + std::to_string(p->WeaponRank + 1) + "]", WEAPON_FORGER_GOSSIP_UPGRADE, 0);

                    if (!bNew && bUpdated)
                        AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Reset Weapon Changes", WEAPON_FORGER_GOSSIP_RESET, 0);

                    if (bNew || (bUpdated && !bNew))
                        AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Save Weapon", WEAPON_FORGER_GOSSIP_SAVE, 0, "Gold Cost " + std::to_string(CalculateWeaponCost(p, bNew)), 0, false);
                }
                else
                {
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "|TInterface/ICONS/spell_holy_borrowedtime:48:48:0:0|t Unable to Modify Weapon until " + TimeToTimestampStr(UpdateTimer), WEAPON_FORGER_GOSSIP_BACK, 0);
                    if (p->WeaponRank < 20)
                        AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Upgrade Weapon", WEAPON_FORGER_GOSSIP_UPGRADE, 0);
                }
            }
            else
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Forge Weapon", WEAPON_FORGER_GOSSIP_CREATE, 0);

            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Exit", WEAPON_FORGER_GOSSIP_EXIT, 0);
            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            return true;
        }

        bool OnGossipSelect(Player* p, uint32 menu_id, uint32 gossipListId) override
        {
            uint32 sender = gossipListId == 999 ? menu_id : GetGossipSenderFor(p, gossipListId);// p->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            ClearGossipMenuFor(p);

            switch (sender)
            {

            case WEAPON_FORGER_GOSSIP_CREATE:
            {
                if (true) // Check for Item Requirements
                {
                    p->WeaponRank = 1;
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
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Save Weapon", WEAPON_FORGER_GOSSIP_SAVE, 0, "Gold Cost " + std::to_string(CalculateWeaponCost(p, true)), 0, false);
                }
                else
                {
                    ChatHandler(p->GetSession()).SendSysMessage("Missing Required Items.");
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Forge Weapon", WEAPON_FORGER_GOSSIP_CREATE, 0);
                }

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Exit", WEAPON_FORGER_GOSSIP_EXIT, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_UPGRADE:
            {
                if (true)
                {
                    p->WeaponRank++;

                    //Update Stats
                    for (int i = 0; i < 5; i++)
                        if (p->CustomWeapon->ItemStat[i].ItemStatType > 0)
                            p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    // Update Damage
                    uint32 newDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = newDamage;

                    SaveCustomWeapon(p);
                    OnGossipSelect(p, WEAPON_FORGER_GOSSIP_BACK, 999);
                }
            } break;

            case WEAPON_FORGER_GOSSIP_SAVE:
            {
                if (SaveCustomWeapon(p))
                {
                    if ((p->CustomWeapon->ItemId > 0))
                        if (auto pItem = p->GetItemByEntry(p->CustomWeapon->ItemId))
                            if (pItem->IsEquipped())
                            {
                                ItemPosCountVec off_dest;
                                if (p->CanStoreItem(NULL_BAG, NULL_SLOT, off_dest, pItem, false) == EQUIP_ERR_OK)
                                {
                                    p->RemoveItem(INVENTORY_SLOT_BAG_0, pItem->GetSlot(), true);
                                    p->StoreItem(off_dest, pItem, true);
                                }
                                else
                                {
                                    p->MoveItemFromInventory(INVENTORY_SLOT_BAG_0, pItem->GetSlot(), true);
                                    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
                                    pItem->DeleteFromInventoryDB(trans);                   // deletes item from character's inventory
                                    pItem->SaveToDB(trans);                                // recursive and not have transaction guard into self, item not in inventory and can be save standalone

                                    std::string subject = p->GetSession()->GetTrinityString(LANG_NOT_EQUIPPED_ITEM);
                                    MailDraft(subject, "There were problems with equipping one or several items").AddItem(pItem).SendMailTo(trans, p, MailSender(p, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_COPIED);

                                    CharacterDatabase.CommitTransaction(trans);
                                }
                            }

                    //sPlayerInfo.GetCharInfo(p->GetGUID())->WeaponUpdated = (std::time(0) + 86400);
                    //sPlayerInfo.SaveCharInfo(p->GetGUID());
                }
                CloseGossipMenuFor(p);
            } break;

            case WEAPON_FORGER_GOSSIP_RESET:
            {
                p->WeaponUpdated = false;
                *p->CustomWeapon = sObjectMgr->GetItemTemplateStore().at(p->CustomWeapon->ItemId);
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_BACK, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_BACK:
            {
                if (p->CustomWeapon)
                {
                    bool bNew = p->CustomWeapon->ItemId == 0;
                    bool bUpdated = p->WeaponUpdated;
                    uint32 UpdateTimer = sPlayerInfo.GetCharacterInfo(p->GetGUID())->PreviousWeaponUpdate;

                    if (std::time(0) > UpdateTimer)
                    {
                        AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Details", WEAPON_FORGER_GOSSIP_DETAILS, 0);
                        AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Stats", WEAPON_FORGER_GOSSIP_STATS, 0);
                        AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Weapon Misc", WEAPON_FORGER_GOSSIP_MISC, 0);

                        if (!bUpdated && !bNew && p->WeaponRank < 20)
                            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Upgrade Weapon to [Rank " + std::to_string(p->WeaponRank + 1) + "]", WEAPON_FORGER_GOSSIP_UPGRADE, 0);

                        if (!bNew && bUpdated)
                            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Reset Weapon Changes", WEAPON_FORGER_GOSSIP_RESET, 0);

                        if (bNew || (bUpdated && !bNew))
                            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Save Weapon", WEAPON_FORGER_GOSSIP_SAVE, 0, "Gold Cost " + std::to_string(CalculateWeaponCost(p, bNew)), 0, false);
                    }
                    else
                    {
                        AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Unable to Modify Weapon until " + TimeToTimestampStr(UpdateTimer), WEAPON_FORGER_GOSSIP_BACK, 0);
                        if (p->WeaponRank < 20)
                            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Upgrade Weapon", WEAPON_FORGER_GOSSIP_UPGRADE, 0);
                    }
                }

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Exit", WEAPON_FORGER_GOSSIP_EXIT, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_EXIT:
            {
                CloseGossipMenuFor(p);
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
                case 0: WeaponSheath = "Ranged"; break;
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
                p->WeaponUpdated = true;
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
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    p->CustomWeapon->Sheath = 3;

                p->CustomWeapon->SubClass = 0;
                if (p->CustomWeapon->InventoryType != 13)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(13, i);

                    p->CustomWeapon->InventoryType = 13;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);
                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 0;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_MACE:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    p->CustomWeapon->Sheath = 3;

                p->CustomWeapon->SubClass = 4;
                if (p->CustomWeapon->InventoryType != 13)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(13, i);
                    p->CustomWeapon->InventoryType = 13;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 0;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_SWORD:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    p->CustomWeapon->Sheath = 3;

                p->CustomWeapon->SubClass = 7;
                if (p->CustomWeapon->InventoryType != 13)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(13, i);
                    p->CustomWeapon->InventoryType = 13;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 0;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_DAGGER:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    p->CustomWeapon->Sheath = 3;

                p->CustomWeapon->SubClass = 15;
                if (p->CustomWeapon->InventoryType != 13)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(13, i);

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    p->CustomWeapon->InventoryType = 13;
                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 0;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_1H_FIST:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    p->CustomWeapon->Sheath = 3;

                p->CustomWeapon->SubClass = 13;
                if (p->CustomWeapon->InventoryType != 13)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(13, i);

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    p->CustomWeapon->InventoryType = 13;
                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 0;
                p->WeaponUpdated = true;

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
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    p->CustomWeapon->Sheath = 1;

                p->CustomWeapon->SubClass = 1;
                if (p->CustomWeapon->InventoryType != 17)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(17, i);
                    p->CustomWeapon->InventoryType = 17;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 0;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_MACE:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    p->CustomWeapon->Sheath = 1;

                p->CustomWeapon->SubClass = 5;
                if (p->CustomWeapon->InventoryType != 17)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(17, i);
                    p->CustomWeapon->InventoryType = 17;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 0;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_SWORD:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    p->CustomWeapon->Sheath = 1;

                p->CustomWeapon->SubClass = 8;
                if (p->CustomWeapon->InventoryType != 17)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(17, i);
                    p->CustomWeapon->InventoryType = 17;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 0;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_STAFF:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    p->CustomWeapon->Sheath = 2;

                p->CustomWeapon->SubClass = 10;
                if (p->CustomWeapon->InventoryType != 17)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(17, i);
                    p->CustomWeapon->InventoryType = 17;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 0;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_2H_POLEARM:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    p->CustomWeapon->Sheath = 1;

                p->CustomWeapon->SubClass = 6;
                if (p->CustomWeapon->InventoryType != 17)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(17, i);
                    p->CustomWeapon->InventoryType = 17;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 0;
                p->WeaponUpdated = true;

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
                if (GetWeaponType(p->CustomWeapon->InventoryType) != 4)
                    p->CustomWeapon->Sheath = 0;

                p->CustomWeapon->SubClass = 18;
                if (p->CustomWeapon->InventoryType != 15)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(15, i);
                    p->CustomWeapon->InventoryType = 15;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 100;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_BOW:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) != 4)
                    p->CustomWeapon->Sheath = 0;

                p->CustomWeapon->SubClass = 2;
                if (p->CustomWeapon->InventoryType != 15)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(15, i);
                    p->CustomWeapon->InventoryType = 15;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 100;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_TYPE_OTHER_GUN:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) != 4)
                    p->CustomWeapon->Sheath = 0;

                p->CustomWeapon->SubClass = 3;
                if (p->CustomWeapon->InventoryType != 26)
                {
                    for (int i = 0; i < 3; i++)
                        if (GetAttackSpeed(p->CustomWeapon->InventoryType, i) == p->CustomWeapon->Delay)
                            p->CustomWeapon->Delay = GetAttackSpeed(26, i);
                    p->CustomWeapon->InventoryType = 26;

                    for (int i = 0; i < 5; i++)
                        p->CustomWeapon->ItemStat[i].ItemStatValue = GetStatAmount(p->CustomWeapon->ItemStat[i].ItemStatType, p);

                    uint32 UpdatedDamage = GetDmgAmount(p);
                    p->CustomWeapon->Damage[0].DamageMin = UpdatedDamage * 0.9f;
                    p->CustomWeapon->Damage[0].DamageMax = UpdatedDamage;
                }
                p->CustomWeapon->RangedModRange = 100;
                p->WeaponUpdated = true;

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_SHEATH:
            {
                if (GetWeaponType(p->CustomWeapon->InventoryType) == 4)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Ranged Weapon", WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_RANGED, 0);
                else
                {
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Staff (Diagonally across the back pointing upwards)", WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_STAFF, 0);
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Two Handed Weapon (On the left-hand side of the character's waist)", WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_2H, 0);
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "One Handed Weapon (On the middle of the character's back)", WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_1H, 0);
                }
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_DETAILS, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_RANGED:
            case WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_2H:
            case WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_STAFF:
            case WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_1H:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->Sheath = sender - WEAPON_FORGER_GOSSIP_DETAILS_SHEATH_RANGED;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS:
            {
                for (int i = 0; i < 5; i++)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Stat " + std::to_string(i + 1) + ": " + GetStatName(p->CustomWeapon->ItemStat[i].ItemStatType) + " - " + std::to_string(p->CustomWeapon->ItemStat[i].ItemStatValue), WEAPON_FORGER_GOSSIP_STATS_1 + i, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Edit Attack Speed: " + std::to_string(p->CustomWeapon->Delay), WEAPON_FORGER_GOSSIP_STATS_DELAY, 0);
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
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_MANA), WEAPON_FORGER_GOSSIP_STATS_TYPE_NONE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_AGILITY), WEAPON_FORGER_GOSSIP_STATS_TYPE_AGILITY, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_STRENGTH), WEAPON_FORGER_GOSSIP_STATS_TYPE_STRENGTH, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_INTELLECT), WEAPON_FORGER_GOSSIP_STATS_TYPE_INTELLECT, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_SPIRIT), WEAPON_FORGER_GOSSIP_STATS_TYPE_SPIRIT, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_STAMINA), WEAPON_FORGER_GOSSIP_STATS_TYPE_STAMINA, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_DEFENSE_SKILL_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_DEFENSE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_DODGE_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_DODGE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_PARRY_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_PARRY, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_HIT_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_HIT, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_CRIT_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_CRIT, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_HASTE_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_HASTE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_EXPERTISE_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_EXPERTISE, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_ARMOR_PENETRATION_RATING), WEAPON_FORGER_GOSSIP_STATS_TYPE_ARP, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_ATTACK_POWER), WEAPON_FORGER_GOSSIP_STATS_TYPE_ATTACKPOWER, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Stat Type: " + GetStatName(ITEM_MOD_SPELL_POWER), WEAPON_FORGER_GOSSIP_STATS_TYPE_SPELLPOWER, 0);

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_STATS, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_DELAY:
            {
                for (int i = 0; i < 3; i++)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Set Attack Speed: " + std::to_string(GetAttackSpeed(p->CustomWeapon->InventoryType, i)), WEAPON_FORGER_GOSSIP_STATS_DELAY + i + 1, 0);

                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_STATS, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_DELAY_1:
            case WEAPON_FORGER_GOSSIP_STATS_DELAY_2:
            case WEAPON_FORGER_GOSSIP_STATS_DELAY_3:
            {
                p->WeaponUpdated = true;
                uint32 DelayID = sender - WEAPON_FORGER_GOSSIP_STATS_DELAY_1;

                uint32 newDamage = GetDmgAmount(p);
                p->CustomWeapon->Damage[0].DamageMax = newDamage;
                p->CustomWeapon->Damage[0].DamageMin = newDamage * 0.9f;

                p->CustomWeapon->Delay = GetAttackSpeed(p->CustomWeapon->InventoryType, DelayID);
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_NONE:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = 0;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_MANA;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_AGILITY:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_AGILITY, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_AGILITY;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_STRENGTH:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_STRENGTH, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_STRENGTH;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_INTELLECT:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_INTELLECT, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_INTELLECT;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_SPIRIT:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_SPIRIT, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_SPIRIT;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_STAMINA:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_STAMINA, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_STAMINA;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_DEFENSE:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_DEFENSE_SKILL_RATING, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_DEFENSE_SKILL_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_DODGE:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_DEFENSE_SKILL_RATING, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_DODGE_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_PARRY:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_PARRY_RATING, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_PARRY_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_HIT:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_HIT_RATING, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_HIT_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_CRIT:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_CRIT_RATING, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_CRIT_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_HASTE:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_HASTE_RATING, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_HASTE_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_EXPERTISE:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_EXPERTISE_RATING, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_EXPERTISE_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_ARP:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_ARMOR_PENETRATION_RATING, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_ARMOR_PENETRATION_RATING;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_ATTACKPOWER:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_ATTACK_POWER, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_ATTACK_POWER;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_STATS_TYPE_SPELLPOWER:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatValue = GetStatAmount(ITEM_MOD_SPELL_POWER, p);
                p->CustomWeapon->ItemStat[nOptionNumber].ItemStatType = ITEM_MOD_SPELL_POWER;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_STATS, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC:
            {
                std::string SpellName;

                switch (p->CustomWeapon->Spells[0].SpellId)
                {
                case 101000: SpellName = "Tank Spell Rename"; break;
                case 101005: SpellName = "Healer Spell Rename"; break;
                case 101010: SpellName = "Caster Spell Rename"; break;
                case 101015: SpellName = "Ranged Spell Rename"; break;
                case 101020: SpellName = "Melee Spell Rename"; break;
                default: SpellName = "<No Spell>"; break;
                }

                for (int i = 0; i < MAX_ITEM_PROTO_SOCKETS; i++)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Gem Socket " + std::to_string(i + 1) + ": " + GetSocketColor(p->CustomWeapon->Socket[i].Color), WEAPON_FORGER_GOSSIP_MISC_SOCKET_1 + i, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Spell: " + SpellName, WEAPON_FORGER_GOSSIP_MISC_SPELL, 0);
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
                p->WeaponUpdated = true;
                p->CustomWeapon->Socket[nOptionNumber].Color = 0;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SOCKET_RED:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->Socket[nOptionNumber].Color = SocketColor::SOCKET_COLOR_RED;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SOCKET_YELLOW:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->Socket[nOptionNumber].Color = SocketColor::SOCKET_COLOR_YELLOW;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SOCKET_BLUE:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->Socket[nOptionNumber].Color = SocketColor::SOCKET_COLOR_BLUE;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_SPELL:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Tank Spells: - Increase Stamina 100%", WEAPON_FORGER_GOSSIP_MISC_TANK_SPELLS, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Healer Spells: - Increase bonus healing 50% of Intellect", WEAPON_FORGER_GOSSIP_MISC_HEALER_SPELLS, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Caster Spells: - Increase bonus damage 50% of Intellect", WEAPON_FORGER_GOSSIP_MISC_CASTER_SPELLS, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Ranged Spells: - Unsure", WEAPON_FORGER_GOSSIP_MISC_RANGED_SPELLS, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Melee Spells: - Unsure", WEAPON_FORGER_GOSSIP_MISC_MELEE_SPELLS, 0);

                //AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Tank Spell - Increases Stamina by 100%, Resistances by 100 and Heals 4% of your health every 5 seconds.", WEAPON_FORGER_GOSSIP_MISC_SPELL_1, 0);
                //AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Caster Spell - Increases Spell Power by 50% of your Intellect and Increases Casting speed by 25%.", WEAPON_FORGER_GOSSIP_MISC_SPELL_2, 0);
                //AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Ranged Spell - Weapon requires no ammo.", WEAPON_FORGER_GOSSIP_MISC_SPELL_3, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_MISC, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_TANK_SPELLS:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Name - Restores 4% of your health every 5 seconds.", WEAPON_FORGER_GOSSIP_MISC_TANK_SPELLS_1, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Name - Gives you a shield for 5% of the damage you deal.", WEAPON_FORGER_GOSSIP_MISC_TANK_SPELLS_2, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_MISC_SPELL, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_TANK_SPELLS_1:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->Spells[0].SpellId = 101000;
                p->CustomWeapon->Spells[0].SpellTrigger = ITEM_SPELLTRIGGER_ON_EQUIP;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_TANK_SPELLS_2:
            {
                //p->WeaponUpdated = true;
                //p->CustomWeapon->Spells[0].SpellId = 101000;
                //p->CustomWeapon->Spells[0].SpellTrigger = ITEM_SPELLTRIGGER_ON_EQUIP;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_HEALER_SPELLS:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Unknown", WEAPON_FORGER_GOSSIP_MISC_HEALER_SPELLS_1, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Unknown", WEAPON_FORGER_GOSSIP_MISC_HEALER_SPELLS_2, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_MISC_SPELL, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_HEALER_SPELLS_1:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->Spells[0].SpellId = 101005;
                p->CustomWeapon->Spells[0].SpellTrigger = ITEM_SPELLTRIGGER_ON_EQUIP;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_HEALER_SPELLS_2:
            {
                //p->WeaponUpdated = true;
                //p->CustomWeapon->Spells[0].SpellId = 101000;
                //p->CustomWeapon->Spells[0].SpellTrigger = ITEM_SPELLTRIGGER_ON_EQUIP;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_CASTER_SPELLS:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Unknown", WEAPON_FORGER_GOSSIP_MISC_CASTER_SPELLS_1, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Unknown", WEAPON_FORGER_GOSSIP_MISC_CASTER_SPELLS_2, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Back", WEAPON_FORGER_GOSSIP_MISC_SPELL, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_CASTER_SPELLS_1:
            {
                p->WeaponUpdated = true;
                p->CustomWeapon->Spells[0].SpellId = 101010;
                p->CustomWeapon->Spells[0].SpellTrigger = ITEM_SPELLTRIGGER_ON_EQUIP;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            case WEAPON_FORGER_GOSSIP_MISC_CASTER_SPELLS_2:
            {
                //p->WeaponUpdated = true;
                //p->CustomWeapon->Spells[0].SpellId = 101000;
                //p->CustomWeapon->Spells[0].SpellTrigger = ITEM_SPELLTRIGGER_ON_EQUIP;
                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_MISC, 999);
            } break;

            }

            return true;
        }

        bool OnGossipSelectCode(Player* p, uint32 menu_id, uint32 gossipListId, char const* code) override
        {
            bool bWeaponBanned = sPlayerInfo.GetAccountInfo(p->GetSession()->GetAccountId())->AllowTextDetails;
            uint32 sender = gossipListId == 999 ? menu_id : p->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            ClearGossipMenuFor(p);

            switch (sender)
            {

            case WEAPON_FORGER_GOSSIP_DETAILS_NAME:
            {
                // Check if AccInfo is banned from naming weapons
                if (!bWeaponBanned)
                {
                    if (std::string(code).size() <= 25)
                    {
                        p->CustomWeapon->Name1 = code;
                        p->WeaponUpdated = true;
                    }
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

                uint32 displayID = std::atoi(code);

                if (sPlayerInfo.CanUseDisplayID(displayID, GetWeaponType(p->CustomWeapon->InventoryType)))
                {
                    p->WeaponUpdated = true;
                    p->CustomWeapon->DisplayInfoID = displayID;
                }
                else
                    ChatHandler(p->GetSession()).SendSysMessage("Unable to use that Display ID.");

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
            } break;


            case WEAPON_FORGER_GOSSIP_DETAILS_DESCRIPTION:
            {
                // Check if AccInfo is banned from naming weapons
                if (!bWeaponBanned)
                    if (std::string(code).size() <= 50)
                    {
                        p->WeaponUpdated = true;
                        p->CustomWeapon->Description = code;
                    }
                    else
                        ChatHandler(p->GetSession()).SendSysMessage("Invalid name, can only be 50 characters or less.");
                else
                    ChatHandler(p->GetSession()).SendSysMessage("You are banned from personlizing the weapon description.");

                OnGossipSelect(p, WEAPON_FORGER_GOSSIP_DETAILS, 999);
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
