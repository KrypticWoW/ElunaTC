#include <Chat.h>
#include <DatabaseEnv.h>
#include <Item.h>
#include <Log.h>
#include <ScriptPCH.h>
#include <ScriptedGossip.h>
#include <WorldSession.h>

#include "TeleportSystem.h"

namespace ItemTeleporterGlobals
{
    std::vector<uint32> InvalidReviveLocations =
    {
        489,			529,			30,				566,				628,				572,
        //	Warsong Gulch,  Arathi Basin,	Alterac Valley, Eye of The Storm,	Isle of Conquest,	Ruins of Lordaeron,
            559,			562,
            //	Nagrand Arena,	Blade's Edge Arena,	Dalaran Arena,	The Ring of Valor,
    };
}

enum Gossip_TPSys
{
    GOSSIP_TPSYS_ZERO = GOSSIP_ACTION_INFO_DEF * 2
};

enum OptionTypes
{
    OPTION_GROUP = 0,
    OPTION_TELEPORT,
    OPTION_RETURN
};

GossipOptionIcon GetGossipIcon(uint8 ID)
{
    switch (ID)
    {
    case OPTION_GROUP: return GOSSIP_ICON_TRAINER; break;
    case OPTION_TELEPORT: return GOSSIP_ICON_TAXI; break;
    case OPTION_RETURN: return GOSSIP_ICON_CHAT; break;
    default: return GOSSIP_ICON_CHAT; break;
    }
}

class DynamicTeleportCreature : public CreatureScript
{
public:
    DynamicTeleportCreature() : CreatureScript("DynamicTeleportCreature") {}

    struct DynamicTeleportCreature_AI : public ScriptedAI
    {
        DynamicTeleportCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        bool FitRequirements(Player* p, TeleItem* item)
        {
            if (p->GetLevel() < item->MinLevel || p->GetLevel() > item->MaxLevel)
                return false;

            if (item->RequiredFaction > -1)
            {
                if (item->RequiredFaction == 0)
                    if (p->GetTeam() == HORDE)
                        return false;
                if (item->RequiredFaction == 1)
                    if (p->GetTeam() == ALLIANCE)
                        return false;
            }

            if (item->RequiredQuest > -1)
            {
                uint32 qStatus = p->GetQuestStatus(item->RequiredQuest);
                if (qStatus == QUEST_STATUS_NONE || qStatus == QUEST_STATUS_INCOMPLETE || qStatus == QUEST_STATUS_FAILED)
                    return false;
            }

            if (item->RequiredAchievement > -1)
                if (!p->HasAchieved(item->RequiredAchievement))
                    return false;

            return true;
        }

        bool HasValidLocation(Player* p, TeleItem* item)
        {
            for (auto& i : item->Childs)
            {
                switch (i.second.Option)
                {
                case OPTION_GROUP:
                    if (HasValidLocation(p, &i.second))
                        return true;
                    break;
                case OPTION_TELEPORT:
                    if (FitRequirements(p, &i.second))
                        return true;
                    break;
                }
            }

            return false;
        }

        void AddTeleLocations(Player* p, int teleLocationID)
        {
            ClearGossipMenuFor(p);
            auto& locations = sTeleSystem.GetLocations();
            if (locations.empty())
                return;

            if (teleLocationID < -1)
                return;

            if (teleLocationID == -1)
                for (auto& item : locations)
                {
                    if (FitRequirements(p, &item.second))
                    {
                        if (item.second.Option == OPTION_GROUP)
                            if (!HasValidLocation(p, &item.second))
                                continue;

                        AddGossipItemFor(p, GetGossipIcon(item.second.Option), item.second.Name, GOSSIP_SENDER_MAIN, item.second.ID, item.second.BoxText, 0, false);
                    }
                }
            else
                for (auto& item : locations)
                {
                    auto found = item.second.Search(teleLocationID);
                    if (found)
                    {
                        for (auto& location : found->Childs)
                        {
                            if (FitRequirements(p, &location.second))
                            {
                                if (location.second.Option == OPTION_GROUP)
                                    if (!HasValidLocation(p, &location.second))
                                        continue;

                                AddGossipItemFor(p, GetGossipIcon(location.second.Option), location.second.Name, GOSSIP_SENDER_MAIN, location.second.ID, location.second.BoxText, 0, false);
                            }
                        }
                        break;
                    }
                }
        }

