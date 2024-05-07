#include <ScriptedCreature.h>
#include <SpellAuras.h>
#include <Chat.h>
#include <Pet.h>
#include <Group.h>
#include <map.h>
#include <GridNotifiers.h>
#include <World.h>

constexpr uint32 TempestGUID = 214426;
constexpr uint32 ZephyrGUID = 214425;
constexpr uint16 customVilethornSpawnTotal = 5;

float customVilethornSpawns[customVilethornSpawnTotal][4] = {
    {4645.399902f, -101.462021f, 153.913467f, 5.999407f},
    {4658.051758f, -123.099335f, 156.534134f, 1.053140f},
    {4690.499023f, -125.589935f, 155.770340f, 2.140151f},
    {4697.130371f, -101.831779f, 147.906357f, 3.271125f},
    {4685.453613f,  -86.116890f, 148.471008f, 4.356395f},
};

enum WB_SUMMON_IDS
{
    WB_SUMMON_BUNNY_VOLATILE_EARTH = 52000,
    WB_SUMMON_VILETHORN_SAPPLING = 52001,
    WB_SUMMON_LIGHTNING_ORB = 52002,
};

enum EVENTS
{
    // Global
    EVENT_UPDATE_STACKS = 1,
    EVENT_CAST_MENDBREAKERS_SHROUD,
    EVENT_CAST_BERSERK,
    EVENT_CAST_DEATHS_EMBRACE,

    // Ironbane
    EVENT_CAST_SPELL_REFLECTION,
    EVENT_CAST_RETALIATION,
    EVENT_CAST_SHOCKWAVE,

    // Vilethorn
    EVENT_CHECK_HEALTH,
    EVENT_CAST_TRANQUILITY,
    EVENT_SAPPLING_INDICATOR,
    EVENT_SUMMON_SAPPLING,
    EVENT_SUMMON_VIOLENT_EARTH_BUNNY,
    EVENT_CAST_VIOLENT_EARTH,

    // Vilethorn Sappling
    EVENT_CHECK_DISTANCE,
    EVENT_CAST_NATURE_BOMB,
    EVENT_SUICIDE,

    // Zephyr & Tempest
    EVENT_ENGAGE_TWIN,
    EVENT_CHECK_RANGE,
    EVENT_WIND_SURGE_INDICATOR,
    EVENT_CAST_WIND_SURGE,
    EVENT_CAST_SKY_VORTEX,
    EVENT_CAST_NATURES_FURY,
    EVENT_SUMMON_LIGHTNING_ORB,

    EVENT_CHECK_TWINS_HEALTH,
    EVENT_CHANNEL_STORMBOUND_RENEWAL,
    EVENT_CHECK_STORMBOUND_AURA,
};

enum SPELLS
{
    // Global
    SPELL_ASTRAL_EMPOWERMENT        = 103000,
    SPELL_MENDBREAKERS_SHROUD       = 103001,
    SPELL_BERSERK                   = 103002,
    SPELL_DEATHS_EMBRACE            = 103003,

    // Ironbane
    SPELL_SPELL_REFLECTION          = 103010,
    SPELL_RETALIATION               = 103011,
    SPELL_SHOCKWAVE                 = 103012,

    // Vilethorn
    SPELL_NATURES_AEGIS             = 103013,
    SPELL_TRANQUILITY               = 103014,
    SPELL_VIOLENT_EARTH             = 103016,

    SPELL_SAPPLING_TARG_IND         = 103018,
    SPELL_NATURE_BOMB               = 103019,

    // Zephyr & Tempest
    SPELL_ELEMENTAL_FUSION          = 103021,
    SPELL_WIND_SURGE_INDICATOR      = 103022,   // Change this
    SPELL_WIND_SURGE                = 103023,
    SPELL_SKY_VORTEX                = 103024,
    SPELL_NATURES_FURY              = 103025,

    SPELL_STORMBOUND_RENEWAL        = 103030,
    SPELL_STORMBOUND_RENEWAL_SHIELD = 103031,
};

struct world_boss_ironbane : public ScriptedAI
{
    world_boss_ironbane(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        _events.Reset();
        me->SetAuraStack(SPELL_ASTRAL_EMPOWERMENT, me, StackCount);
        _events.ScheduleEvent(EVENT_UPDATE_STACKS, 1s);
        _events.ScheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 0ms);
        _events.ScheduleEvent(EVENT_CAST_BERSERK, 300s);
        _events.ScheduleEvent(EVENT_CAST_SPELL_REFLECTION, 10s);
        _events.ScheduleEvent(EVENT_CAST_RETALIATION, 15s);
        _events.ScheduleEvent(EVENT_CAST_SHOCKWAVE, 25s);
    }

    void JustEngagedWith(Unit* victim) override
    {
        Player* InitialTarget = nullptr;

        if (victim->GetOwner())
        {
            if (victim->GetOwner()->IsPlayer())
                InitialTarget = victim->GetOwner()->ToPlayer();
        }
        else if (victim->IsPlayer())
            InitialTarget = victim->ToPlayer();

        if (!InitialTarget)
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been engaged.").c_str());

        if (Group* initialGroup = InitialTarget->GetGroup())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been engaged by |Hplayer:" + initialGroup->GetLeaderName() + "|h[" + initialGroup->GetLeaderName() + "]|h|r's Group.").c_str());

            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been engaged by |Hplayer:" + InitialTarget->GetName() + "|h[" + InitialTarget->GetName() + "]|h|r").c_str());
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
            if (me->isTappedBy(victim->ToPlayer()) || (me->GetLootRecipient()->IsInSameGroupWith(victim->ToPlayer())))
                _events.ScheduleEvent(EVENT_CAST_DEATHS_EMBRACE, 0ms);
    }

    void JustDied(Unit* killer) override
    {
        Player* ActualKiller = nullptr;

        if (killer->GetOwner())
        {
            if (killer->GetOwner()->IsPlayer())
                ActualKiller = killer->GetOwner()->ToPlayer();
        }
        else if (killer->IsPlayer())
            ActualKiller = killer->ToPlayer();

        if (!ActualKiller)
            sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been slain.").c_str());

        if (Group* sGroup = me->GetLootRecipientGroup())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been slain by [" + sGroup->GetLeaderName() + "]'s Group.").c_str());

        if (Player* sPlayer = me->GetLootRecipient())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been slain by [" + sPlayer->GetName() + "].").c_str());
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

            case EVENT_UPDATE_STACKS:
            {
                uint8 Count = 0;
                for (auto* victim : me->GetThreatManager().GetUnsortedThreatList())
                    if (victim->GetVictim()->IsPlayer())
                        if (me->isTappedBy(victim->GetVictim()->ToPlayer()))
                            Count++;

                me->SetAuraStack(SPELL_ASTRAL_EMPOWERMENT, me, StackCount - Count);
                _events.RescheduleEvent(EVENT_UPDATE_STACKS, 3s);
            } break;

            case EVENT_CAST_MENDBREAKERS_SHROUD:
            {
                me->CastSpell(nullptr, SPELL_MENDBREAKERS_SHROUD);
                _events.RescheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 27s);
            } break;

            case EVENT_CAST_BERSERK:
            {
                me->CastSpell(nullptr, SPELL_BERSERK);
            } break;

            case EVENT_CAST_DEATHS_EMBRACE:
            {
                me->CastSpell(nullptr, SPELL_DEATHS_EMBRACE);
            } break;

            case EVENT_CAST_SPELL_REFLECTION:
            {
                me->CastSpell(nullptr, SPELL_SPELL_REFLECTION);
                _events.RescheduleEvent(EVENT_CAST_SPELL_REFLECTION, 60s);
            } break;

            case EVENT_CAST_RETALIATION:
            {
                me->CastSpell(nullptr, SPELL_RETALIATION);
                _events.RescheduleEvent(EVENT_CAST_RETALIATION, 150s);
            } break;

            case EVENT_CAST_SHOCKWAVE:
            {
                me->CastSpell(me, SPELL_SHOCKWAVE);
                _events.RescheduleEvent(EVENT_CAST_SHOCKWAVE, 25s);
            } break;

            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    uint8 StackCount = 19;
};

