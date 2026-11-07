#pragma once

namespace REASONUS_API {
    inline void FP8SetTrackFilterReceives() {
        DAW::SetExtState(
            EXT_STATE_FP_FILTER_BY_TYPE,
            std::to_string(TrackReceivesFilter),
            false
        );
    }

    inline auto defstring_FP8SetTrackFilterReceives =
            "void\0\0\0"
            "Set the FaderPort's mix mode to show all tracks with receives.";
}
