#pragma once
#include <string>
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API
{
    int GetFP8TrackOffset()
    {
        const std::string track_offset = DAW::GetExtState(EXT_STATE_FP_TRACK_OFFSET, "0");

        return stoi(track_offset);
    }

    auto defstring_GetFP8TrackOffset =
        "int\0\0\0"
        "Get the track offset for the FaderPort 8 or 16";

}
