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
#include "csurf_transport_manager.cpp"
#include "csurf_mix_manager.cpp"
#include "csurf_automation_manager.cpp"
#include "csurf_general_control_manager.cpp"
#include "csurf_last_touched_fx_manager.hpp"
#include "csurf_channel_context_manager.hpp"
#include "csurf_navigator.hpp"
#include "controls/csurf_button.hpp"
#include "csurf_utils.hpp"
#include "csurf_faderport_ui_init.hpp"
#include <WDL/ptrlist.h>
#include "../src/resource.h"
#include <vector>
#include <string>
#include <swell/swell-functions.h>
#include <localize/localize.h>

extern HWND g_hwnd;

using namespace CSURF_FADERPORT_UI_INIT;

class CSurf_FaderPort : public IReaperControlSurface
{
  int m_midi_in_dev, m_midi_out_dev;
  midi_Output *m_midiout;
  midi_Input *m_midiin;

  CSurf_Context *context;
  CSurf_SessionManager *sessionManager;
  CSurf_ChannelContextManager *channelContextManager;
  CSurf_Navigator *trackNavigator;
  CSurf_MixManager *mixManager;
  CSurf_TransportManager *transportManager;
  CSurf_AutomationManager *automationManager;
  CSurf_GeneralControlManager *generalControlManager;
  CSurf_LastTouchedFXManager *lastTouchedFxManager;

