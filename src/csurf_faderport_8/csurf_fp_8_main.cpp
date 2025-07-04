/*
** reaper_csurf
** FaderPort support
** Copyright (C) 2007-2008 Cockos Incorporated
** License: LGPL.
*/

#include <string>
#include <vector>
#include <reaper_plugin.h>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <WDL/ptrlist.h>
#include <reaper_plugin_functions.h>
#include "../controls/csurf_button.hpp"
#include "../resource.h"
#include "../shared/csurf.h"
#include "../shared/csurf_transport_manager.cpp"
#include "../shared/csurf_utils.hpp"
#include "csurf_fp_8_session_manager.cpp"
#include "csurf_fp_8_mix_manager.cpp"
#include "csurf_fp_8_automation_manager.cpp"
#include "csurf_fp_8_general_control_manager.cpp"
#include "csurf_fp_8_last_touched_fx_manager.hpp"
#include "csurf_fp_8_fader_manager.hpp"
#include "csurf_fp_8_ui_init.hpp"
#include "csurf_fp_8_navigator.hpp"

extern HWND g_hwnd;
extern REAPER_PLUGIN_HINSTANCE g_hInst;

class CSurf_FaderPort : public IReaperControlSurface
{
  int m_midi_in_dev, m_midi_out_dev;
  midi_Output *m_midiout;
  midi_Input *m_midiin;

  CSurf_Context *context;
  CSurf_FP_8_SessionManager *sessionManager;
  CSurf_FP_8_FaderManager *faderManager;
  CSurf_FP_8_Navigator *trackNavigator;
  CSurf_FP_8_MixManager *mixManager;
  CSurf_TransportManager *transportManager;
  CSurf_FP_8_AutomationManager *automationManager;
  CSurf_FP_8_GeneralControlManager *generalControlManager;
  CSurf_FP_8_LastTouchedFXManager *lastTouchedFxManager;

  std::vector<CSurf_FP_8_Track *> tracks;
  CSurf_FP_8_Track *lastTouchedFxTrack;

  DWORD surface_update_lastrun;
  DWORD surface_update_keepalive;

  WDL_String descspace;
  char configtmp[1024];

