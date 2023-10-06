#include <ScriptedCreature.h>
#include <SpellAuras.h>
#include <Chat.h>
#include <Pet.h>
#include <Group.h>
#include <map.h>

constexpr uint32 TempestGUID = 214426;
constexpr uint32 ZephyrGUID = 214425;

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

    // Zephyr & Tempest
    EVENT_ENGAGE_TWIN,
    EVENT_CHECK_RANGE,
    EVENT_CAST_HURRICANE,
};

enum SPELLS
{
    // Global
    SPELL_ASTRAL_EMPOWERMENT = 103000,
    SPELL_MENDBREAKERS_SHROUD = 103001,
    SPELL_BERSERK = 103002,
    SPELL_DEATHS_EMBRACE = 103003,

    // Ironbane
    SPELL_SPELL_REFLECTION = 103010,
    SPELL_RETALIATION = 103011,
    SPELL_SHOCKWAVE = 103012,

    // Zephyr & Tempest
    SPELL_ELEMENTAL_FUSION = 103016,
    SPELL_HURRICANE = 103017,
};

struct world_boss_ironbane : public ScriptedAI
{
    world_boss_ironbane(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        _events.Reset();
        aura = nullptr;
        aura = me->AddAura(SPELL_ASTRAL_EMPOWERMENT, me);
        aura->SetStackAmount(StackCount);
        _events.ScheduleEvent(EVENT_UPDATE_STACKS, 1000ms);
        _events.ScheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 0ms);
        _events.ScheduleEvent(EVENT_CAST_BERSERK, 300s);
        _events.ScheduleEvent(EVENT_CAST_SPELL_REFLECTION, 10s);
        _events.ScheduleEvent(EVENT_CAST_RETALIATION, 15s);
        _events.ScheduleEvent(EVENT_CAST_SHOCKWAVE, 25s);
    }

    void JustAppeared() override
    {

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

        //"|Hplayer:" + sender->GetName() + "|h[" + sender->GetName() + "]|h|r

        if (Group* initialGroup = InitialTarget->GetGroup())
            return ChatHandler(victim->ToPlayer()->GetSession()).SendGlobalSysMessage(std::string("[" + me->GetName() + "] has been engaged by |Hplayer:" + initialGroup->GetLeaderName() + "|h[" + initialGroup->GetLeaderName() + "]|h|r's Group.").c_str());

        if (InitialTarget)
            ChatHandler(victim->ToPlayer()->GetSession()).SendGlobalSysMessage(std::string("[" + me->GetName() + "] has been engaged by |Hplayer:" + InitialTarget->GetName() + "|h[" + InitialTarget->GetName() + "]|h|r").c_str());
        else
            ChatHandler(victim->ToPlayer()->GetSession()).SendGlobalSysMessage(std::string("[" + me->GetName() + "] has been engaged by [" + victim->GetName() + "].").c_str());
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
            if (me->isTappedBy(victim->ToPlayer()) || me->GetLootRecipient()->IsInSameGroupWith(victim->GetVictim()->ToPlayer()))
                _events.ScheduleEvent(EVENT_CAST_DEATHS_EMBRACE, 0ms);
    }

    void JustDied(Unit* killer) override
    {
        if (Group* sGroup = me->GetLootRecipientGroup())
            return ChatHandler(killer->ToPlayer()->GetSession()).SendGlobalSysMessage(std::string("[" + me->GetName() + "] has been slain by [" + sGroup->GetLeaderName() + "]'s Group.").c_str());

        if (Player* sPlayer = me->GetLootRecipient())
            return ChatHandler(killer->ToPlayer()->GetSession()).SendGlobalSysMessage(std::string("[" + me->GetName() + "] has been slain by [" + sPlayer->GetName() + "].").c_str());

        if (killer)
            ChatHandler(killer->ToPlayer()->GetSession()).SendGlobalSysMessage(std::string("[" + me->GetName() + "] has been slain by [" + killer->GetName() + "].").c_str());
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
                    {
                        if (me->isTappedBy(victim->GetVictim()->ToPlayer()))
                        {
                            Count++;
                            continue;
                        }

                        if (auto info = me->GetLootRecipient())
                            if (info->IsInSameGroupWith(victim->GetVictim()->ToPlayer()))
                                Count++;
                    }

                if (aura)
                    aura->SetStackAmount(StackCount - Count);
                _events.RescheduleEvent(EVENT_UPDATE_STACKS, 3s);
                break;
            }

            case EVENT_CAST_MENDBREAKERS_SHROUD:
            {
                me->CastSpell(nullptr, SPELL_MENDBREAKERS_SHROUD);
                _events.RescheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 27s);
                break;
            }

            case EVENT_CAST_BERSERK:
            {
                me->CastSpell(nullptr, SPELL_BERSERK);
                break;
            }

            case EVENT_CAST_DEATHS_EMBRACE:
            {
                me->CastSpell(nullptr, SPELL_DEATHS_EMBRACE);
                break;
            }

            case EVENT_CAST_SPELL_REFLECTION:
            {
                me->CastSpell(nullptr, SPELL_SPELL_REFLECTION);
                _events.RescheduleEvent(EVENT_CAST_SPELL_REFLECTION, 60s);
                break;
            }

            case EVENT_CAST_RETALIATION:
            {
                me->CastSpell(nullptr, SPELL_RETALIATION);
                _events.RescheduleEvent(EVENT_CAST_RETALIATION, 150s);
                break;
            }

            case EVENT_CAST_SHOCKWAVE:
            {
                me->CastSpell(nullptr, SPELL_SHOCKWAVE);
                _events.RescheduleEvent(EVENT_CAST_SHOCKWAVE, 25s);
                break;
            }

            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    Aura* aura = nullptr;
    uint8 StackCount = 19;
};

