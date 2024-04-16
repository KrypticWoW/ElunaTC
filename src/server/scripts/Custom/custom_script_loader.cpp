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

// This is where scripts' loading functions should be declared:

// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddSC_DynamicTeleporter();
void AddSC_DynamicUpgradeItem();
void AddSC_WeaponForgerCreature();
void AddSC_CustomEventScripts();
void AddSC_AccountBoundItem();
void AddSC_CustomArtifactItem();
void AddSC_CustomNeckItem();
void AddSC_custom_world_bosses();
void AddSC_custom_spell_scripts();
void AddSC_CustomEventCreature();
void AddSC_TierTestNpc(); // Remove This Later
void AddSC_GuardianCreatures();

// void AddCustomInstanceScripts();

void AddCustomScripts()
{
    AddSC_DynamicTeleporter();
    AddSC_DynamicUpgradeItem();
    AddSC_WeaponForgerCreature();
    AddSC_CustomEventScripts();
    AddSC_AccountBoundItem();
    AddSC_CustomArtifactItem();
    AddSC_CustomNeckItem();
    AddSC_custom_world_bosses();
    AddSC_custom_spell_scripts();
    AddSC_CustomEventCreature();
    AddSC_TierTestNpc(); // Remove This Later
    AddSC_GuardianCreatures();

    // AddCustomInstanceScripts();
}
