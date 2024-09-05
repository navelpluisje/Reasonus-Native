/*
** reaper_csurf
** FaderPort support
** Copyright (C) 2007-2008 Cockos Incorporated
** License: LGPL.
*/

#define LOCALIZE_IMPORT_PREFIX "csurf_"
#include <reaper_plugin.h>
#include <localize-import.h>
#include <reaper_plugin_functions.h>
#include <string>
#include "csurf.h"
#include "csurf_session_manager.cpp"
#include "csurf_shift_manager.cpp"
#include "csurf_track_manager.cpp"
#include "csurf_navigator.cpp"
#include "csurf_button.hpp"
#include <WDL/ptrlist.h>
#include <vector>

/*
 * Todo: automation status, automation mode setting using "auto" button, more
 */

// static bool g_csurf_mcpmode = false; // we may wish to allow an action to set this

// static double int14ToVol(unsigned char msb, unsigned char lsb)
// {
//   int val = lsb | (msb << 7);
//   double pos = ((double)val * 1000.0) / 16383.0;
//   pos = SLIDER2DB(pos);
//   return DB2VAL(pos);
// }
// static double int14ToPan(unsigned char msb, unsigned char lsb)
// {
//   int val = lsb | (msb << 7);
//   return 1.0 - (val / (16383.0 * 0.5));
// }

// static int volToInt14(double vol)
// {
//   double d = (DB2SLIDER(VAL2DB(vol)) * 16383.0 / 1000.0);
//   if (d < 0.0)
//     d = 0.0;
//   else if (d > 16383.0)
//     d = 16383.0;

//   return (int)(d + 0.5);
// }
// static int panToInt14(double pan)
// {
//   double d = ((1.0 - pan) * 16383.0 * 0.5);
//   if (d < 0.0)
//     d = 0.0;
//   else if (d > 16383.0)
//     d = 16383.0;

//   return (int)(d + 0.5);
// }

class CSurf_FaderPort : public IReaperControlSurface
{
  int m_midi_in_dev, m_midi_out_dev;
  midi_Output *m_midiout;
  midi_Input *m_midiin;

  CSurf_Context *context;

  CSurf_SessionManager *sessionManager;
  CSurf_ShiftManager *shiftManager;
  CSurf_TrackManager *trackManager;
  CSurf_Navigator *trackNavigator;

  CSurf_Button *playButton;
  CSurf_Button *stopButton;
  CSurf_Button *recordButton;
  CSurf_Button *loopButton;
  // CSurf_ColorButton *linkButton;

  std::vector<CSurf_Track *> tracks;

  int m_vol_lastpos;
  int m_flipmode;
  int m_faderport_lasthw, m_faderport_buttonstates;

  char m_fader_touchstate;
  int m_bank_offset;

  DWORD m_frameupd_lastrun;
  int m_button_states;
  DWORD m_buttonstate_lastrun;
  DWORD m_pan_lasttouch;

  WDL_String descspace;
  char configtmp[1024];

