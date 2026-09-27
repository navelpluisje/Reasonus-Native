#include "api.hpp"

#include <config.h>
#include <string>
#include "reaper_vararg.hpp"
#include <WDL/wdltypes.h>
#include <reaper_plugin_functions.h>

#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    const char *GetReasonusVersion()
    {
        return GIT_VERSION;
    }

    auto defstring_GetReasonusVersion =
        "const char*\0\0\0"
        "Get the installed ReaSonus version";

    int GetFP8TrackOffset()
    {
        const std::string track_offset = DAW::GetExtState(FP_TRACK_OFFSET, "0");

        return stoi(track_offset);
    }

    auto defstring_GetFP8TrackOffset =
        "int\0\0\0"
        "Get the track offset for the FaderPort 8 or 16";

    int GetV2ControlledTrack()
    {
        const std::string controlled_track = DAW::GetExtState(FP_V2_CONTROLLED_TRACK, "0");
;
        return stoi(controlled_track);
    }

    auto defstring_GetV2ControlledTrack =
        "int\0\0\0"
        "Get the controlld track for the FaderPort V2. Return -1 for master and teh track index (zero-based) otherwise";

    void Register() {
        plugin_register("API_RSN_GetVersion", reinterpret_cast<void*>(GetReasonusVersion));
        plugin_register("APIdef_RSN_GetVersion", (void*)defstring_GetReasonusVersion);
        plugin_register("APIvararg_RSN_GetVersion", reinterpret_cast<void*>(&InvokeReaScriptAPI<&GetReasonusVersion>));

        plugin_register("API_RSN_GetFP8TrackOffset", reinterpret_cast<void*>(GetFP8TrackOffset));
        plugin_register("APIdef_RSN_GetFP8TrackOffset", (void*)defstring_GetFP8TrackOffset);
        plugin_register("APIvararg_RSN_GetFP8TrackOffset", reinterpret_cast<void*>(&InvokeReaScriptAPI<&GetFP8TrackOffset>));

        plugin_register("API_RSN_GetV2ControlledTrack", reinterpret_cast<void*>(GetV2ControlledTrack));
        plugin_register("APIdef_RSN_GetV2ControlledTrack", (void*)defstring_GetV2ControlledTrack);
        plugin_register("APIvararg_RSN_GetV2ControlledTrack", reinterpret_cast<void*>(&InvokeReaScriptAPI<&GetV2ControlledTrack>));
    }
}