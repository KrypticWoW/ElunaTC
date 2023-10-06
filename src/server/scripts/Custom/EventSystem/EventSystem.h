#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

constexpr uint16 MaxEventOptions = 3;

enum AnnouncementType
{
    GLOBAL_ANNOUNCE = 0,
    MEMBER_ANNOUNCE,
    NONMEMBER_ANNOUNCE
};

enum EventFlags
{
    EVENT_FLAG_ALLOW_RESPAWN  = 0x001,
    EVENT_FLAG_ALLOW_TELEPORT = 0x002
};

struct PlayerEventInfo
{
    Player* player = nullptr;
    int16 VoteID = -1;
    uint16 Placement = 0;
    int16 TeleCreatureID = -1;

    WorldLocation RespawnLocation;
    WorldLocation TeleportLocation;
    WorldLocation OriginLocation;
};

struct GameEventInformation
{
    uint16 ID;
    std::string Name;
    uint16 MapID;
    float Pos_X;
    float Pos_Y;
    float Pos_Z;
    float Pos_O;
    uint16 Flags;
    std::vector<uint32> CreatureGUIDS;
    uint16 MinCharacters;
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
    void StartEvent();
    void EndEvent();
    void HandleEventCreatureGossip(Player* p, Creature* creature);
    void HandleModifyCreatureFlags();
    void HandleEventTeleport(Player* p);
    void HandleEventRespawn(Player* p);

    bool IsActive();
    bool IsEnabled();

    void Load();

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
    std::vector<GameEventInformation> GameEventInfo;
    bool AllowRespawn = false;
    bool AllowTeleport = false;
    uint16 MaxPlayerCount = 40;

    std::map<uint32, PlayerEventInfo> m_PlayerList;
};

#define sEventSystem EventSystem::instance()

#endif
