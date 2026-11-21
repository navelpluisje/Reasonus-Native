#pragma once
#include <string>
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    inline void SetTransportSeek(const int seek_mode) {
        if (seek_mode > -2 && seek_mode < 2) {
            DAW::SetExtState(
                EXT_STATE_FP_TRANSPORT_SEEK_MODE,
                std::to_string(seek_mode),
                false
            );
        }
    }

    inline auto defstring_SetTransportSeek =
            "void\0int\0seekMode\0"
            "Set the transport seek mode. Possible values\n"
            "  - -1: rewind\n"
            "  - 0: stop\n"
            "  - 1: forward";

    inline void SetTransportSeekRewind() {
        DAW::SetExtState(EXT_STATE_FP_TRANSPORT_SEEK_MODE, "-1", false);
    }

    inline auto defstring_SetTransportSeekRewind =
            "void\0\0\0"
            "Set the transport seek mode to rewind. This will stop after calling ReaSonus_SetTransportSeekStop";

    inline void SetTransportSeekStop() {
        DAW::SetExtState(EXT_STATE_FP_TRANSPORT_SEEK_MODE, "0", false);
    }

    inline auto defstring_SetTransportSeekStop =
            "void\0\0\0"
            "Set the transport seek mode to stop";

    inline void SetTransportSeekForward() {
        DAW::SetExtState(EXT_STATE_FP_TRANSPORT_SEEK_MODE, "1", false);
    }

    inline auto defstring_SetTransportSeekForward =
            "void\0\0\0"
            "Set the transport seek mode to forward. This will stop after calling ReaSonus_SetTransportSeekStop";
}
