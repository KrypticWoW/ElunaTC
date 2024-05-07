#include "MallArena.h"
#include <DatabaseEnv.h>
#include <ScriptedGossip.h>
#include <GameObject.h>
#include <Chat.h>
#include <WorldSession.h>

void MallArenaSystem::AddResult(std::string str)
{
    if (l_PastResults.size() >= 10)
        l_PastResults.pop_front();

    l_PastResults.push_back(str);
}

uint32 MallArenaSystem::GetCooldown(uint32 GUID)
{
    if (m_ChallengeCooldown.find(GUID) != m_ChallengeCooldown.end())
        return m_ChallengeCooldown[GUID];

    return 0;
}

void MallArenaSystem::PhaseObjects(bool phaseOut)
{
    for (uint32& itr : v_GateGameobjects)
        if (GameObject* obj = ArenaChallengeCreature->GetMap()->GetGameObjectBySpawnId(itr))
            obj->SetPhaseMask(phaseOut ? 0 : 1, true);
}

void MallArenaSystem::Update(uint32 diff)
{
    for (auto itr = m_ChallengeCooldown.begin(); itr != m_ChallengeCooldown.end();)
    {
        if (itr->second <= diff)
            itr = m_ChallengeCooldown.erase(itr);
        else
        {
            itr->second -= diff;
            ++itr;
        }
    }

    if (state > ARENA_STATE_INACTIVE)
    {
        // Check if players exist
        if (!Member_A->p ||! Member_B->p)
        {
            EndDuel(REASON_DISCONNECT, 0);
            return;
        }
    }

    if (Timer[TIMER_TYPE_CHALLENGE] > 0)
    {
        if (Timer[TIMER_TYPE_CHALLENGE] > diff)
            Timer[TIMER_TYPE_CHALLENGE] -= diff;
        else
        {
            Timer[TIMER_TYPE_CHALLENGE] = 0;
            CloseGossipMenuFor(Member_B->p);
            DeclineChallenge();
        }
        return;
    }

    if (Timer[TIMER_TYPE_TELEPORT] > 0)
    {
        if (Timer[TIMER_TYPE_TELEPORT] > diff)
        {
            if (Timer[TIMER_TYPE_TELEPORT] > 5000)
                if (Timer[TIMER_TYPE_TELEPORT] / 1000 > (Timer[TIMER_TYPE_TELEPORT] - diff) / 1000)
                {
                    uint32 TeleportTime = (999 + Timer[TIMER_TYPE_TELEPORT] - diff) / 1000;
                    ChatHandler(Member_A->p->GetSession()).SendNotify("You will be Teleported in %u seconds!", TeleportTime);
                    ChatHandler(Member_B->p->GetSession()).SendNotify("You will be Teleported in %u seconds!", TeleportTime);
                }
            Timer[TIMER_TYPE_TELEPORT] -= diff;
        }
        else
        {
            Timer[TIMER_TYPE_TELEPORT] = 0;
            Timer[TIMER_TYPE_START] = 10000;

            Member_A->p->TeleportTo(0, 4249.722168f, -2851.923340f, 8.834438f, 1.498352f);
            Member_B->p->TeleportTo(0, 4250.012695f, -2714.421387f, 8.838498f, 4.690998f);
        }

        return;
    }

    if (Timer[TIMER_TYPE_START] > 0)
    {
        if (Timer[TIMER_TYPE_START] > diff)
        {
            if (Timer[TIMER_TYPE_START] > 5000)
                if (Timer[TIMER_TYPE_TELEPORT] / 1000 > (Timer[TIMER_TYPE_TELEPORT] - diff) / 1000)
                {
                    uint32 StartTime = (999 + Timer[TIMER_TYPE_TELEPORT] - diff) / 1000;
                    ChatHandler(Member_A->p->GetSession()).SendNotify("Duel starting in %u seconds!", StartTime);
                    ChatHandler(Member_B->p->GetSession()).SendNotify("Duel starting in %u seconds!", StartTime);
                }
            Timer[TIMER_TYPE_START] -= diff;
        }
        else
        {
            Timer[TIMER_TYPE_START] = 0;
            Timer[TIMER_TYPE_DEFAULT] = 3000000;

            PhaseObjects(true);
            Member_A->p->PlayDirectSound(8232, Member_A->p);
            Member_B->p->PlayDirectSound(8232, Member_B->p);
        }
        return;
    }

    if (Timer[TIMER_TYPE_DEFAULT] > 0)
    {
        if (Member_A->p->isDead() && Member_B->p->isDead())
            return EndDuel(REASON_COMPLETE, 0);
        if (Member_B->p->isDead())
            return EndDuel(REASON_COMPLETE, 1);
        if (Member_A->p->isDead())
            return EndDuel(REASON_COMPLETE, 2);

        if (Timer[TIMER_TYPE_DEFAULT] > diff)
            Timer[TIMER_TYPE_DEFAULT] -= diff;
        else
            return EndDuel(REASON_COMPLETE, 0);
    }
}

