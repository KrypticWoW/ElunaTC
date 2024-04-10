#include "ScriptMgr.h"
#include "ScarletRaid.h"
#include "InstanceScript.h"

class instance_scarlet_raid : public InstanceMapScript
{
public:
    instance_scarlet_raid() : InstanceMapScript(CustomSMScriptName, 725) { }

    struct instance_scarlet_raid_InstanceMapScript : public InstanceScript
    {
        instance_scarlet_raid_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_scarlet_raid_InstanceMapScript(map);
    }
};

void AddSC_instance_scarlet_raid()
{
    new instance_scarlet_raid();
}
