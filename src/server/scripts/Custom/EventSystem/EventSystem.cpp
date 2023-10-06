#include <Chat.h>
#include <Log.h>
#include <WorldSession.h>

#include <World.h>
#include "EventSystem.h"
#include <GameTime.h>
#include <DatabaseEnv.h>
#include <ObjectMgr.h>
#include <MapManager.h>

constexpr auto EventString = "|CFF0e1eec[Event Manager]:|CFF6ba3b9 ";

void EventSystem::Update(uint32 diff)
{
    if (!CreateEvents)
        return;

    time_t currenttime = GameTime::GetGameTime();

    if (!Active)
    {
        if (!((currenttime - 1683690300) % 60))
        {
            GlobalAnnounce("Event Starting to Invite. Type .GameEvent Join to join the fun!");
            AllowInvites = true;
            Active = true;
            EventCounter = 9;

            for (uint16 i = 0; i < GameEventInfo.size(); i++)
                EventOptions.push_back(i);

            while (EventOptions.size() > MaxEventOptions)
                EventOptions.erase(EventOptions.begin() + urand(0, EventOptions.size() - 1));
        }
        return;
    };

    UpdateTimer += diff;

    if (AllowInvites)
    {
        if (UpdateTimer > 5000)
        {
            if (EventCounter > 0)
            {

                std::string Message = "Event Starting in " + std::to_string(EventCounter--) + " minutes.";
                GlobalAnnounce(Message, MEMBER_ANNOUNCE);
                GlobalAnnounce(Message + " Type .GameEvent Join to join the fun!", NONMEMBER_ANNOUNCE);
                UpdateTimer = 0;

                if (EventCounter == 2)
                {
                    uint16 BigNum = 0;
                    std::vector<uint16> UpdatedOptions;

                    for (int i = 0; i < EventOptions.size(); i++)
                    {
                        if (EventVoteCounts[i] > BigNum)
                        {
                            BigNum = EventVoteCounts[i];
                            UpdatedOptions.clear();
                            UpdatedOptions.push_back(EventOptions[i]);
                            continue;
                        }
                        if (EventVoteCounts[i] == BigNum)
                            UpdatedOptions.push_back(EventOptions[i]);
                    }
                    uint16 SelectedEventID = urand(0, UpdatedOptions.size() - 1);
                    EventID = UpdatedOptions[SelectedEventID];
                    GlobalAnnounce("Event: " + GameEventInfo[EventID].Name + " has been chosen.", MEMBER_ANNOUNCE);
                }

                return;
            }

            if (EventCounter == 0)
            {
                if (m_PlayerList.size() < GameEventInfo[EventID].MinCharacters)
                {
                    GlobalAnnounce("Not enough players joined, ending event.");
                    EndEvent();
                    return;
                }

                StartEvent();
            }
        }
        return;
    }

    if (UpdateTimer > 60000)
    {
        EndEvent();
        GlobalAnnounce("Event Has Finished.");
    }

}

void EventSystem::GlobalAnnounce(std::string msg, AnnouncementType type)
{
    if (type == MEMBER_ANNOUNCE)
    {
        for (auto& p : m_PlayerList)
            if (p.second.player)
                ChatHandler(p.second.player->GetSession()).SendSysMessage(EventString + msg);
        return;
    }

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, nullptr, nullptr, EventString + msg);
    SessionMap::const_iterator itr;
    for (itr = sWorld->GetAllSessions().begin(); itr != sWorld->GetAllSessions().end(); ++itr)
    {

        if (itr->second &&
            itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld() &&
            ((type == NONMEMBER_ANNOUNCE &&
            !PlayerIsInEvent(itr->second->GetPlayer()))
            || type == GLOBAL_ANNOUNCE))
        {
            itr->second->SendPacket(&data);
        }
    }
}

bool EventSystem::PlayerIsInEvent(Player* p)
{
    if (p)
        if (m_PlayerList.find(p->GetGUID()) != m_PlayerList.end())
            return true;

    return false;
}

