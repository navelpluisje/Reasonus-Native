#define REAPERAPI_IMPLEMENT
// #define REAIMGUIAPI_IMPLEMENT

#include "actions/action_toggle_play_cursor.hpp"
#include "actions/action_show_reaper_resource_path.hpp"
#include "actions/action_ui_fp_v2_control_panel.hpp"
#include "actions/action_ui_fp_8_control_panel.hpp"
#include "actions/action_close_all_floating_fx_windows.hpp"
#include "actions/action_ui_convert_plugin_zon_to_ini.hpp"
#include "actions/action_ui_translations_editor.hpp"
#include "actions/action_fp_8_setting_distraction_free_mode.hpp"
#include "actions/action_fp_8_setting_disable_plugins.hpp"
#include "actions/action_fp_8_setting_fader_reset.hpp"
#include "actions/action_fp_8_setting_master_fader_mode.hpp"
#include "actions/action_fp_8_setting_momentary_mute_solo.hpp"
#include "actions/action_fp_8_setting_swap_shift_buttons.hpp"
#include "actions/action_fp_8_setting_untouch_last_touched_param.hpp"
#include "actions/action_fp_v2_setting_control_hidden_tracks.hpp"
#include "actions/action_fp_v2_setting_disable_fader_move.hpp"
#include "actions/action_fp_v2_setting_endless_track_scroll.hpp"
#include "actions/action_fp_v2_setting_momentary_mute_solo.hpp"
#include "ui/csurf_ui_function_keys_page.hpp"
#include "resource.h"
#include "shared/csurf.h"

extern reaper_csurf_reg_t
    csurf_faderport_8_reg,
    csurf_faderport_v2_reg;

/**
 * Set some initial values
 */
bool CSurf_UI_FunctionKeysPage::querying_actions = false;
FunctionTypes CSurf_UI_FunctionKeysPage::selected_type = TypeFunction;
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
      ACTION_CLOSE_ALL_FLOATING_FX_WINDOWS::Unregister();
      ACTION_FP_8_SETTING_DISABLE_PLUGINS::Unregister();
      ACTION_FP_8_SETTING_DISTRACTION_FREE_MODE::Unregister();
      ACTION_FP_8_SETTING_FADER_RESET_SETTING::Unregister();
      ACTION_FP_8_SETTING_MASTER_FADER_MODE::Unregister();
      ACTION_FP_8_SETTING_MOMENTARY_MUTE_SOLO::Unregister();
      ACTION_FP_8_SETTING_SWAP_SHIFT_BUTTONS::Unregister();
      ACTION_FP_8_SETTING_UNTOUCH_LAST_TOUCHED_PARAM::Unregister();
      ACTION_FP_V2_SETTING_CONTROL_HIDDEN_TRACKS::Unregister();
      ACTION_FP_V2_SETTING_DISABLE_FADER_MOVE::Unregister();
      ACTION_FP_V2_SETTING_ENDLESS_TRACK_SCROLL::Unregister();
      ACTION_FP_V2_SETTING_MOMENTARY_MUTE_SOLO::Unregister();
      ACTION_SHOW_REAPER_RESOURCE_PATH::Unregister();
      ACTION_TOGGLE_PLAY_CURSOR::Unregister();
      ACTION_UI_CONVERT_PLUGIN_ZON_TO_INI::Unregister();
      ACTION_UI_FP_8_CONTROL_PANEL::Unregister();
      ACTION_UI_FP_V2_CONTROL_PANEL::Unregister();
      ACTION_UI_TRANSLATIONS_EDITOR::Unregister();

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

    ACTION_CLOSE_ALL_FLOATING_FX_WINDOWS::Register();
    ACTION_FP_8_SETTING_DISABLE_PLUGINS::Register();
    ACTION_FP_8_SETTING_DISTRACTION_FREE_MODE::Register();
    ACTION_FP_8_SETTING_FADER_RESET_SETTING::Register();
    ACTION_FP_8_SETTING_MASTER_FADER_MODE::Register();
    ACTION_FP_8_SETTING_MOMENTARY_MUTE_SOLO::Register();
    ACTION_FP_8_SETTING_SWAP_SHIFT_BUTTONS::Register();
    ACTION_FP_8_SETTING_UNTOUCH_LAST_TOUCHED_PARAM::Register();
    ACTION_FP_V2_SETTING_CONTROL_HIDDEN_TRACKS::Register();
    ACTION_FP_V2_SETTING_DISABLE_FADER_MOVE::Register();
    ACTION_FP_V2_SETTING_ENDLESS_TRACK_SCROLL::Register();
    ACTION_FP_V2_SETTING_MOMENTARY_MUTE_SOLO::Register();
    ACTION_SHOW_REAPER_RESOURCE_PATH::Register();
    ACTION_TOGGLE_PLAY_CURSOR::Register();
    ACTION_UI_CONVERT_PLUGIN_ZON_TO_INI::Register();
    ACTION_UI_FP_8_CONTROL_PANEL::Register();
    ACTION_UI_FP_V2_CONTROL_PANEL::Register();
    ACTION_UI_TRANSLATIONS_EDITOR::Register();

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