        bool OnGossipHello(Player* p) override
        {
            auto& locations = sTeleSystem.GetLocations();
            if (locations.empty())
                return true;

            AddTeleLocations(p, -1);
            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* p, uint32 /*sender*/, uint32 action) override
        {
            uint32 const newAction = GetGossipActionFor(p, action);

            auto& locations = sTeleSystem.GetLocations();
            if (locations.empty())
                return true;

            for (auto& loc : locations)
            {
                if (auto ptr = loc.second.Search(newAction))
                {
                    switch (ptr->Option)
                    {
                    case OPTION_GROUP:
                        AddTeleLocations(p, newAction);
                        SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
                        break;
                    case OPTION_TELEPORT:
                    {
                        if (p->IsInCombat())
                        {
                            ChatHandler(p->GetSession()).PSendSysMessage("You are unable to teleport while in combat.");
                            return true;
                        }
                        if (p->IsInFlight())
                            p->FinishTaxiFlight();

                        auto& loc = *ptr;
                        p->TeleportTo(loc.MapID, loc.PosX, loc.PosY, loc.PosZ, loc.PosO);
                        CloseGossipMenuFor(p);
                        break;
                    }
                    case OPTION_RETURN:
                        AddTeleLocations(p, ptr->ReturnId);
                        SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
                        break;
                    }
                }
            }
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new DynamicTeleportCreature_AI(creature);
    }
};

class DynamicTeleportItem : public ItemScript
{
public:
    DynamicTeleportItem() : ItemScript("DynamicTeleportItem") {}

    bool FitRequirements(Player* p, TeleItem* item)
    {
        if (item->RequiredSecurity > p->GetSession()->GetSecurity())
            return false;

        if (p->IsGameMaster())
            return true;

        if (p->GetLevel() < item->MinLevel || p->GetLevel() > item->MaxLevel)
            return false;

        if (item->RequiredFaction > -1)
        {
            if (item->RequiredFaction == 0)
                if (p->GetTeam() == HORDE)
                    return false;
            if (item->RequiredFaction == 1)
                if (p->GetTeam() == ALLIANCE)
                    return false;
        }

        if (item->RequiredClass > -1)
            if (!(item->RequiredClass & p->GetClassMask()))
                return false;

        if (item->RequiredQuest > -1)
        {
            uint32 qStatus = p->GetQuestStatus(item->RequiredQuest);
            if (qStatus == QUEST_STATUS_NONE || qStatus == QUEST_STATUS_INCOMPLETE || qStatus == QUEST_STATUS_FAILED)
                return false;
        }

        if (item->RequiredAchievement > -1)
            if (!p->HasAchieved(item->RequiredAchievement))
                return false;

        if (item->RequiredSpell > -1)
            if (!p->HasSpell(item->RequiredSpell))
                return false;

        return true;
    }

    bool HasValidLocation(Player* p, TeleItem* item)
    {
        for (auto& i : item->Childs)
        {
            switch (i.second.Option)
            {
            case OPTION_GROUP:
                if (HasValidLocation(p, &i.second))
                    return true;
                break;
            case OPTION_TELEPORT:
                if (FitRequirements(p, &i.second))
                    return true;
                break;
            }
        }

        return false;
    }

    void AddTeleLocations(Player* p, int teleLocationID)
    {
        ClearGossipMenuFor(p);
        auto& locations = sTeleSystem.GetLocations();
        if (locations.empty())
            return;

        if (teleLocationID < -1)
            return;

        if (teleLocationID == -1)
            for (auto& item : locations)
            {
                if (FitRequirements(p, &item.second))
                {
                    if (item.second.Option == OPTION_GROUP)
                        if (!HasValidLocation(p, &item.second))
                            continue;

                    std::string GossipString;
                    if (p->GetTeam() == ALLIANCE)
                        GossipString = item.second.A_Icon + GossipString;
                    else
                        GossipString = item.second.H_Icon + GossipString;

                    if (GossipString.size() > 0)
                        GossipString = "|TInterface/ICONS/" + GossipString + ":22:22:-26:0|t|r";
                    GossipString += item.second.Name;

                    AddGossipItemFor(p, GOSSIP_ICON_DOT, GossipString, GOSSIP_SENDER_MAIN, item.second.ID, item.second.BoxText, 0, false);
                }
            }
        else
            for (auto& item : locations)
            {
                auto found = item.second.Search(teleLocationID);
                if (found)
                {
                    for (auto& location : found->Childs)
                    {
                        if (FitRequirements(p, &location.second))
                        {
                            if (location.second.Option == OPTION_GROUP)
                                if (!HasValidLocation(p, &location.second))
                                    continue;

                            std::string GossipString;
                            if (p->GetTeam() == ALLIANCE)
                                GossipString = location.second.A_Icon + GossipString;
                            else
                                GossipString = location.second.H_Icon + GossipString;

                            if (GossipString.size() > 0)
                                GossipString = "|TInterface/ICONS/" + GossipString + ":22:22:-26:0|t|r";
                            GossipString += location.second.Name;

                            AddGossipItemFor(p, GOSSIP_ICON_DOT, GossipString, GOSSIP_SENDER_MAIN, location.second.ID, location.second.BoxText, 0, false);
                        }
                    }
                    break;
                }
            }
    }

