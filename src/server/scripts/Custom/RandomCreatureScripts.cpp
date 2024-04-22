#include <World.h>
#include <Spell.h>
#include <Group.h>
#include <ScriptedGossip.h>
#include <WorldSession.h>
#include <Chat.h>
#include <DatabaseEnv.h>
#include <GameObject.h>

class ArenaChallengerCreature : public CreatureScript
{
public:
    ArenaChallengerCreature() : CreatureScript("ArenaChallengerCreature") {}

    enum ArenaChallengeGossip
    {
        GOSSIP_CHALLENGE_DUEL = 0,
        GOSSIP_CHALLENGE,
        GOSSIP_CHALLENGE_MAKGORA,
        GOSSIP_CHALLENGE_REPLY_YES,
        GOSSIP_CHALLENGE_REPLY_NO,
        GOSSIP_VIEW_RESULTS,
        GOSSIP_RESULTS,
        GOSSIP_BACK,
        GOSSIP_EXIT,

        GOSSIP_REPLY = 99
    };

    enum ArenaChallengeState : uint16
    {
        ARENA_STATE_INACTIVE = 0,
        ARENA_STATE_CHALLENEGE_DUEL,
        ARENA_STATE_CHALLENEGE_MAKGORA,
        ARENA_STATE_DUELING,
        ARENA_STATE_MAKGORA
    };

    enum DuelCompleteReason
    {
        REASON_DISCONNECT = 0,
        REASON_COMPLETE,
        REASON_UNKNOWN
    };

    struct ArenaChallengerCreature_AI : public ScriptedAI
    {
        ArenaChallengerCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        void JustAppeared() override
        {
            me->setActive(true);
        }

        bool ChallengePlayer(Player* challenger, Player* opponent, bool makgora, uint32 gold)
        {
            ArenaState = ARENA_STATE_CHALLENEGE_DUEL;
            ArenaMember_A = challenger;
            CharGUID_A = challenger->GetGUID();
            ArenaMember_B = opponent;
            CharGUID_B = opponent->GetGUID();
            ChallengeTimer = 30000;

            ClearGossipMenuFor(opponent);
            InitGossipMenuFor(opponent, 0);
            AddGossipItemFor(opponent, GOSSIP_ICON_TRAINER, challenger->GetName() + " has challenged you to a duel." + (gold ? " Betting " + std::to_string(gold) + " Gold." : ""), GOSSIP_EXIT, 0, "Do you accept this challenge?.", 0, false);
            AddGossipItemFor(opponent, GOSSIP_ICON_TRAINER, "I accept the challenge", GOSSIP_CHALLENGE_REPLY_YES, me->GetEntry());
            AddGossipItemFor(opponent, GOSSIP_ICON_TRAINER, "I decline the challenge", GOSSIP_CHALLENGE_REPLY_NO, me->GetEntry());
            SendGossipMenuFor(opponent, DEFAULT_GOSSIP_MESSAGE, opponent->GetGUID());
            return true;
        }

        void DeclineChallenge()
        {
            ArenaState = ARENA_STATE_INACTIVE;
            ArenaMember_A = nullptr;
            ArenaMember_B = nullptr;
            CharGUID_A = 0;
            CharGUID_B = 0;
            ChallengeTimer = 0;
        }

        void AcceptChallenge()
        {
            ChallengeTimer = 0;
            TeleportTimer = 10000;

            if (ArenaState == ARENA_STATE_CHALLENEGE_DUEL)
                ArenaState = ARENA_STATE_DUELING;
            if (ArenaState == ARENA_STATE_CHALLENEGE_MAKGORA)
                ArenaState = ARENA_STATE_MAKGORA;

            std::string ChallengeMessage = ArenaMember_B->GetName() + " has accepted " + ArenaMember_A->GetName() + "'s challenge to " + (ArenaState == ARENA_STATE_DUELING ? "a duel!" : "Makgora!");
            me->Yell(ChallengeMessage, LANG_COMMON);
        }