void MallArenaSystem::ChallengePlayer(Player* challenger, Player* opponent, bool makgora, uint32 gold)
{
    if (makgora)
        state = ARENA_STATE_CHALLENGE_MAKGORA;
    else
        state = ARENA_STATE_CHALLENGE_DUEL;

    Member_A = new ArenaPlayerInfo(challenger);
    Member_B = new ArenaPlayerInfo(opponent);
    WagerAmt = gold * 10000;

    m_ChallengeCooldown.emplace(challenger->GetGUID(), 30000);
    Timer[TIMER_TYPE_CHALLENGE] = 30000;
}

void MallArenaSystem::AcceptChallenge()
{
    if (WagerAmt > 0)
    {
        if (Member_A->p->HasEnoughMoney(WagerAmt) && Member_B->p->HasEnoughMoney(WagerAmt))
        {
            Member_A->p->ModifyMoney(-WagerAmt);
            Member_B->p->ModifyMoney(-WagerAmt);
        }
        else
            return DeclineChallenge();
    }

    Timer[TIMER_TYPE_CHALLENGE] = 0;
    Timer[TIMER_TYPE_TELEPORT] = 10000;

    if (state == ARENA_STATE_CHALLENGE_DUEL)
        state = ARENA_STATE_DUELING;
    if (state == ARENA_STATE_CHALLENGE_MAKGORA)
        state = ARENA_STATE_MAKGORA;

    std::string msg = Member_B->name + " has accepted " + Member_A->name + "'s challenge to " + (state == ARENA_STATE_DUELING ? "a duel!" : "Makgora!");
    ArenaChallengeCreature->Yell(msg, LANG_COMMON);
}

void MallArenaSystem::DeclineChallenge()
{
    state = ARENA_STATE_INACTIVE;
    delete Member_A;
    Member_A = nullptr;
    delete Member_B;
    Member_B = nullptr;
    Timer[TIMER_TYPE_CHALLENGE] = 0;
}

