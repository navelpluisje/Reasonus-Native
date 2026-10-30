#pragma once

namespace REASONUS_API {
    inline void FP8SetTrackFilterEffects() {
        DAW::SetExtState(
            EXT_STATE_FP_FILTER_BY_TYPE,
            std::to_string(TrackEffectsFilter),
            false
        );
    }

    inline auto defstring_FP8SetTrackFilterEffects =
            "void\0\0\0"
            "Set the FaderPort's mix mode to show all tracks with effects.";
}