    bool OnUse(Player* p, Item* item, SpellCastTargets const& /*targets*/) override
    {
        p->AttackStop();

        if (p->IsAlive())
        {
            auto& locations = sTeleSystem.GetLocations();
            if (locations.empty())
                return true;

            AddTeleLocations(p, -1);

            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
        }
        else
        {
            uint32 mapID = p->GetMap()->GetId();

            for (auto itr = ItemTeleporterGlobals::InvalidReviveLocations.begin(); itr != ItemTeleporterGlobals::InvalidReviveLocations.end(); itr++)
            {
                if ((*itr) == mapID)
                {
                    ChatHandler(p->GetSession()).SendNotify("You are unable revive here.");
                    return true;
                }
            }

            p->ResurrectPlayer(100, false);
            p->SpawnCorpseBones();
            p->SaveToDB();
        }


        return true;
    }

    void OnGossipSelect(Player* p, Item* item, uint32 sender, uint32 action) override
    {
        auto& locations = sTeleSystem.GetLocations();
        if (locations.empty())
            return;

        if (sender != GOSSIP_SENDER_MAIN)
            return;

        for (auto& loc : locations)
        {
            if (auto ptr = loc.second.Search(action))
            {
                switch (ptr->Option)
                {
                case OPTION_GROUP:
                    AddTeleLocations(p, action);
                    SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
                    break;
                case OPTION_TELEPORT:
                {
                    CloseGossipMenuFor(p);
                    if (p->IsInCombat())
                        return ChatHandler(p->GetSession()).PSendSysMessage("You are unable to teleport while in combat.");

                    if (p->IsInFlight())
                        p->FinishTaxiFlight();
                    else
                        p->SaveRecallPosition();

                    p->TeleportTo(ptr->MapID, ptr->PosX, ptr->PosY, ptr->PosZ, ptr->PosO);
                    break;
                }
                case OPTION_RETURN:
                    AddTeleLocations(p, ptr->ReturnId);
                    SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
                    break;
                }
            }
        }
        return;
    }
};

void AddSC_DynamicTeleporter()
{
    new DynamicTeleportItem();
    //new DynamicTeleportCreature();
}

void TeleSystem::Load()
{
    TC_LOG_INFO("server.loading", "Loading TeleSystem...");
    m_Locations.clear();
    uint32 msStartTime = getMSTime();
    int nCounter = 0;

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_TELEPORT_LOCATIONS);
    PreparedQueryResult res = WorldDatabase.Query(stmt);
    if (res)
    {
        std::set<TeleItem*> lookupTable;
        do
        {
            Field* pField = res->Fetch();
            TeleItem item = { };

            item.ID = pField[0].GetInt32();
            item.A_Icon = pField[1].GetString();
            item.H_Icon = pField[2].GetString();
            item.Name = pField[3].GetString();
            item.Parent = pField[4].GetInt32();
            item.MapID = pField[5].GetUInt32();
            item.PosX = pField[6].GetFloat();
            item.PosY = pField[7].GetFloat();
            item.PosZ = pField[8].GetFloat();
            item.PosO = pField[9].GetFloat();
            item.MinLevel = pField[10].GetUInt8();
            item.MaxLevel = pField[11].GetUInt8();
            item.RequiredFaction = pField[12].GetInt8();
            item.RequiredClass = pField[13].GetInt32();
            item.RequiredQuest = pField[14].GetInt32();
            item.RequiredAchievement = pField[15].GetInt32();
            item.RequiredSpell = pField[16].GetInt32();
            item.RequiredSecurity = pField[17].GetUInt8();
            item.Option = pField[18].GetUInt8();
            item.ReturnId = pField[19].GetInt32();
            item.BoxText = pField[20].GetString();

            auto found = std::find_if(lookupTable.begin(), lookupTable.end(), [&item](auto itemPtr)
                {
                    return itemPtr->ID == item.Parent;
                });

            if (item.Parent == -1)
                m_Locations.emplace(item.ID, std::move(item));
            else if (found != lookupTable.end())
                (*found)->Childs.emplace(item.ID, std::move(item));
            else // Still need to search parent
            {
                bool wasFound = false;
                for (auto& loc : m_Locations)
                {
                    if (auto ptr = loc.second.Search(item.Parent))
                    {
                        wasFound = true;
                        ptr->Childs.emplace(item.ID, std::move(item));
                    }
                }
                if (!wasFound)
                    TC_LOG_WARN("server.loading", "TeleSystem: Item with ID {} has no valid parent({}). Ignoring this item.", item.ID, item.Parent);
            }
            ++nCounter;
        } while (res->NextRow());
    }
    TC_LOG_INFO("server.loading", "Loaded TeleSystem ({} entries) in {} ms", nCounter, GetMSTimeDiffToNow(msStartTime));
}
