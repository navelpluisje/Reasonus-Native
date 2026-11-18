#pragma once
#include <string>
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    inline void DecrementTransportSeekSpeed() {
        DAW::SetExtState(
            EXT_STATE_FP_TRANSPORT_SEEK_SPEED,
            "-1",
            false
        );
    }

    inline auto defstring_DecrementTransportSeekSpeed =
            "void\0\0\0"
            "Decrement the transport seek speed.";
}
