#pragma once

#include <DatabaseEnv.h>
#include <DBCStores.h>
#include <Log.h>
#include <SpellMgr.h>
#include <Unit.h>

class ModSpellInfo
{
public:
    float PlayerModifier = 100.0f;
    float CreatureModifier = 100.0f;
    std::string Comment;
};


class SpellDamageModifier
{
public:

    static SpellDamageModifier& instance()
    {
        static SpellDamageModifier SpDmgModInstance;
        return SpDmgModInstance;
    }

    std::string GetSpellFamilyClassName(uint32 spellClass)
    {
        switch (spellClass)
        {
        case SPELLFAMILY_WARRIOR:       return "Warrior - ";        break;
        case SPELLFAMILY_PALADIN:       return "Paladin - ";        break;
        case SPELLFAMILY_HUNTER:        return "Hunter - ";         break;
        case SPELLFAMILY_ROGUE:         return "Rogue - ";          break;
        case SPELLFAMILY_PRIEST:        return "Priest - ";         break;
        case SPELLFAMILY_DEATHKNIGHT:   return "Death Knight - ";   break;
        case SPELLFAMILY_SHAMAN:        return "Shaman - ";         break;
        case SPELLFAMILY_MAGE:          return "Mage - ";           break;
        case SPELLFAMILY_WARLOCK:       return "Warlock - ";        break;
        case SPELLFAMILY_DRUID:         return "Druid - ";          break;
        case SPELLFAMILY_PET:           return "Pet - ";            break;
        default:                        return "Generic - ";        break;
        }
    }

    void ModifySpellDamage(float& damagePct, uint32 spellId, bool victimIsPlayer, uint32 spellClass, bool isPet = false)
    {
        if (ModifiedSpells.find(spellId) == ModifiedSpells.end())
        {
            ModSpellInfo info;

            auto SpellInfo = sSpellStore.LookupEntry(spellId);
            if (SpellInfo)
            {
                info.Comment += "MISSING SPELL: " + GetSpellFamilyClassName(spellClass);
                for (auto& c : SpellInfo->Name)
                    info.Comment += c;

                if (isPet)
                    info.Comment += " - Pet";

                info.Comment += " (";
                for (auto& c : SpellInfo->NameSubtext)
                {
                    info.Comment += c;
                }
                info.Comment += ')';

                WorldDatabase.PQuery("INSERT INTO custom.spell_modifier (SpellID, PvpModifier, PveModifier, Comment) VALUES (%u, %f, %f, \"%s\")", spellId, info.PlayerModifier, info.CreatureModifier, info.Comment);
                ModifiedSpells.emplace(spellId, info);
                std::cout << "Adding Missed Spell:  " << spellId << std::endl;
            }
            else
                std::cout << "Error Finding SpellID: " << spellId << std::endl;
            return;
        }
        float mValue = victimIsPlayer ? ModifiedSpells[spellId].PlayerModifier : ModifiedSpells[spellId].CreatureModifier;
        damagePct *= (mValue * 0.01f);
    }

    void ModifyMeleeDamage(float& damagePct, uint32 spellId, bool victimIsPlayer, uint32 spellClass, bool isPet = false)
    {
        if (ModifiedSpells.find(spellId) == ModifiedSpells.end())
        {
            ModSpellInfo info;

            auto SpellInfo = sSpellStore.LookupEntry(spellId);
            if (SpellInfo)
            {
                info.Comment += "MISSING SPELL: " + GetSpellFamilyClassName(spellClass);
                for (auto& c : SpellInfo->Name)
                    info.Comment += c;

                if (isPet)
                    info.Comment += " - Pet";

                info.Comment += " (";
                for (auto& c : SpellInfo->NameSubtext)
                {
                    info.Comment += c;
                }
                info.Comment += ')';

                WorldDatabase.PQuery("INSERT INTO custom.spell_modifier (SpellID, PvpModifier, PveModifier, Comment) VALUES (%u, %f, %f, \"%s\")", spellId, info.PlayerModifier, info.CreatureModifier, info.Comment);
                ModifiedSpells.emplace(spellId, info);
                std::cout << "Adding Missed Spell:  " << spellId << std::endl;
            }
            else
                std::cout << "Error Finding SpellID: " << spellId << std::endl;
            return;
        }

        float mValue = victimIsPlayer ? ModifiedSpells[spellId].PlayerModifier : ModifiedSpells[spellId].CreatureModifier;
        damagePct *= (mValue * 0.01f);
    }

    void Load()
    {
        TC_LOG_INFO("server.loading", "Loading Spell Modifiers...");
        ModifiedSpells.clear();
        uint32 msStartTime = getMSTime();
        int nCounter = 0;

        QueryResult res = WorldDatabase.PQuery("SELECT SpellID, PvpModifier, PveModifier, Comment FROM custom.spell_modifier");
        if (res)
        {
            do
            {
                Field* pField = res->Fetch();
                ModSpellInfo info{};

                uint32 SpellID = pField[0].GetUInt32();;
                info.PlayerModifier = pField[1].GetFloat();
                info.CreatureModifier = pField[2].GetFloat();
                info.Comment = pField[3].GetString();

                if (ModifiedSpells.find(SpellID) != ModifiedSpells.end())
                {
                    TC_LOG_ERROR("server.loading", "Spell ID %u exists in the database multiple times...", SpellID);
                    continue;
                }

                if (info.CreatureModifier < 0.0f || info.PlayerModifier < 0.0f)
                {
                    TC_LOG_ERROR("server.loading", "Spell ID %u Modifier is negative...", SpellID);
                    continue;
                }

                ModifiedSpells.emplace(SpellID, info);
                nCounter++;

            } while (res->NextRow());
        }
        TC_LOG_INFO("server.loading", "Loaded %d Modified Spells in %ums", nCounter, GetMSTimeDiffToNow(msStartTime));
    }

    private:
        std::unordered_map<uint32, ModSpellInfo> ModifiedSpells;
};

#define sSpellModifier SpellDamageModifier::instance()
