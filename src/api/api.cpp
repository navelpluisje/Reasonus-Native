#include "api.hpp"

#include "reaper_vararg.hpp"
#include <WDL/wdltypes.h>
#include <reaper_plugin_functions.h>
#include "get_reasonus_version.hpp"
#include "get_fp_8_track_offset.hpp"
#include "get_fp_v2_controlled_track.hpp"
#include "trigger_fp_8_plugin_control.hpp"
#include "trigger_fp_8_thing_under_pointer.hpp"

namespace REASONUS_API {
    void Register() {
        plugin_register("API_ReaSonus_GetVersion", reinterpret_cast<void*>(GetReasonusVersion));
        plugin_register("APIdef_ReaSonus_GetVersion", (void*)defstring_GetReasonusVersion);
        plugin_register("APIvararg_ReaSonus_GetVersion", reinterpret_cast<void*>(&InvokeReaScriptAPI<&GetReasonusVersion>));

        plugin_register("API_ReaSonus_GetFP8TrackOffset", reinterpret_cast<void*>(GetFP8TrackOffset));
        plugin_register("APIdef_ReaSonus_GetFP8TrackOffset", (void*)defstring_GetFP8TrackOffset);
        plugin_register("APIvararg_ReaSonus_GetFP8TrackOffset", reinterpret_cast<void*>(&InvokeReaScriptAPI<&GetFP8TrackOffset>));

        plugin_register("API_ReaSonus_GetFPv2ControlledTrack", reinterpret_cast<void*>(GetFPv2ControlledTrack));
        plugin_register("APIdef_ReaSonus_GetFPv2ControlledTrack", (void*)defstring_GetFPv2ControlledTrack);
        plugin_register("APIvararg_ReaSonus_GetFPv2ControlledTrack", reinterpret_cast<void*>(&InvokeReaScriptAPI<&GetFPv2ControlledTrack>));

        plugin_register("API_ReaSonus_ControlFP8Plugin", reinterpret_cast<void*>(ControlFP8Plugin));
        plugin_register("APIdef_ReaSonus_ControlFP8Plugin", (void*)defstring_ControlFP8Plugin);
        plugin_register("APIvararg_ReaSonus_ControlFP8Plugin", reinterpret_cast<void*>(&InvokeReaScriptAPI<&ControlFP8Plugin>));

        plugin_register("API_ReaSonus_ControlFP8ThingUnderPointer", reinterpret_cast<void*>(ControlFP8ThingUnderPointer));
        plugin_register("APIdef_ReaSonus_ControlFP8ThingUnderPointer", (void*)defstring_ControlFP8ThingUnderPointer);
        plugin_register("APIvararg_ReaSonus_ControlFP8ThingUnderPointer", reinterpret_cast<void*>(&InvokeReaScriptAPI<&ControlFP8ThingUnderPointer>));
    }
}