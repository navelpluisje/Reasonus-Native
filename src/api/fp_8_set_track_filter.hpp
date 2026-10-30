#pragma once
#include <string>
#include "reaper_plugin.h"
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

namespace REASONUS_API {
    inline void FP8SetTrackFilter(int filter_mode) {
        DAW::SetExtState(
            EXT_STATE_FP_FILTER_BY_TYPE,
            std::to_string(filter_mode),
            false
        );
    }

    inline auto defstring_FP8SetTrackFilter =
            "void\0int\0filterMode\0"
            "Set the mix management (filter) mode for the FaderPort. The parameter is the mode.\n"
            "More about mix management in the <a href=\"https://reasonus.net/documentation/faderport8/#mix-management\" target=\"_blank\">documentation</a>\n\n"
            "Filter modes:\n"
            "  - 0: All: Select all tracks in the project\n"
            "  - 1: Sends: Select all tracks that have sends\n"
            "  - 2: Receives: Select all tracks that have receives\n"
            "  - 3: Hardware Out: Select all tracks that have a hardware output\n"
            "  - 4: Instrument: Select all tracks that have a virtual instrument\n"
            "  - 5: Effects: Select all tracks that have one or more effects\n"
            "  - 6: Top Folders: Select all tracks that are a folder on top level\n"
            "  - 7: All Folders: Select all tracks that are a folder\n"
            "  - 8: With Midi: Select all tracks that have a media item with MIDI\n"
            "  - 9: With Audio: Select all tracks that have a media item with Audio\n"
            "  - 10: VCA: Select all tracks that are a VCA Lead\n";
}