  void OnMIDIEvent(MIDI_event_t *evt)
  {

    /**
     * ENCODERS & VALUEBAR
     */
    if (evt->midi_message[0] == MIDI_MESSAGE_ENDCODER)
    {
      //       if (evt->midi_message[1] == 0)
      //         m_faderport_lasthw = evt->midi_message[2];
      //       else if (evt->midi_message[1] == 0x20)
      //       {
      //         MediaTrack *tr = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);

      //         if (tr)
      //         {
      //           if (m_flipmode)
      //           {
      //             CSurf_SetSurfacePan(tr, CSurf_OnPanChange(tr, int14ToPan(m_faderport_lasthw, evt->midi_message[2]), false), NULL);
      //           }
      //           else
      //             CSurf_SetSurfaceVolume(tr, CSurf_OnVolumeChange(tr, int14ToVol(m_faderport_lasthw, evt->midi_message[2]), false), NULL);
      //         }
      //       }
    }

    /**
     * BUTTONS
     */
    else if (evt->midi_message[0] == MIDI_MESSAGE_BUTTON)
    {
      /**
       * The next and previous button light up when pressed so need the '0' value when releasing
       */
      if (!evt->midi_message[2] && !(
                                       evt->midi_message[1] == BTN_NEXT || evt->midi_message[1] == BTN_PREV || evt->midi_message[1] == BTN_SHIFT_LEFT || evt->midi_message[1] == BTN_SHIFT_RIGHT || evt->midi_message[1] == BTN_ARM))
      {
        return;
      }
      if (evt->midi_message[1] == 0x7f)
      {
        //         m_fader_touchstate = !!evt->midi_message[2];
      }
      //       else if (evt->midi_message[1] == 0x2) // shift
      //       {
      //         if (evt->midi_message[2])
      //           m_faderport_buttonstates |= 2;
      //         else
      //           m_faderport_buttonstates &= ~2;

      //         if (m_midiout)
      //           m_midiout->Send(0xa0, 5, evt->midi_message[2], -1);
      //       }
      //       else if (evt->midi_message[1] == 0x10) // rec arm key push
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           if (m_faderport_buttonstates & 2)
      //             ClearAllRecArmed();
      //           else
      //           {
      //             MediaTrack *tr = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);
      //             if (tr)
      //               SetSurfaceRecArm(tr, CSurf_OnRecArmChange(tr, -1));
      //           }
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0x17) // automation off
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           MediaTrack *tr = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);

