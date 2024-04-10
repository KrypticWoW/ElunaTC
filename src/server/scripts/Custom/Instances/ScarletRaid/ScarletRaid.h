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

#ifndef DEF_CUSTOM_SCARLETMONASTERY_H
#define DEF_CUSTOM_SCARLETMONASTERY_H

#include "CreatureAIImpl.h"

#define CustomSMScriptName "instance_scarlet_raid"
#define DataHeader "CSM"

uint32 constexpr EncounterCount = 7;

enum CSMData
{
    // Encounters
    BOSS_RHAHKZOR = 0,
    BOSS_SNEED = 1,
    BOSS_GILNID = 2,
    BOSS_MR_SMITE = 3,
    BOSS_GREENSKIN = 4,
    BOSS_VANCLEEF = 5,
    BOSS_COOKIE = 6,

    // Additional
    EVENT_STATE = 7
};

//enum DMData64
//{
//    DATA_SMITE_CHEST
//};

enum DMGameObjectsIds
{
    GO_FACTORY_DOOR = 13965,
    GO_MAST_ROOM_DOOR = 16400,
    GO_FOUNDRY_DOOR = 16399,
    GO_IRONCLAD_DOOR = 16397,
    GO_DEFIAS_CANNON = 16398,
    GO_DOOR_LEVER = 300010,
    GO_MR_SMITE_CHEST = 144111
};

enum DMCreaturesIds
{
    NPC_RHAHKZOR = 60007,
    NPC_SNEED = 60023,
    NPC_GILNID = 60016,
    NPC_MR_SMITE = 60009,
    NPC_GREENSKIN = 60010,
    NPC_VANCLEEF = 60004,
    NPC_COOKIE = 60008,
    NPC_BLACKGUARD = 60003
};

template <class AI, class T>
inline AI* GetCustomScarletMonasteryAI(T* obj)
{
    return GetInstanceAI<AI>(obj, CustomSMScriptName);
}

#define RegisterCustomScarletMonasteryCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetCustomScarletMonasteryAI)

#endif
