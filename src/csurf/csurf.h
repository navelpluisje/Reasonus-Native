#ifndef _CSURF_H_
#define _CSURF_H_

#include <reaper_plugin.h>
#include <localize/localize.h>

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

#endif
