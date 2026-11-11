#pragma once
#include <config.h>

namespace REASONUS_API {
    inline const char *GetReasonusVersion() {
        return GIT_VERSION;
    }

    inline auto defstring_GetReasonusVersion =
            "const char*\0\0\0"
            "Get the installed ReaSonus version";
}
