#pragma once

namespace REASONUS_API {
    inline void FP8SetTrackFilterAllFolders() {
        DAW::SetExtState(
            EXT_STATE_FP_FILTER_BY_TYPE,
            std::to_string(TrackAllFoldersFilter),
            false
        );
    }

    inline auto defstring_FP8SetTrackFilterAllFolders =
            "void\0\0\0"
            "Set the FaderPort's mix mode to show all tracks that are folder tracks.";
}
