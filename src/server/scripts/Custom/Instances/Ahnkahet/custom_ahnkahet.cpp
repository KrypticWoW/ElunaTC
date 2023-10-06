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

#include "DBCStores.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "SpellMgr.h"
#include "UnitAI.h"

// 102025 - Combined Toxins
class custom_spell_ahnkahet_combined_toxins : public AuraScript
{
    PrepareAuraScript(custom_spell_ahnkahet_combined_toxins);

    bool CheckProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        // only procs on poisons (damage class check to exclude stuff like Envenom)
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        return (spellInfo && spellInfo->Dispel == DISPEL_POISON && spellInfo->DmgClass != SPELL_DAMAGE_CLASS_MELEE);
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(custom_spell_ahnkahet_combined_toxins::CheckProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_DAMAGE);
    }
};

// 102027, 102028 - Shadow Blast
class custom_spell_ahnkahet_shadow_blast : public SpellScript
{
    PrepareSpellScript(custom_spell_ahnkahet_shadow_blast);

    void HandleDamageCalc(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        if (!target)
            return;

        SetHitDamage(target->GetMaxHealth() * GetEffectInfo().BasePoints / 100);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(custom_spell_ahnkahet_shadow_blast::HandleDamageCalc, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

enum ShadowSickle
{
    SPELL_SHADOW_SICKLE_TRIGGERED   = 56701
};

// 102029, 102030 - Shadow Sickle
class custom_spell_ahnkahet_shadow_sickle : public AuraScript
{
    PrepareAuraScript(custom_spell_ahnkahet_shadow_sickle);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHADOW_SICKLE_TRIGGERED });
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        Unit* owner = GetUnitOwner();

        uint32 spellId = sSpellMgr->GetSpellIdForDifficulty(SPELL_SHADOW_SICKLE_TRIGGERED, owner);
        if (!spellId)
            return;

        if (owner->IsAIEnabled())
            if (Unit* target = owner->GetAI()->SelectTarget(SelectTargetMethod::Random, 0, 40.f))
                owner->CastSpell(target, spellId, CastSpellExtraArgs(aurEff).SetTriggerFlags(TriggerCastFlags::TRIGGERED_FULL_MASK));
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(custom_spell_ahnkahet_shadow_sickle::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 102031, 102032, 102033, 102031- Creature - Yogg-Saron Whisper
class custom_spell_ahnkahet_yogg_saron_whisper : public SpellScript
{
    PrepareSpellScript(custom_spell_ahnkahet_yogg_saron_whisper);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return sObjectMgr->GetBroadcastText(uint32(spellInfo->GetEffect(EFFECT_0).CalcValue())) &&
            sSoundEntriesStore.LookupEntry(uint32(spellInfo->GetEffect(EFFECT_1).CalcValue()));
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Player* player = GetHitPlayer())
                caster->Unit::Whisper(uint32(GetEffectValue()), player, false);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Player* player = GetHitPlayer())
            player->PlayDistanceSound(uint32(GetEffectValue()), player);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(custom_spell_ahnkahet_yogg_saron_whisper::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnEffectHitTarget += SpellEffectFn(custom_spell_ahnkahet_yogg_saron_whisper::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_custom_ahnkahet()
{
    RegisterSpellScript(custom_spell_ahnkahet_combined_toxins);
    RegisterSpellScript(custom_spell_ahnkahet_shadow_blast);
    RegisterSpellScript(custom_spell_ahnkahet_shadow_sickle);
    RegisterSpellScript(custom_spell_ahnkahet_yogg_saron_whisper);
}
