#pragma once
#include <string>

#include "reaper_plugin_functions.h"
#include "../shared/csurf.h"
#include "../shared/csurf_utils.hpp"

namespace REASONUS_API {
    inline void FP8ControlPlugin(MediaTrack *media_track, int plugin_index) {
        // Get the track index (zero-based)
        const int track_index = stoi(DAW::GetTrackIndex(media_track)) - 1;

        // If the plugin index is empty, clear the extState, otherwise set the track index and plugin index
        if (plugin_index > -1) {
            DAW::SetExtState(EXT_STATE_FP_PLUGIN_CONTROL, fmt::format("{}:{}", track_index, plugin_index), false);
        } else {
            DAW::SetExtState(EXT_STATE_FP_PLUGIN_CONTROL, "", false);
        }
    }

    inline auto defstring_FP8ControlPlugin =
            "void\0MediaTrack*,int\0track,pluginIndex\0"
            "Start controlling the plugin on the given track with the given plugin id with the controler";
}