struct world_boss_vilethorn : public ScriptedAI
{
    world_boss_vilethorn(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        _events.Reset();
        summon = nullptr;
        Phase = 3;
        me->SetAuraStack(SPELL_ASTRAL_EMPOWERMENT, me, StackCount);
        me->AddAura(SPELL_NATURES_AEGIS, me);
        _events.ScheduleEvent(EVENT_UPDATE_STACKS, 1000ms);
        _events.ScheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 0ms);
        _events.ScheduleEvent(EVENT_CAST_BERSERK, 300s);
        _events.ScheduleEvent(EVENT_CHECK_HEALTH, 2s);
        _events.ScheduleEvent(EVENT_SAPPLING_INDICATOR, 13s);
        _events.ScheduleEvent(EVENT_SUMMON_VIOLENT_EARTH_BUNNY, 15s);

        DespawnSummons();
    }

    void DespawnSummons()
    {
        std::vector<Creature*> SummonList;
        me->GetCreatureListWithEntryInGrid(SummonList, WB_SUMMON_VILETHORN_SAPPLING, 250.0f);

        for (auto& i : SummonList)
            i->DespawnOrUnsummon();
    }

    void JustEngagedWith(Unit* victim) override
    {
        Player* InitialTarget = nullptr;

        if (victim->GetOwner())
        {
            if (victim->GetOwner()->IsPlayer())
                InitialTarget = victim->GetOwner()->ToPlayer();
        }
        else if (victim->IsPlayer())
            InitialTarget = victim->ToPlayer();

        if (!InitialTarget)
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been engaged.").c_str());

        if (Group* initialGroup = InitialTarget->GetGroup())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been engaged by |Hplayer:" + initialGroup->GetLeaderName() + "|h[" + initialGroup->GetLeaderName() + "]|h|r's Group.").c_str());

        return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been engaged by |Hplayer:" + InitialTarget->GetName() + "|h[" + InitialTarget->GetName() + "]|h|r").c_str());
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
            if (me->isTappedBy(victim->ToPlayer()) || (me->GetLootRecipient()->IsInSameGroupWith(victim->ToPlayer())))
                _events.ScheduleEvent(EVENT_CAST_DEATHS_EMBRACE, 0ms);
    }

    void JustDied(Unit* killer) override
    {
        Player* ActualKiller = nullptr;

        if (killer->GetOwner())
        {
            if (killer->GetOwner()->IsPlayer())
                ActualKiller = killer->GetOwner()->ToPlayer();
        }
        else if (killer->IsPlayer())
            ActualKiller = killer->ToPlayer();

        if (!ActualKiller)
            sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been slain.").c_str());

        if (Group* sGroup = me->GetLootRecipientGroup())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been slain by [" + sGroup->GetLeaderName() + "]'s Group.").c_str());

        if (Player* sPlayer = me->GetLootRecipient())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been slain by [" + sPlayer->GetName() + "].").c_str());
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        me->Say("You kill my plants!!!", LANG_COMMON);
        if (summon->GetEntry() == WB_SUMMON_VILETHORN_SAPPLING)
            _events.ScheduleEvent(EVENT_SAPPLING_INDICATOR, Milliseconds(RAND(11000, 15000)));
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

            case EVENT_UPDATE_STACKS:
            {
                uint8 Count = 0;
                for (auto* victim : me->GetThreatManager().GetUnsortedThreatList())
                    if (victim->GetVictim()->IsPlayer())
                        if (me->isTappedBy(victim->GetVictim()->ToPlayer()))
                            Count++;

                me->SetAuraStack(SPELL_ASTRAL_EMPOWERMENT, me, StackCount - Count);
                _events.RescheduleEvent(EVENT_UPDATE_STACKS, 3s);
            } break;

            case EVENT_CAST_MENDBREAKERS_SHROUD:
            {
                me->CastSpell(nullptr, SPELL_MENDBREAKERS_SHROUD);
                _events.RescheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 27s);
            } break;

            case EVENT_CAST_BERSERK:
            {
                me->CastSpell(nullptr, SPELL_BERSERK);
            } break;

            case EVENT_CAST_DEATHS_EMBRACE:
            {
                me->CastSpell(nullptr, SPELL_DEATHS_EMBRACE);
            } break;

            case EVENT_CHECK_HEALTH:
            {
                if (me->GetHealthPct() <= Phase * 25.0f)
                {
                    _events.ScheduleEvent(EVENT_CAST_TRANQUILITY, 0s);
                    Phase -= 1;
                }

                if (Phase > 0)
                    _events.RescheduleEvent(EVENT_CHECK_HEALTH, 2s);
            } break;

            case EVENT_CAST_TRANQUILITY:
            {
                me->CastSpell(nullptr, SPELL_TRANQUILITY);
            } break;

            case EVENT_SUMMON_VIOLENT_EARTH_BUNNY:
            {
                Unit* target = SelectTarget(SelectTargetMethod::Random, 0, -10.0f, true, false);

                if (!target)
                    target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 0.0f, true, true);

                if (target)
                    summon = me->SummonCreature(WB_SUMMON_BUNNY_VOLATILE_EARTH, target->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 2600ms);

                _events.ScheduleEvent(EVENT_CAST_VIOLENT_EARTH, 2s);
            } break;

            case EVENT_CAST_VIOLENT_EARTH:
            {
                if (summon)
                    me->CastSpell(summon, SPELL_VIOLENT_EARTH);

                _events.ScheduleEvent(EVENT_SUMMON_VIOLENT_EARTH_BUNNY, 15s);
                return;
            } break;

            case EVENT_SAPPLING_INDICATOR:
            {
                if (me->CastSpell(SelectTarget(SelectTargetMethod::Random, 0, 12.0f, true, false, -SPELL_SAPPLING_TARG_IND), SPELL_SAPPLING_TARG_IND))
                    _events.ScheduleEvent(EVENT_SUMMON_SAPPLING, 2s);
                else
                    _events.ScheduleEvent(EVENT_SAPPLING_INDICATOR, 5s);
            } break;

            case EVENT_SUMMON_SAPPLING:
            {
                uint16 spawnID = rand() % customVilethornSpawnTotal;
                Position pos(customVilethornSpawns[spawnID][0], customVilethornSpawns[spawnID][1], customVilethornSpawns[spawnID][2], customVilethornSpawns[spawnID][3]);
                Creature* sappling = me->SummonCreature(WB_SUMMON_VILETHORN_SAPPLING, pos, TEMPSUMMON_DEAD_DESPAWN);


                std::vector<Player*> pList;
                me->GetPlayerListInGrid(pList, 40.0f, true);

                for (auto& i : pList)
                    if (i->HasAura(SPELL_SAPPLING_TARG_IND))
                    {
                        sappling->AI()->AttackStart(i);
                        sappling->TextEmote(me->GetName() + ": Fixates upon " + i->GetName() + ".");
                        sappling->GetThreatManager().FixateTarget(i);
                        break;
                    }
            } break;

            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    uint8 StackCount = 17;
    uint16 Phase = 3;
    TempSummon* summon = nullptr;
};