        void EndDuel(DuelCompleteReason reason, uint16 winner)
        {
            switch (reason)
            {

            case REASON_DISCONNECT:
            {
                bool bMakGora = ArenaState == ARENA_STATE_MAKGORA;

                if (ArenaState == ARENA_STATE_DUELING || bMakGora)
                {
                    if (!ArenaMember_A && !ArenaMember_B)
                        winner = 0;
                    else if (!ArenaMember_B)
                        winner = 1;
                    else if (!ArenaMember_A)
                        winner = 2;
                }
                else
                {

                }
            }

            }

            if (ArenaState >= ARENA_STATE_DUELING)
            {
                if (ArenaMember_A)
                    ArenaMember_A->TeleportTo(0, 4250.052246f, -2872.312012f, 9.249156f, 4.686810f);
                else
                {
                    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_POSITION);

                    stmt->setFloat(0, 4250.052246f);
                    stmt->setFloat(1, -2872.312012f);
                    stmt->setFloat(2, 9.249156f);
                    stmt->setFloat(3, 4.686810f);
                    stmt->setUInt16(4, uint16(0));
                    stmt->setUInt16(5, 2037);
                    stmt->setUInt32(6, CharGUID_A);

                    CharacterDatabase.Execute(stmt);
                }
                if (ArenaMember_B)
                    ArenaMember_B->TeleportTo(0, 4250.265137f, -2690.220947f, 9.069143f, 1.631879f);
                else
                {
                    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_POSITION);

                    stmt->setFloat(0, 4250.265137f);
                    stmt->setFloat(1, -2690.220947f);
                    stmt->setFloat(2, 9.069143f);
                    stmt->setFloat(3, 1.631879f);
                    stmt->setUInt16(4, uint16(0));
                    stmt->setUInt16(5, 2037);
                    stmt->setUInt32(6, CharGUID_B);

                    CharacterDatabase.Execute(stmt);
                }
            }

            for (int i = 0; i < 4; i++)
                if (GameObject* obj = me->GetMap()->GetGameObjectBySpawnId(GateObjects[i]))
                    obj->SetPhaseMask(1, true);

