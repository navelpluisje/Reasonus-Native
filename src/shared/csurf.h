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

inline void (*osara_outputMessage)(const char *message); // global

// void Speak(const char *phrase)
// {
//   static void (*osara_outputMessage)(const char *message);
//   static bool chk;

//   if (!chk)
//   {
//     *(void **)&osara_outputMessage = plugin_getapi("osara_outputMessage");
//     chk = true;
//   }

//   if (osara_outputMessage)
//     osara_outputMessage(phrase);
// }

#endif