struct vilethorn_sappling : ScriptedAI
{
    vilethorn_sappling(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        _events.ScheduleEvent(EVENT_CHECK_DISTANCE, 100ms);
        target = nullptr;
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo*/) override
    {
        // Molten Golems cannot die from foreign damage. They will kill themselves via suicide spell when getting shattered
        if (damage >= me->GetHealth() && attacker != me)
        {
            damage = me->GetHealth() - 1;
            _events.ScheduleEvent(EVENT_CAST_NATURE_BOMB, 0s);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);
        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {

            case EVENT_CHECK_DISTANCE:
            {
                if (!target)
                {
                    if (me->GetVictim())
                        target = me->GetVictim();
                    else
                    {
                        _events.ScheduleEvent(EVENT_CAST_NATURE_BOMB, 0s);
                        return;
                    }
                }
                else
                {
                    if (!(target->IsAlive()))
                    {
                        _events.ScheduleEvent(EVENT_CAST_NATURE_BOMB, 0s);
                        break;
                    }

                    if (me->GetVictim() != target)
                    {
                        if (me->GetDistance(target->GetPosition()) <= 40)
                            me->AI()->AttackStart(target);
                        else
                        {
                            _events.ScheduleEvent(EVENT_CAST_NATURE_BOMB, 0s);
                            break;
                        }
                    }

                    if (me->GetVictim())
                        if (me->GetDistance(me->GetVictim()->GetPosition()) <= 3.0f)
                        {
                            _events.ScheduleEvent(EVENT_CAST_NATURE_BOMB, 0s);
                            break;
                        }

                _events.RescheduleEvent(EVENT_CHECK_DISTANCE, 100ms);
                }
            } break;

            case EVENT_CAST_NATURE_BOMB:
            {
                me->CastSpell(me, SPELL_NATURE_BOMB);
                me->SetDisplayId(11686);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                _events.ScheduleEvent(EVENT_SUICIDE, 2s);

                if (Unit* victim = me->GetThreatManager().GetCurrentVictim())
                    if (victim->GetAura(SPELL_SAPPLING_TARG_IND))
                        victim->RemoveAura(SPELL_SAPPLING_TARG_IND);
            } break;

            case EVENT_SUICIDE:
            {
                me->KillSelf(false);
                if (me->IsSummon())
                    me->DespawnOrUnsummon(0s);
            } break;

            }

        }
    }

private:
    EventMap _events;
    Unit* target = nullptr;
};

struct world_boss_zephyr : public ScriptedAI
{
    world_boss_zephyr(Creature* creature) : ScriptedAI(creature) {}

