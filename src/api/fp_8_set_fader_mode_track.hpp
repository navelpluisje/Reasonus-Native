#pragma once

#include <string>
#include "../shared/csurf.h"
#include "../shared/csurf_context_resources.hpp"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    inline void FP8SetFaderModeTrack() {
        DAW::SetExtState(EXT_STATE_FP_FADER_MODE, std::to_string(TrackMode), false);
    }

    inline auto defstring_FP8SetFaderModeTrack =
            "void\0\0\0"
            "Set the FaderPort's fader mode to track mode.";
}
