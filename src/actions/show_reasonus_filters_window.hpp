// std::min
#include "config.h"

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <gsl/gsl>
#include <reaper_plugin_functions.h>

namespace SHOW_REASONUS_FILTERS_WINDOW
{
    extern REAPER_PLUGIN_HINSTANCE hInstance; // used for dialogs, if any
    auto Register() -> void;
    auto Unregister() -> void;

} // namespace SHOW_REASONUS_FILTERS_WINDOW
