#include <Chat.h>
#include <Log.h>
#include <WorldSession.h>

#include <World.h>
#include "EventSystem.h"
#include <GameTime.h>

constexpr auto EventString = "|CFF0e1eec[Event Manager]:|CFF6ba3b9 ";

void EventSystem::Update(uint32 diff)
{
    if (!CreateEvents)
        return;

    time_t currenttime = GameTime::GetGameTime();

    if (!Active)
    {
        if (!((currenttime - 1683690300) % 600))
        {
            GlobalAnnounce("Event Starting to Invite. Type .GameEvent Join to join the fun!");
            AllowInvites = true;
            Active = true;
            EventCounter = 9;

            EventOptions = { 0,1,2,3,4,5,6,7,8,9 };
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
                GlobalAnnounce("Event: " + std::to_string(EventID) + " has been chosen.", MEMBER_ANNOUNCE);
            }
        }

        if (EventCounter == 0)
        {
            if (m_PlayerList.size() < MinPlayerCount)
            {
                GlobalAnnounce("Not enough players joined, ending event.");
                EndEvent();
                return;
            }

            GlobalAnnounce("Event has started, no longer inviting players.");
            AllowInvites = false;
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
    {
        ReturnString += "\n" + std::string(EventString) + "Option " + std::to_string(i + 1);

        switch (EventOptions[i])
        {
        case 0: ReturnString += ": 0 [" + std::to_string(EventVoteCounts[i]) + "]"; break;
        case 1: ReturnString += ": 1 [" + std::to_string(EventVoteCounts[i]) + "]"; break;
        case 2: ReturnString += ": 2 [" + std::to_string(EventVoteCounts[i]) + "]"; break;
        case 3: ReturnString += ": 3 [" + std::to_string(EventVoteCounts[i]) + "]"; break;
        case 4: ReturnString += ": 4 [" + std::to_string(EventVoteCounts[i]) + "]"; break;
        case 5: ReturnString += ": 5 [" + std::to_string(EventVoteCounts[i]) + "]"; break;
        case 6: ReturnString += ": 6 [" + std::to_string(EventVoteCounts[i]) + "]"; break;
        case 7: ReturnString += ": 7 [" + std::to_string(EventVoteCounts[i]) + "]"; break;
        case 8: ReturnString += ": 8 [" + std::to_string(EventVoteCounts[i]) + "]"; break;
        case 9: ReturnString += ": 9 [" + std::to_string(EventVoteCounts[i]) + "]"; break;
        }
    }
    return ReturnString;
}

void EventSystem::SendGameEventInfo(Player* p)
{
    std::string OutputString = EventString;
    if (!p || !p->IsInWorld())
        return;

    if (Active)
        if (AllowInvites)
        {
            OutputString += "--------------------\n" + std::string(EventString);
            OutputString += "Event is inviting players: " + std::to_string(m_PlayerList.size()) + ". Starting in " + std::to_string(EventCounter) + " minutes.\n" + EventString;
            if (EventID == -1)
                OutputString += "Voting on the following options:" + GetGameEventNames();
            else
                OutputString += "Event: Name";
        }
        else
            OutputString += "Event is in progress.";
    else
        OutputString += "Currently there isn't an ongoing event.";

    OutputString += std::string(EventString) + "\n--------------------";
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

    PlayerEventInfo info;
    info.player = p;

    if (EventID == -1)
        ChatHandler(p->GetSession()).PSendSysMessage("%s Vote for the Event, by typing \".GameEvent Vote <ID>\"%s", EventString, GetGameEventNames());

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
    delete info.RespawnLocation;
    delete info.TeleportLocation;

    if (EventID == -1 && info.VoteID >= 0)
        EventVoteCounts[info.VoteID]--;

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
    if (EventID != -1)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sEvent voting has finished.", EventString);
    if (voteID >= MaxEventOptions)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sIncorrect vote ID.", EventString);

    auto &info = m_PlayerList[p->GetGUID()];

    if (voteID == info.VoteID)
        return ChatHandler(p->GetSession()).PSendSysMessage("%sYou already voted for Option %u.", EventString, voteID);

    if (info.VoteID >= 0)
        EventVoteCounts[info.VoteID] -= 1;
    EventVoteCounts[voteID]++;
    info.VoteID = voteID;

    return ChatHandler(p->GetSession()).PSendSysMessage("%sYou successfully voted for Option: %u.", EventString, voteID+1);
}

void EventSystem::UpdateAllowEventCreation(bool val)
{
    CreateEvents = val;
}

void EventSystem::EndEvent()
{
    Active = false;
    AllowInvites = false;
    UpdateTimer = 0;
    EventCounter = 0;
    EventOptions.clear();
    Placements.clear();
    for (int i = 0; i < MaxEventOptions; i++)
        EventVoteCounts[i] = 0;
    EventID = -1;
    for (auto& i : m_PlayerList)
    {
        i.second.VoteID = -1;
        KickPlayer(i.second.player, false);
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