struct world_boss_vilethorn : public ScriptedAI
{
    world_boss_vilethorn(Creature* creature) : ScriptedAI(creature) {}

private:
    EventMap _events;
    Aura* aura = nullptr;
    uint8 StackCount = 17;
};

struct world_boss_zephyr : public ScriptedAI
{
    world_boss_zephyr(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        cTempest = me->GetMap()->GetCreatureBySpawnId(TempestGUID);

        _events.Reset();
        aura = nullptr;
        aura = me->AddAura(SPELL_ASTRAL_EMPOWERMENT, me);
        aura->SetStackAmount(StackCount);
        _events.ScheduleEvent(EVENT_UPDATE_STACKS, 1000ms);
        _events.ScheduleEvent(EVENT_CAST_BERSERK, 300s);
        _events.ScheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 0ms);
        _events.ScheduleEvent(EVENT_CHECK_RANGE, 2s);

        if (cTempest)
            if (cTempest->IsAlive())
                cTempest->ClearInCombat();
            else
            {
                cTempest->RemoveCorpse();
                cTempest->Respawn();
            }
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

        if (Group* initialGroup = InitialTarget->GetGroup())
            return ChatHandler(victim->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been engaged by |Hplayer:" + std::string(initialGroup->GetLeaderName()) + "|h[" + initialGroup->GetLeaderName() + "]|h|r's Group.").c_str());

        if (InitialTarget)
            ChatHandler(victim->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been engaged by |Hplayer:" + InitialTarget->GetName() + "|h[" + InitialTarget->GetName() + "]|h|r.").c_str());
        else
            ChatHandler(victim->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been engaged by [" + victim->GetName() + "].").c_str());
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
            if (me->isTappedBy(victim->ToPlayer()) || me->GetLootRecipient()->IsInSameGroupWith(victim->GetVictim()->ToPlayer()))
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
                std::cout << "Zephyr Respawn Time: " << me->GetRespawnTime() << std::endl;
                std::cout << "Tempest Respawn Time: " << cTempest->GetRespawnTime() << std::endl;
                
                me->SetRespawnTime(cTempest->GetRespawnTime());
                
                std::cout << "Zephyr Respawn Time: " << me->GetRespawnTime() << std::endl;
                std::cout << "Tempest Respawn Time: " << cTempest->GetRespawnTime() << std::endl;

                if (Group* sGroup = me->GetLootRecipientGroup())
                    return ChatHandler(killer->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been slain by [" + std::string(sGroup->GetLeaderName()) + "]'s Group.").c_str());

                if (Player* sPlayer = me->GetLootRecipient())
                    return ChatHandler(killer->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been slain by [" + sPlayer->GetName() + "].").c_str());

                if (killer)
                    ChatHandler(killer->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been slain by [" + killer->GetName() + "].").c_str());
            }
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
                    {
                        if (me->isTappedBy(victim->GetVictim()->ToPlayer()))
                        {
                            Count++;
                            continue;
                        }

                        if (auto info = me->GetLootRecipient())
                            if (info->IsInSameGroupWith(victim->GetVictim()->ToPlayer()))
                                Count++;
                    }

                if (aura)
                    aura->SetStackAmount(StackCount - Count);
                _events.RescheduleEvent(EVENT_UPDATE_STACKS, 3s);
            } break;

            case EVENT_CAST_DEATHS_EMBRACE:
            {
                me->CastSpell(nullptr, SPELL_DEATHS_EMBRACE);
                if (cTempest)
                    cTempest->CastSpell(nullptr, SPELL_DEATHS_EMBRACE);
            } break;

            case EVENT_CAST_BERSERK:
            {
                me->CastSpell(nullptr, SPELL_BERSERK);
                break;
            }

            case EVENT_CAST_MENDBREAKERS_SHROUD:
            {
                me->CastSpell(nullptr, SPELL_MENDBREAKERS_SHROUD);
                _events.RescheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 27s);
                break;
            }

            case EVENT_ENGAGE_TWIN:
            {
                cTempest = me->GetMap()->GetCreatureBySpawnId(TempestGUID);

                if (cTempest)
                {
                    if (!cTempest->IsAlive())
                        _events.RescheduleEvent(EVENT_ENGAGE_TWIN, 1s);

                    if (!cTempest->IsInCombat())
                        cTempest->EngageWithTarget(me->GetLootRecipient());
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

            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    Aura* aura = nullptr;
    uint8 StackCount = 15;
    Creature* cTempest = nullptr;
};

struct world_boss_tempest : public ScriptedAI
{
    world_boss_tempest(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        cZephyr = me->GetMap()->GetCreatureBySpawnId(ZephyrGUID);

        _events.Reset();
        aura = nullptr;
        aura = me->AddAura(SPELL_ASTRAL_EMPOWERMENT, me);
        aura->SetStackAmount(StackCount);
        _events.ScheduleEvent(EVENT_UPDATE_STACKS, 1000ms);
        _events.ScheduleEvent(EVENT_CAST_BERSERK, 300s);
        _events.ScheduleEvent(EVENT_CHECK_RANGE, 2s);
        _events.ScheduleEvent(EVENT_CAST_MENDBREAKERS_SHROUD, 0ms);
        //_events.ScheduleEvent(EVENT_CAST_HURRICANE, 8s);

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

        if (Group* initialGroup = InitialTarget->GetGroup())
            return ChatHandler(victim->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been engaged by |Hplayer:" + std::string(initialGroup->GetLeaderName()) + "|h[" + initialGroup->GetLeaderName() + "]|h|r's Group.").c_str());

        if (InitialTarget)
            ChatHandler(victim->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been engaged by |Hplayer:" + InitialTarget->GetName() + "|h[" + InitialTarget->GetName() + "]|h|r.").c_str());
        else
            ChatHandler(victim->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been engaged by [" + victim->GetName() + "].").c_str());
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
            if (me->isTappedBy(victim->ToPlayer()) || me->GetLootRecipient()->IsInSameGroupWith(victim->GetVictim()->ToPlayer()))
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
                me->SetRespawnTime(cZephyr->GetRespawnTime());

                if (Group* sGroup = me->GetLootRecipientGroup())
                    return ChatHandler(killer->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been slain by [" + std::string(sGroup->GetLeaderName()) + "]'s Group.").c_str());

                if (Player* sPlayer = me->GetLootRecipient())
                    return ChatHandler(killer->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been slain by [" + sPlayer->GetName() + "].").c_str());

                if (killer)
                    ChatHandler(killer->ToPlayer()->GetSession()).SendGlobalSysMessage(("[The Elemental Twins] have been slain by [" + killer->GetName() + "].").c_str());
            }
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
                    {
                        if (me->isTappedBy(victim->GetVictim()->ToPlayer()))
                        {
                            Count++;
                            continue;
                        }

                        if (auto info = me->GetLootRecipient())
                            if (info->IsInSameGroupWith(victim->GetVictim()->ToPlayer()))
                                Count++;
                    }

                if (aura)
                    aura->SetStackAmount(StackCount - Count);
                _events.RescheduleEvent(EVENT_UPDATE_STACKS, 3s);
            } break;

            case EVENT_CAST_DEATHS_EMBRACE:
            {
                me->CastSpell(nullptr, SPELL_DEATHS_EMBRACE);
                if (cZephyr)
                    cZephyr->CastSpell(nullptr, SPELL_DEATHS_EMBRACE);
            } break;

            case EVENT_CAST_BERSERK:
            {
                me->CastSpell(nullptr, SPELL_BERSERK);
            } break;

            case EVENT_CAST_MENDBREAKERS_SHROUD:
            {
                me->CastSpell(nullptr, SPELL_MENDBREAKERS_SHROUD);
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
                        cZephyr->EngageWithTarget(me->GetLootRecipient());
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

            case EVENT_CAST_HURRICANE:
            {
                me->CastSpell(nullptr, SPELL_HURRICANE);
                _events.RescheduleEvent(EVENT_CAST_HURRICANE, 32s);
            } break;

            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    Aura* aura = nullptr;
    uint8 StackCount = 15;
    Creature* cZephyr = nullptr;
};

void AddSC_custom_world_bosses()
{
    RegisterCreatureAI(world_boss_ironbane);
    //RegisterCreatureAI(world_boss_vilethorn);
    RegisterCreatureAI(world_boss_zephyr);
    RegisterCreatureAI(world_boss_tempest);
}
