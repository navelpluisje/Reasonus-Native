#pragma once

namespace REASONUS_API {
    inline void FP8SetTrackFilterWithMidi() {
        DAW::SetExtState(
            EXT_STATE_FP_FILTER_BY_TYPE,
            std::to_string(TrackWithMidiFilter),
            false
        );
    }

    inline auto defstring_FP8SetTrackFilterWithMidi =
            "void\0\0\0"
            "Set the FaderPort's mix mode to show all tracks with midi items.";
}