    void ChangeEventUpdater(bool isDelayed) // is changing _events to _DelayedEvents
    {
        if (isDelayed)
        {
            if (_events.HasEventScheduled(EVENT_CAST_WIND_SURGE))
            {
                _events.CancelEvent(EVENT_CAST_WIND_SURGE);
                _events.ScheduleEvent(EVENT_WIND_SURGE_INDICATOR, 5s);
            }

            _events.CancelEvent(EVENT_CHECK_RANGE);

            _DelayedEvents = _events;
            _events.Reset();
            _events.ScheduleEvent(EVENT_UPDATE_STACKS, _DelayedEvents.GetTimeUntilEvent(EVENT_UPDATE_STACKS));
            _events.ScheduleEvent(EVENT_CAST_BERSERK, _DelayedEvents.GetTimeUntilEvent(EVENT_CAST_BERSERK));
            _events.ScheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, _DelayedEvents.GetTimeUntilEvent(EVENT_CAST_MENDBREAKERS_SHROUD));
        }
        else
        {
            _events = _DelayedEvents;
            _events.ScheduleEvent(EVENT_CHECK_RANGE, 5s);
            _DelayedEvents.Reset();
        }
    }

    void Reset() override
    {
        cTempest = me->GetMap()->GetCreatureBySpawnId(TempestGUID);
        pList.clear();
        Phase = 2;

        _events.Reset();
        me->SetAuraStack(SPELL_ASTRAL_EMPOWERMENT, me, StackCount);
        _events.ScheduleEvent(EVENT_UPDATE_STACKS, 1s);
        _events.ScheduleEvent(EVENT_CAST_BERSERK, 300s);
        _events.ScheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 0ms);
        _events.ScheduleEvent(EVENT_CHECK_RANGE, 2s);
        _events.ScheduleEvent(EVENT_WIND_SURGE_INDICATOR, Seconds(RAND(27, 33)));
        _events.ScheduleEvent(EVENT_CAST_SKY_VORTEX, 60s);
        _events.ScheduleEvent(EVENT_CHECK_TWINS_HEALTH, 3s);

        if (cTempest)
            if (cTempest->IsAlive())
                cTempest->ClearInCombat();
            else
            {
                cTempest->RemoveCorpse();
                cTempest->Respawn();
            }
    }

    void SpellHit(WorldObject* /*caster*/, SpellInfo const* spellInfo)
    {
        if (spellInfo)
            if (spellInfo->Id == SPELL_STORMBOUND_RENEWAL)
            {
                _events.CancelEvent(EVENT_CHECK_RANGE);
                _events.ScheduleEvent(EVENT_CHECK_STORMBOUND_AURA, 1s);
            }
    }

    void JustStartedThreateningMe(Unit* who) override
    {
        if (!IsEngaged()) EngagementStart(who);
        cTempest->GetThreatManager().AddThreat(who, 0.1f);
    }

    void JustEngagedWith(Unit* victim) override
    {
        cTempest = me->GetMap()->GetCreatureBySpawnId(TempestGUID);

        if (cTempest)
            if (cTempest->IsAlive())
                if (cTempest->IsInCombat())
                    return;
                else
                    _events.ScheduleEvent(EVENT_ENGAGE_TWIN, 250ms);
            else
            {
                cTempest->RemoveCorpse();
                cTempest->Respawn();
            }

        Player* InitialTarget = nullptr;

        if (victim->GetOwner())
        {
            if (victim->GetOwner()->IsPlayer())
                InitialTarget = victim->GetOwner()->ToPlayer();
        }
        else if (victim->IsPlayer())
            InitialTarget = victim->ToPlayer();

        if (!InitialTarget)
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[The Elemental Twins] has been engaged.").c_str());

        if (Group* initialGroup = InitialTarget->GetGroup())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been engaged by |Hplayer:" + std::string(initialGroup->GetLeaderName()) + "|h[" + initialGroup->GetLeaderName() + "]|h|r's Group.").c_str());

        return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been engaged by |Hplayer:" + InitialTarget->GetName() + "|h[" + InitialTarget->GetName() + "]|h|r.").c_str());
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
            if (me->isTappedBy(victim->ToPlayer()) || (me->GetLootRecipient()->IsInSameGroupWith(victim->ToPlayer())))
                _events.ScheduleEvent(EVENT_CAST_DEATHS_EMBRACE, 0ms);
    }

    void JustDied(Unit* killer) override
    {
        if (cTempest)
            if (cTempest->IsAlive())
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            else
            {
                cTempest->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);                
                cTempest->SetRespawnTime(me->GetRespawnTime());

                Player* ActualKiller = nullptr;

                if (killer->GetOwner())
                {
                    if (killer->GetOwner()->IsPlayer())
                        ActualKiller = killer->GetOwner()->ToPlayer();
                }
                else if (killer->IsPlayer())
                    ActualKiller = killer->ToPlayer();

                if (!ActualKiller)
                    return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been slain by [" + killer->GetName() + "].").c_str());

                if (Group* sGroup = me->GetLootRecipientGroup())
                    return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been slain by [" + std::string(sGroup->GetLeaderName()) + "]'s Group.").c_str());

                if (Player* sPlayer = me->GetLootRecipient())
                    return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been slain by [" + sPlayer->GetName() + "].").c_str());

                return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been slain by [" + killer->GetName() + ".").c_str());
            }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING) && me->GetChannelSpellId() != SPELL_STORMBOUND_RENEWAL)
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {

            case EVENT_UPDATE_STACKS:
            {
                uint8 Count = 0;
                for (auto* victim : me->GetThreatManager().GetUnsortedThreatList())
                    if (victim->GetVictim()->IsPlayer())
                        if (me->isTappedBy(victim->GetVictim()->ToPlayer()))
                            Count++;

                me->SetAuraStack(SPELL_ASTRAL_EMPOWERMENT, me, StackCount - Count);
                _events.RescheduleEvent(EVENT_UPDATE_STACKS, 3s);
            } break;

            case EVENT_CAST_DEATHS_EMBRACE:
            {
                me->CastSpell(nullptr, SPELL_DEATHS_EMBRACE, true);
                if (cTempest)
                    cTempest->CastSpell(nullptr, SPELL_DEATHS_EMBRACE, true);
            } break;

            case EVENT_CAST_BERSERK:
            {
                me->CastSpell(nullptr, SPELL_BERSERK, true);
            } break;

            case EVENT_CAST_MENDBREAKERS_SHROUD:
            {
                me->CastSpell(nullptr, SPELL_MENDBREAKERS_SHROUD, true);
                _events.RescheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 27s);
            } break;

            case EVENT_ENGAGE_TWIN:
            {
                cTempest = me->GetMap()->GetCreatureBySpawnId(TempestGUID);

                if (cTempest)
                {
                    if (!cTempest->IsAlive())
                        _events.RescheduleEvent(EVENT_ENGAGE_TWIN, 1s);

                    if (!cTempest->IsInCombat())
                    {
                        cTempest->EngageWithTarget(me->GetLootRecipient());
                        cTempest->SetLootRecipient(me->GetLootRecipient());
                    }
                }
            } break;

            case EVENT_CHECK_RANGE:
            {
                if (cTempest)
                {
                    if (cTempest->IsAlive())
                    {
                        if (!cTempest->IsInCombat())
                            cTempest->EngageWithTarget(me->GetLootRecipient());

                        if (me->GetDistance(cTempest->GetPosition()) < 25.0f)
                        {
                            if (!me->HasAura(SPELL_ELEMENTAL_FUSION))
                                me->AddAura(SPELL_ELEMENTAL_FUSION, me);
                        }
                        else
                            if (me->HasAura(SPELL_ELEMENTAL_FUSION))
                                me->RemoveAura(SPELL_ELEMENTAL_FUSION);
                    }
                    else
                        if (me->HasAura(SPELL_ELEMENTAL_FUSION))
                            me->RemoveAura(SPELL_ELEMENTAL_FUSION);
                }
                else
                    if (me->HasAura(SPELL_ELEMENTAL_FUSION))
                        me->RemoveAura(SPELL_ELEMENTAL_FUSION);

                _events.RescheduleEvent(EVENT_CHECK_RANGE, 1s);
            } break;

            case EVENT_WIND_SURGE_INDICATOR:
            {
                if (me->HasAura(SPELL_ELEMENTAL_FUSION))
                {
                    for (auto& ref : me->GetThreatManager().GetModifiableThreatList())
                    {
                        Unit* entity = ref->GetVictim();

                        if (entity)
                            if (me->GetDistance(entity) <= 80.0f)
                                me->CastSpell(entity, SPELL_WIND_SURGE);
                    }
                    _events.RescheduleEvent(EVENT_WIND_SURGE_INDICATOR, 5s);
                }
                else
                {
                    std::vector<Player*> v_NearPlayers;
                    std::vector<Player*> v_FarPlayers;

                    me->GetPlayerListInGrid(v_NearPlayers, 12.0f, true);
                    me->GetPlayerListInGrid(v_FarPlayers, 80.0f, true);

                    for (int i = 0; i < v_NearPlayers.size(); i++)
                    {
                        if (me->GetThreatManager().GetThreat(v_NearPlayers[i]) == 0.0f)
                        {
                            v_NearPlayers.erase(v_NearPlayers.begin() + i);
                            i = 0;
                        }
                    }
                    for (int i = 0; i < v_FarPlayers.size(); i++)
                    {
                        if (me->GetThreatManager().GetThreat(v_FarPlayers[i]) == 0.0f)
                        {
                            v_FarPlayers.erase(v_FarPlayers.begin() + i);
                            i = 0;
                        }
                    }

                    if (v_NearPlayers.size() <= 3)
                    {
                        if (v_FarPlayers.size() <= 3)
                            pList = v_FarPlayers;
                        else
                        {
                            for (int i = 0; i < 3; i++)
                            {
                                uint16 TargetID = urand(0, v_FarPlayers.size());
                                if (me->GetThreatManager().GetCurrentVictim() != v_FarPlayers[TargetID])
                                    pList.push_back(v_FarPlayers[TargetID]);
                                else
                                    i -= 1;
                                v_FarPlayers.erase(v_FarPlayers.begin() + TargetID);
                            }
                        }
                    }
                    else
                    {
                        for (int i = 0; i < 3; i++)
                        {
                            uint16 TargetID = urand(0, v_NearPlayers.size());
                            if (me->GetThreatManager().GetCurrentVictim() != v_NearPlayers[TargetID])
                                pList.push_back(v_NearPlayers[TargetID]);
                            else
                                i -= 1;
                            v_NearPlayers.erase(v_NearPlayers.begin() + TargetID);
                        }
                    }

                    for (auto& target : pList)
                        me->CastSpell(target, SPELL_WIND_SURGE_INDICATOR, true);

                    _events.ScheduleEvent(EVENT_CAST_WIND_SURGE, 3s);
                    _events.SetMinimalDelay(EVENT_CAST_SKY_VORTEX, 3050ms);
                }
            } break;

            case EVENT_CAST_WIND_SURGE:
            {
                for (auto& p : pList)
                {
                    if (p)
                        if (p->IsAlive() && me->GetDistance(p->GetPosition()) <= 80.0f)
                            me->CastSpell(p, SPELL_WIND_SURGE);
                }
                _events.ScheduleEvent(EVENT_WIND_SURGE_INDICATOR, Seconds(RAND(27, 33)));
                pList.clear();
            } break;

            case EVENT_CAST_SKY_VORTEX:
            {
                if (me->HasAura(SPELL_ELEMENTAL_FUSION))
                    me->CastSpell(me, SPELL_SKY_VORTEX, true);
                else
                    me->CastSpell(me, SPELL_SKY_VORTEX);
                _events.RescheduleEvent(EVENT_CAST_SKY_VORTEX, 60s);
            } break;

            case EVENT_CHECK_TWINS_HEALTH:
            {
                if (cTempest->GetHealthPct() <= Phase * 25.0f)
                {
                    _events.ScheduleEvent(EVENT_CHANNEL_STORMBOUND_RENEWAL, 0s);
                    Phase--;
                }
                else
                    _events.RescheduleEvent(EVENT_CHECK_TWINS_HEALTH, 3s);
            } break;

            case EVENT_CHANNEL_STORMBOUND_RENEWAL:
            {
                if (me->GetDistance(cTempest->GetPosition()) >= 10.0f)
                {
                    me->AddAura(SPELL_STORMBOUND_RENEWAL_SHIELD, me);
                    me->CastSpell(cTempest, SPELL_STORMBOUND_RENEWAL);
                    ChangeEventUpdater(true);
                    _events.ScheduleEvent(EVENT_CHECK_DISTANCE, 1s);
                }
                else
                    if (Phase > 0)
                        _events.ScheduleEvent(EVENT_CHECK_TWINS_HEALTH, 3s);
            } break;

            case EVENT_CHECK_DISTANCE:
            {
                if (me->GetDistance(cTempest->GetPosition()) < 10.0f)
                {
                    me->CastStop();
                    me->RemoveAura(SPELL_STORMBOUND_RENEWAL_SHIELD);
                    ChangeEventUpdater(false);

                    if (Phase > 0)
                        _events.ScheduleEvent(EVENT_CHECK_TWINS_HEALTH, 3s);
                }
                else
                    _events.RescheduleEvent(EVENT_CHECK_DISTANCE, 1s);
            } break;

            case EVENT_CHECK_STORMBOUND_AURA:
            {
                if (me->HasAura(SPELL_STORMBOUND_RENEWAL))
                    _events.RescheduleEvent(EVENT_CHECK_STORMBOUND_AURA, 1s);
                else
                    _events.ScheduleEvent(EVENT_CHECK_RANGE, 5s);
            } break;

            }
        }
        if (!me->HasUnitState(UNIT_STATE_CASTING))
            DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    EventMap _DelayedEvents;
    uint8 StackCount = 15;
    Creature* cTempest = nullptr;
    std::vector<Player*> pList;
    uint16 Phase = 2;
};

