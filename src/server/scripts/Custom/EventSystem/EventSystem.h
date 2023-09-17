#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

constexpr uint16 MaxEventOptions = 3;

enum AnnouncementType
{
    GLOBAL_ANNOUNCE = 0,
    MEMBER_ANNOUNCE,
    NONMEMBER_ANNOUNCE
};

struct PlayerEventInfo
{
    Player* player = nullptr;
    int16 VoteID = -1;

    WorldLocation* RespawnLocation = nullptr;
    WorldLocation* TeleportLocation = nullptr;
    WorldLocation* OriginLocation = nullptr;
};

class EventSystem
{
    EventSystem() {}

public:
    static EventSystem& instance()
    {
        static EventSystem eventSys;
        return eventSys;
    }
    void Update(uint32 diff);
    void GlobalAnnounce(std::string msg, AnnouncementType type = GLOBAL_ANNOUNCE);
    bool PlayerIsInEvent(Player* p);
    std::string GetGameEventNames();
    void SendGameEventInfo(Player* p);
    void InvitePlayer(Player* p);
    void KickPlayer(Player* p, bool outputMsg);
    void HandlePlayerVote(Player* p, uint16 voteId);
    void UpdateAllowEventCreation(bool val);
    void EndEvent();

    bool IsActive();
    bool IsEnabled();

private:
    bool CreateEvents = true;
    uint32 UpdateTimer = 0;
    uint32 EventCounter = 0;
    int16 EventID = -1;
    uint16 EventVoteCounts[MaxEventOptions] = { 0 };
    bool Active = false;
    bool AllowInvites = false;
    std::vector<std::string> Placements;
    std::vector<uint16> EventOptions;
    bool AllowRespawn = false;
    bool AllowTeleport = false;
    uint16 MinPlayerCount = 1;
    uint16 MaxPlayerCount = 40;

    std::map<uint32, PlayerEventInfo> m_PlayerList;
};

#define sEventSystem EventSystem::instance()

#endif
