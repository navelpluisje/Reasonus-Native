#pragma once
#include <string>
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    inline int FP8GetTrackOffset() {
        const std::string track_offset = DAW::GetExtState(EXT_STATE_FP_TRACK_OFFSET, "0");

        return stoi(track_offset);
    }

    inline auto defstring_FP8GetTrackOffset =
            "int\0\0\0"
            "Get the track offset for the FaderPort 8 or 16. This is not the index of the first track on the controller as it does not count hidden tracks";
}