  void OnMIDIEvent(MIDI_event_t *evt)
  {

    /**
     * Fader values
     */
    if (evt->midi_message[0] >= FADER_1 && evt->midi_message[0] <= FADER_16)
    {
      if (
          (context->GetNbChannels() == 7 && context->GetLastTouchedFxMode() && evt->midi_message[0] == FADER_8) ||
          (context->GetNbChannels() == 15 && context->GetLastTouchedFxMode() && evt->midi_message[0] == FADER_16))
      {
        lastTouchedFxManager->HandleFaderMove(evt->midi_message[2], evt->midi_message[1]);
      }
      else
      {
        faderManager->HandleFaderMove(evt->midi_message[0] - FADER_1, evt->midi_message[2], evt->midi_message[1]);
      }
    }

    /**
     * ENCODERS
     */
    else if (evt->midi_message[0] == MIDI_MESSAGE_ENDCODER)
    {
      if (evt->midi_message[1] == ENCODER_PAN)
      {
        generalControlManager->HandleEncoderChange(evt->midi_message[2]);
      }
      if (evt->midi_message[1] == ENCODER_NAV)
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
      if (evt->midi_message[1] >= FADER_TOUCH_1 && evt->midi_message[1] <= FADER_TOUCH_16)
      {
        faderManager->HandleFaderTouch(evt->midi_message[1] - FADER_TOUCH_1, evt->midi_message[2]);
      }

      /**
       * Track Select Buttons
       */
      else if (evt->midi_message[1] >= BTN_SELECT_1 && evt->midi_message[1] <= BTN_SELECT_8)
      {
        faderManager->HandleSelectClick(evt->midi_message[1] - BTN_SELECT_1, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SELECT_9)
      {
        faderManager->HandleSelectClick(8, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] >= BTN_SELECT_10 && evt->midi_message[1] <= BTN_SELECT_16)
      {
        faderManager->HandleSelectClick(evt->midi_message[1] - 0x20 + 8, evt->midi_message[2]);
      }
      /**
       * Track Mute Buttons
       */
      else if (evt->midi_message[1] >= BTN_MUTE_1 && evt->midi_message[1] <= BTN_MUTE_8)
      {
        faderManager->HandleMuteClick(evt->midi_message[1] - BTN_MUTE_1, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] >= BTN_MUTE_9 && evt->midi_message[1] <= BTN_MUTE_16)
      {
        faderManager->HandleMuteClick(evt->midi_message[1] - BTN_MUTE_9 + 8, evt->midi_message[2]);
      }
      /**
       * Track Solo Buttons
       */
      else if (evt->midi_message[1] >= BTN_SOLO_1 && evt->midi_message[1] <= BTN_SOLO_8)
      {
        faderManager->HandleSoloClick(evt->midi_message[1] - BTN_SOLO_1, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SOLO_9)
      {
        faderManager->HandleSoloClick(8, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SOLO_10)
      {
        faderManager->HandleSoloClick(9, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SOLO_11)
      {
        faderManager->HandleSoloClick(10, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SOLO_12)
      {
        faderManager->HandleSoloClick(11, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SOLO_13)
      {
        faderManager->HandleSoloClick(12, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SOLO_14)
      {
        faderManager->HandleSoloClick(13, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SOLO_15)
      {
        faderManager->HandleSoloClick(14, evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SOLO_16)
      {
        faderManager->HandleSoloClick(15, evt->midi_message[2]);
      }

      /**
       * Fader Buttons
       */
      else if (evt->midi_message[1] == BTN_TRACK)
      {
        faderManager->HandleTrackButtonClick(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_EDIT_PLUGINS)
      {
        faderManager->HandlePluginsButtonClick(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SEND)
      {
        faderManager->HandleSendButtonClick(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_PAN)
      {
        faderManager->HandlePanButtonClick(evt->midi_message[2]);
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
       * Mix Management
       */
      else if (evt->midi_message[1] == BTN_AUDIO)
      {
        mixManager->HandleMixAudioButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_VI)
      {
        mixManager->HandleMixViButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_BUS)
      {
        mixManager->HandleMixBusButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_VCA)
      {
        mixManager->HandleMixVcaButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_ALL)
      {
        mixManager->HandleMixAllButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SHIFT_RIGHT)
      {
        mixManager->HandleShiftButton(evt->midi_message[2]);
      }

      /**
       * General Control Management
       */
      else if (evt->midi_message[1] == ENCODER_CLICK_PAN)
      {
        generalControlManager->HandleEncoderClick(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_ARM)
      {
        generalControlManager->HandleArmButton(evt->midi_message[2]);
        sessionManager->Refresh();
      }
      else if (evt->midi_message[1] == BTN_SOLO_CLEAR)
      {
        generalControlManager->HandleSoloClearButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_MUTE_CLEAR)
      {
        generalControlManager->HandleMuteClearButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_BYPASS)
      {
        generalControlManager->HandleBypassButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_MACRO)
      {
        generalControlManager->HandleMacroButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_LINK)
      {
        generalControlManager->HandleLinkButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SHIFT_LEFT)
      {
        generalControlManager->HandleShiftButton(evt->midi_message[2]);
        sessionManager->Refresh();
      }

      /**
       * Automation Management
       */
      else if (evt->midi_message[1] == BTN_LATCH)
      {
        automationManager->HandleLatchButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_TRIM)
      {
        automationManager->HandleTrimButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_OFF)
      {
        automationManager->HandleOffButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_TOUCH)
      {
        automationManager->HandleTouchButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_WRITE)
      {
        automationManager->HandleWriteButton(evt->midi_message[2]);
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
      else if (evt->midi_message[1] == BTN_CHANNEL)
      {
        sessionManager->HandleChannelButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_ZOOM)
      {
        sessionManager->HandleZoomButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_SCROLL)
      {
        sessionManager->HandleScrollButton(evt->midi_message[2]);
      }
      else if (evt->midi_message[1] == BTN_BANK)
      {
        sessionManager->HandleBankButton(evt->midi_message[2]);
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
      else if (evt->midi_message[1] == ENCODER_CLICK_NAV)
      {
        sessionManager->HandleEncoderClick(evt->midi_message[2]);
      }
    }
  }

public:
  CSurf_FaderPort(int indev, int outdev, int *errStats)
  {
    (void)indev;
    (void)outdev;

    /**
     * First we check if we have the ini file. If not we create it with default values
     *
     */
    mINI::INIStructure ini;
    readAndCreateIni(ini, FP_8);

    errStats = 0;
    m_midi_in_dev = stoi(ini["surface"]["midiin"]);
    m_midi_out_dev = stoi(ini["surface"]["midiout"]);

    surface_update_lastrun = 0;

    // create midi hardware access
    m_midiin = m_midi_in_dev >= 0 ? CreateMIDIInput(m_midi_in_dev) : NULL;
    m_midiout = m_midi_out_dev >= 0 ? CreateMIDIOutput(m_midi_out_dev, false, NULL) : NULL;

    context = new CSurf_Context(stoi(ini["surface"]["surface"]));
    context->SetPluginControl(ini["surface"].has("disable-plugins") && ini["surface"]["disable-plugins"] != "1");
    context->SetUntouchAfterLearn(ini["surface"].has("erase-last-param-after-learn") && ini["surface"]["erase-last-param-after-learn"] == "1");
    context->SetMasterFaderModeEnabled(ini["surface"].has("master-fader-mode") && ini["surface"]["master-fader-mode"] == "1");
    context->SetSwapShiftButtons(ini["surface"].has("swap-shift-buttons") && ini["surface"]["swap-shift-buttons"] == "1");
    context->SetMuteSoloMomentary(ini["surface"].has("mute-solo-momentary") && ini["surface"]["mute-solo-momentary"] == "1");
    context->SetOverwriteTimeCode(ini["surface"].has("overwrite-time-code") && ini["surface"]["overwrite-time-code"] == "1");
    context->SetSurfaceTimeCode(ini["surface"].has("time-code") && std::stoi(ini["surface"]["time-code"]));

    for (int i = 0; i < context->GetNbChannels(); i++)
    {
      CSurf_FP_8_Track *track = new CSurf_FP_8_Track(i, context, m_midiout);
      tracks.push_back(track);
    }
    lastTouchedFxTrack = new CSurf_FP_8_Track(context->GetNbChannels() - 1, context, m_midiout);

    trackNavigator = new CSurf_FP_8_Navigator(context);
    sessionManager = new CSurf_FP_8_SessionManager(context, trackNavigator, m_midiout);
    faderManager = new CSurf_FP_8_FaderManager(context, trackNavigator, m_midiout);
    mixManager = new CSurf_FP_8_MixManager(context, trackNavigator, faderManager, m_midiout);
    transportManager = new CSurf_TransportManager(context, m_midiout);
    automationManager = new CSurf_FP_8_AutomationManager(context, faderManager, m_midiout);
    generalControlManager = new CSurf_FP_8_GeneralControlManager(context, trackNavigator, faderManager, m_midiout);
    lastTouchedFxManager = new CSurf_FP_8_LastTouchedFXManager(lastTouchedFxTrack, context, m_midiout);

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

  ~CSurf_FaderPort()
  {
    if (m_midiout)
    {
      // Reset all displays, faders and track buttons after closing Reaper
      for (int i = 0; i < (int)tracks.size(); i++)
      {
        tracks.at(i)->ClearTrack(true, true);
      }
      tracks.at(0)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at(1)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at(2)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at(3)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at((int)tracks.size() - 2)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at((int)tracks.size() - 1)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at(0)->SetDisplayLine(0, ALIGN_CENTER, "Good");
      tracks.at(1)->SetDisplayLine(0, ALIGN_CENTER, "bye.");
      tracks.at(0)->SetDisplayLine(1, ALIGN_CENTER, "Have");
      tracks.at(1)->SetDisplayLine(1, ALIGN_CENTER, "a");
      tracks.at(2)->SetDisplayLine(1, ALIGN_CENTER, "nice");
      tracks.at(3)->SetDisplayLine(1, ALIGN_CENTER, "day");
      tracks.at((int)tracks.size() - 2)->SetDisplayLine(0, ALIGN_RIGHT, "ReaS", INVERT);
      tracks.at((int)tracks.size() - 1)->SetDisplayLine(0, ALIGN_LEFT, "onus", INVERT);
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
    return "REASONUS_FADERPORT_8";
  }

  const char *GetDescString()
  {
    snprintf(configtmp, 100, "ReaSonus FaderPort 8 & 16 (dev %d, %d)", m_midi_in_dev, m_midi_out_dev);
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
        faderManager->UpdateTracks();
        if (context->GetLastTouchedFxMode())
        {
          lastTouchedFxManager->UpdateTrack();
        }
        else
        {
          lastTouchedFxManager->resetLastTouchedFxEnabled();
        }
        transportManager->Update();
        automationManager->Update();
        generalControlManager->Update();

        surface_update_lastrun = now;
      }
      if ((now - surface_update_keepalive) >= 1200)
      {
        faderManager->Refresh(true);
        sessionManager->Refresh(true);
        mixManager->Refresh(true);
        transportManager->Refresh(true);
        automationManager->Refresh(true);
        generalControlManager->Refresh(true);
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
    /**
     * Skip the master track selection
     */
    if (trackId > -1)
    {
      trackNavigator->SetOffset(trackId - 1);
    }
  }
};

static IReaperControlSurface *createFunc(const char *type_string, const char *configString, int *errStats)
{
  (void)type_string;
  int parms[4];
  parseParms(configString, parms);

  return new CSurf_FaderPort(parms[2], parms[3], errStats);
}

reaper_csurf_reg_t csurf_faderport_8_reg = {
    "REASONUS_FADERPORT_8",
    "ReaSonus FaderPort 8 & 16",
    createFunc,
    CSURF_FP_8_UI_INIT::CreateInitDialog,
};
