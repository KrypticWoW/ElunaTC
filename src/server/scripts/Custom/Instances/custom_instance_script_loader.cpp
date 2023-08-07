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

// Deadmines
void AddSC_custom_instance_deadmines();
void AddSC_custom_boss_mr_smite();
void AddSC_custom_boss_vancleef();

// Zul'Farrak
void AddSC_custom_boss_zum_rah();
void AddSC_custom_zulfarrak();
void AddSC_custom_instance_zulfarrak();

// Scarlet Raid
void AddSC_instance_scarlet_raid();

void AddCustomInstanceScripts()
{
    // Deadmines
    AddSC_custom_instance_deadmines();
    AddSC_custom_boss_mr_smite();
    AddSC_custom_boss_vancleef();

    // Zul'Farrak
    AddSC_custom_boss_zum_rah();
    AddSC_custom_zulfarrak();
    AddSC_custom_instance_zulfarrak();

    // Scarlet Raid
    AddSC_instance_scarlet_raid();
}
