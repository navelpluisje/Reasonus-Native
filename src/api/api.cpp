#include "api.hpp"

#include "reaper_vararg.hpp"
#include <WDL/wdltypes.h>
#include <reaper_plugin_functions.h>
#include "get_reasonus_version.hpp"
#include "fp_8_get_track_offset.hpp"
#include "fp_v2_get_controlled_track.hpp"
#include "fp_8_control_plugin.hpp"
#include "fp_8_control_thing_under_pointer.hpp"
#include "fp_8_filter_by_name.hpp"
#include "fp_8_set_fader_mode.hpp"

namespace REASONUS_API {
    void Register() {
        plugin_register("API_ReaSonus_GetVersion", reinterpret_cast<void *>(GetReasonusVersion));
        plugin_register("APIdef_ReaSonus_GetVersion", (void *) defstring_GetReasonusVersion);
        plugin_register("APIvararg_ReaSonus_GetVersion",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&GetReasonusVersion>));

        plugin_register("API_ReaSonus_FPv2_GetControlledTrack", reinterpret_cast<void *>(FPv2GetControlledTrack));
        plugin_register("APIdef_ReaSonus_FPv2_GetControlledTrack", (void *) defstring_FPv2GetControlledTrack);
        plugin_register("APIvararg_ReaSonus_FPv2_GetControlledTrack",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FPv2GetControlledTrack>));

        plugin_register("API_ReaSonus_FP8_GetTrackOffset", reinterpret_cast<void *>(FP8GetTrackOffset));
        plugin_register("APIdef_ReaSonus_FP8_GetTrackOffset", (void *) defstring_FP8GetTrackOffset);
        plugin_register("APIvararg_ReaSonus_FP8_GetTrackOffset",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8GetTrackOffset>));

        plugin_register("API_ReaSonus_FP8_ControlPlugin", reinterpret_cast<void *>(FP8ControlPlugin));
        plugin_register("APIdef_ReaSonus_FP8_ControlPlugin", (void *) defstring_FP8ControlPlugin);
        plugin_register("APIvararg_ReaSonus_FP8_ControlPlugin",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8ControlPlugin>));

        plugin_register("API_ReaSonus_FP8_ControlThingUnderPointer",
                        reinterpret_cast<void *>(FP8ControlThingUnderPointer));
        plugin_register("APIdef_ReaSonus_FP8_ControlThingUnderPointer", (void *) defstring_FP8ControlThingUnderPointer);
        plugin_register("APIvararg_ReaSonus_FP8_ControlThingUnderPointer",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8ControlThingUnderPointer>));

        plugin_register("API_ReaSonus_FP8_FilterbyName", reinterpret_cast<void *>(FP8FilterbyName));
        plugin_register("APIdef_ReaSonus_FP8_FilterbyName", (void *) defstring_FP8FilterbyName);
        plugin_register("APIvararg_ReaSonus_FP8_FilterbyName",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8FilterbyName>));

        plugin_register("API_ReaSonus_FP8_SetFaderMode", reinterpret_cast<void *>(FP8SetFaderMode));
        plugin_register("APIdef_ReaSonus_FP8_SetFaderMode", (void *) defstring_FP8SetFaderMode);
        plugin_register("APIvararg_ReaSonus_FP8_SetFaderMode",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetFaderMode>));
    }
}
