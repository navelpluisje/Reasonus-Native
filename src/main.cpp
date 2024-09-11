#define REAPERAPI_IMPLEMENT

#include "actions/toggle_play_cursor.hpp"
#include "actions/show_reaper_resource_path.hpp"
#include "resource.h"

extern reaper_csurf_reg_t csurf_faderport_reg;

REAPER_PLUGIN_HINSTANCE g_hInst; // used for dialogs, if any
HWND g_hwnd;
reaper_plugin_info_t *g_reaper_plugin_info;

extern "C"
{
  REAPER_PLUGIN_DLL_EXPORT auto REAPER_PLUGIN_ENTRYPOINT(
      REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *reaper_plugin_info) -> int
  {
    g_hInst = hInstance;
    g_hwnd = reaper_plugin_info->hwnd_main;

    if (!reaper_plugin_info || reaper_plugin_info->caller_version != REAPER_PLUGIN_VERSION || !reaper_plugin_info->GetFunc)
    {
      return 0;
    }

    if (reaper_plugin_info != nullptr)
    {
      g_hwnd = reaper_plugin_info->hwnd_main;
      g_reaper_plugin_info = reaper_plugin_info;

      // load Reaper API functions
      // check that our plugin hasn't been already loaded
      if (REAPERAPI_LoadAPI(reaper_plugin_info->GetFunc) > 0 || reaper_plugin_info->GetFunc("ReaScriptAPIFunctionExample"))
      {
        return 0;
      }

      TOGGLE_PLAY_CURSOR::Register();
      SHOW_REAPER_RESOURCE_PATH::Register();
      reaper_plugin_info->Register("csurf", &csurf_faderport_reg);
      return 1;
    }
    // quit
    TOGGLE_PLAY_CURSOR::Unregister();
    SHOW_REAPER_RESOURCE_PATH::Unregister();
    return 0;
  }
};

#ifndef _WIN32 // import the resources. Note: if you do not have these files, run "php WDL/swell/mac_resgen.php res.rc" from this directory
#include "WDL/swell/swell-dlggen.h"
// #include "res.rc_mac_dlg"
#include "WDL/swell/swell-menugen.h"
#include "res.rc_mac_menu"

#endif
