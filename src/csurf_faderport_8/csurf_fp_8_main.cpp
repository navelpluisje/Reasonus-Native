#include <string>
#include <vector>
#include <array>
#include <reaper_plugin.h>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <WDL/ptrlist.h>
#include <reaper_plugin_functions.h>
#include "../controls/csurf_button.hpp"
#include "../shared/csurf.h"
#include "../shared/csurf_transport_manager.hpp"
#include "../shared/csurf_utils.hpp"
#include "../i18n/i18n.hpp"
#include "../ui/windows/csurf_ui_message.hpp"
#include "../shared/csurf_reasonus_settings.hpp"
#include "csurf_fp_8_session_manager.cpp"
#include "csurf_fp_8_mix_manager.cpp"
#include "csurf_fp_8_automation_manager.cpp"
#include "csurf_fp_8_general_control_manager.cpp"
#include "csurf_fp_8_last_touched_fx_manager.hpp"
#include "csurf_fp_8_fader_manager.hpp"
#include "csurf_fp_8_ui_init.hpp"
#include "csurf_fp_8_navigator.hpp"

I18n *I18n::instancePtr = nullptr;
ReaSonusSettings *ReaSonusSettings::instance8Ptr = nullptr;

const int MOMENTARY_TIMEOUT = 500;

class CSurf_FaderPort : public IReaperControlSurface {
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
  DWORD surface_update_settings_check;

