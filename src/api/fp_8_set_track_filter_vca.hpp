#pragma once

namespace REASONUS_API {
    inline void FP8SetTrackFilterVca() {
        DAW::SetExtState(
            EXT_STATE_FP_FILTER_BY_TYPE,
            std::to_string(TrackIsVcaFilter),
            false
        );
    }

    inline auto defstring_FP8SetTrackFilterVca =
            "void\0\0\0"
            "Set the FaderPort's mix mode to show all vca tracks.";
}
