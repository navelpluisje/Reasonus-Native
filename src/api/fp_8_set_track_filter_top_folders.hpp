#pragma once

namespace REASONUS_API {
    inline void FP8SetTrackFilterTopFolders() {
        DAW::SetExtState(
            EXT_STATE_FP_FILTER_BY_TYPE,
            std::to_string(TrackTopFoldersFilter),
            false
        );
    }

    inline auto defstring_FP8SetTrackFilterTopFolders =
            "void\0\0\0"
            "Set the FaderPort's mix mode to show all tracks that are top folder tracks.";
}
