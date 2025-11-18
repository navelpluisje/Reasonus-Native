#define REAPERAPI_IMPLEMENT
// #define REAIMGUIAPI_IMPLEMENT

#include "actions/toggle_play_cursor.hpp"
#include "actions/show_reaper_resource_path.hpp"
#include "actions/show_reasonus_v2_control_panel.hpp"
#include "actions/show_reasonus_8_control_panel.hpp"
#include "actions/close_all_floating_fx_windows.hpp"
#include "actions/convert_plugin_zon_to_ini.hpp"
#include "actions/translations_editor.hpp"
#include "actions/toggle_distraction_free_mode_setting.hpp"
#include "ui/csurf_ui_function_keys_page.cpp"
#include "resource.h"
#include "shared/csurf.h"
#include "actions/toggle_disable_plugins_setting.hpp"
#include "actions/toggle_untouch_last_touched_param_setting.hpp"
#include "actions/toggle_master_fader_mode_setting.hpp"
#include "actions/toggle_swap_shift_buttons_setting.hpp"
#include "actions/toggle_fader_reset_setting.hpp"
#include "actions/toggle_momentary_mute_solo_setting.hpp"

extern reaper_csurf_reg_t
    csurf_faderport_8_reg,
    csurf_faderport_v2_reg;

/**
 * Set some initial values
 */
bool CSurf_UI_FunctionKeysPage::querying_actions = false;
int CSurf_UI_FunctionKeysPage::selected_function = -1;
int CSurf_UI_FunctionKeysPage::selected_action = -1;

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
      SHOW_REASONUS_V2_CONTROL_PANEL::Unregister();
      SHOW_REASONUS_8_CONTROL_PANEL::Unregister();
      CLOSE_ALL_FLOATING_FX_WINDOWS::Unregister();
      CONVERT_PLUGIN_ZON_TO_INI::Unregister();
      TRANSLATION_EDITOR::Unregister();
      TOGGLE_DISTRACTION_FREE_MODE_SETTING::Unregister();
      TOGGLE_DISABLE_PLUGINS_SETTING::Unregister();
      TOGGLE_UNTOUCH_LAST_TOUCHED_PARAM_SETTING::Unregister();
      TOGGLE_MASTER_FADER_MODE_SETTING::Unregister();
      TOGGLE_SWAP_SHIFT_BUTTONS_SETTING::Unregister();
      TOGGLE_FADER_RESET_SETTING::Unregister();
      TOGGLE_MOMENTARY_MUTE_SOLO_SETTING::Unregister();

      return 0;
    }

    g_hwnd = reaper_plugin_info->hwnd_main;
    g_reaper_plugin_info = reaper_plugin_info;

    if (reaper_plugin_info)
    {
      // First, import REAPER's C++ API functions for use in this extension.
      //		Load all listed API functions at once.
      if (REAPERAPI_LoadAPI(reaper_plugin_info->GetFunc) != 0)
      {
        // This is the WIN32 / swell MessageBox, not REAPER's API MB.  This should create a separate window that is listed in the taskbar,
        //		and more easily visible behind REAPER's splash screen.
        MessageBox(NULL, "Unable to import default API functions.\n\nNOTE:\nThis extension requires REAPER v7.40 or higher.", "ERROR: ReaSonus Native", 0);
        return 0;
      }
      //		Load each of the undocumented functions.
      if (!((*(void **)&CoolSB_GetScrollInfo) = (void *)reaper_plugin_info->GetFunc("CoolSB_GetScrollInfo")))
      {
        MessageBox(NULL, "Unable to import CoolSB_GetScrollInfo function.", "ERROR: ReaSonus Native", 0);
        return 0;
      }
      if (!((*(void **)&CoolSB_SetScrollPos) = (void *)reaper_plugin_info->GetFunc("CoolSB_SetScrollPos")))
      {
        MessageBox(NULL, "Unable to import CoolSB_SetScrollPos function.", "ERROR: ReaSonus Native", 0);
        return 0;
      }
    }

    // load Reaper API functions
    // check that our plugin hasn't been already loaded
    if (REAPERAPI_LoadAPI(reaper_plugin_info->GetFunc) > 0 || reaper_plugin_info->GetFunc("ReaScriptAPIFunctionExample"))
    {
      return 0;
    }

    TOGGLE_PLAY_CURSOR::Register();
    SHOW_REAPER_RESOURCE_PATH::Register();
    SHOW_REASONUS_V2_CONTROL_PANEL::Register();
    SHOW_REASONUS_8_CONTROL_PANEL::Register();
    CLOSE_ALL_FLOATING_FX_WINDOWS::Register();
    CONVERT_PLUGIN_ZON_TO_INI::Register();
    TRANSLATION_EDITOR::Register();
    TOGGLE_DISTRACTION_FREE_MODE_SETTING::Register();
    TOGGLE_DISABLE_PLUGINS_SETTING::Register();
    TOGGLE_UNTOUCH_LAST_TOUCHED_PARAM_SETTING::Register();
    TOGGLE_MASTER_FADER_MODE_SETTING::Register();
    TOGGLE_SWAP_SHIFT_BUTTONS_SETTING::Register();
    TOGGLE_FADER_RESET_SETTING::Register();
    TOGGLE_MOMENTARY_MUTE_SOLO_SETTING::Register();

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
