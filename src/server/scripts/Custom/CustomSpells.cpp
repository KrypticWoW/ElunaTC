#include <SpellMgr.h>

// -101011 - Duplicate Spell
class spell_custom_dupilcate_spell : public AuraScript
{
    PrepareAuraScript(spell_custom_dupilcate_spell);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        uint32 spellId = spellInfo->Id;

        if (spellInfo->HasOnlyDamageEffects() && !spellInfo->IsChanneled())
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), spellId, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_custom_dupilcate_spell::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

void AddSC_custom_spell_scripts()
{
    RegisterSpellScript(spell_custom_dupilcate_spell);
}
