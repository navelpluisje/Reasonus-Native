/*
** reaper_csurf
** FaderPort V2 support
** Copyright (C) 2007-2008 Cockos Incorporated
** License: LGPL.
*/

#include <string>
#include <vector>
#include <reaper_plugin.h>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include "../controls/csurf_button.hpp"
#include "../resource.h"
#include "../shared/csurf.h"
#include "../shared/csurf_transport_manager.hpp"
#include "../shared/csurf_utils.hpp"
#include "csurf_fp_v2_session_manager.cpp"
#include "csurf_fp_v2_track_manager.hpp"
#include "csurf_fp_v2_automation_manager.cpp"
#include "csurf_fp_v2_general_control_manager.cpp"
#include "csurf_fp_v2_navigator.hpp"
#include "csurf_fp_v2_ui_init.hpp"

extern HWND g_hwnd;
extern REAPER_PLUGIN_HINSTANCE g_hInst;

class CSurf_FaderPortV2 : public IReaperControlSurface
{
  int m_midi_in_dev, m_midi_out_dev;
  midi_Output *m_midiout;
  midi_Input *m_midiin;

  CSurf_Context *context;
  CSurf_FP_V2_SessionManager *sessionManager;
  CSurf_FP_V2_Navigator *trackNavigator;
  CSurf_FP_V2_TrackManager *trackManager;
  CSurf_TransportManager *transportManager;
  CSurf_FP_V2_AutomationManager *automationManager;
  CSurf_FP_V2_GeneralControlManager *generalControlManager;

  DWORD surface_update_lastrun;
  DWORD surface_update_keepalive;

  WDL_String descspace;
  char configtmp[1024];

