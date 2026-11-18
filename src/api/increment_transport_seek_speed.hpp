#pragma once
#include <string>
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    inline void IncrementTransportSeekSpeed() {
        DAW::SetExtState(
            EXT_STATE_FP_TRANSPORT_SEEK_SPEED,
            "0",
            false
        );
    }

    inline auto defstring_IncrementTransportSeekSpeed =
            "void\0\0\0"
            "Increment the transport seek speed.";
}
