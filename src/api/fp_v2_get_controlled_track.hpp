#pragma once
#include <string>
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    inline int FPv2GetControlledTrack() {
        const std::string controlled_track = DAW::GetExtState(EXT_STATE_FP_V2_CONTROLLED_TRACK, "0");;
        return stoi(controlled_track);
    }

    inline auto defstring_FPv2GetControlledTrack =
            "int\0\0\0"
            "Get the controlled track for the FaderPort V2. Return -1 for master and the track index (zero-based) otherwise";
}
