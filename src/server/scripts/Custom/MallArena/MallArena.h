#ifndef MALLARENA_H
#define MALLARENA_H

constexpr uint32 ArenaChallengeMenu = 999;

enum ArenaChallengeState : uint16
{
    ARENA_STATE_INACTIVE = 0,
    ARENA_STATE_CHALLENGE_DUEL,
    ARENA_STATE_CHALLENGE_MAKGORA,
    ARENA_STATE_DUELING,
    ARENA_STATE_MAKGORA
};

enum DuelCompleteReason
{
    REASON_DISCONNECT = 0,
    REASON_COMPLETE,
    REASON_UNKNOWN
};

enum ArenaTimerType : uint16
{
    TIMER_TYPE_DEFAULT = 0,
    TIMER_TYPE_TEMP,
    TIMER_TYPE_CHALLENGE,
    TIMER_TYPE_TELEPORT,
    TIMER_TYPE_START,
    TIMER_TYPE_MAX
};

struct ArenaPlayerInfo
{
    ArenaPlayerInfo(Player* plr) { p = plr; name = plr->GetName(); guid = plr->GetGUID(); }
    Player* p;
    std::string name;
    uint32 guid;
};

class MallArenaSystem
{
    MallArenaSystem() { }

public:
    static MallArenaSystem& instance()
    {
        static MallArenaSystem mallsys;
        return mallsys;
    }

    void AddResult(std::string str);

    bool AllowMakgora() { return MakGora; }
    std::list<std::string>& GetResults() { return l_PastResults; }
    uint32 GetTimer(ArenaTimerType type) { return Timer[type]; }
    uint32 GetListSize() { return l_PastResults.size(); }
    ArenaChallengeState GetArenaState() { return state; }
    std::string GetMemberName(bool challenger) { if (challenger) return Member_A->name; return Member_B->name; } // Member_A = challenger, Member_B = opponent
    Player* GetPlayer(bool challenger) { if (challenger) if (Member_A) return Member_A->p; else return nullptr; if (Member_B) return Member_B->p; else return nullptr; } // Member_A = challenger, Member_B = opponent
    uint32 GetCooldown(uint32 GUID);

    void SetArenaCreature(Creature* cr) { ArenaChallengeCreature = cr; }
    void PhaseObjects(bool phaseOut);
    void Update(uint32 diff);

    void ChallengePlayer(Player* challenger, Player* opponent, bool makgora, uint32 gold);
    void AcceptChallenge();
    void DeclineChallenge();
    void EndDuel(DuelCompleteReason reason, uint16 winner); // 0 = tie, 1 = challenger, 2 = opponent
    void HandleWager(uint16 winner);
    void SendWagerByMail();
    void HandleDisconnect(Player* p);
    void HandleRessurection(DuelCompleteReason reason, uint16 winner);

private:
    bool MakGora = true;
    std::list<std::string> l_PastResults;
    std::vector<uint32> v_GateGameobjects = { 302204, 302205, 302229, 302230 };
    Creature* ArenaChallengeCreature = nullptr;

    ArenaPlayerInfo* Member_A = nullptr;
    ArenaPlayerInfo* Member_B = nullptr;
    int32 WagerAmt = 0;

    uint32 Timer[TIMER_TYPE_MAX] = { 0 };
    ArenaChallengeState state = ARENA_STATE_INACTIVE;

    std::map<uint32, uint32> m_ChallengeCooldown;
};

#define sMAS MallArenaSystem::instance()

#endif
