#include <SpellMgr.h>
#include <SpellAuraEffects.h>
#include <iostream>

std::set<uint32> AllowedSpells =
{
/* Priest */    14914, 585, 8092, 32379,
/* Shaman */    421, 8042, 8050, 8056, 51502, 403,
/* Mage */      44425, 30451, 2136, 133, 44614, 11366, 2948, 116, 30455,
/* Warlock */   48181, 50796, 17962, 348, 29722, 5676, 686, 17877, 6353,
/* Druid */     8921, 2912, 5176
};

// -101011 - Duplicate Spell
class spell_custom_dupilcate_spell : public AuraScript
{
    PrepareAuraScript(spell_custom_dupilcate_spell);

    bool IsAllowedSpell(uint32 spellID)
    {
        return true;
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
