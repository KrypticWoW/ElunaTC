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

// Shattered Halls
void AddSC_custom_boss_grand_warlock_nethekurse();
void AddSC_custom_boss_warbringer_omrogg();
void AddSC_custom_boss_warchief_kargath_bladefist();
void AddSC_custom_shattered_halls();
void AddSC_custom_instance_shattered_halls();

// Azjol-Nerub - Ahn'kahet
void AddSC_custom_boss_elder_nadox();
void AddSC_custom_boss_taldaram();
void AddSC_custom_boss_amanitar();
void AddSC_custom_boss_jedoga_shadowseeker();
void AddSC_custom_boss_volazj();
void AddSC_custom_instance_ahnkahet();
void AddSC_custom_ahnkahet();

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

    //HC Shattered Halls
    AddSC_custom_boss_grand_warlock_nethekurse();
    AddSC_custom_boss_warbringer_omrogg();
    AddSC_custom_boss_warchief_kargath_bladefist();
    AddSC_custom_shattered_halls();
    AddSC_custom_instance_shattered_halls();

    // Azjol-Nerub - Ahn'kahet
    AddSC_custom_boss_elder_nadox();
    AddSC_custom_boss_taldaram();
    AddSC_custom_boss_amanitar();
    AddSC_custom_boss_jedoga_shadowseeker();
    AddSC_custom_boss_volazj();
    AddSC_custom_instance_ahnkahet();
    AddSC_custom_ahnkahet();

    // Scarlet Raid
    AddSC_instance_scarlet_raid();
}
