#pragma once

#include <DatabaseEnv.h>
#include <Log.h>
#include <Unit.h>
#include <DBCStores.h>
#include <SpellMgr.h>

class ModSpellInfo
{
public:
    float PlayerModifier = 100.0f;
    float CreatureModifier = 100.0f;
    std::string Comment;
    uint64 Base = 0;
};


class SpellDamageModifier
{
public:

    static SpellDamageModifier& instance()
    {
        static SpellDamageModifier SpDmgModInstance;
        return SpDmgModInstance;
    }

    void ModifyDamage(uint32& damage, uint32 spellId, bool victimIsPlayer)
    {
        if (ModifiedSpells.find(spellId) == ModifiedSpells.end())
        {
            ModSpellInfo info;

            auto SpellInfo = sSpellStore.LookupEntry(spellId);
            if (SpellInfo)
            {
                info.Comment += "MISSING SPELL: ";
                for (auto& c : SpellInfo->Name)
                {
                    info.Comment += c;
                }
                if (info.Comment.size() > 0)
                    info.Comment += ' ';
                for (auto& c : SpellInfo->NameSubtext)
                {
                    info.Comment += c;
                }

                WorldDatabase.PQuery("INSERT INTO custom.spell_modifier (SpellID, PvpModifier, PveModifier, Comment) VALUES (%u, %f, %f, \"%s\")", spellId, info.PlayerModifier, info.CreatureModifier, info.Comment);
                ModifiedSpells.emplace(spellId, info);
                std::cout << "Adding Missed Spell:  " << spellId << std::endl;
            }
            else
                std::cout << "Error Finding SpellID: " << spellId << std::endl;
            return;
        }

        if (ModifiedSpells[spellId].Base == 0)
        {
            ModifiedSpells[spellId].Base = damage;
            WorldDatabase.PQuery("Update custom.spell_modifier Set BaseDmg = %u WHERE SpellID = %u", damage, spellId);
            std::cout << "Updated Spell Damage ID:  " << spellId << " - Damage: " << damage << std::endl;
        }

        float mValue;
        if (victimIsPlayer)
            mValue = ModifiedSpells[spellId].PlayerModifier;
        else
            mValue = ModifiedSpells[spellId].CreatureModifier;

        damage = (damage / 100.0f) * mValue;
    }

    void Load()
    {
        TC_LOG_INFO("server.loading", "Loading Spell Modifiers...");
        ModifiedSpells.clear();
        uint32 msStartTime = getMSTime();
        int nCounter = 0;

        QueryResult res = WorldDatabase.PQuery("SELECT SpellID, PvpModifier, PveModifier, Comment, BaseDmg FROM custom.spell_modifier");
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
                info.Base = pField[4].GetUInt64();

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