  void OnMIDIEvent(MIDI_event_t *evt)
  {

    /**
     * Fader values
     */
    if (evt->midi_message[0] == FADER_1)
    {
      trackManager->HandleFaderMove(evt->midi_message[2], evt->midi_message[1]);
    }

    /**
     * ENCODERS
     */
    else if (evt->midi_message[0] == MIDI_MESSAGE_ENDCODER)
    {
      if (evt->midi_message[1] == ENCODER_PAN)
      {
        sessionManager->HandleSessionNavEncoderChange(evt->midi_message[2]);
      }
    }

    /**
     * BUTTONS
     */
    else if (evt->midi_message[0] == MIDI_MESSAGE_BUTTON)
    {
      /**
       * Fader Touch
       */
      if (evt->midi_message[1] == FADER_TOUCH_1)
      {
        trackManager->HandleFaderTouch(evt->midi_message[2]);
      }

      /**
       * Track Mute Buttons
       */
      else if (evt->midi_message[1] == BTN_MUTE_1)
      {
        trackManager->HandleMuteClick(1, evt->midi_message[2]);
      }
      /**
       * Track Solo Buttons
       */
      else if (evt->midi_message[1] == BTN_SOLO_1)
      {
        trackManager->HandleSoloClick(1, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_ARM)
      {
        trackManager->HandleArmClick(1, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_BYPASS)
      {
        trackManager->HandleBypassClick(1, evt->midi_message[2]);
      }

      /**
       * Transport Buttons
       */
      else if (evt->midi_message[1] == BTN_PLAY)
      {
        transportManager->HandlePlayButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_STOP)
      {
        transportManager->HandleStopButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_RECORD)
      {
        transportManager->HandleRecordButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_REWIND)
      {
        transportManager->HandleRewindButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_FORWARD)
      {
        transportManager->HandleForwardButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_LOOP)
      {
        transportManager->HandleRepeatButton(evt->midi_message[2]);
      }

      /**
       * General Control Management
       */
      else if (evt->midi_message[1] == BTN_SHIFT_LEFT)
      {
        generalControlManager->HandleShiftButton(evt->midi_message[2]);
        sessionManager->Refresh();
      }

      /**
       * Automation Management
       */
      else if (evt->midi_message[1] == BTN_TOUCH)
      {
        automationManager->HandleTouchButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_WRITE)
      {
        automationManager->HandleWriteButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_READ)
      {
        automationManager->HandleReadButton(evt->midi_message[2]);
      }

      /**
       * Session Manager Buttons
       */
      else if (evt->midi_message[1] == BTN_LINK)
      {
        sessionManager->HandleLinkButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_PAN)
      {
        sessionManager->HandlePanButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_CHANNEL)
      {
        sessionManager->HandleChannelButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_ZOOM)
      {
        // sessionManager->HandleZoomButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SCROLL)
      {
        sessionManager->HandleScrollButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_MASTER)
      {
        sessionManager->HandleMasterButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_CLICK)
      {
        sessionManager->HandleClickButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SECTION)
      {
        sessionManager->HandleSectionButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_MARKER)
      {
        sessionManager->HandleMarkerButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_NEXT)
      {
        sessionManager->HandleNextButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_PREV)
      {
        sessionManager->HandlePrevButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == ENCODER_CLICK_PAN)
      {
        sessionManager->HandleEncoderClick(evt->midi_message[2]);
      }
    }
  }

public:
  CSurf_FaderPortV2(int indev, int outdev, int *errStats)
  {
    (void)indev;
    (void)outdev;

    /**
     * First we check if we have the ini file. If not we create it with default values
     */
    mINI::INIStructure ini;
    readAndCreateIni(ini, FP_V2);

    errStats = 0;
    m_midi_in_dev = stoi(ini["surface"]["midiin"]);
    m_midi_out_dev = stoi(ini["surface"]["midiout"]);

    surface_update_lastrun = 0;

    // create midi hardware access
    m_midiin = m_midi_in_dev >= 0 ? CreateMIDIInput(m_midi_in_dev) : NULL;
    m_midiout = m_midi_out_dev >= 0 ? CreateMIDIOutput(m_midi_out_dev, false, NULL) : NULL;

    context = new CSurf_Context(1);
    context->SetMuteSoloMomentary(ini["surface"].has("mute-solo-momentary") && ini["surface"]["mute-solo-momentary"] == "1");

    trackNavigator = new CSurf_FP_V2_Navigator(context);
    generalControlManager = new CSurf_FP_V2_GeneralControlManager(context, trackNavigator, m_midiout);
    sessionManager = new CSurf_FP_V2_SessionManager(context, trackNavigator, m_midiout);
    trackManager = new CSurf_FP_V2_TrackManager(context, trackNavigator, m_midiout);
    transportManager = new CSurf_TransportManager(context, m_midiout);
    automationManager = new CSurf_FP_V2_AutomationManager(context, trackNavigator, m_midiout);

    if (errStats)
    {
      ShowConsoleMsg("Error: ");
      ShowConsoleMsg((char *)errStats);
      if (m_midi_in_dev >= 0 && !m_midiin)
      {
        *errStats |= 1;
      }
      if (m_midi_out_dev >= 0 && !m_midiout)
      {
        *errStats |= 2;
      }
    }

    if (m_midiin)
    {
      m_midiin->start();
    }

    if (m_midiout)
    {
      m_midiout->Send(0xb0, 0x00, 0x06, -1);
      m_midiout->Send(0xb0, 0x20, 0x27, -1);
    }
  }

  ~CSurf_FaderPortV2()
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

  const char *GetTypeString()
  {
    return "REASONUS_FADERPORT_V2";
  }

  const char *GetDescString()
  {
    snprintf(configtmp, 100, "ReaSonus FaderPort V2 (dev %d, %d)", m_midi_in_dev, m_midi_out_dev);
    return configtmp;
  }

  const char *GetConfigString() // string of configuration data
  {
    snprintf(configtmp, 100, "0 0 %d %d", m_midi_in_dev, m_midi_out_dev);

    return configtmp;
  }

  void CloseNoReset()
  {
    DELETE_ASYNC(m_midiout);
    DELETE_ASYNC(m_midiin);
    m_midiout = 0;
    m_midiin = 0;
  }

  void Run()
  {
    if (m_midiin)
    {
      m_midiin->SwapBufsPrecise(GetTickCount(), 0.0);
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
      DWORD now = GetTickCount();
      if ((now - surface_update_lastrun) >= 100)
      {
        trackManager->UpdateTrack();
        generalControlManager->Update();
        sessionManager->Update();
        transportManager->Update();
        automationManager->Update();

        surface_update_lastrun = now;
      }
      if ((now - surface_update_keepalive) >= 990)
      {
        surface_update_keepalive = now;
        m_midiout->Send(0xa0, 0x00, 0x00, -1);
      }
    }
  }

  // Probably not needed
  // void SetTrackListChange()
  // {
  //   ShowConsoleMsg("SetTrackListChange");
  // }

  void OnTrackSelection(MediaTrack *media_track)
  {
    int trackId = (int)::GetMediaTrackInfo_Value(media_track, "IP_TRACKNUMBER");
    trackNavigator->SetOffset(trackId - 1);
  }
};

static IReaperControlSurface *createFuncV2(const char *type_string, const char *configString, int *errStats)
{
  (void)type_string;
  int parms[4];
  parseParms(configString, parms);

  return new CSurf_FaderPortV2(parms[2], parms[3], errStats);
}

reaper_csurf_reg_t csurf_faderport_v2_reg = {
    "REASONUS_FADERPORT_V2",
    "ReaSonus FaderPort V2",
    createFuncV2,
    CSURF_FP_V2_UI_INIT::CreateInitDialog,
};
