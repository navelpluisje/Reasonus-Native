#include "config.h"

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <gsl/gsl>
#include <reaper_plugin_functions.h>

namespace SHOW_REAPER_RESOURCE_PATH
{
    auto Register() -> void;
    auto Unregister() -> void;

} // namespace SHOW_REAPER_RESOURCE_PATH
