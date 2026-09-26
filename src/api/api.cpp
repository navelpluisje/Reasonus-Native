#include "api.hpp"

#include <config.h>
#include <WDL/wdltypes.h>
#include <reaper_plugin_functions.h>

#include "reaper_vararg.hpp"

namespace REASONUS_API {
    const char *GetReasonusVersion()
    {
        return GIT_VERSION;
    }

    auto defstring_GetReasonusVersion =
        "const char*\0\0\0"
        "Get the installed ReaSonus version";

    void Register() {
        plugin_register("API_RSN_GetVersion", reinterpret_cast<void*>(GetReasonusVersion));
        plugin_register("APIdef_RSN_GetVersion", (void*)defstring_GetReasonusVersion);
        plugin_register("APIvararg_RSN_GetVersion", reinterpret_cast<void*>(&InvokeReaScriptAPI<&GetReasonusVersion>));
    }
}