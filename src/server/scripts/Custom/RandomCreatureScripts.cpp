#include <Group.h>
#include <ScriptedGossip.h>

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

void AddSC_RandomCreatureScripts()
{
    new DivineGuardianCreature();
}
