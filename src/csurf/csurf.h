#ifndef _CSURF_H_
#define _CSURF_H_

#include <reaper_plugin.h>
#include <localize.h>

#include <WDL/wdlstring.h>
#include <WDL/win32_utf8.h>

enum
{
  HZOOM_EDITPLAYCUR = 0,
  HZOOM_EDITCUR = 1,
  HZOOM_VIEWCTR = 2,
  HZOOM_MOUSECUR = 3
};

enum
{
  VZOOM_VIEWCTR = 0,
  VZOOM_TOPVIS = 1,
  VZOOM_LASTSEL = 2,
  VZOOM_MOUSECUR = 3
};

extern REAPER_PLUGIN_HINSTANCE g_hInst; // used for dialogs
extern HWND g_hwnd;

extern int *g_config_csurf_rate;
extern int *g_config_zoommode;

extern int *g_vu_minvol;
extern int *g_vu_maxvol;
extern int *g_config_vudecay;

extern int __g_projectconfig_timemode2;
extern int __g_projectconfig_timemode;
extern int __g_projectconfig_timeoffs;
extern int __g_projectconfig_measoffs;
extern int __g_projectconfig_autoxfade;
extern int __g_projectconfig_metronome_en;

/*
** REAPER command message defines
*/

#define IDC_REPEAT 1068
#define ID_FILE_SAVEAS 40022
#define ID_FILE_NEWPROJECT 40023
#define ID_FILE_OPENPROJECT 40025
#define ID_FILE_SAVEPROJECT 40026
#define IDC_EDIT_UNDO 40029
#define IDC_EDIT_REDO 40030
#define ID_MARKER_PREV 40172
#define ID_MARKER_NEXT 40173
#define ID_INSERT_MARKERRGN 40174
#define ID_INSERT_MARKER 40157
#define ID_LOOP_SETSTART 40222
#define ID_LOOP_SETEND 40223
#define ID_METRONOME 40364
#define ID_GOTO_MARKER1 40161
#define ID_SET_MARKER1 40657

// Reaper track automation modes
enum AutoMode
{
  AUTO_MODE_TRIM,
  AUTO_MODE_READ,
  AUTO_MODE_TOUCH,
  AUTO_MODE_WRITE,
  AUTO_MODE_LATCH,
};

#define DELETE_ASYNC(x) \
  do                    \
  {                     \
    if (x)              \
      (x)->Destroy();   \
  } while (0)

// midi_Output *CreateThreadedMIDIOutput(midi_Output *output); // returns null on null

#define PREF_DIRCH WDL_DIRCHAR
#define PREF_DIRSTR WDL_DIRCHAR_STR

#define DEFAULT_DEVICE_REMAP()                                                  \
  if (call == CSURF_EXT_MIDI_DEVICE_REMAP)                                      \
  {                                                                             \
    if ((int)(INT_PTR)parm1 == 0 && m_midi_in_dev == (int)(INT_PTR)parm2)       \
    {                                                                           \
      m_midi_in_dev = (int)(INT_PTR)parm3;                                      \
      return 1;                                                                 \
    }                                                                           \
    else if ((int)(INT_PTR)parm1 == 1 && m_midi_out_dev == (int)(INT_PTR)parm2) \
    {                                                                           \
      m_midi_out_dev = (int)(INT_PTR)parm3;                                     \
      return 1;                                                                 \
    }                                                                           \
  }

#endif
