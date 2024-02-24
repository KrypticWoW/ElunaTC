#include <World.h>

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

class ActiveCreature : public CreatureScript // Rename "ActiveCreature" to "(Zone / Boss)ActiveCreature" without ()
{
public:
    ActiveCreature() : CreatureScript("ActiveCreature") {} // Rename "ActiveCreature" to "(Zone / Boss)ActiveCreature" without ()

    struct ActiveCreature_AI : public ScriptedAI // Rename "ActiveCreature_AI" to "(Zone / Boss)ActiveCreature_AI" without ()
    {
        ActiveCreature_AI(Creature* creature) : ScriptedAI(creature) {} // Rename "ActiveCreature_AI" to "(Zone / Boss)ActiveCreature_AI" without ()

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
                        sWorld->SendGlobalText("Creature has respawned.", nullptr); // Respawned Announcement
                    }
                    else
                    {
                        AliveTimer += 5000;

                        if (AliveTimer >= 300000)
                        {
                            sWorld->SendGlobalText("Creature Is Alive.", nullptr); // Alive Announcement
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
        uint32 BossID = 101301; // BOSS ID
        uint32 AliveTimer = 0;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new ActiveCreature_AI(creature); // Rename "ActiveCreature_AI" to "(Zone / Boss)ActiveCreature_AI" without ()
    }
};

void AddSC_ActiveCreature()
{
    new ClashActiveCreature();
    // add new "(Zone / Boss)ActiveCreature" without ()
}