      //           if (tr)
      //           {
      //             SetTrackAutomationMode(tr, 0);
      //             CSurf_SetAutoMode(-1, NULL);
      //           }
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0x8) // automation  touch
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           MediaTrack *tr = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);

      //           if (tr)
      //           {
      //             SetTrackAutomationMode(tr, 2);
      //             CSurf_SetAutoMode(-1, NULL);
      //           }
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0x9) // automation write
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           MediaTrack *tr = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);

      //           if (tr)
      //           {
      //             SetTrackAutomationMode(tr, 3);
      //             CSurf_SetAutoMode(-1, NULL);
      //           }
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0xa) // automation read
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           MediaTrack *tr = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);

      //           if (tr)
      //           {
      //             SetTrackAutomationMode(tr, 1);
      //             CSurf_SetAutoMode(-1, NULL);
      //           }
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0xb || evt->midi_message[1] == 0xc || evt->midi_message[1] == 0xd) // automation read
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           int a = (evt->midi_message[1] - 0xb) + 8;
      //           if (m_faderport_buttonstates & 2)
      //             a += 3;
      //           MIDI_event_t evt = {0, 3, {0xbf, a, 0}};
      //           kbd_OnMidiEvent(&evt, -1);
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0x3) // rew
      //       {
      //         m_button_states &= ~1;
      //         if (evt->midi_message[2])
      //         {
      //           if ((m_faderport_buttonstates & 2))
      //             CSurf_GoStart();
      //           else
      //             m_button_states |= 1;
      //         }
      //         if (m_midiout)
      //           m_midiout->Send(0xa0, 4, evt->midi_message[2], -1);
      //       }
      //       else if (evt->midi_message[1] == 0x4) // fwd
      //       {
      //         m_button_states &= ~2;
      //         if (evt->midi_message[2])
      //         {
      //           if ((m_faderport_buttonstates & 2))
      //           {
      //             CSurf_GoEnd();
      //           }
      //           else
      //             m_button_states |= 2;
      //         }
      //         if (m_midiout)
      //           m_midiout->Send(0xa0, 3, evt->midi_message[2], -1);
      //       }
      //       else if (evt->midi_message[1] == 0x5) // stop
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           CSurf_OnStop();
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0x1) // punch
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           if ((m_faderport_buttonstates & 2))
      //           {
      //             SendMessage(g_hwnd, WM_COMMAND, ID_MARKER_PREV, 0);
      //           }
      //           else
      //             SendMessage(g_hwnd, WM_COMMAND, ID_LOOP_SETSTART, 0);
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0x0) // user
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           if ((m_faderport_buttonstates & 2))
      //           {
      //             SendMessage(g_hwnd, WM_COMMAND, ID_MARKER_NEXT, 0);
      //           }
      //           else
      //             SendMessage(g_hwnd, WM_COMMAND, ID_LOOP_SETEND, 0);
      //         }
      //       }

      /**
       * Track Select Buttons
       */
      else if (evt->midi_message[1] >= BTN_SELECT_1 && evt->midi_message[1] <= BTN_SELECT_8)
      {
        trackManager->HandleSelectClick(evt->midi_message[1] - BTN_SELECT_1);
      }
      /**
       * Track Mute Buttons
       */
      else if (evt->midi_message[1] >= BTN_MUTE_1 && evt->midi_message[1] <= BTN_MUTE_8)
      {
        trackManager->HandleMuteClick(evt->midi_message[1] - BTN_MUTE_1);
      }
      /**
       * Track Solo Buttons
       */
      else if (evt->midi_message[1] >= BTN_SOLO_1 && evt->midi_message[1] <= BTN_SOLO_8)
      {
        trackManager->HandleSoloClick(evt->midi_message[1] - BTN_SOLO_1);
      }

      /**
       * Transport Buttons
       */
      else if (evt->midi_message[1] == BTN_PLAY)
      {
        CSurf_OnPlay();
      }
      else if (evt->midi_message[1] == BTN_STOP)
      {
        CSurf_OnStop();
      }
      else if (evt->midi_message[1] == BTN_RECORD)
      {
        CSurf_OnRecord();
      }
      else if (evt->midi_message[1] == BTN_REWIND)
      {
        CSurf_OnRewFwd(1, -1);
      }
      else if (evt->midi_message[1] == BTN_FORWARD)
      {
        CSurf_OnRewFwd(1, 1);
      }
      else if (evt->midi_message[1] == BTN_LOOP)
      {
        SendMessage(g_hwnd, WM_COMMAND, IDC_REPEAT, 0);
      }
      else if (evt->midi_message[1] == BTN_FORWARD)
      {
        // Implement Fast Forward
      }
      else if (evt->midi_message[1] == BTN_REWIND)
      {
        // Implement Fast Rewind
      }

      /**
       * Arm Button
       */
      else if (evt->midi_message[1] == BTN_ARM) // loop
      {
        shiftManager->HandleArmButton(evt->midi_message[2], m_midiout);
        sessionManager->Refresh(m_midiout);
      }
      /**
       * Shift Manager Buttons
       */
      else if (evt->midi_message[1] == BTN_SHIFT_LEFT) // loop
      {
        shiftManager->HandleShiftLeftButton(evt->midi_message[2], m_midiout);
        sessionManager->Refresh(m_midiout);
      }
      else if (evt->midi_message[1] == BTN_SHIFT_RIGHT) // loop
      {
        // shiftManager->HandleShiftLeftButton(evt->midi_message[2], m_midiout);
      }

      /**
       * Session Manager Buttons
       */
      else if (evt->midi_message[1] == BTN_CHANNEL) // loop
      {
        // sessionManager->HandleChannelButton(m_midiout);
      }
      else if (evt->midi_message[1] == BTN_ZOOM) // loop
      {
        // sessionManager->HandleZoomButton(m_midiout);
      }
      else if (evt->midi_message[1] == BTN_SCROLL) // loop
      {
        // sessionManager->HandleScrollButton(m_midiout);
      }
      else if (evt->midi_message[1] == BTN_BANK) // loop
      {
        // sessionManager->HandleBankButton(m_midiout);
      }
      else if (evt->midi_message[1] == BTN_MASTER) // loop
      {
        // sessionManager->HandleMasterButton(m_midiout);
      }
      else if (evt->midi_message[1] == BTN_CLICK) // loop
      {
        // sessionManager->HandleClickButton(m_midiout);
      }
      else if (evt->midi_message[1] == BTN_SECTION) // loop
      {
        // sessionManager->HandleSectionButton(m_midiout);
      }
      else if (evt->midi_message[1] == BTN_MARKER) // loop
      {
        // sessionManager->HandleMarkerButton(m_midiout);
      }
      else if (evt->midi_message[1] == BTN_NEXT) // loop
      {
        // sessionManager->HandleNextButton(evt->midi_message[2], m_midiout);
      }
      else if (evt->midi_message[1] == BTN_PREV) // loop
      {
        // sessionManager->HandlePrevButton(evt->midi_message[2], m_midiout);
      }

      //       else if (evt->midi_message[1] == 0x0e) // undo/redo
      //       {
      //         if (evt->midi_message[2])
      //         {

      //           SendMessage(g_hwnd, WM_COMMAND, (m_faderport_buttonstates & 2) ? IDC_EDIT_REDO : IDC_EDIT_UNDO, 0);
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0x16) // output (flip)
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           m_flipmode = !m_flipmode;
      //           if (m_midiout)
      //             m_midiout->Send(0xa0, 0x11, m_flipmode ? 1 : 0, -1);
      //           CSurf_ResetAllCachedVolPanStates();
      //           TrackList_UpdateAllExternalSurfaces();
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0x11) // solo key push
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           if (m_faderport_buttonstates & 2)
      //             SoloAllTracks(0);
      //           else
      //           {
      //             MediaTrack *tr = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);
      //             if (tr)
      //               SetSurfaceSolo(tr, CSurf_OnSoloChange(tr, -1));
      //           }
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0x12) // mute key push
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           if (m_faderport_buttonstates & 2)
      //             MuteAllTracks(false);
      //           else
      //           {
      //             MediaTrack *tr = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);
      //             if (tr)
      //               SetSurfaceMute(tr, CSurf_OnMuteChange(tr, -1));
      //           }
      //         }
      //       }
      //       else if (evt->midi_message[1] == 0x13) // prevchan
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           AdjustBankOffset((m_faderport_buttonstates & 1) ? -8 : -1, true);
      //           TrackList_UpdateAllExternalSurfaces();
      //         }
      //         if (m_midiout)
      //           m_midiout->Send(0xa0, 0x14, evt->midi_message[2], -1);
      //       }
      //       else if (evt->midi_message[1] == 0x14) // bank
      //       {
      //         if (evt->midi_message[2])
      //           m_faderport_buttonstates |= 1;
      //         else
      //           m_faderport_buttonstates &= ~1;

      //         if (m_midiout)
      //           m_midiout->Send(0xa0, 0x13, evt->midi_message[2], -1);
      //       }
      //       else if (evt->midi_message[1] == 0x15) // nextchan
      //       {
      //         if (evt->midi_message[2])
      //         {
      //           AdjustBankOffset((m_faderport_buttonstates & 1) ? 8 : 1, true);
      //           TrackList_UpdateAllExternalSurfaces();
      //         }
      //         if (m_midiout)
      //           m_midiout->Send(0xa0, 0x12, evt->midi_message[2], -1);
      //       }
      //       else if (evt->midi_message[1] == 0x7e)
      //       {
      //         // passthrough footswitch
      //         MIDI_event_t e = {0, 3, {0x97, evt->midi_message[1], evt->midi_message[2]}};
      //         kbd_OnMidiEvent(&e, -1);
      //       }
      //     }
      //     else if (evt->midi_message[0] == 0xe0)
      //     {
      //       if (!evt->midi_message[1])
      //       {
      //         m_pan_lasttouch = timeGetTime();

      //         double adj = 0.02;
      //         if (evt->midi_message[2] > 0x3f)
      //           adj = -adj;
      //         MediaTrack *tr = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);

      //         if (tr)
      //         {
      //           if (m_flipmode)
      //           {
      //             CSurf_SetSurfaceVolume(tr, CSurf_OnVolumeChange(tr, adj * 11.0, true), NULL);
      //           }
      //           else
      //             CSurf_SetSurfacePan(tr, CSurf_OnPanChange(tr, adj, true), NULL);
      //         }
      //       }
    }
  }

  //   void AdjustBankOffset(int amt, bool dosel)
  //   {
  //     if (!amt)
  //       return;

  //     if (amt < 0)
  //     {
  //       if (m_bank_offset > 0)
  //       {
  //         m_bank_offset += amt;
  //         if (m_bank_offset < 0)
  //           m_bank_offset = 0;

  //         if (dosel)
  //         {
  //           int x;
  //           MediaTrack *t = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);
  //           for (x = 0;; x++)
  //           {
  //             int f = 0;
  //             if (!GetTrackInfo(x - 1, &f))
  //               break;

  //             MediaTrack *tt = CSurf_TrackFromID(x, false);
  //             bool sel = tt == t;
  //             if (tt && !(f & 2) == sel)
  //             {
  //               SetTrackSelected(tt, sel);
  //             }
  //           }
  //         }
  //       }
  //     }
  //     else
  //     {
  //       int msize = CSurf_NumTracks(g_csurf_mcpmode);

  //       if (m_bank_offset < msize)
  //       {
  //         m_bank_offset += amt;
  //         if (m_bank_offset > msize)
  //           m_bank_offset = msize;

  //         if (dosel)
  //         {
  //           int x;
  //           MediaTrack *t = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);
  //           for (x = 0;; x++)
  //           {
  //             int f = 0;
  //             if (!GetTrackInfo(x - 1, &f))
  //               break;

  //             MediaTrack *tt = CSurf_TrackFromID(x, false);
  //             bool sel = tt == t;
  //             if (tt && !(f & 2) == sel)
  //             {
  //               SetTrackSelected(tt, sel);
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }

