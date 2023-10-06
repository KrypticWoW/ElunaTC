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

#ifndef CUSTOM_AHNKAHET_H_
#define CUSTOM_AHNKAHET_H_

#include "CreatureAIImpl.h"

#define CustomAhnKahetScriptName "custom_instance_ahnkahet"
#define DataHeader         "CAK"

uint32 const EncounterCount = 5;

enum AKDataTypes
{
    // Encounter States/Boss GUIDs
    DATA_ELDER_NADOX                = 0,
    DATA_PRINCE_TALDARAM            = 1,
    DATA_JEDOGA_SHADOWSEEKER        = 2,
    DATA_AMANITAR                   = 3,
    DATA_HERALD_VOLAZJ              = 4,

    // Additional Data
    DATA_SPHERE_1                   = 5,
    DATA_SPHERE_2                   = 6,
    DATA_PRINCE_TALDARAM_PLATFORM   = 7
};

enum AKCreatureIds
{
    NPC_ELDER_NADOX                 = 60229,
    NPC_PRINCE_TALDARAM             = 60228,
    NPC_JEDOGA_SHADOWSEEKER         = 60230,
    NPC_AMANITAR                    = 60236,
    NPC_HERALD_VOLAZJ               = 60231,

    // Elder Nadox
    NPC_AHNKAHAR_GUARDIAN           = 60258,
    NPC_AHNKAHAR_SWARMER            = 60259,

    // Jedoga Shadowseeker
    NPC_TWILIGHT_INITIATE           = 60260,
    NPC_TWILIGHT_VOLUNTEER          = 60261,
    NPC_TWILIGHT_WORSHIPPER         = 60232,
    NPC_JEDOGA_CONTROLLER           = 60262,

    // Amanitar
    NPC_HEALTHY_MUSHROOM            = 60263,
    NPC_POISONOUS_MUSHROOM          = 60264,

    // Herald Volazj
    //NPC_TWISTED_VISAGE_1          = 30621,
    //NPC_TWISTED_VISAGE_2          = 30622,
    //NPC_TWISTED_VISAGE_3          = 30623,
    //NPC_TWISTED_VISAGE_4          = 30624,
    NPC_TWISTED_VISAGE              = 60265
};

enum AKGameObjectIds
{
    GO_PRINCE_TALDARAM_GATE         = 192236,
    GO_PRINCE_TALDARAM_PLATFORM     = 193564,
    GO_SPHERE_1                     = 300012,
    GO_SPHERE_2                     = 300013
};

enum AKSpellIds
{
    SPELL_HEROIC_BUFF = 102003
};

template <class AI, class T>
inline AI* GetCustomAhnKahetAI(T* obj)
{
    return GetInstanceAI<AI>(obj, CustomAhnKahetScriptName);
}

#define RegisterCustomAhnKahetCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetCustomAhnKahetAI)
#define RegisterCustomAhnKahetGameObjectAI(ai_name) RegisterGameObjectAIWithFactory(ai_name, GetCustomAhnKahetAI)

#endif // AHNKAHET_H_