  std::vector<CSurf_Track *> tracks;
  CSurf_Track *lastTouchedFxTrack;

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
      channelContextManager->HandleFaderMove(evt->midi_message[0] - FADER_1, evt->midi_message[2], evt->midi_message[1]);
      if (
          (context->GetNbChannels() == 8 && context->GetLastTouchedFxMode() && evt->midi_message[0] == FADER_8) ||
          (context->GetNbChannels() == 16 && context->GetLastTouchedFxMode() && evt->midi_message[0] == FADER_16))
      {
        lastTouchedFxManager->HandleFaderMove(evt->midi_message[2], evt->midi_message[1]);
      }
    }

    /**
     * ENCODERS & VALUEBAR
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
       * The next and previous button light up when pressed so need the '0' value when releasing
       */
      if (!evt->midi_message[2] && !(
                                       evt->midi_message[1] == BTN_NEXT ||
                                       evt->midi_message[1] == BTN_PREV ||
                                       evt->midi_message[1] == BTN_SHIFT_LEFT ||
                                       evt->midi_message[1] == BTN_SHIFT_RIGHT ||
                                       evt->midi_message[1] == BTN_ARM))
      {
        return;
      }

      /**
       * Fader Touch
       */
      else if (evt->midi_message[1] >= FADER_TOUCH_1 && evt->midi_message[1] <= FADER_TOUCH_16)
      {
        channelContextManager->HandleFaderTouch(evt->midi_message[1] - FADER_TOUCH_1);
      }

      /**
       * Track Select Buttons
       */
      else if (evt->midi_message[1] >= BTN_SELECT_1 && evt->midi_message[1] <= BTN_SELECT_8)
      {
        channelContextManager->HandleSelectClick(evt->midi_message[1] - BTN_SELECT_1);
      }
      else if (evt->midi_message[1] == BTN_SELECT_9)
      {
        channelContextManager->HandleSelectClick(8);
      }
      else if (evt->midi_message[1] >= BTN_SELECT_10 && evt->midi_message[1] <= BTN_SELECT_16)
      {
        channelContextManager->HandleSelectClick(evt->midi_message[1] - 0x20 + 8);
      }
      /**
       * Track Mute Buttons
       */
      else if (evt->midi_message[1] >= BTN_MUTE_1 && evt->midi_message[1] <= BTN_MUTE_8)
      {
        channelContextManager->HandleMuteClick(evt->midi_message[1] - BTN_MUTE_1);
      }
      else if (evt->midi_message[1] >= BTN_MUTE_9 && evt->midi_message[1] <= BTN_MUTE_16)
      {
        channelContextManager->HandleMuteClick(evt->midi_message[1] - BTN_MUTE_9 + 8);
      }
      /**
       * Track Solo Buttons
       */
      else if (evt->midi_message[1] >= BTN_SOLO_1 && evt->midi_message[1] <= BTN_SOLO_8)
      {
        channelContextManager->HandleSoloClick(evt->midi_message[1] - BTN_SOLO_1);
      }
      else if (evt->midi_message[1] >= BTN_SOLO_9 && evt->midi_message[1] <= BTN_SOLO_14)
      {
        channelContextManager->HandleSoloClick(evt->midi_message[1] - BTN_SOLO_9 + 8);
      }
      else if (evt->midi_message[1] == BTN_SOLO_14 || evt->midi_message[1] == BTN_SOLO_15)
      {
        channelContextManager->HandleSoloClick(evt->midi_message[1] == BTN_SOLO_14 ? 14 : 15);
      }

      /**
       * Fader Buttons
       */
      else if (evt->midi_message[1] == BTN_TRACK)
      {
        channelContextManager->HandleTrackButtonClick();
      }
      else if (evt->midi_message[1] == BTN_EDIT_PLUGINS)
      {
        channelContextManager->HandlePluginsButtonClick();
      }
      else if (evt->midi_message[1] == BTN_SEND)
      {
        channelContextManager->HandleSendButtonClick();
      }
      else if (evt->midi_message[1] == BTN_PAN)
      {
        channelContextManager->HandlePanButtonClick();
      }

      /**
       * Transport Buttons
       */
      else if (evt->midi_message[1] == BTN_PLAY)
      {
        transportManager->HandlePlayButton();
      }
      else if (evt->midi_message[1] == BTN_STOP)
      {
        transportManager->HandleStopButton();
      }
      else if (evt->midi_message[1] == BTN_RECORD)
      {
        transportManager->HandleRecordButton();
      }
      else if (evt->midi_message[1] == BTN_REWIND)
      {
        transportManager->HandleRewindButton();
      }
      else if (evt->midi_message[1] == BTN_FORWARD)
      {
        transportManager->HandleForwardButton();
      }
      else if (evt->midi_message[1] == BTN_LOOP)
      {
        transportManager->HandleRepeatButton();
      }

      /**
       * Mix Management
       */
      else if (evt->midi_message[1] == BTN_AUDIO)
      {
        mixManager->HandleMixAudioButton();
      }
      else if (evt->midi_message[1] == BTN_VI)
      {
        mixManager->HandleMixViButton();
      }
      else if (evt->midi_message[1] == BTN_BUS)
      {
        mixManager->HandleMixBusButton();
      }
      else if (evt->midi_message[1] == BTN_VCA)
      {
        mixManager->HandleMixVcaButton();
      }
      else if (evt->midi_message[1] == BTN_ALL)
      {
        mixManager->HandleMixAllButton();
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
        generalControlManager->HandleEncoderClick();
      }
      else if (evt->midi_message[1] == BTN_ARM)
      {
        generalControlManager->HandleArmButton(evt->midi_message[2]);
        sessionManager->Refresh();
      }
      else if (evt->midi_message[1] == BTN_SOLO_CLEAR)
      {
        generalControlManager->HandleSoloClearButton();
      }
      else if (evt->midi_message[1] == BTN_MUTE_CLEAR)
      {
        generalControlManager->HandleMuteClearButton();
      }
      else if (evt->midi_message[1] == BTN_BYPASS)
      {
        generalControlManager->HandleBypassButton();
      }
      else if (evt->midi_message[1] == BTN_MACRO)
      {
        generalControlManager->HandleMacroButton();
      }
      else if (evt->midi_message[1] == BTN_LINK)
      {
        generalControlManager->HandleLinkButton();
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
        automationManager->HandleLatchButton();
      }
      else if (evt->midi_message[1] == BTN_TRIM)
      {
        automationManager->HandleTrimButton();
      }
      else if (evt->midi_message[1] == BTN_OFF)
      {
        automationManager->HandleOffButton();
      }
      else if (evt->midi_message[1] == BTN_TOUCH)
      {
        automationManager->HandleTouchButton();
      }
      else if (evt->midi_message[1] == BTN_WRITE)
      {
        automationManager->HandleWriteButton();
      }
      else if (evt->midi_message[1] == BTN_WRITE)
      {
        automationManager->HandleWriteButton();
      }
      else if (evt->midi_message[1] == BTN_READ)
      {
        automationManager->HandleReadButton();
      }

      /**
       * Session Manager Buttons
       */
      else if (evt->midi_message[1] == BTN_CHANNEL)
      {
        sessionManager->HandleChannelButton();
      }
      else if (evt->midi_message[1] == BTN_ZOOM)
      {
        sessionManager->HandleZoomButton();
      }
      else if (evt->midi_message[1] == BTN_SCROLL)
      {
        sessionManager->HandleScrollButton();
      }
      else if (evt->midi_message[1] == BTN_BANK)
      {
        sessionManager->HandleBankButton();
      }
      else if (evt->midi_message[1] == BTN_MASTER)
      {
        sessionManager->HandleMasterButton();
      }
      else if (evt->midi_message[1] == BTN_CLICK)
      {
        sessionManager->HandleClickButton();
      }
      else if (evt->midi_message[1] == BTN_SECTION)
      {
        sessionManager->HandleSectionButton();
      }
      else if (evt->midi_message[1] == BTN_MARKER)
      {
        sessionManager->HandleMarkerButton();
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
        sessionManager->HandleEncoderClick();
      }
    }
  }