void MallArenaSystem::EndDuel(DuelCompleteReason reason, uint16 winner)
{
    bool bMakGora = state == ARENA_STATE_MAKGORA;

    HandleRessurection(reason, winner);

    switch (reason)
    {

    case REASON_DISCONNECT:
    {
        if (state == ARENA_STATE_DUELING || bMakGora)
        {
            if (!Member_A->p && !Member_B->p)
                winner = 0;
            else if (!Member_B->p)
                winner = 1;
            else if (!Member_A->p)
                winner = 2;
        }
        else
            winner = 0;
    } break;

    case REASON_COMPLETE:
    {
        switch (winner)
        {
        case 0:
            AddResult(Member_A->name + " Vs " + Member_B->name + " - Tied.");
            break;
        case 1:
            AddResult(Member_A->name + " Vs " + Member_B->name + " - " + Member_A->name + ".");
            break;
        case 2:
            AddResult(Member_A->name + " Vs " + Member_B->name + " - " + Member_B->name + ".");
            break;
        }
    } break;

    }

    if (state >= ARENA_STATE_DUELING)
    {
        if (Member_A->p)
            Member_A->p->TeleportTo(0, 4250.052246f, -2872.312012f, 9.249156f, 4.686810f);
        else
        {
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_POSITION);

            stmt->setFloat(0, 4250.052246f);
            stmt->setFloat(1, -2872.312012f);
            stmt->setFloat(2, 9.249156f);
            stmt->setFloat(3, 4.686810f);
            stmt->setUInt16(4, uint16(0));
            stmt->setUInt16(5, 2037);
            stmt->setUInt32(6, Member_A->guid);

            CharacterDatabase.Execute(stmt);
        }
        if (Member_B->p)
            Member_B->p->TeleportTo(0, 4250.265137f, -2690.220947f, 9.069143f, 1.631879f);
        else
        {
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_POSITION);

            stmt->setFloat(0, 4250.265137f);
            stmt->setFloat(1, -2690.220947f);
            stmt->setFloat(2, 9.069143f);
            stmt->setFloat(3, 1.631879f);
            stmt->setUInt16(4, uint16(0));
            stmt->setUInt16(5, 2037);
            stmt->setUInt32(6, Member_B->guid);

            CharacterDatabase.Execute(stmt);
        }
    }

    HandleWager(winner);

    PhaseObjects(false);
    for (uint16 i = TIMER_TYPE_DEFAULT; i < TIMER_TYPE_MAX; i++)
        Timer[i] = 0;

    delete Member_A;
    Member_A = nullptr;
    delete Member_B;
    Member_B = nullptr;
    state = ARENA_STATE_INACTIVE;
}

void MallArenaSystem::HandleWager(uint16 winner)
{
    if (WagerAmt == 0)
        return;

    switch (winner)
    {

    case 0:
    {
        if (Member_A->p)
            Member_A->p->ModifyMoney(WagerAmt);
        else
            SendWagerByMail();

        if (Member_B->p)
            Member_B->p->ModifyMoney(WagerAmt);
        else
            SendWagerByMail();
    } break;

    case 1:
    {
        if (Member_A->p)
            Member_A->p->ModifyMoney(WagerAmt * 2);
        else
            SendWagerByMail();
    } break;

    case 2:
    {
        if (Member_B->p)
            Member_B->p->ModifyMoney(WagerAmt * 2);
        else
            SendWagerByMail();
    } break;

    }
}

void MallArenaSystem::SendWagerByMail()
{
}

void MallArenaSystem::HandleDisconnect(Player* p)
{
    if (state >= ARENA_STATE_CHALLENGE_DUEL)
    {
        if (Member_A->p == p)
            Member_A->p = nullptr;
        if (Member_B->p == p)
            Member_B->p = nullptr;
    }
}

void MallArenaSystem::HandleRessurection(DuelCompleteReason reason, uint16 winner)
{
    bool bMakGora = state == ARENA_STATE_MAKGORA;

    if (!bMakGora)
    {
        Member_A->p->ResurrectPlayer(100.0f);
        Member_B->p->ResurrectPlayer(100.0f);
    }
    else
    {
        if (reason != REASON_DISCONNECT)
        {
            switch (winner)
            {
            case 0:
                Member_A->p->AddAura(47440, ArenaChallengeCreature);
                Member_B->p->AddAura(47440, ArenaChallengeCreature);
                break;
            case 1:
                Member_B->p->AddAura(47440, ArenaChallengeCreature);
                Member_B->p->ResurrectPlayer(100.0f);
                break;
            case 2:
                Member_A->p->AddAura(47440, ArenaChallengeCreature);
                Member_A->p->ResurrectPlayer(100.0f);
                break;
            }
        }
        else
        {

        }
    }
}
