#pragma once

#include <fmt/format.h>
#include "reaper_plugin.h"
#include "reaper_plugin_functions.h"
#include "../shared/csurf_context_resources.hpp"

namespace REASONUS_API {
    inline void FP8SetFaderModePan(const bool panMode = false) {
        MediaTrack *media_track = GetSelectedTrack(nullptr, 0);

        if (media_track == nullptr) {
            return;
        }

        const int track_index = stoi(DAW::GetTrackIndex(media_track)) - 1;
        const int mode = panMode ? PanMode2 : PanMode1;

        DAW::SetExtState(
            EXT_STATE_FP_FADER_MODE,
            fmt::format("{}:{}", mode, track_index),
            false
        );
    }

    inline auto defstring_FP8SetFaderModePan =
            "void\0bool\0panModeOptional\0"
            "Set the FaderPort's fader mode to pan mode.\n"
            "If panMode is set to true, it will control the pan 2 value. Default is pan 1 value\n"
            "More about pan modes in the <a href=\"https://reasonus.net/documentation/faderport8/#pan-values\" target=\"_blank\">documentation</a>";
}
