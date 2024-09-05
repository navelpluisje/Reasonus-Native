// std::min
#include "config.h"

#include <reaper_plugin_functions.h>

namespace TOGGLE_PLAY_CURSOR
{
    extern REAPER_PLUGIN_HINSTANCE hInstance; // used for dialogs, if any
    auto Register() -> void;
    auto Unregister() -> void;

} // namespace TOGGLE_PLAY_CURSOR
