#include <World.h>
#include <Spell.h>
#include <Group.h>
#include <ScriptedGossip.h>
#include <WorldSession.h>

class ClashActiveCreature : public CreatureScript
{
public:
    ClashActiveCreature() : CreatureScript("ClashActiveCreature") {}

    struct ClashActiveCreature_AI : public ScriptedAI
    {
        ClashActiveCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        void JustAppeared() override
        {
            me->setActive(true);
        }

        void UpdateAI(uint32 diff) override
        {
            timer += diff;

            if (timer >= 5000)
            {
                if (me->FindNearestCreature(BossID, 50, true))
                {
                    if (!bAlive)
                    {
                        bAlive = true;
                        sWorld->SendGlobalText("Clash has respawned.", nullptr);
                    }
                    else
                    {
                        AliveTimer += 5000;

                        if (AliveTimer >= 300000)
                        {
                            sWorld->SendGlobalText("Clash Is Alive.", nullptr);
                            AliveTimer -= 300000;
                        }
                    }
                }
                else
                    if (bAlive)
                    {
                        bAlive = false;
                        AliveTimer = 0;
                    }

                timer -= 5000;
            }
        }

        bool bAlive = false;
        uint32 timer = 0;
        uint32 BossID = 101301;
        uint32 AliveTimer = 0;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new ClashActiveCreature_AI(creature);
    }
};

enum CreatureState {
    STATE_IDLE,
    STATE_FOLLOWING,
    STATE_CASTING
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
        CreatureState m_state;
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

        bool OnGossipSelect(Player* p, uint32 menu_id, uint32 gossipListId) override
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
}
