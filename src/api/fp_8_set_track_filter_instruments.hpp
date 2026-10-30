#pragma once

namespace REASONUS_API {
    inline void FP8SetTrackFilterInstruments() {
        DAW::SetExtState(
            EXT_STATE_FP_FILTER_BY_TYPE,
            std::to_string(TrackInstrumentsFilter),
            false
        );
    }

    inline auto defstring_FP8SetTrackFilterInstruments =
            "void\0\0\0"
            "Set the FaderPort's mix mode to show all tracks with virtual instruments.";
}
