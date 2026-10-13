#ifndef REASONUS_CSURF_H_
#define REASONUS_CSURF_H_

#include <reaper_plugin.h>

#include <WDL/wdlstring.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <WDL/win32_utf8.h>

// WARNING: REAPER has a handful of UNDOCUMENTED API functions that are not declared in reaper_plugin_functions.h, so must declare here:
inline bool (*CoolSB_GetScrollInfo)(HWND hwnd, int nBar, LPSCROLLINFO lpsi);

inline int (*CoolSB_SetScrollPos)(HWND hwnd, int nBar, int nPos, BOOL fRedraw);

extern REAPER_PLUGIN_HINSTANCE g_hInst; // used for dialogs
extern HWND g_hwnd;

#define DELETE_ASYNC(x) \
  do                    \
  {                     \
    if (x)              \
      (x)->Destroy();   \
  } while (0)

inline auto EXT_STATE_VALUE_FALSE = "0";
inline auto EXT_STATE_VALUE_TRUE = "1";
inline auto EXT_STATE_SECTION = "ReaSonus";
inline auto EXT_STATE_KEY_SAVED_SETTINGS = "saved_settings";
inline auto EXT_STATE_KEY_UI_LANGUAGE = "language";
inline auto EXT_STATE_KEY_VERSION = "version";
inline auto EXT_STATE_FP_TRACK_OFFSET = "FP_TRACK_OFFSET";
inline auto EXT_STATE_FP_V2_CONTROLLED_TRACK = "FP_V2_CONTROLLED_TRACK";
inline auto EXT_STATE_FP_V2_WIDGET_MODE = "FP_V2_WIDGET_MODE";
inline auto EXT_STATE_FP_FILTER_BY_NAME = "FP_FILTER_BY_NAME";
inline auto EXT_STATE_FP_PLUGIN_CONTROL = "FP_PLUGIN_CONTROL";
inline auto EXT_STATE_FP_SENDS_CONTROL = "FP_SENDS_CONTROL";
inline auto EXT_STATE_FP_TRACK_SELECT = "FP_TRACK_SELECT";
inline auto EXT_STATE_FP_PAN_CONTROL = "FP_PAN_CONTROL";

enum FP_V2_WIDGET_MODE {
    FP_V2_WIDGET_TRACK_MODE,
    FP_V2_WIDGET_MASTER_MODE,
    FP_V2_WIDGET_PARAM_MODE
};

#endif
