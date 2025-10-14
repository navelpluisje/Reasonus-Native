// std::min
#include "config.h"

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <gsl/gsl>
#include <reaper_plugin_functions.h>

namespace CONVERT_PLUGIN_ZON_TO_INI
{
    extern REAPER_PLUGIN_HINSTANCE hInstance; // used for dialogs, if any
    auto Register() -> void;
    auto Unregister() -> void;

} // namespace CONVERT_PLUGIN_ZON_TO_INI