struct world_boss_tempest : public ScriptedAI
{
    world_boss_tempest(Creature* creature) : ScriptedAI(creature) {}

    void ChangeEventUpdater(bool isDelayed) // is changing _events to _DelayedEvents
    {
        if (isDelayed)
        {
            if (_events.HasEventScheduled(EVENT_CAST_WIND_SURGE))
            {
                _events.CancelEvent(EVENT_CAST_WIND_SURGE);
                _events.ScheduleEvent(EVENT_WIND_SURGE_INDICATOR, 5s);
            }

            _events.CancelEvent(EVENT_CHECK_RANGE);

            _DelayedEvents = _events;
            _events.Reset();
            _events.ScheduleEvent(EVENT_UPDATE_STACKS, _DelayedEvents.GetTimeUntilEvent(EVENT_UPDATE_STACKS));
            _events.ScheduleEvent(EVENT_CAST_BERSERK, _DelayedEvents.GetTimeUntilEvent(EVENT_CAST_BERSERK));
            _events.ScheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, _DelayedEvents.GetTimeUntilEvent(EVENT_CAST_MENDBREAKERS_SHROUD));
        }
        else
        {
            _events = _DelayedEvents;
            _events.ScheduleEvent(EVENT_CHECK_RANGE, 5s);
            _DelayedEvents.Reset();
        }
    }

    void Reset() override
    {
        cZephyr = me->GetMap()->GetCreatureBySpawnId(ZephyrGUID);
        Phase = 2;

        _events.Reset();
        me->SetAuraStack(SPELL_ASTRAL_EMPOWERMENT, me, StackCount);
        _events.ScheduleEvent(EVENT_UPDATE_STACKS, 1000ms);
        _events.ScheduleEvent(EVENT_CAST_BERSERK, 300s);
        _events.ScheduleEvent(EVENT_CAST_NATURES_FURY, 2s);
        _events.ScheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 0ms);
        _events.ScheduleEvent(EVENT_CHECK_TWINS_HEALTH, 3s);
        _events.ScheduleEvent(EVENT_WIND_SURGE_INDICATOR, Seconds(RAND(13, 16)));
        _events.ScheduleEvent(EVENT_SUMMON_LIGHTNING_ORB, 20s);

        if (me->getDeathState() != JUST_RESPAWNED)
            if (cZephyr)
                if (cZephyr->IsAlive())
                    cZephyr->ClearInCombat();
                else
                {
                    cZephyr->RemoveCorpse();
                    cZephyr->Respawn();
                }
    }

    void SpellHit(WorldObject* /*caster*/, SpellInfo const* spellInfo)
    {
        if (spellInfo)
            if (spellInfo->Id == SPELL_STORMBOUND_RENEWAL)
            {
                _events.CancelEvent(EVENT_CHECK_RANGE);
                _events.ScheduleEvent(EVENT_CHECK_STORMBOUND_AURA, 1s);
            }
    }

    void JustStartedThreateningMe(Unit* who) override
    {
        if (!IsEngaged()) EngagementStart(who);
        cZephyr->GetThreatManager().AddThreat(who, 0.1f);
    }

    void JustEngagedWith(Unit* victim) override
    {
        cZephyr = me->GetMap()->GetCreatureBySpawnId(ZephyrGUID);

        if (cZephyr)
            if (cZephyr->IsAlive())
                if (cZephyr->IsInCombat())
                    return;
                else
                    _events.ScheduleEvent(EVENT_ENGAGE_TWIN, 250ms);
            else
            {
                cZephyr->RemoveCorpse();
                cZephyr->Respawn();
            }

        Player* InitialTarget = nullptr;

        if (victim->GetOwner())
        {
            if (victim->GetOwner()->IsPlayer())
                InitialTarget = victim->GetOwner()->ToPlayer();
        }
        else if (victim->IsPlayer())
            InitialTarget = victim->ToPlayer();

        if (!InitialTarget)
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[The Elemental Twins] has been engaged.").c_str());

        if (Group* initialGroup = InitialTarget->GetGroup())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been engaged by |Hplayer:" + std::string(initialGroup->GetLeaderName()) + "|h[" + initialGroup->GetLeaderName() + "]|h|r's Group.").c_str());

        return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been engaged by |Hplayer:" + InitialTarget->GetName() + "|h[" + InitialTarget->GetName() + "]|h|r.").c_str());
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
            if (me->isTappedBy(victim->ToPlayer()) || (me->GetLootRecipient()->IsInSameGroupWith(victim->ToPlayer())))
                _events.ScheduleEvent(EVENT_CAST_DEATHS_EMBRACE, 0ms);
    }

    void JustDied(Unit* killer) override
    {
        if (cZephyr)
            if (cZephyr->IsAlive())
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            else
            {
                cZephyr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                cZephyr->SetRespawnTime(me->GetRespawnTime());

                Player* ActualKiller = nullptr;

                if (killer->GetOwner())
                {
                    if (killer->GetOwner()->IsPlayer())
                        ActualKiller = killer->GetOwner()->ToPlayer();
                }
                else if (killer->IsPlayer())
                    ActualKiller = killer->ToPlayer();

                if (!ActualKiller)
                    return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been slain by [" + killer->GetName() + "].").c_str());

                if (Group* sGroup = me->GetLootRecipientGroup())
                    return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been slain by [" + std::string(sGroup->GetLeaderName()) + "]'s Group.").c_str());

                if (Player* sPlayer = me->GetLootRecipient())
                    return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been slain by [" + sPlayer->GetName() + "].").c_str());

                return sWorld->SendServerMessage(SERVER_MSG_STRING, ("[The Elemental Twins] have been slain by [" + killer->GetName() + ".").c_str());
            }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING) && me->GetChannelSpellId() != SPELL_STORMBOUND_RENEWAL)
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {

            case EVENT_UPDATE_STACKS:
            {
                uint8 Count = 0;
                for (auto* victim : me->GetThreatManager().GetUnsortedThreatList())
                    if (victim->GetVictim()->IsPlayer())
                        if (me->isTappedBy(victim->GetVictim()->ToPlayer()))
                            Count++;

                me->SetAuraStack(SPELL_ASTRAL_EMPOWERMENT, me, StackCount - Count);
                _events.RescheduleEvent(EVENT_UPDATE_STACKS, 3s);
            } break;

            case EVENT_CAST_DEATHS_EMBRACE:
            {
                me->CastSpell(nullptr, SPELL_DEATHS_EMBRACE);
                if (cZephyr)
                    cZephyr->CastSpell(nullptr, SPELL_DEATHS_EMBRACE, true);
            } break;

            case EVENT_CAST_BERSERK:
            {
                me->CastSpell(nullptr, SPELL_BERSERK, true);
            } break;

            case EVENT_CAST_MENDBREAKERS_SHROUD:
            {
                me->CastSpell(nullptr, SPELL_MENDBREAKERS_SHROUD, true);
                _events.RescheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 27s);
                break;
            }

            case EVENT_ENGAGE_TWIN:
            {
                cZephyr = me->GetMap()->GetCreatureBySpawnId(ZephyrGUID);

                if (cZephyr)
                {
                    if (!cZephyr->IsAlive())
                        _events.RescheduleEvent(EVENT_ENGAGE_TWIN, 1s);

                    if (!cZephyr->IsInCombat())
                    {
                        cZephyr->EngageWithTarget(me->GetLootRecipient());
                        cZephyr->SetLootRecipient(me->GetLootRecipient());
                    }
                }
            } break;

            case EVENT_CHECK_RANGE:
            {
                if (cZephyr)
                {
                    if (cZephyr->IsAlive())
                    {
                        if (!cZephyr->IsInCombat())
                            cZephyr->EngageWithTarget(me->GetLootRecipient());

                        if (me->GetDistance(cZephyr->GetPosition()) < 25.0f)
                        {
                            if (!me->HasAura(SPELL_ELEMENTAL_FUSION))
                                me->AddAura(SPELL_ELEMENTAL_FUSION, me);
                        }
                        else
                            if (me->HasAura(SPELL_ELEMENTAL_FUSION))
                                me->RemoveAura(SPELL_ELEMENTAL_FUSION);
                    }
                    else
                        if (me->HasAura(SPELL_ELEMENTAL_FUSION))
                            me->RemoveAura(SPELL_ELEMENTAL_FUSION);
                }
                else
                    if (me->HasAura(SPELL_ELEMENTAL_FUSION))
                        me->RemoveAura(SPELL_ELEMENTAL_FUSION);

                _events.RescheduleEvent(EVENT_CHECK_RANGE, 1s);
            } break;

            case EVENT_CHECK_TWINS_HEALTH:
            {
                if (cZephyr->GetHealthPct() <= Phase * 25.0f)
                {
                    _events.ScheduleEvent(EVENT_CHANNEL_STORMBOUND_RENEWAL, 0s);
                    Phase--;
                }
                else
                    _events.RescheduleEvent(EVENT_CHECK_TWINS_HEALTH, 3s);
            } break;

            case EVENT_CAST_NATURES_FURY:
            {
                if (me->HasAura(SPELL_ELEMENTAL_FUSION))
                {
                    for (auto& ref : me->GetThreatManager().GetModifiableThreatList())
                    {
                        Unit* entity = ref->GetVictim();

                        if (entity)
                            if (me->GetDistance(entity) <= 80.0f)
                                me->CastSpell(entity, SPELL_NATURES_FURY);
                    }
                    _events.RescheduleEvent(EVENT_CAST_NATURES_FURY, 5s);
                }
                else
                {
                    std::vector<Player*> v_NearPlayers;
                    std::vector<Player*> v_FarPlayers;

                    me->GetPlayerListInGrid(v_NearPlayers, 12.0f, true);
                    me->GetPlayerListInGrid(v_FarPlayers, 80.0f, true);

                    for (int i = 0; i < v_NearPlayers.size(); i++)
                    {
                        if (me->GetThreatManager().GetThreat(v_NearPlayers[i]) == 0.0f)
                        {
                            v_NearPlayers.erase(v_NearPlayers.begin() + i);
                            i = 0;
                        }
                    }

                    for (int i = 0; i < v_FarPlayers.size(); i++)
                    {
                        if (me->GetThreatManager().GetThreat(v_FarPlayers[i]) == 0.0f)
                        {
                            v_FarPlayers.erase(v_FarPlayers.begin() + i);
                            i = 0;
                        }
                    }

                    if (v_NearPlayers.size() <= 3)
                    {
                        v_NearPlayers.clear();

                        if (v_FarPlayers.size() <= 3)
                            v_NearPlayers = v_FarPlayers;
                        else
                        {
                            for (int i = 0; i < 3; i++)
                            {
                                uint16 TargetID = urand(0, v_FarPlayers.size());

                                if (me->GetThreatManager().GetCurrentVictim() != v_FarPlayers[TargetID])
                                    v_NearPlayers.push_back(v_FarPlayers[TargetID]);
                                else
                                    i -= 1;
                                v_FarPlayers.erase(v_FarPlayers.begin() + TargetID);
                            }
                        }
                    }
                    else
                    {
                        v_FarPlayers = v_NearPlayers;

                        for (int i = 0; i < 3; i++)
                        {
                            uint16 TargetID = urand(0, v_FarPlayers.size());
                            if (me->GetThreatManager().GetCurrentVictim() != v_FarPlayers[TargetID])
                                v_NearPlayers.push_back(v_FarPlayers[TargetID]);
                            else
                                i -= 1;
                            v_FarPlayers.erase(v_FarPlayers.begin() + TargetID);
                        }
                    }

                    for (auto& target : v_NearPlayers)
                        me->CastSpell(target, SPELL_NATURES_FURY, true);
                }
                _events.RescheduleEvent(EVENT_CAST_NATURES_FURY, Seconds(urand(27, 33)));

                } break;

            case EVENT_SUMMON_LIGHTNING_ORB:
            {
                Position pos;
                if (Unit* unit = SelectTarget(SelectTargetMethod::Random, 0, -15.0f, true, false))
                    me->SummonCreature(WB_SUMMON_LIGHTNING_ORB, unit->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 45s);

                _events.RescheduleEvent(EVENT_SUMMON_LIGHTNING_ORB, me->HasAura(SPELL_ELEMENTAL_FUSION) ? 7s : 30s);
            } break;

            case EVENT_CHANNEL_STORMBOUND_RENEWAL:
            {
                if (me->GetDistance(cZephyr->GetPosition()) >= 10.0f)
                {
                    me->AddAura(SPELL_STORMBOUND_RENEWAL_SHIELD, me);
                    me->CastSpell(cZephyr, SPELL_STORMBOUND_RENEWAL);
                    ChangeEventUpdater(true);
                    _events.ScheduleEvent(EVENT_CHECK_DISTANCE, 1s);
                }
                else
                    if (Phase > 0)
                        _events.ScheduleEvent(EVENT_CHECK_TWINS_HEALTH, 3s);
            } break;

            case EVENT_CHECK_DISTANCE:
            {
                if (me->GetDistance(cZephyr->GetPosition()) < 10.0f)
                {
                    me->CastStop();
                    me->RemoveAura(SPELL_STORMBOUND_RENEWAL_SHIELD);
                    ChangeEventUpdater(false);

                    if (Phase > 0)
                        _events.ScheduleEvent(EVENT_CHECK_TWINS_HEALTH, 3s);
                }
                else
                    _events.RescheduleEvent(EVENT_CHECK_DISTANCE, 1s);
            } break;

            case EVENT_CHECK_STORMBOUND_AURA:
            {
                if (me->HasAura(SPELL_STORMBOUND_RENEWAL))
                    _events.RescheduleEvent(EVENT_CHECK_STORMBOUND_AURA, 1s);
                else
                    _events.ScheduleEvent(EVENT_CHECK_RANGE, 5s);
            } break;

            }
        }

        if (!me->HasUnitState(UNIT_STATE_CASTING))
            DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    EventMap _DelayedEvents;
    uint8 StackCount = 15;
    Creature* cZephyr = nullptr;
    uint16 Phase = 2;
};

