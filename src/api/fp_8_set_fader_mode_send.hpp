#pragma once

#include <fmt/format.h>
#include "reaper_plugin.h"
#include "reaper_plugin_functions.h"
#include "../shared/csurf_context_resources.hpp"

namespace REASONUS_API {
    inline void FP8SetFaderModeSend(const bool trackMode = false) {
        MediaTrack *media_track = GetSelectedTrack(nullptr, 0);

        if (media_track == nullptr) {
            return;
        }

        const int track_index = stoi(DAW::GetTrackIndex(media_track)) - 1;
        const int mode = trackMode ? TrackSendMode : SendMode;

        DAW::SetExtState(
            EXT_STATE_FP_FADER_MODE,
            fmt::format("{}:{}", mode, track_index),
            false
        );
    }

    inline auto defstring_FP8SetFaderModeSend =
            "void\0bool\0trackModeOptional\0"
            "Set the FaderPort's fader mode to send mode.\n"
            "If trackMode is set to true, it will go to trackmode. Default is HUI mode";
}