public:
  CSurf_FaderPort(int indev, int outdev, int *errStats)
  {
    (void)indev;
    (void)outdev;
    errStats = 0;
    m_midi_in_dev = 6;   // indev;
    m_midi_out_dev = 11; // outdev;

    m_faderport_lasthw = 0;
    m_faderport_buttonstates = 0;
    m_flipmode = 0;
    m_vol_lastpos = -1000;

    m_fader_touchstate = 0;

    m_bank_offset = 0;
    m_frameupd_lastrun = 0;
    m_button_states = 0;
    m_buttonstate_lastrun = 0;
    m_pan_lasttouch = 0;

    // create midi hardware access
    m_midiin = m_midi_in_dev >= 0 ? CreateMIDIInput(m_midi_in_dev) : NULL;
    m_midiout = m_midi_out_dev >= 0 ? CreateMIDIOutput(m_midi_out_dev, false, NULL) : NULL;

    context = new CSurf_Context();

    ButtonColor active{0x7f, 0x7f, 0x7f};
    ButtonColor dim{0x7f, 0x00, 0x7f};
    for (int i = 0; i < 8; i++)
    {
      CSurf_Track *track = new CSurf_Track(active, dim, i, context, m_midiout);
      tracks.push_back(track);
    }

    sessionManager = new CSurf_SessionManager(context, m_midiout);
    trackNavigator = new CSurf_Navigator();
    shiftManager = new CSurf_ShiftManager(context, m_midiout);
    trackManager = new CSurf_TrackManager(tracks, trackNavigator, context, m_midiout);

    playButton = new CSurf_Button(BTN_PLAY, BTN_VALUE_OFF);
    stopButton = new CSurf_Button(BTN_STOP, BTN_VALUE_OFF);
    recordButton = new CSurf_Button(BTN_RECORD, BTN_VALUE_OFF);
    loopButton = new CSurf_Button(BTN_RECORD, BTN_VALUE_OFF);

    // int colorActive[3] = {0x7f, 0x7f, 0x00};
    // int colorDim[3] = {0x00, 0x7f, 0x00};
    // linkButton = new CSurf_ColorButton(colorActive, colorDim, BTN_LINK, BTN_VALUE_OFF);

    if (errStats)
    {
      ShowConsoleMsg("Error: ");
      ShowConsoleMsg((char *)errStats);
      if (m_midi_in_dev >= 0 && !m_midiin)
        *errStats |= 1;
      if (m_midi_out_dev >= 0 && !m_midiout)
        *errStats |= 2;
    }

    if (m_midiin)
    {
      m_midiin->start();
    }

    if (m_midiout)
    {
      m_midiout->Send(0xb0, 0x00, 0x06, -1);
      m_midiout->Send(0xb0, 0x20, 0x27, -1);

      int x;
      for (x = 0; x < 0x30; x++) // lights out
        m_midiout->Send(0xa0, x, 0x00, -1);

      // m_midiout->Send(0xa0,0xf,m_flipmode?1:0);// fucko

      m_midiout->Send(0x91, 0x00, 0x64, -1); // send these every so often?
    }
  }

  ~CSurf_FaderPort()
  {
    if (m_midiout)
    {
      int x;
      for (x = 0; x < 0x30; x++) // lights out§
        m_midiout->Send(0xa0, x, 0x00, -1);
      Sleep(5);
    }

    DELETE_ASYNC(m_midiout);
    DELETE_ASYNC(m_midiin);
  }

  CSurf_Context GetContext()
  {
    return *context;
  }

  const char *
  GetTypeString()
  {
    return "REASONUS_FADERPORT";
  }

  const char *GetDescString()
  {
    descspace.SetFormatted(512, __LOCALIZE_VERFMT("ReaSonus FaderPort (dev %d,%d)", "csurf"), m_midi_in_dev, m_midi_out_dev);
    return descspace.Get();
  }

  const char *GetConfigString() // string of configuration data
  {
    snprintf(configtmp, 100, "0 0 %d %d", m_midi_in_dev, m_midi_out_dev);

    return configtmp;
  }

  //   void CloseNoReset()
  //   {
  //     DELETE_ASYNC(m_midiout);
  //     DELETE_ASYNC(m_midiin);
  //     m_midiout = 0;
  //     m_midiin = 0;
  //   }

  void Run()
  {
    if (m_midiin)
    {
      m_midiin->SwapBufsPrecise(timeGetTime(), 0.0);
      int l = 0;
      MIDI_eventlist *list = m_midiin->GetReadBuf();
      MIDI_event_t *evts;

      while ((evts = list->EnumItems(&l)))
      {
        OnMIDIEvent(evts);
      }
    }
    if (m_midiout)
    {
      DWORD now = timeGetTime();
      if ((now - m_buttonstate_lastrun) >= 1000)
      {
        trackManager->UpdateTracks(m_midiout);
        m_buttonstate_lastrun = now;
      }

      if (m_button_states)
      {
        DWORD now = timeGetTime();
        if ((now - m_buttonstate_lastrun) >= 100)
        {
          m_buttonstate_lastrun = now;

          if ((m_button_states & 3) != 3)
          {
            if (m_button_states & 1)
            {
              CSurf_OnRewFwd(1, -1);
            }
            else if (m_button_states & 2)
            {
              CSurf_OnRewFwd(1, 1);
            }
          }
        }
      }
    }
  }

  void SetTrackListChange()
  {
    SetAutoMode(0);
  }

  // #define FIXID(id)                                            \
