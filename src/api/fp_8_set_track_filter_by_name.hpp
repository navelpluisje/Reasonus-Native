#pragma once

#include <string>

#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    inline void FP8SetTrackFilterByName(const char *filter_name) {
        const std::string name = filter_name;

        // If the plugin index is empty, clear the extState, otherwise set the track index and plugin index
        DAW::SetExtState(EXT_STATE_FP_FILTER_BY_NAME, name, false);
    }

    inline auto defstring_FP8SetTrackFilterByName =
            "void\0char*\0filterName\0"
            "Trigger a custom filter by its name. The API only supports global filters";
}
