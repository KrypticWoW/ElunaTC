#include <World.h>
#include <Spell.h>
#include <Group.h>
#include <ScriptedGossip.h>
#include <WorldSession.h>
#include <Chat.h>
#include <DatabaseEnv.h>
#include <Custom/MallArena/MallArena.h>

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
        GOSSIP_EXIT
    };

    struct ArenaChallengerCreature_AI : public ScriptedAI
    {
        ArenaChallengerCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        void JustAppeared() override
        {
            me->setActive(true);
            sMAS.SetArenaCreature(me);
        }

        bool ChallengePlayer(Player* challenger, Player* opponent, bool makgora, uint32 gold)
        {
            me->Say("Challenge!", LANG_COMMON);
            sMAS.ChallengePlayer(challenger, opponent, makgora, gold);

            ClearGossipMenuFor(opponent);
            InitGossipMenuFor(opponent, ArenaChallengeMenu);
            AddGossipItemFor(opponent, GOSSIP_ICON_TRAINER, challenger->GetName() + " has challenged you to a duel." + (gold ? " Betting " + std::to_string(gold) + " Gold." : ""), GOSSIP_EXIT, 0, "Do you accept this challenge?.", 0, false);
            AddGossipItemFor(opponent, GOSSIP_ICON_TRAINER, "I accept the challenge", GOSSIP_CHALLENGE_REPLY_YES, 0);
            AddGossipItemFor(opponent, GOSSIP_ICON_TRAINER, "I decline the challenge", GOSSIP_CHALLENGE_REPLY_NO, 0);
            SendGossipMenuFor(opponent, DEFAULT_GOSSIP_MESSAGE, opponent->GetGUID());
            return true;
        }

        bool OnGossipHello(Player* p) override
        {
            ClearGossipMenuFor(p);

            if (sMAS.GetArenaState() == ARENA_STATE_INACTIVE)
            {
                uint32 Cooldown = sMAS.GetCooldown(p->GetGUID());

                if (Cooldown > 0)
                    AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "You can not challenge someone for another " + std::to_string(Cooldown / 1000) + " seconds.", GOSSIP_BACK, 0);
                else
                {
                    AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge Player to a Duel.", GOSSIP_CHALLENGE_DUEL, 0);
                    if (sMAS.AllowMakgora())
                        AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge Player to Mak'Gora!", GOSSIP_CHALLENGE_MAKGORA, 0, "Enter the players name you wish to challenge.", 0, true);
                }
            }
            else
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, sMAS.GetMemberName(true) + " has challenged " + sMAS.GetMemberName(false) + "to a duel.", GOSSIP_BACK, 0);

            if (sMAS.GetListSize() > 0)
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "View past results.", GOSSIP_VIEW_RESULTS, 0);

            AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Exit.", GOSSIP_EXIT, 0);

            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            return true;
        }

        bool OnGossipSelect(Player* p, uint32 /*menu_id*/, uint32 gossipListId) override
        {
            uint32 sender = GetGossipSenderFor(p, gossipListId);
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
                for (auto& itr : sMAS.GetResults())
                    AddGossipItemFor(p, GOSSIP_ICON_TRAINER, itr, GOSSIP_RESULTS, 0);

                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Back", GOSSIP_BACK, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case GOSSIP_BACK:
            {
                OnGossipHello(p);
            } break;

            case GOSSIP_EXIT:
            {
                if (p == sMAS.GetPlayer(false))
                    sMAS.DeclineChallenge();

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

            std::string playerName = code;
            Player* target = ObjectAccessor::FindConnectedPlayerByName(playerName);
            if (!target)
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ".");
                return false;
            }

            if (target->IsLoading() || !target->IsInWorld())
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ".");
                return false;
            }

            if (!target->HasEnoughMoney(action * 10000) || target->IsInCombat())
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ".");
                return false;
            }

            if (!p->HasEnoughMoney(action * 10000))
            {
                ChatHandler(p->GetSession()).SendSysMessage("You don't have enough money.");
                return false;
            }

            if (target->GetZoneId() != me->GetZoneId())
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ", they are in a different zone.");
                return false;
            }

            if (target->IsInSameGroupWith(p))
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ", You are both in the same group.");
                return false;
            }

            ChallengePlayer(p, target, sender == GOSSIP_CHALLENGE_MAKGORA, action);

            return true;
        }

        void UpdateAI(uint32 diff) override
        {
            //if (sMAS.GetArenaState() > ARENA_STATE_INACTIVE)
                sMAS.Update(diff);
        }
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
                UpdateTimer += diff;
                if (diff > 1000)
                {
                    diff -= 1000;

                    Player* master = GetMaster();

                    CheckGroupHealth();

                    if (!me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                        if (master && master->IsAlive())
                            if (me->GetDistance(master) > PET_FOLLOW_DIST)
                                me->GetMotionMaster()->MoveFollow(master, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                }
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

        uint32 UpdateTimer = 0;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new DivineGuardianCreature_AI(creature); // Rename "ActiveCreature_AI" to "(Zone / Boss)ActiveCreature_AI" without ()
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

        bool OnGossipSelect(Player* p, uint32 /*menu_id*/, uint32 /*gossipListId*/) override
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
    new ClassTrainerCreature();
    new MariaLucenteCreature();
    new ArenaChallengerCreature();
}