//   const int oid = CSurf_TrackToID(trackid, g_csurf_mcpmode); \
//   const int id = oid - m_bank_offset;

  //   void SetSurfaceVolume(MediaTrack *trackid, double volume)
  //   {
  //     FIXID(id)
  //     if (m_midiout && !id && !m_flipmode)
  //     {
  //       int volint = volToInt14(volume);
  //       volint /= 16;

  //       if (m_vol_lastpos != volint)
  //       {
  //         m_vol_lastpos = volint;
  //         m_midiout->Send(0xb0, 0x00, volint >> 7, -1);
  //         m_midiout->Send(0xb0, 0x20, volint & 127, -1);
  //       }
  //     }
  //   }
  //   void SetSurfacePan(MediaTrack *trackid, double pan)
  //   {
  //     FIXID(id)
  //     if (m_midiout && !id && m_flipmode)
  //     {
  //       int volint = panToInt14(pan);
  //       volint /= 16;

  //       if (m_vol_lastpos != volint)
  //       {
  //         m_vol_lastpos = volint;
  //         m_midiout->Send(0xb0, 0x00, volint >> 7, -1);
  //         m_midiout->Send(0xb0, 0x20, volint & 127, -1);
  //       }
  //     }
  //   }
  //   void SetSurfaceMute(MediaTrack *trackid, bool mute)
  //   {
  //     FIXID(id)
  //     if (!id && m_midiout)
  //       m_midiout->Send(0xa0, 0x15, mute ? 1 : 0, -1);
  //   }
  //   void SetSurfaceSelected(MediaTrack *trackid, bool selected)
  //   {
  //     (void)trackid;
  //     (void)selected;
  //   }
  //   void SetSurfaceSolo(MediaTrack *trackid, bool solo)
  //   {
  //     FIXID(id)
  //     if (!id && m_midiout)
  //       m_midiout->Send(0xa0, 0x16, solo ? 1 : 0, -1);
  //   }
  //   void SetSurfaceRecArm(MediaTrack *trackid, bool recarm)
  //   {
  //     FIXID(id)
  //     if (!id && m_midiout)
  //       m_midiout->Send(0xa0, 0x17, recarm ? 1 : 0, -1);
  //   }

  void SetPlayState(bool play, bool pause, bool rec)
  {
    (void)play;
    (void)pause;
    (void)rec;

    if (m_midiout)
    {
      // Btn_Value play_value = play ? BTN_VALUE_ON : pause ? BTN_VALUE_BLINK
      //                                                    : BTN_VALUE_OFF;
      // playButton->SetValue(play_value, m_midiout);
      // linkButton->SetValue(play_value, m_midiout);
      // recordButton->SetValue(rec ? BTN_VALUE_ON : BTN_VALUE_OFF, m_midiout);
      // stopButton->SetValue((!play && !pause) ? BTN_VALUE_ON : BTN_VALUE_OFF, m_midiout);
    }
  }

  void SetRepeatState(bool rep)
  {
    if (m_midiout)
    {
      loopButton->SetValue(rep ? BTN_VALUE_ON : BTN_VALUE_OFF, m_midiout);
    }
  }

  //   void SetTrackTitle(MediaTrack *trackid, const char *title)
  //   {
  //     (void)trackid;
  //     (void)title;
  //   }

  //   bool GetTouchState(MediaTrack *trackid, int isPan)
  //   {
  //     if (isPan != 0 && isPan != 1)
  //       return false;

  //     FIXID(id)
  //     if (!id)
  //     {
  //       if (!m_flipmode != !isPan)
  //       {
  //         if (m_pan_lasttouch == 1 || (timeGetTime() - m_pan_lasttouch) < 3000) // fake touch, go for 3s after last movement
  //         {
  //           return true;
  //         }
  //         return false;
  //       }
  //       return !!m_fader_touchstate;
  //     }
  //     return false;
  //   }

  //   void SetAutoMode(int mode)
  //   {
  //     if (m_midiout)
  //     {
  //       MediaTrack *tr = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);
  //       if (tr)
  //         mode = GetTrackAutomationMode(tr);

  //       if (mode < 0)
  //         return;

  //       m_midiout->Send(0xa0, 0x10, 0, -1); // mode==0); // dont set off light, it disables the fader?!
  //       m_midiout->Send(0xa0, 0xf, mode == 2 || mode == 4, -1);
  //       m_midiout->Send(0xa0, 0xe, mode == 3, -1);
  //       m_midiout->Send(0xa0, 0xd, mode == 1, -1);
  //     }
  //   }

  //   void ResetCachedVolPanStates()
  //   {
  //     m_vol_lastpos = -1000;
  //   }

  //   void OnTrackSelection(MediaTrack *trackid)
  //   {
  //     int newpos = CSurf_TrackToID(trackid, g_csurf_mcpmode);
  //     if (newpos >= 0 && newpos != m_bank_offset)
  //     {
  //       AdjustBankOffset(newpos - m_bank_offset, false);
  //       TrackList_UpdateAllExternalSurfaces();
  //     }
  //   }

  //   bool IsKeyDown(int key)
  //   {
  //     (void)key;
  //     return false;
  //   }

  //   virtual int Extended(int call, void *parm1, void *parm2, void *parm3)
  //   {
  //     DEFAULT_DEVICE_REMAP()
  //     return 0;
  //   }
};