            Timer = 0;
            ChallengeTimer = 0;
            TeleportTimer = 0;
            StartTimer = 0;
            ArenaMember_A = nullptr;
            ArenaMember_B = nullptr;
            uint32 CharGUID_A = 0;
            uint32 CharGUID_B = 0;
            ArenaState = ARENA_STATE_INACTIVE;
        }

        bool OnGossipHello(Player* p) override
        {
            ClearGossipMenuFor(p);
            if (ArenaState == ARENA_STATE_INACTIVE)
            {
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge Player to a Duel.", GOSSIP_CHALLENGE_DUEL, 0);
                if (Makgora)
                    AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge Player to Mak'Gora!", GOSSIP_CHALLENGE_MAKGORA, 0, "Enter the players name you wish to challenge.", 0, true);
            }
            else
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, ArenaMember_A->GetName() + " has challenged " + ArenaMember_B->GetName() + "to a duel.", GOSSIP_BACK, 0);

            if (v_PastResults.size() > 0)
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "View past results.", GOSSIP_VIEW_RESULTS, 0);

            AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Exit.", GOSSIP_EXIT, 0);

            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            return true;
        }

        bool OnGossipSelect(Player* p, uint32 menu_id, uint32 gossipListId) override
        {
            uint32 sender = menu_id == 99 ? menu_id : GetGossipSenderFor(p, gossipListId);
            uint32 action = menu_id == 99 ? gossipListId : GetGossipActionFor(p, menu_id);
            ClearGossipMenuFor(p);

            switch (sender)
            {

            case GOSSIP_CHALLENGE_DUEL:
            {
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge for Fun", GOSSIP_CHALLENGE, 0, "Enter the players name you wish to challenge.", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge for 50g", GOSSIP_CHALLENGE, 50, "Enter the players name you wish to challenge.", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge for 150g", GOSSIP_CHALLENGE, 150, "Enter the players name you wish to challenge.", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge for 300g", GOSSIP_CHALLENGE, 300, "Enter the players name you wish to challenge.", 0, true);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case GOSSIP_VIEW_RESULTS:
            case GOSSIP_RESULTS:
            {
                for (int i = 0; i < v_PastResults.size(); i++)
                    AddGossipItemFor(p, GOSSIP_ICON_TRAINER, v_PastResults[i], GOSSIP_RESULTS, 0);

                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Back", GOSSIP_BACK, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case GOSSIP_BACK:
            {
                OnGossipHello(p);
            } break;

            case GOSSIP_EXIT:
            {
                if (p == ArenaMember_B)
                    DeclineChallenge();

                CloseGossipMenuFor(p);
            } break;

            case GOSSIP_REPLY:
            {
                if (action)
                    DeclineChallenge();
                else
                    AcceptChallenge();

                CloseGossipMenuFor(p);
            } break;

            }

            return true;

        }

        bool OnGossipSelectCode(Player* p, uint32 menu_id, uint32 gossipListId, char const* code) override
        {
            uint32 sender = GetGossipSenderFor(p, gossipListId);
            uint32 action = GetGossipActionFor(p, menu_id);
            ClearGossipMenuFor(p);

            CloseGossipMenuFor(p);

            Player* target = nullptr;
            std::string playerName;
            if (!ChatHandler(p->GetSession()).extractPlayerTarget((char*)code, &target, nullptr, &playerName))
                return false;

            if (target->IsLoading() || !target->HasEnoughMoney(action * 10000))
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ".");
                return true;
            }

            if (!p->HasEnoughMoney(action * 10000))
            {
                ChatHandler(p->GetSession()).SendSysMessage("You don't have enough money.");
                return true;
            }

            if (target->GetZoneId() != me->GetZoneId())
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ", they are in a different zone.");
                return true;
            }

            if (target->IsInSameGroupWith(p))
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ", You are both in the same group." );
                return true;
            }

            switch (sender)
            {

            case GOSSIP_CHALLENGE:
            {
                ChallengePlayer(p, target, false, action);
            } break;

            case GOSSIP_CHALLENGE_MAKGORA:
            {
                ChallengePlayer(p, target, true, action);
            } break;

            }

            return true;
        }

        void UpdateAI(uint32 diff) override
        {
            if (ArenaState >= ARENA_STATE_INACTIVE)
                if (!ArenaMember_A || !ArenaMember_B)
                {
                    EndDuel(REASON_DISCONNECT, 0);
                    return;
                }

            if (ChallengeTimer > 0)
            {
                if (ChallengeTimer > diff)
                    ChallengeTimer -= diff;
                else
                {
                    ChallengeTimer = 0;
                    CloseGossipMenuFor(ArenaMember_B);
                    DeclineChallenge();
                }
                return;
            }

            if (TeleportTimer > 0)
            {
                if (TeleportTimer > diff)
                {
                    if (TeleportTimer / 1000 > (TeleportTimer - diff) / 1000)
                    {
                        ChatHandler(ArenaMember_A->GetSession()).SendNotify("Will be Teleported in %u seconds!", (999 + TeleportTimer - diff) / 1000);
                        ChatHandler(ArenaMember_B->GetSession()).SendNotify("Will be Teleported in %u seconds!", (999 + TeleportTimer - diff) / 1000);
                    }

                    TeleportTimer -= diff;
                }
                else
                {
                    TeleportTimer = 0;
                    StartTimer = 10000;
                    ArenaMember_A->TeleportTo(0, 4249.722168f, -2851.923340f, 8.834438f, 1.498352f);
                    ArenaMember_B->TeleportTo(0, 4250.012695f, -2714.421387f, 8.838498f, 4.690998f);
                    // Teleport Players
                }

                return;
            }

            if (StartTimer > 0)
            {
                if (StartTimer > diff)
                    StartTimer -= diff;
                else
                {
                    StartTimer = 0;
                    Timer = 3000000;
                    me->Yell("START!", LANG_COMMON);

                    for (int i = 0; i < 4; i++)
                    {
                        if (GameObject* obj = me->GetMap()->GetGameObjectBySpawnId(GateObjects[i]))
                            obj->SetPhaseMask(0, true);
                    }
                }
                return;
            }

            if (Timer > 0)
            {
                if (ArenaMember_A->isDead() && ArenaMember_B->isDead())
                    return EndDuel(DuelCompleteReason::REASON_COMPLETE, 0);
                if (ArenaMember_A->isDead())
                    return EndDuel(DuelCompleteReason::REASON_COMPLETE, 2);
                if (ArenaMember_B->isDead())
                    return EndDuel(DuelCompleteReason::REASON_COMPLETE, 1);

                if (Timer > diff)
                    Timer -= diff;
                else
                    return EndDuel(DuelCompleteReason::REASON_COMPLETE, 0);
            }
        }

        uint32 Timer = 0;
        uint32 ChallengeTimer = 0;
        uint32 TeleportTimer = 0;
        uint32 StartTimer = 0;
        bool Makgora = true;
        ArenaChallengeState ArenaState = ARENA_STATE_INACTIVE;
        Player* ArenaMember_A = nullptr;
        Player* ArenaMember_B = nullptr;
        uint32 CharGUID_A = 0;
        uint32 CharGUID_B = 0;

        uint32 GateObjects[4] = { 302204, 302205, 302206, 302207 };

        std::vector<std::string> v_PastResults;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new ArenaChallengerCreature_AI(creature);
    }
};

