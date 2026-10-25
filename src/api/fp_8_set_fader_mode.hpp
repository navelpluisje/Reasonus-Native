#pragma once
#include <string>
#include "reaper_plugin.h"
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    inline void FP8SetFaderMode(int fader_mode, MediaTrack *media_track) {
        if (fader_mode == 0 && media_track == nullptr) {
            DAW::SetExtState(EXT_STATE_FP_FADER_MODE, fmt::format("{}", fader_mode), false);
        } else if (fader_mode > 0 && media_track != nullptr) {
            // Get the track index (zero-based)
            const int track_index = stoi(DAW::GetTrackIndex(media_track)) - 1;
            DAW::SetExtState(EXT_STATE_FP_FADER_MODE, fmt::format("{}:{}", fader_mode, track_index), false);
        }
    }

    inline auto defstring_FP8SetFaderMode =
            "void\0int,MediaTrack*\0faderMode,trackOptional\0"
            "Set the fader mode for the FaderPort. The first parameter is the mode, the second the track to set the mode for.\n"
            "More about fadermodes in the <a href=\"https://reasonus.net/documentation/faderport8/#fader-modes\" target=\"_blank\">documentation</a>  \n\n"
            "Only for TrackMode the track parameter is not needed. If fadermode > 0 nd not track is set, the call does not do a thing\n"
            "Fader modes:\n"
            "  - 0: TrackMode, does not need a track as parameter\n"
            "  - 1: PluginMode HUI\n"
            "  - 2: PluginMode Trackmode\n"
            "  - 3: SendMode HUI\n"
            "  - 4: SendMode Trackmode\n"
            "  - 5: ReceiveMode HUI\n"
            "  - 6: ReceiveMode Trackmode\n"
            "  - 7: PanMode1\n"
            "  - 8: PanMode2\n"
            "More on HUI and trackmode in the <a href=\"https://reasonus.net/documentation/faderport8/#hui-or-track-mode\" target=\"_blank\">documentation</a>  \n";
}