std::string EventSystem::GetGameEventNames()
{
    std::string ReturnString;
    for (int i = 0; i < EventOptions.size(); i++)
        ReturnString += "\n" + std::string(EventString) + "Option: " + std::to_string(i + 1) + " [" + GameEventInfo[EventOptions[i]].Name + "]" + " - " + std::to_string(EventVoteCounts[i]);

    return ReturnString;
}

void EventSystem::SendGameEventInfo(Player* p)
{
    std::string OutputString = EventString;
    OutputString += "|r--------------------\n" + std::string(EventString);
    if (!p || !p->IsInWorld())
        return;

    if (Active)
        if (AllowInvites)
        {
            OutputString += "Event is inviting players: " + std::to_string(m_PlayerList.size()) + ". Starting in " + std::to_string(EventCounter) + " minutes.\n" + EventString;
            if (EventID == -1)
                OutputString += "Voting on the following options:" + GetGameEventNames();
            else
                OutputString += "Event: " + GameEventInfo[EventID].Name;
        }
        else
            OutputString += "Event is in progress.";
    else
        OutputString += "Currently there isn't an ongoing event.";

    OutputString += "\n" + std::string(EventString) + "|r--------------------";
    ChatHandler(p->GetSession()).SendSysMessage(OutputString);
}

void EventSystem::InvitePlayer(Player* p)
{
    if (!p || !p->IsInWorld())
        return;
    if (PlayerIsInEvent(p))
        return ChatHandler(p->GetSession()).PSendSysMessage("%sYou already joined the event.", EventString);
    if (!AllowInvites)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sEvent is not inviting players currently.", EventString);
    if (p->InBattlegroundQueue() || p->InBattleground() || p->InArena())
        return ChatHandler(p->GetSession()).PSendSysMessage("%sUnable to join, when queued for or in a battleground.", EventString);
    if (p->isUsingLfg())
        return ChatHandler(p->GetSession()).PSendSysMessage("%sUnable to join, when queued for or in a lfg dungeon.", EventString);

    PlayerEventInfo info;
    info.player = p;

    if (EventID == -1)
        ChatHandler(p->GetSession()).PSendSysMessage("%sVote for the Event, by typing \".GameEvent Vote <ID>\"%s", EventString, GetGameEventNames());
    else
        ChatHandler(p->GetSession()).PSendSysMessage("%sYou were able to join the Event.", EventString);

    m_PlayerList.emplace(p->GetGUID(), info);
}

void EventSystem::KickPlayer(Player* p, bool outputMsg)
{
    if (!p || !p->IsInWorld())
        return;
    if (!PlayerIsInEvent(p))
        if (outputMsg)
            return ChatHandler(p->GetSession()).PSendSysMessage("%sYou haven't joined the event.", EventString);
        else
            return;

    auto& info = m_PlayerList[p->GetGUID()];

    if (EventID == -1 && info.VoteID >= 0)
        if (EventVoteCounts[info.VoteID] > 0)
            EventVoteCounts[info.VoteID]--;

    p->TeleportTo(info.OriginLocation);

    m_PlayerList.erase(p->GetGUID());
    if (outputMsg)
        ChatHandler(p->GetSession()).PSendSysMessage("%sYou have been removed from the event.", EventString);
}

void EventSystem::HandlePlayerVote(Player* p, uint16 voteID)
{
    voteID--;
    if (!p || !p->IsInWorld())
        return;
    if (!Active)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sCurrently there isn't an ongoing event.", EventString);

    if (!PlayerIsInEvent(p))
        return ChatHandler(p->GetSession()).PSendSysMessage("%sYou must join the event in order to vote.", EventString);
    if (EventCounter <= 2)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sEvent voting has finished.", EventString);
    if (voteID >= MaxEventOptions)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sIncorrect vote ID.", EventString);

    auto &info = m_PlayerList[p->GetGUID()];

    if (voteID == info.VoteID)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sYou already voted for Option %u.", EventString, voteID);

    if (info.VoteID >= 0)
        EventVoteCounts[info.VoteID]--;
    EventVoteCounts[voteID]++;
    info.VoteID = voteID;

    return ChatHandler(p->GetSession()).PSendSysMessage("%sYou successfully voted for Option: %u.", EventString, voteID+1);
}

