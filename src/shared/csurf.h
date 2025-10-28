#ifndef _CSURF_H_
#define _CSURF_H_

#include <reaper_plugin.h>

#include <WDL/wdlstring.h>
#include <WDL/win32_utf8.h>

extern REAPER_PLUGIN_HINSTANCE g_hInst; // used for dialogs
extern HWND g_hwnd;

#define DELETE_ASYNC(x) \
  do                    \
  {                     \
    if (x)              \
      (x)->Destroy();   \
  } while (0)

inline const char *EXT_STATE_VALUE_FALSE = "0";
inline const char *EXT_STATE_VALUE_TRUE = "1";
inline const char *EXT_STATE_SECTION = "ReaSonus";
inline const char *EXT_STATE_KEY_SAVED_SETTINGS = "saved_settings";
inline const char *EXT_STATE_KEY_UI_LANGUAGE = "language";

#endif
