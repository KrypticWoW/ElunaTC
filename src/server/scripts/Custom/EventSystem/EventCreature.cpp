#include <ScriptedGossip.h>
#include <WorldSession.h>

#include "EventSystem.h"

class CustomEventCreature : public CreatureScript
{
public:
    CustomEventCreature() : CreatureScript("CustomEventCreature") {}

    struct CustomEventCreature_AI : public ScriptedAI
    {
        CustomEventCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        void UpdateAI(uint32 diff) override
        {
            if (me->HasNpcFlag(UNIT_NPC_FLAG_GOSSIP))
            {
                UpdateTimer += diff;

                if (UpdateTimer >= 500)
                {
                    UpdateTimer -= 500;

                    // Check Players Surrounding

                    std::list<Player*> pList;
                    me->GetPlayerListInGrid(pList, 2.0f, true);

                    for (auto&i: pList)
                        sEventSystem.HandleEventCreatureGossip(i, me);
                }
            }
        }

    private:
        uint32 UpdateTimer = 0;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new CustomEventCreature_AI(creature);
    }

};

void AddSC_CustomEventCreature()
{
    new CustomEventCreature();
}
