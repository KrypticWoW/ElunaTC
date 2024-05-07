
#include "ScriptMgr.h"
#include "ScarletRaid.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "GridNotifiersImpl.h"

class ScarletMagusCreature : public CreatureScript
{
public:
    ScarletMagusCreature() : CreatureScript("ScarletMagusCreature") {}

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetCustomScarletMonasteryAI<ScarletMagusCreature_AI>(creature);
    }

    struct ScarletMagusCreature_AI : public ScriptedAI
    {
        ScarletMagusCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        void JustAppeared() override
        {

        }

        void UpdateAI(uint32 diff) override
        {

        }
    };
};

class ScarletInvokerCreature : public CreatureScript
{
public:
    ScarletInvokerCreature() : CreatureScript("ScarletInvokerCreature") {}

    enum InvokerSpells
    {
        SPELL_GREATER_HEAL = 71131,
    };

    enum InvokerEvents
    {
        EVENT_TEST = 1,
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new ScarletInvokerCreature_AI(creature);
    }

    struct ScarletInvokerCreature_AI : public ScriptedAI
    {
        ScarletInvokerCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        void Reset() override
        {
            _events.ScheduleEvent(EVENT_TEST, 5s);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_TEST:
                {
                    _events.RescheduleEvent(EVENT_TEST, 5s);

                    std::vector<Creature*> tempList;
                    Trinity::AllFriendlyCreaturesInGrid check(me);

                    Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid> searcher(me, tempList, check);
                    Cell::VisitGridObjects(me, searcher, 10.0f);

                    Creature* LowestHealth = me;

                    if (tempList.empty())
                        return;

                    for (Creature* creature : tempList)
                    {
                        if (creature->GetHealthPct() < LowestHealth->GetHealthPct())
                            LowestHealth = creature;
                    }

                    me->CastSpell(LowestHealth, SPELL_GREATER_HEAL);
                } break;
                }
            }
        }

        EventMap _events;
    };
};

class ScarletGuardianCreature : public CreatureScript
{
public:
    ScarletGuardianCreature() : CreatureScript("ScarletGuardianCreature") {}

    enum InvokerSpells
    {
        SPELL_BLADESTORM = 65947,
        SPELL_EXPOSE_WEAKNESS = 102048,
        SPELL_RETALIATION = 102047,
    };

    enum InvokerEvents
    {
        EVENT_BLADESTORM = 1,
        EVENT_EXPOSE_WEAKNESS,
        EVENT_CHECK_HEALTH,
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetCustomScarletMonasteryAI<ScarletGuardianCreature_AI>(creature);
    }

    struct ScarletGuardianCreature_AI : public ScriptedAI
    {
        ScarletGuardianCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        void Reset() override
        {
            _events.ScheduleEvent(EVENT_BLADESTORM, 7s);
            _events.ScheduleEvent(EVENT_EXPOSE_WEAKNESS, 5s);
            _events.ScheduleEvent(EVENT_CHECK_HEALTH, 2s);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_BLADESTORM:
                    me->CastSpell(me, SPELL_BLADESTORM);
                    _events.RescheduleEvent(EVENT_BLADESTORM, 45s);
                    break;

                case EVENT_EXPOSE_WEAKNESS:
                    me->CastSpell(nullptr, SPELL_EXPOSE_WEAKNESS, true);
                    _events.RescheduleEvent(EVENT_EXPOSE_WEAKNESS, 50s);
                    break;

                case EVENT_CHECK_HEALTH:
                    if (me->GetHealthPct() <= 25.0f)
                        me->CastSpell(me, SPELL_RETALIATION);
                    else
                        _events.RescheduleEvent(EVENT_CHECK_HEALTH, 2s);
                    break;


                }
            }
            DoMeleeAttackIfReady();
        }

        EventMap _events;
    };
};

void AddSC_custom_scarlet_creatures()
{
    //ScarletMagusCreature();
    new ScarletInvokerCreature();
    new ScarletGuardianCreature();
}
