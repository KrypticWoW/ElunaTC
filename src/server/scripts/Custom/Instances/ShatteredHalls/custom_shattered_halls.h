/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEF_CUSTOM_SHATTERED_H
#define DEF_CUSTOM_SHATTERED_H

#include "CreatureAIImpl.h"

#define CustomSHScriptName "custom_instance_shattered_halls"
#define DataHeader "CSH"

uint32 const EncounterCount          = 4;
uint32 const VictimCount             = 3;

enum SHDataTypes
{
    DATA_NETHEKURSE                  = 0,
    DATA_PORUNG                      = 1,
    DATA_OMROGG                      = 2,
    DATA_KARGATH                     = 3,

    DATA_SHATTERED_EXECUTIONER       = 4,
    DATA_PRISONERS_EXECUTED          = 5,

    DATA_TEAM_IN_INSTANCE            = 6,

    DATA_FIRST_PRISONER,
    DATA_SECOND_PRISONER,
    DATA_THIRD_PRISONER
};

enum SHCreatureIds
{
    NPC_GRAND_WARLOCK_NETHEKURSE     = 60136,
    NPC_BLOOD_GUARD_PORUNG           = 60162,
    NPC_KARGATH_BLADEFIST            = 60137,

    NPC_SHATTERED_EXECUTIONER        = 60163,

    // Alliance Ids
    NPC_RANDY_WHIZZLESPROCKET        = 60140,

    NPC_CAPTAIN_ALINA                = 60164,
    NPC_ALLIANCE_VICTIM_1            = 60165,
    NPC_ALLIANCE_VICTIM_2            = 60166,

    // Horde Ids
    NPC_DRISELLA                     = 60196,

    NPC_CAPTAIN_BONESHATTER          = 60197,
    NPC_HORDE_VICTIM_1               = 60198,
    NPC_HORDE_VICTIM_2               = 60199
};

enum SHGameobjectIds
{
    GO_GRAND_WARLOCK_CHAMBER_DOOR_1  = 182539,
    GO_GRAND_WARLOCK_CHAMBER_DOOR_2  = 182540
};

enum SHQuestIds
{
    QUEST_IMPRISONED_A               = 9524,
    QUEST_IMPRISONED_H               = 9525
};

enum SHInstanceSpells
{
    SPELL_HEROIC_BUFF                = 102003,
    SPELL_KARGATH_EXECUTIONER_1      = 102004,
    SPELL_KARGATH_EXECUTIONER_2      = 102005,
    SPELL_KARGATH_EXECUTIONER_3      = 102006,

    SPELL_REMOVE_KARGATH_EXECUTIONER = 102007
};

enum SHActions
{
    ACTION_EXECUTIONER_TAUNT = 1
};

extern Position const CustomExecutioner;

struct FactionSpawnerHelper
{
    FactionSpawnerHelper(uint32 allianceEntry, uint32 hordeEntry, Position const& pos) : _allianceNPC(allianceEntry), _hordeNPC(hordeEntry), _spawnPos(pos) { }

    inline uint32 operator()(uint32 teamID) const { return teamID == ALLIANCE ? _allianceNPC : _hordeNPC; }
    inline Position const& GetPos() const { return _spawnPos; }

private:
    uint32 const _allianceNPC;
    uint32 const _hordeNPC;
    Position const _spawnPos;
};

FactionSpawnerHelper const executionerVictims[VictimCount] =
{
    { NPC_CAPTAIN_ALINA,     NPC_CAPTAIN_BONESHATTER, { 138.8807f, -84.22707f, 1.992269f, 0.06981317f } },
    { NPC_ALLIANCE_VICTIM_1, NPC_HORDE_VICTIM_1,      { 151.2411f, -91.02930f, 2.019741f, 1.57079600f } },
    { NPC_ALLIANCE_VICTIM_2, NPC_HORDE_VICTIM_2,      { 151.0459f, -77.51981f, 2.021008f, 4.74729500f } }
};

template <class AI, class T>
inline AI* GetCustomShatteredHallsAI(T* obj)
{
    return GetInstanceAI<AI>(obj, CustomSHScriptName);
}

#endif