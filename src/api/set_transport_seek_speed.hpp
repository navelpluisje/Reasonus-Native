#pragma once
#include <string>
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    inline void SetTransportSeekSpeed(const int seek_speed) {
        if (seek_speed > 0 && seek_speed < 11) {
            DAW::SetExtState(
                EXT_STATE_FP_TRANSPORT_SEEK_SPEED,
                std::to_string(seek_speed),
                false
            );
        }
    }

    inline auto defstring_SetTransportSeekSpeed =
            "void\0int\0seekSpeed\0"
            "Set the new transport seek speed.\n"
            "Min value: 1\n"
            "Max value: 10\n";
}