class DivineGuardianCreature : public CreatureScript // Rename "ActiveCreature" to "(Zone / Boss)ActiveCreature" without ()
{
public:
    DivineGuardianCreature() : CreatureScript("DivineGuardianCreature") {} // Rename "ActiveCreature" to "(Zone / Boss)ActiveCreature" without ()

    struct DivineGuardianCreature_AI : public ScriptedAI // Rename "ActiveCreature_AI" to "(Zone / Boss)ActiveCreature_AI" without ()
    {
        DivineGuardianCreature_AI(Creature* creature) : ScriptedAI(creature) {} // Rename "ActiveCreature_AI" to "(Zone / Boss)ActiveCreature_AI" without ()

        void Reset() override
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(uint32 diff) override
        {
            if (me->IsAlive())
            {
                Player* master = GetMaster();

                CheckGroupHealth();

                if (!me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    if (master && master->IsAlive())
                        if (me->GetDistance(master) > PET_FOLLOW_DIST)
                           me->GetMotionMaster()->MoveFollow(master, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
            }
        }

    private:
        Player* GetMaster()
        {
            return me->GetVictim() ? me->GetVictim()->ToPlayer() : nullptr;
        }

        void CheckGroupHealth()
        {
            if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                return;


            Player* summoner = me->GetOwner()->ToPlayer();
            Player* TargetToHeal = summoner;
            Group* group = summoner->GetGroup();
            if (group)
            {
                Group::MemberSlotList const& members = group->GetMemberSlots();
                for (auto const& member : members)
                {
                    Player* player = ObjectAccessor::FindPlayer(member.guid);
                    if (player && me->IsWithinDistInMap(player, 35.0f))
                        if (player->GetHealthPct() < TargetToHeal->GetHealthPct())
                            TargetToHeal = player;
                }
            }

            if (TargetToHeal->GetHealthPct() < 50.0f)
                me->CastSpell(TargetToHeal, 48063);
            else if (TargetToHeal->GetHealthPct() < 90.0f)
                me->CastSpell(TargetToHeal, 48071);
            else
                if (me->GetHealthPct() < 75.0f)
                    me->CastSpell(me, 48071);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new DivineGuardianCreature_AI(creature); // Rename "ActiveCreature_AI" to "(Zone / Boss)ActiveCreature_AI" without ()
    }
};

class WrathfulGuardianCreature : public CreatureScript // Rename "ActiveCreature" to "(Zone / Boss)ActiveCreature" without ()
{
public:
    WrathfulGuardianCreature() : CreatureScript("WrathfulGuardianCreature") {} // Rename "ActiveCreature" to "(Zone / Boss)ActiveCreature" without ()

    struct WrathfulGuardianCreature_AI : public ScriptedAI // Rename "ActiveCreature_AI" to "(Zone / Boss)ActiveCreature_AI" without ()
    {
        WrathfulGuardianCreature_AI(Creature* creature) : ScriptedAI(creature) {} // Rename "ActiveCreature_AI" to "(Zone / Boss)ActiveCreature_AI" without ()

        void Reset() override
        {
            me->SetReactState(REACT_DEFENSIVE);
        }

        void UpdateAI(uint32 diff) override
        {
            if (me->IsAlive())
            {
                if (GetMaster()->IsInCombat())
                {
                    if (me->GetTarget())
                    {

                        return;
                    }

                    if (Unit* masterTarget = GetMaster()->GetSelectedUnit())
                    {
                        if (masterTarget->IsHostileTo(GetMaster()))
                        {
                            me->Attack(masterTarget, true);
                        }
                    }
                }
            }
        }

    private:
        Player* GetMaster()
        {
            return me->GetOwner()->ToPlayer();
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new WrathfulGuardianCreature_AI(creature); // Rename "ActiveCreature_AI" to "(Zone / Boss)ActiveCreature_AI" without ()
    }
};

class ClassTrainerCreature : public CreatureScript
{
public:
    ClassTrainerCreature() : CreatureScript("ClassTrainerCreature") {}

    struct ClassTrainerCreature_AI : public ScriptedAI
    {
        ClassTrainerCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        bool OnGossipHello(Player* p) override
        {
            ClearGossipMenuFor(p);

            uint16 Trainer = me->GetEntry() - 50003;
            if (Trainer == 10)
                Trainer = 11;

            if (p->GetClass() == Trainer)
            {
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Train me.", 0, 0);
                AddGossipItemFor(p, GOSSIP_ICON_TAXI, "I wish to unlearn my talents.", 1, 0);
                if (p->GetSpecsCount() == 1)
                    AddGossipItemFor(p, GOSSIP_ICON_TAXI, "I wish to learn Dual Specialization. ", 2, 0);
                SendGossipMenuFor(p, me->GetEntry() + 9996, me);
            }
            else
                SendGossipMenuFor(p, 60010, me);

            return true;
        }

        bool OnGossipSelect(Player* p, uint32 /*menu_id*/, uint32 gossipListId) override
        {
            uint32 sender = GetGossipSenderFor(p, gossipListId);
            ClearGossipMenuFor(p);

            switch (sender)
            {
            case 0:
                p->GetSession()->SendTrainerList(me);
                break;
            case 1:
                p->SendTalentWipeConfirm(me->GetGUID());
                break;
            case 2:
                p->CastSpell(p, 63680, p->GetGUID());
                p->CastSpell(p, 63624, p->GetGUID());
                break;
            }

            CloseGossipMenuFor(p);
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new ClassTrainerCreature_AI(creature);
    }
};

class MariaLucenteCreature : public CreatureScript
{
public:
    MariaLucenteCreature() : CreatureScript("MariaLucenteCreature") {}

    struct MariaLucenteCreature_AI : public ScriptedAI
    {
        MariaLucenteCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        bool OnGossipHello(Player* p) override
        {
            ClearGossipMenuFor(p);

            if (p->HasItemCount(60011))
            {
                AddGossipItemFor(p, GOSSIP_ICON_TAXI, "Oh....", GOSSIP_SENDER_MAIN, 0, "What am I?", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Nevermind", GOSSIP_SENDER_MAIN, 1);
            }

            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);

            return true;
        }

        bool OnGossipSelect(Player* p, uint32 menu_id, uint32 gossipListId) override
        {
            CloseGossipMenuFor(p);
            return true;
        }

        bool OnGossipSelectCode(Player* p, uint32 menu_id, uint32 gossipListId, char const* code) override
        {
            ClearGossipMenuFor(p);
            std::string message = code;

            for (int i = 0; i < message.size(); i++)
                message[i] = std::tolower(message[i]);

            if (message == "pen")
                me->Say("Reward", LANG_UNIVERSAL);

            CloseGossipMenuFor(p);
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new MariaLucenteCreature_AI(creature);
    }
};

void AddSC_GuardianCreatures()
{
    new DivineGuardianCreature();
    new WrathfulGuardianCreature();
    new ClassTrainerCreature();
    new MariaLucenteCreature();
    new ArenaChallengerCreature();
}
