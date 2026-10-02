#pragma once
#include <string>
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API
{
    int GetFPv2ControlledTrack()
    {
        const std::string controlled_track = DAW::GetExtState(EXT_STATE_FP_V2_CONTROLLED_TRACK, "0");
        ;
        return stoi(controlled_track);
    }

    auto defstring_GetFPv2ControlledTrack =
        "int\0\0\0"
        "Get the controlld track for the FaderPort V2. Return -1 for master and teh track index (zero-based) otherwise";

}