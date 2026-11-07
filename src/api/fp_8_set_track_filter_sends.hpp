#pragma once

namespace REASONUS_API {
    inline void FP8SetTrackFilterSends() {
        DAW::SetExtState(
            EXT_STATE_FP_FILTER_BY_TYPE,
            std::to_string(TrackSendsFilter),
            false
        );
    }

    inline auto defstring_FP8SetTrackFilterSends =
            "void\0\0\0"
            "Set the FaderPort's mix mode to show all tracks with sends.";
}
