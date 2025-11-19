#include "config.h"

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <gsl/gsl>
#include <reaper_plugin_functions.h>

namespace ACTION_FP_V2_SETTING_ENDLESS_TRACK_SCROLL
{
    extern REAPER_PLUGIN_HINSTANCE hInstance; // used for dialogs, if any
    auto Register() -> void;
    auto Unregister() -> void;

} // namespace ACTION_FP_V2_SETTING_ENDLESS_TRACK_SCROLL
