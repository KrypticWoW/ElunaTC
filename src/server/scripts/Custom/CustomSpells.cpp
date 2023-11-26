#include <SpellMgr.h>
#include <SpellAuraEffects.h>
#include <iostream>

std::map<uint32, uint32> AllowedSpells = { {50796, 50796} };

// -101011 - Duplicate Spell
class spell_custom_dupilcate_spell : public AuraScript
{
    PrepareAuraScript(spell_custom_dupilcate_spell);

    bool IsAllowedSpell(uint32 spellID)
    {
        return AllowedSpells.find(spellID) != AllowedSpells.end();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        uint32 spellId = spellInfo->Id;
        SpellInfo const* FirstRank = spellInfo->GetFirstRankSpell();

        if (FirstRank)
        {
            if (IsAllowedSpell(FirstRank->Id))
                eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), spellId, static_cast<TriggerCastFlags>(540671));
        }
        else
            if (IsAllowedSpell(spellId))
                eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), spellId, static_cast<TriggerCastFlags>(540671));
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
