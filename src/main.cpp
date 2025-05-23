#define REAIMGUIAPI_IMPLEMENT
#define REAPERAPI_IMPLEMENT

#include "actions/toggle_play_cursor.hpp"
#include "actions/show_reaper_resource_path.hpp"
#include "actions/show_reasonus_function_window.hpp"
#include "actions/show_reasonus_filters_window.hpp"
#include "actions/show_reasonus_test_window.hpp"
#include "actions/close_all_floating_fx_windows.hpp"
#include "resource.h"

extern reaper_csurf_reg_t
    csurf_faderport_8_reg,
    csurf_faderport_v2_reg;

REAPER_PLUGIN_HINSTANCE g_hInst; // used for dialogs, if any
HWND g_hwnd;

reaper_plugin_info_t *g_reaper_plugin_info;

extern "C"
{
  REAPER_PLUGIN_DLL_EXPORT auto REAPER_PLUGIN_ENTRYPOINT(
      REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *reaper_plugin_info) -> int
  {
    g_hInst = hInstance;

    if (!reaper_plugin_info || reaper_plugin_info->caller_version != REAPER_PLUGIN_VERSION || !reaper_plugin_info->GetFunc)
    {
      return 0;
    }

    /**
     * reaper_plugin_info is not available, so no need to run.
     */
    if (reaper_plugin_info == nullptr)
    {
      TOGGLE_PLAY_CURSOR::Unregister();
      SHOW_REAPER_RESOURCE_PATH::Unregister();
      SHOW_REASONUS_FUNCTION_WINDOW::Unregister();
      SHOW_REASONUS_FILTERS_WINDOW::Unregister();
      SHOW_REASONUS_TEST_WINDOW::Unregister();
      CLOSE_ALL_FLOATING_FX_WINDOWS::Unregister();
      return 0;
    }

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
    SHOW_REASONUS_FUNCTION_WINDOW::Register();
    SHOW_REASONUS_FILTERS_WINDOW::Register();
    SHOW_REASONUS_TEST_WINDOW::Register();
    CLOSE_ALL_FLOATING_FX_WINDOWS::Register();
    reaper_plugin_info->Register("csurf", &csurf_faderport_8_reg);
    reaper_plugin_info->Register("csurf", &csurf_faderport_v2_reg);
    return 1;
  }
};

// import the resources. Note: if you do not have these files, run "php WDL/swell/mac_resgen.php res.rc" from this directory
#if __APPLE__ || __linux__

#include "WDL/swell/swell-dlggen.h"
#include "res.rc_mac_dlg"

#endif