void EventSystem::UpdateAllowEventCreation(bool val)
{
    CreateEvents = val;
}

void EventSystem::StartEvent()
{
    GlobalAnnounce("Event has started, no longer inviting players.");
    AllowInvites = false;
    auto &Info = GameEventInfo[EventID];
    WorldLocation Loc;
    Loc.m_mapId = Info.MapID;
    Loc.m_positionX = Info.Pos_X;
    Loc.m_positionY = Info.Pos_Y;
    Loc.m_positionZ = Info.Pos_Z;
    Loc.SetOrientation(Info.Pos_O);

    for (auto& p : m_PlayerList)
    {
        if (p.second.player)
        {
            p.second.OriginLocation = p.second.player->GetWorldLocation();

            if (Info.Flags & EVENT_FLAG_ALLOW_RESPAWN)
                p.second.RespawnLocation = Loc;
            if (Info.Flags & EVENT_FLAG_ALLOW_TELEPORT)
                p.second.TeleportLocation = Loc;

            p.second.player->TeleportTo(Loc, 0);
        }
        else
            KickPlayer(p.second.player, false);
    }

    HandleModifyCreatureFlags();
}

void EventSystem::EndEvent()
{
    Active = false;
    HandleModifyCreatureFlags();

    AllowInvites = false;
    UpdateTimer = 0;
    EventCounter = 0;
    EventOptions.clear();
    Placements.clear();
    for (int i = 0; i < MaxEventOptions; i++)
        EventVoteCounts[i] = 0;
    EventID = -1;
    for (auto& i : m_PlayerList)
        KickPlayer(i.second.player, false);

    m_PlayerList.clear();
}

void EventSystem::HandleEventCreatureGossip(Player* p, Creature* creature)
{
    if (Active && PlayerIsInEvent(p) && EventCounter == 0)
    {
        auto& EventCreatureGUIDs = GameEventInfo[EventID].CreatureGUIDS;

        for (int i = 0; i < EventCreatureGUIDs.size(); i++)
        {
            if (creature->GetSpawnId() == EventCreatureGUIDs[i])
            {
                auto& info = m_PlayerList[p->GetGUID()];

                if (info.Placement == 0 && info.TeleCreatureID < i)
                {
                    info.TeleportLocation = creature->GetWorldLocation();
                    info.TeleCreatureID = i;

                    if (i == EventCreatureGUIDs.size() - 1)
                    {
                        // Player Won
                        uint16 PlaceValue = Placements.size() + 1;
                        static const char suffixes[][3] = { "th", "st", "nd", "rd" };
                        auto ord = PlaceValue % 100;
                        if (ord / 10 == 1) { ord = 0; }
                        ord = ord % 10;
                        if (ord > 3) { ord = 0; }
                        std::string AnnString = p->GetName() + " has finished the event in " + std::to_string(PlaceValue) + suffixes[ord] + " place";
                        info.Placement = PlaceValue;
                        GlobalAnnounce(AnnString, MEMBER_ANNOUNCE);
                        Placements.push_back(p->GetName());
                    }
                    else
                        ChatHandler(p->GetSession()).SendSysMessage("Your teleport location has been updated.");
                }

                return;
            }
        }
    }
}

void EventSystem::HandleEventTeleport(Player* p)
{
    if (!p || !p->IsInWorld())
        return;
    if (!Active)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sCurrently there isn't an ongoing event.", EventString);
    if (!PlayerIsInEvent(p))
        return ChatHandler(p->GetSession()).PSendSysMessage("%sYou are not in the event.", EventString);
    if (EventCounter > 0)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sEvent hasn't started yet.", EventString);

    auto& info = m_PlayerList[p->GetGUID()];

    if (GameEventInfo[EventID].Flags & EVENT_FLAG_ALLOW_TELEPORT)
        p->TeleportTo(info.TeleportLocation);
    else
        return ChatHandler(p->GetSession()).PSendSysMessage("%sYou are unable to teleport in this event.", EventString);
}