static void parseParms(const char *str, int parms[4])
{
  parms[0] = 0;
  parms[1] = 9;
  parms[2] = parms[3] = -1;

  const char *p = str;
  if (p)
  {
    int x = 0;
    while (x < 4)
    {
      while (*p == ' ')
        p++;
      if ((*p < '0' || *p > '9') && *p != '-')
        break;
      parms[x++] = atoi(p);
      while (*p && *p != ' ')
        p++;
    }
  }
}

static IReaperControlSurface *createFunc(const char *type_string, const char *configString, int *errStats)
{
  // ShowConsoleMsg("IReaperControlSurface");

  (void)type_string;

  int parms[4];
  parseParms(configString, parms);

  return new CSurf_FaderPort(parms[2], parms[3], errStats);
}

static WDL_DLGRET dlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_INITDIALOG:
  {
    int parms[4];
    parseParms((const char *)lParam, parms);

    // ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT1), SW_HIDE);
    // ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT1_LBL), SW_HIDE);
    // ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT2), SW_HIDE);
    // ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT2_LBL), SW_HIDE);
    // ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT2_LBL2), SW_HIDE);

    WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO1));
    WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO2));

    int n = GetNumMIDIInputs();
    int x = SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)__LOCALIZE("None", "csurf"));
    SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_SETITEMDATA, x, -1);
    x = SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)__LOCALIZE("None", "csurf"));
    SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_SETITEMDATA, x, -1);
    for (x = 0; x < n; x++)
    {
      char buf[512];
      if (GetMIDIInputName(x, buf, sizeof(buf)))
      {
        int a = SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)buf);
        SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_SETITEMDATA, a, x);
        if (x == parms[2])
          SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_SETCURSEL, a, 0);
      }
    }
    n = GetNumMIDIOutputs();
    for (x = 0; x < n; x++)
    {
      char buf[512];
      if (GetMIDIOutputName(x, buf, sizeof(buf)))
      {
        int a = SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)buf);
        SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_SETITEMDATA, a, x);
        if (x == parms[3])
          SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_SETCURSEL, a, 0);
      }
    }
  }
  break;
  case WM_USER + 1024:
    if (wParam > 1 && lParam)
    {
      char tmp[512];

      int indev = -1, outdev = -1;
      int r = SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
      if (r != CB_ERR)
        indev = SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_GETITEMDATA, r, 0);
      r = SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_GETCURSEL, 0, 0);
      if (r != CB_ERR)
        outdev = SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_GETITEMDATA, r, 0);

      snprintf(tmp, 100, "0 0 %d %d", indev, outdev);
      lstrcpyn((char *)lParam, tmp, wParam);
    }
    break;
  }
  return 0;
}

static HWND configFunc(const char *type_string, HWND parent, const char *initConfigString)
{
  (void)type_string;
  return CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_SURFACEEDIT_MCU), parent, dlgProc, (LPARAM)initConfigString);
}

reaper_csurf_reg_t csurf_faderport_reg =
    {
        "REASONUS_FADERPORT",
        "ReaSonus FaderPort",
        createFunc,
        configFunc,
};

#ifndef _WIN32 // import the resources. Note: if you do not have these files, run "php WDL/swell/mac_resgen.php res.rc" from this directory
#include <WDL/swell/swell-dlggen.h>
// #include "res.rc_mac_dlg"
#endif