struct world_boss_taelon : public ScriptedAI
{
    world_boss_taelon(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        _events.Reset();
        me->SetAuraStack(SPELL_ASTRAL_EMPOWERMENT, me, StackCount);
        _events.ScheduleEvent(EVENT_UPDATE_STACKS, 1s);
        _events.ScheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 0ms);
        _events.ScheduleEvent(EVENT_CAST_BERSERK, 300s);
    }

    void JustEngagedWith(Unit* victim) override
    {
        Player* InitialTarget = nullptr;

        if (victim->GetOwner())
        {
            if (victim->GetOwner()->IsPlayer())
                InitialTarget = victim->GetOwner()->ToPlayer();
        }
        else if (victim->IsPlayer())
            InitialTarget = victim->ToPlayer();

        if (!InitialTarget)
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been engaged.").c_str());

        if (Group* initialGroup = InitialTarget->GetGroup())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been engaged by |Hplayer:" + initialGroup->GetLeaderName() + "|h[" + initialGroup->GetLeaderName() + "]|h|r's Group.").c_str());

        return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been engaged by |Hplayer:" + InitialTarget->GetName() + "|h[" + InitialTarget->GetName() + "]|h|r").c_str());
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
            if (me->isTappedBy(victim->ToPlayer()) || (me->GetLootRecipient()->IsInSameGroupWith(victim->ToPlayer())))
                _events.ScheduleEvent(EVENT_CAST_DEATHS_EMBRACE, 0ms);
    }

    void JustDied(Unit* killer) override
    {
        Player* ActualKiller = nullptr;

        if (killer->GetOwner())
        {
            if (killer->GetOwner()->IsPlayer())
                ActualKiller = killer->GetOwner()->ToPlayer();
        }
        else if (killer->IsPlayer())
            ActualKiller = killer->ToPlayer();

        if (!ActualKiller)
            sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been slain.").c_str());

        if (Group* sGroup = me->GetLootRecipientGroup())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been slain by [" + sGroup->GetLeaderName() + "]'s Group.").c_str());

        if (Player* sPlayer = me->GetLootRecipient())
            return sWorld->SendServerMessage(SERVER_MSG_STRING, std::string("[" + me->GetName() + "] has been slain by [" + sPlayer->GetName() + "].").c_str());
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

            case EVENT_UPDATE_STACKS:
            {
                uint8 Count = 0;
                for (auto* victim : me->GetThreatManager().GetUnsortedThreatList())
                    if (victim->GetVictim()->IsPlayer())
                        if (me->isTappedBy(victim->GetVictim()->ToPlayer()))
                            Count++;

                me->SetAuraStack(SPELL_ASTRAL_EMPOWERMENT, me, StackCount - Count);
                _events.RescheduleEvent(EVENT_UPDATE_STACKS, 3s);
            } break;

            case EVENT_CAST_MENDBREAKERS_SHROUD:
            {
                me->CastSpell(nullptr, SPELL_MENDBREAKERS_SHROUD);
                _events.RescheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 27s);
            } break;

            case EVENT_CAST_BERSERK:
            {
                me->CastSpell(nullptr, SPELL_BERSERK);
            } break;

            case EVENT_CAST_DEATHS_EMBRACE:
            {
                me->CastSpell(nullptr, SPELL_DEATHS_EMBRACE);
            } break;

            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    uint8 StackCount = 19;
};

void AddSC_custom_world_bosses()
{
    RegisterCreatureAI(world_boss_ironbane);
    RegisterCreatureAI(world_boss_vilethorn);
    RegisterCreatureAI(world_boss_zephyr);
    RegisterCreatureAI(vilethorn_sappling);
    RegisterCreatureAI(world_boss_tempest);
    RegisterCreatureAI(world_boss_taelon);
}