public:
  CSurf_FaderPort(int indev, int outdev, int *errStats)
  {
    (void)indev;
    (void)outdev;

    /**
     * Fisrst e check if we have the ini file. If not we create it with default values
     *
     */
    mINI::INIFile file(GetReaSonusIniPath());
    mINI::INIStructure ini;
    if (!file.read(ini))
    {
      ini["Surface"]["MidiIn"] = "0";
      ini["Surface"]["MidiOut"] = "0";
      ini["Surface"]["Surface"] = "0";
      ini["Functions"]["1"] = "0";
      ini["Functions"]["2"] = "0";
      ini["Functions"]["3"] = "0";
      ini["Functions"]["4"] = "0";
      ini["Functions"]["5"] = "0";
      ini["Functions"]["6"] = "0";
      ini["Functions"]["7"] = "0";
      ini["Functions"]["8"] = "0";
      file.generate(ini);
    };

    errStats = 0;
    m_midi_in_dev = stoi(ini["Surface"]["MidiIn"]);
    m_midi_out_dev = stoi(ini["Surface"]["MidiOut"]);

    surface_update_lastrun = 0;

    // create midi hardware access
    m_midiin = m_midi_in_dev >= 0 ? CreateMIDIInput(m_midi_in_dev) : NULL;
    m_midiout = m_midi_out_dev >= 0 ? CreateMIDIOutput(m_midi_out_dev, false, NULL) : NULL;

    context = new CSurf_Context(stoi(ini["Surface"]["Surface"]));
    context->SetPluginControl(ini["Surface"].has("disable-plugins") && ini["Surface"]["disable-plugins"] != "1");

    for (int i = 0; i < context->GetNbChannels(); i++)
    {
      CSurf_Track *track = new CSurf_Track(i, context, m_midiout);
      tracks.push_back(track);
    }
    lastTouchedFxTrack = new CSurf_Track(context->GetNbChannels() - 1, context, m_midiout);

    trackNavigator = new CSurf_Navigator(context);
    sessionManager = new CSurf_SessionManager(context, trackNavigator, m_midiout);
    channelContextManager = new CSurf_ChannelContextManager(context, trackNavigator, m_midiout);
    mixManager = new CSurf_MixManager(context, trackNavigator, channelContextManager, m_midiout);
    transportManager = new CSurf_TransportManager(context, m_midiout);
    automationManager = new CSurf_AutomationManager(context, m_midiout);
    generalControlManager = new CSurf_GeneralControlManager(context, trackNavigator, channelContextManager, m_midiout);
    lastTouchedFxManager = new CSurf_LastTouchedFXManager(lastTouchedFxTrack, context, m_midiout);

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
    return "REASONUS_FADERPORT";
  }

  const char *GetDescString()
  {
    descspace.SetFormatted(512, __LOCALIZE_VERFMT("ReaSonus FaderPort (dev %d, %d)", "reasonus-faderport"), m_midi_in_dev, m_midi_out_dev);
    return descspace.Get();
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
      if ((now - surface_update_lastrun) >= 50)
      {
        channelContextManager->UpdateTracks();
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
    int trackId = GetMediaTrackInfo_Value(media_track, "IP_TRACKNUMBER");
    trackNavigator->SetOffset(trackId - 1);
  }
};

static IReaperControlSurface *createFunc(const char *type_string, const char *configString, int *errStats)
{
  (void)type_string;
  int parms[4];
  parseParms(configString, parms);

  return new CSurf_FaderPort(parms[2], parms[3], errStats);
}

reaper_csurf_reg_t csurf_faderport_reg = {
    "REASONUS_FADERPORT",
    "ReaSonus FaderPort",
    createFunc,
    CreateInitDialog,
};