void EventSystem::HandleEventRespawn(Player* p)
{
    if (!p || !p->IsInWorld())
        return;
    if (!Active)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sCurrently there isn't an ongoing event.", EventString);
    if (!PlayerIsInEvent(p))
        return ChatHandler(p->GetSession()).PSendSysMessage("%sYou are not in the event.", EventString);
    if (EventCounter > 0)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sEvent hasn't started yet.", EventString);

    auto& info = m_PlayerList[p->GetGUID()];

    if (GameEventInfo[EventID].Flags & EVENT_FLAG_ALLOW_RESPAWN)
        if (!p->IsAlive())
        {
            p->ResurrectPlayer(100.0f, false);
            p->TeleportTo(info.RespawnLocation);
        }
        else
            return ChatHandler(p->GetSession()).PSendSysMessage("%sYou are alive.", EventString);
    else
        return ChatHandler(p->GetSession()).PSendSysMessage("%sYou are unable to respawn in this event.", EventString);

}

void EventSystem::HandleModifyCreatureFlags()
{
    if (EventID < 0 || EventID > GameEventInfo.size())
        return;

    std::unordered_map<uint32, std::unordered_set<ObjectGuid::LowType>> creaturesByMap;

    for (auto& GUID : GameEventInfo[EventID].CreatureGUIDS)
        if (CreatureData const* data = sObjectMgr->GetCreatureData(GUID))
            creaturesByMap[data->mapId].insert(GUID);

    for (auto const& p : creaturesByMap)
    {
        sMapMgr->DoForAllMapsWithMapId(p.first, [this, &p](Map* map)
            {
                for (auto& spawnId : p.second)
                {
                    auto creatureBounds = map->GetCreatureBySpawnIdStore().equal_range(spawnId);
                    for (auto &itr = creatureBounds.first; itr != creatureBounds.second; ++itr)
                    {
                        Creature* creature = itr->second;
                        creature->ReplaceAllNpcFlags(NPCFlags(IsActive() ? 1 : 0));
                    }
                }
            });
    }
}

bool EventSystem::IsActive()
{
    return Active;
}

bool EventSystem::IsEnabled()
{
    return CreateEvents;
}

void EventSystem::Load()
{
    TC_LOG_INFO("server.loading", "Loading GameEvent Information...");
    GameEventInfo.clear();
    uint32 msStartTime = getMSTime();
    int nCounter = 0;

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_GAMEEVENT_INFORMATION);
    PreparedQueryResult res = WorldDatabase.Query(stmt);
    if (res)
    {
        do
        {
            Field* pField = res->Fetch();
            GameEventInformation item = { 0 };

            item.ID = pField[0].GetInt32();
            item.Name = pField[1].GetString();
            item.MapID = pField[2].GetUInt16();
            item.Pos_X = pField[3].GetFloat();
            item.Pos_Y = pField[4].GetFloat();
            item.Pos_Z = pField[5].GetFloat();
            item.Pos_O = pField[6].GetFloat();
            item.Flags = pField[7].GetUInt16();
            std::string cGUIDs = pField[8].GetString();
            item.MinCharacters = pField[9].GetUInt16();
            std::string GUID;

            if (cGUIDs.size() > 0)
            {
                for (auto& c : cGUIDs)
                {
                    if (c == ' ')
                    {
                        item.CreatureGUIDS.push_back(std::atoi(GUID.c_str()));
                        GUID.clear();
                    }
                    else
                        GUID += c;
                }
                item.CreatureGUIDS.push_back(std::atoi(GUID.c_str()));
            }

            GameEventInfo.push_back(item);
            ++nCounter;
        } while (res->NextRow());
    }
    TC_LOG_INFO("server.loading", "Loaded GameEvent System ({} entries) in {} ms", nCounter, GetMSTimeDiffToNow(msStartTime));
}