  I18n *i18n = I18n::GetInstance();
  ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_8);

  char configtmp[1024]; // NOLINT(*-avoid-c-arrays)

  void OnMIDIEvent(const MIDI_event_t *evt) const { // NOLINT(*-function-cognitive-complexity)

    /**
     * Fader values
     */
    if (evt->midi_message[0] >= FADER_1 && evt->midi_message[0] <= FADER_16) {
      if (
        (context->GetNbChannels() == 7 && context->GetLastTouchedFxMode() && evt->midi_message[0] == FADER_8) ||
        (context->GetNbChannels() == 15 && context->GetLastTouchedFxMode() && evt->midi_message[0] == FADER_16)) {
        lastTouchedFxManager->HandleFaderMove(evt->midi_message[2], evt->midi_message[1]);
      } else {
        faderManager->HandleFaderMove(evt->midi_message[0] - FADER_1, evt->midi_message[2], evt->midi_message[1]);
      }
    }

    /**
     * ENCODERS
     */
    else if (evt->midi_message[0] == MIDI_MESSAGE_ENCODER) {
      if (evt->midi_message[1] == ENCODER_PAN) {
        generalControlManager->HandleEncoderChange(evt->midi_message[2]);
      }
      if (evt->midi_message[1] == ENCODER_NAV) {
        sessionManager->HandleSessionNavEncoderChange(evt->midi_message[2]);
      }
    }

    /**
     * BUTTONS
     */
    else if (evt->midi_message[0] == MIDI_MESSAGE_BUTTON) {
      /**
       * Fader Touch
       */
      if (evt->midi_message[1] >= FADER_TOUCH_1 && evt->midi_message[1] <= FADER_TOUCH_16) {
        faderManager->HandleFaderTouch(evt->midi_message[1] - FADER_TOUCH_1, evt->midi_message[2]);
        trackNavigator->SetTrackTouched(evt->midi_message[1] - FADER_TOUCH_1, evt->midi_message[2] > 0);
      }

      /**
       * Track Select Buttons
       */
      else if (evt->midi_message[1] >= BTN_SELECT_1 && evt->midi_message[1] <= BTN_SELECT_8) {
        faderManager->HandleSelectClick(evt->midi_message[1] - BTN_SELECT_1, evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SELECT_9) {
        faderManager->HandleSelectClick(8, evt->midi_message[2]);
      } else if (evt->midi_message[1] >= BTN_SELECT_10 && evt->midi_message[1] <= BTN_SELECT_16) {
        faderManager->HandleSelectClick(evt->midi_message[1] - 0x20 + 8, evt->midi_message[2]);
      }
      /**
       * Track Mute Buttons
       */
      else if (evt->midi_message[1] >= BTN_MUTE_1 && evt->midi_message[1] <= BTN_MUTE_8) {
        faderManager->HandleMuteClick(evt->midi_message[1] - BTN_MUTE_1, evt->midi_message[2]);
      } else if (evt->midi_message[1] >= BTN_MUTE_9 && evt->midi_message[1] <= BTN_MUTE_16) {
        faderManager->HandleMuteClick(evt->midi_message[1] - BTN_MUTE_9 + 8, evt->midi_message[2]);
      }
      /**
       * Track Solo Buttons
       */
      else if (evt->midi_message[1] >= BTN_SOLO_1 && evt->midi_message[1] <= BTN_SOLO_8) {
        faderManager->HandleSoloClick(evt->midi_message[1] - BTN_SOLO_1, evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SOLO_9) {
        faderManager->HandleSoloClick(8, evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SOLO_10) {
        faderManager->HandleSoloClick(9, evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SOLO_11) {
        faderManager->HandleSoloClick(10, evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SOLO_12) {
        faderManager->HandleSoloClick(11, evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SOLO_13) {
        faderManager->HandleSoloClick(12, evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SOLO_14) {
        faderManager->HandleSoloClick(13, evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SOLO_15) {
        faderManager->HandleSoloClick(14, evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SOLO_16) {
        faderManager->HandleSoloClick(15, evt->midi_message[2]);
      }

      /**
       * Fader Buttons
       */
      else if (evt->midi_message[1] == BTN_TRACK) {
        faderManager->HandleTrackButtonClick(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_EDIT_PLUGINS) {
        faderManager->HandlePluginsButtonClick(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SEND) {
        faderManager->HandleSendButtonClick(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_PAN) {
        faderManager->HandlePanButtonClick(evt->midi_message[2]);
      }

      /**
       * Transport Buttons
       */
      else if (evt->midi_message[1] == BTN_PLAY) {
        transportManager->HandlePlayButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_STOP) {
        transportManager->HandleStopButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_RECORD) {
        transportManager->HandleRecordButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_REWIND) {
        transportManager->HandleRewindButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_FORWARD) {
        transportManager->HandleForwardButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_LOOP) {
        transportManager->HandleRepeatButton(evt->midi_message[2]);
      }

      /**
       * Footswitch
       */
      else if (evt->midi_message[1] == BTN_FOOTSWITCH) {
        transportManager->HandleFootSwitchClick(evt->midi_message[2]);
      }

      /**
       * Mix Management
       */
      else if (evt->midi_message[1] == BTN_AUDIO) {
        mixManager->HandleMixAudioButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_VI) {
        mixManager->HandleMixViButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_BUS) {
        mixManager->HandleMixBusButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_VCA) {
        mixManager->HandleMixVcaButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_ALL) {
        mixManager->HandleMixAllButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SHIFT_RIGHT) {
        mixManager->HandleShiftButton(evt->midi_message[2]);
      }

      /**
       * General Control Management
       */
      else if (evt->midi_message[1] == ENCODER_CLICK_PAN) {
        generalControlManager->HandleEncoderClick(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_ARM) {
        generalControlManager->HandleArmButton(evt->midi_message[2]);
        sessionManager->Refresh();
      } else if (evt->midi_message[1] == BTN_SOLO_CLEAR) {
        generalControlManager->HandleSoloClearButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_MUTE_CLEAR) {
        generalControlManager->HandleMuteClearButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_BYPASS) {
        generalControlManager->HandleBypassButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_MACRO) {
        generalControlManager->HandleMacroButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_LINK) {
        generalControlManager->HandleLinkButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SHIFT_LEFT) {
        generalControlManager->HandleShiftButton(evt->midi_message[2]);
        sessionManager->Refresh();
      }

      /**
       * Automation Management
       */
      else if (evt->midi_message[1] == BTN_LATCH) {
        automationManager->HandleLatchButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_TRIM) {
        automationManager->HandleTrimButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_OFF) {
        automationManager->HandleOffButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_TOUCH) {
        automationManager->HandleTouchButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_WRITE) {
        automationManager->HandleWriteButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_READ) {
        automationManager->HandleReadButton(evt->midi_message[2]);
      }

      /**
       * Session Manager Buttons
       */
      else if (evt->midi_message[1] == BTN_CHANNEL) {
        sessionManager->HandleChannelButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_ZOOM) {
        sessionManager->HandleZoomButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SCROLL) {
        sessionManager->HandleScrollButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_BANK) {
        sessionManager->HandleBankButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_MASTER) {
        sessionManager->HandleMasterButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_CLICK) {
        sessionManager->HandleClickButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_SECTION) {
        sessionManager->HandleSectionButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_MARKER) {
        sessionManager->HandleMarkerButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_NEXT) {
        sessionManager->HandleNextButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == BTN_PREV) {
        sessionManager->HandlePrevButton(evt->midi_message[2]);
      } else if (evt->midi_message[1] == ENCODER_CLICK_NAV) {
        sessionManager->HandleEncoderClick(evt->midi_message[2]);
      }
    }
  }

  void updateSettings() const {
    i18n->SetLanguage(DAW::GetExtState(EXT_STATE_KEY_UI_LANGUAGE, "en-US"));
    settings->UpdateSettings();
  }

public:
  CSurf_FaderPort(const int in_dev, const int out_dev, int *errStats) // NOLINT(*-pro-type-member-init)
  {
    (void) in_dev;
    (void) out_dev;

    if (std::string(GIT_VERSION) != DAW::GetExtState(EXT_STATE_KEY_VERSION, "")) {
      ReaSonusMessage::Start();
      DAW::SetExtState(EXT_STATE_KEY_VERSION, GIT_VERSION, true);
      I18n::checkLocalesFiles();
    }

    // TODO: Need to get rid of the nullptr here as ot will never display an error this way when needed
    // errStats = nullptr;
    m_midi_in_dev = settings->GetMidiInput();
    m_midi_out_dev = settings->GetMidiOutput();

    surface_update_lastrun = 0;

    // create midi hardware access
    m_midiin = m_midi_in_dev >= 0 ? CreateMIDIInput(m_midi_in_dev) : nullptr;
    m_midiout = m_midi_out_dev >= 0 ? CreateMIDIOutput(m_midi_out_dev, false, nullptr) : nullptr;

    context = new CSurf_Context(settings->GetSurface());
    updateSettings();

    for (int i = 0; i < context->GetNbChannels(); i++) {
      auto *track = new CSurf_FP_8_Track(i, context, m_midiout);
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

    if (errStats != nullptr && *errStats > 0) {
      ShowConsoleMsg("Error: ");
      ShowConsoleMsg(reinterpret_cast<char *>(errStats));
      if (m_midi_in_dev >= 0 && m_midiin == nullptr) {
        *errStats |= 1;
      }
      if (m_midi_out_dev >= 0 && m_midiout == nullptr) {
        *errStats |= 2;
      }
    }

    if (m_midiin != nullptr) {
      m_midiin->start();
    }

    if (m_midiout != nullptr) {
      m_midiout->Send(0xb0, 0x00, 0x06, -1);
      m_midiout->Send(0xb0, 0x20, 0x27, -1);
    }
  }

  ~CSurf_FaderPort() override {
    if (m_midiout != nullptr) {
      // Reset all displays, faders and track buttons after closing Reaper
      for (const auto &track: tracks) {
        track->ClearTrack(true, true);
      }
      tracks.at(0)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at(1)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at(2)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at(3)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at(static_cast<int>(tracks.size()) - 2)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at(static_cast<int>(tracks.size()) - 1)->SetDisplayMode(DISPLAY_MODE_3);
      tracks.at(0)->SetDisplayLine(0, ALIGN_CENTER, "Good");
      tracks.at(1)->SetDisplayLine(0, ALIGN_CENTER, "bye.");
      tracks.at(0)->SetDisplayLine(1, ALIGN_CENTER, "Have");
      tracks.at(1)->SetDisplayLine(1, ALIGN_CENTER, "a");
      tracks.at(2)->SetDisplayLine(1, ALIGN_CENTER, "nice");
      tracks.at(3)->SetDisplayLine(1, ALIGN_CENTER, "day");
      tracks.at(static_cast<int>(tracks.size()) - 2)->SetDisplayLine(0, ALIGN_RIGHT, "ReaS", INVERT);
      tracks.at(static_cast<int>(tracks.size()) - 1)->SetDisplayLine(0, ALIGN_LEFT, "onus", INVERT);
    }

    DELETE_ASYNC(m_midiout);
    DELETE_ASYNC(m_midiin);
  }

  // [[nodiscard]] CSurf_Context GetContext() const {
  //   return *context;
  // }

  const char *GetTypeString() override {
    return "REASONUS_FADERPORT_8";
  }

  const char *GetDescString() override {
    snprintf(configtmp, 100, "ReaSonus FaderPort 8 & 16 (dev %d, %d)", m_midi_in_dev, m_midi_out_dev);
    return configtmp;
  }

  const char *GetConfigString() override {
    snprintf(configtmp, 100, "0 0 %d %d", m_midi_in_dev, m_midi_out_dev);

    return configtmp;
  }

  void CloseNoReset() override {
    DELETE_ASYNC(m_midiout);
    DELETE_ASYNC(m_midiin);
    m_midiout = nullptr;
    m_midiin = nullptr;
  }

  bool GetTouchState(MediaTrack *media_track, const int is_pan) override {
    return trackNavigator->IsTrackTouched(media_track, is_pan);
  }

  void Run() override {
    if (m_midiin != nullptr) {
      m_midiin->SwapBufsPrecise(GetTickCount(), 0.0);
      int l = 0;
      MIDI_eventlist *list = m_midiin->GetReadBuf();
      MIDI_event_t *midi_events;

      while ((midi_events = list->EnumItems(&l)) != nullptr) {
        OnMIDIEvent(midi_events);
      }
    }

    if (m_midiout != nullptr) {
      DWORD const now = GetTickCount();
      if ((now - surface_update_lastrun) >= 10) {
        const bool force_update = (now - surface_update_lastrun) >= 2000;

        faderManager->UpdateTracks(force_update);
        if (context->GetLastTouchedFxMode()) {
          lastTouchedFxManager->UpdateTrack(force_update);
        } else {
          lastTouchedFxManager->resetLastTouchedFxEnabled();
        }
        transportManager->Update(force_update);
        automationManager->Update(force_update);
        generalControlManager->Update(force_update);

        if (force_update) {
          mixManager->Refresh(force_update);
        }

        surface_update_lastrun = now;
      }

      /**
       * @brief Life cycle tick for the FaderPort
       *
       */
      if ((now - surface_update_keepalive) >= 990) {
        surface_update_keepalive = now;
        m_midiout->Send(0xa0, 0x00, 0x00, -1);
      }

      /**
       * every 1500 ms we check if the settings have been saved.
       * If so, we updet the settings in the context
       *
       */
      if ((now - surface_update_settings_check) >= 1500) {
        surface_update_settings_check = now;
        std::string const is_saved = DAW::GetExtState(EXT_STATE_KEY_SAVED_SETTINGS, "");

        if (is_saved == EXT_STATE_VALUE_TRUE) {
          updateSettings();
          DAW::SetExtState(EXT_STATE_KEY_SAVED_SETTINGS, EXT_STATE_VALUE_FALSE, false);
        }
      }
    }
  }

  void OnTrackSelection(MediaTrack *media_track) override {
    const int track_index = stoi(DAW::GetTrackIndex(media_track));

    // Master track returns -1. Do not a=do anything right now
    if (track_index < 0) {
      return;
    }

    trackNavigator->SetOffsetByTrack(media_track);
  }

  void SetPlayState(const bool play, const bool pause, const bool rec) override {
    (void) play;
    (void) pause;
    (void) rec;
    
    transportManager->HandlePlayStateChange();
  }

  int Extended(const int call, void *param1, void *param2, void *param3) override {
    (void) param1;
    (void) param2;
    (void) param3;

    if (call == CSURF_EXT_SUPPORTS_EXTENDED_TOUCH) {
      return 1;
    }

    return 1;
  }
};

static IReaperControlSurface *createFunc(const char *type_string, const char *configString, int *errStats) {
  (void) type_string;
  constexpr std::array<int, 4> parms{};
  parseParms(configString, parms);

  return new CSurf_FaderPort(parms[2], parms[3], errStats);
}

reaper_csurf_reg_t csurf_faderport_8_reg = {
  "REASONUS_FADERPORT_8",
  "ReaSonus FaderPort 8 & 16",
  createFunc,
  CSURF_FP_8_UI_INIT::CreateInitDialog,
};
