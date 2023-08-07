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

#ifndef DEF_ZF_H
#define DEF_ZF_H

#include "CreatureAIImpl.h"

#define CustomZFScriptName "custom_instance_zulfarrak"
#define DataHeader "CZF"

enum ZFEntries
{
    ENTRY_ZUM_RAH       = 60058,
    ENTRY_BLY           = 60061,
    ENTRY_RAVEN         = 60062,
    ENTRY_ORO           = 60063,
    ENTRY_WEEGLI        = 60064,
    ENTRY_MURTA         = 60065,

    GO_END_DOOR         = 146084,

    EVENT_PYRAMID       = 1,
    EVENT_GAHZRILLA
};

enum ZFDataTypes
{
    DATA_ZUM_RAH = 0
};

enum ZFPyramidPhases
{
    PYRAMID_NOT_STARTED, //default
    PYRAMID_CAGES_OPEN, //happens in GO hello for cages
    PYRAMID_ARRIVED_AT_STAIR, //happens in Weegli's movementinform
    PYRAMID_WAVE_1,
    PYRAMID_PRE_WAVE_2,
    PYRAMID_WAVE_2,
    PYRAMID_PRE_WAVE_3,
    PYRAMID_WAVE_3,
    PYRAMID_KILLED_ALL_TROLLS,
};

template <class AI, class T>
inline AI* GetCustomZulFarrakAI(T* obj)
{
    return GetInstanceAI<AI>(obj, CustomZFScriptName);
}

#endif
