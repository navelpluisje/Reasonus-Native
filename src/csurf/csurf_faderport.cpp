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
#include "csurf_transport_manager.cpp"
#include "csurf_mix_manager.cpp"
#include "csurf_channel_context.cpp"
#include "csurf_navigator.cpp"
#include "csurf_fader_resources.hpp"
#include "csurf_button.hpp"
#include <WDL/ptrlist.h>
#include "../src/resource.h"
#include <vector>

class CSurf_FaderPort : public IReaperControlSurface
{
  int m_midi_in_dev, m_midi_out_dev;
  midi_Output *m_midiout;
  midi_Input *m_midiin;

  CSurf_Context *context;
  CSurf_SessionManager *sessionManager;
  CSurf_ShiftManager *shiftManager;
  CSurf_ChannelContext *channelContext;
  CSurf_Navigator *trackNavigator;
  CSurf_MixManager *mixManager;
  CSurf_TransportManager *transportManager;

  std::vector<CSurf_Track *> tracks;

  DWORD m_frameupd_lastrun;
  DWORD m_buttonstate_lastrun;
  DWORD m_pan_lasttouch;

  WDL_String descspace;
  char configtmp[1024];

  void OnMIDIEvent(MIDI_event_t *evt)
  {

    /**
     * Fader values
     */
    if (evt->midi_message[0] >= FADER_1 && evt->midi_message[0] <= FADER_8)
    {
      channelContext->HandleFaderMove(evt->midi_message[0] - FADER_1, evt->midi_message[2], evt->midi_message[1]);
    }

    /**
     * ENCODERS & VALUEBAR
     */
    else if (evt->midi_message[0] == MIDI_MESSAGE_ENDCODER)
    {
      if (evt->midi_message[1] == ENCODER_PAN)
      {
        if (context->GetPanEncoderMode() == PanEncoderPanMode)
        {
          // Handle the pan pan mode
        }
        if (context->GetPanEncoderMode() == PanEncoderNavigateMode)
        {
          trackNavigator->HandlePanEncoderChange(evt->midi_message[2]);
        }
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
                                       evt->midi_message[1] == BTN_NEXT || evt->midi_message[1] == BTN_PREV || evt->midi_message[1] == BTN_SHIFT_LEFT || evt->midi_message[1] == BTN_SHIFT_RIGHT || evt->midi_message[1] == BTN_ARM))
      {
        return;
      }

      /**
       * Track Select Buttons
       */
      else if (evt->midi_message[1] >= BTN_SELECT_1 && evt->midi_message[1] <= BTN_SELECT_8)
      {
        channelContext->HandleSelectClick(evt->midi_message[1] - BTN_SELECT_1);
      }
      /**
       * Track Mute Buttons
       */
      else if (evt->midi_message[1] >= BTN_MUTE_1 && evt->midi_message[1] <= BTN_MUTE_8)
      {
        channelContext->HandleMuteClick(evt->midi_message[1] - BTN_MUTE_1);
      }
      /**
       * Track Solo Buttons
       */
      else if (evt->midi_message[1] >= BTN_SOLO_1 && evt->midi_message[1] <= BTN_SOLO_8)
      {
        channelContext->HandleSoloClick(evt->midi_message[1] - BTN_SOLO_1);
      }

      /**
       * Fader Buttons
       */
      else if (evt->midi_message[1] == BTN_TRACK)
      {
        channelContext->handleTrackButtonClick();
      }
      else if (evt->midi_message[1] == BTN_EDIT_PLUGINS)
      {
        channelContext->handlePluginsButtonClick();
      }
      else if (evt->midi_message[1] == BTN_SEND)
      {
        channelContext->handleSendButtonClick();
      }
      else if (evt->midi_message[1] == BTN_PAN)
      {
        channelContext->handlePanButtonClick();
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
       * Arm Button
       */
      else if (evt->midi_message[1] == BTN_ARM)
      {
        shiftManager->HandleArmButton(evt->midi_message[2]);
        sessionManager->Refresh();
      }

      /**
       * Shift Manager Buttons
       */
      else if (evt->midi_message[1] == BTN_SHIFT_LEFT)
      {
        shiftManager->HandleShiftLeftButton(evt->midi_message[2]);
        sessionManager->Refresh();
      }
      else if (evt->midi_message[1] == BTN_SHIFT_RIGHT)
      {
        shiftManager->HandleShiftRightButton(evt->midi_message[2]);
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
    }
  }

public:
  CSurf_FaderPort(int indev, int outdev, int *errStats)
  {
    (void)indev;
    (void)outdev;
    errStats = 0;
    m_midi_in_dev = 6;   // indev;
    m_midi_out_dev = 11; // outdev;

    m_frameupd_lastrun = 0;
    m_buttonstate_lastrun = 0;
    m_pan_lasttouch = 0;

    // create midi hardware access
    m_midiin = m_midi_in_dev >= 0 ? CreateMIDIInput(m_midi_in_dev) : NULL;
    m_midiout = m_midi_out_dev >= 0 ? CreateMIDIOutput(m_midi_out_dev, false, NULL) : NULL;

    context = new CSurf_Context();

    for (int i = 0; i < 8; i++)
    {
      CSurf_Track *track = new CSurf_Track(i, context, m_midiout);
      tracks.push_back(track);
    }

    trackNavigator = new CSurf_Navigator();
    sessionManager = new CSurf_SessionManager(context, trackNavigator, m_midiout);
    shiftManager = new CSurf_ShiftManager(context, m_midiout);
    channelContext = new CSurf_ChannelContext(context, trackNavigator, m_midiout);
    mixManager = new CSurf_MixManager(context, trackNavigator, m_midiout);
    transportManager = new CSurf_TransportManager(context, m_midiout);

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

  // void CloseNoReset()
  // {
  //   DELETE_ASYNC(m_midiout);
  //   DELETE_ASYNC(m_midiin);
  //   m_midiout = 0;
  //   m_midiin = 0;
  // }

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
      if ((now - m_buttonstate_lastrun) >= 10)
      {
        channelContext->UpdateTracks();
        transportManager->Update();
        m_buttonstate_lastrun = now;
      }
    }
  }

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

static void AddComboEntry(HWND hwndDlg, int x, char *buf, int comboId)
{
  int a = (int)SendDlgItemMessage(hwndDlg, comboId, CB_ADDSTRING, 0, (LPARAM)buf);
  SendDlgItemMessage(hwndDlg, comboId, CB_SETITEMDATA, a, x);
}

static WDL_DLGRET dlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  (void)wParam;
  (void)lParam;
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
    char buf[520];
    int currentIndex = 0;

    WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MidiIn));
    WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MidiOut));

    for (int i = 0; i < GetNumMIDIInputs(); ++i)
      if (GetMIDIInputName(i, buf, sizeof(buf)))
      {
        AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MidiIn);
        SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_MidiIn), CB_SETCURSEL, currentIndex, 0);
        currentIndex++;
      }

    currentIndex = 0;

    for (int i = 0; i < GetNumMIDIOutputs(); ++i)
      if (GetMIDIOutputName(i, buf, sizeof(buf)))
      {
        AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MidiOut);
        SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_MidiOut), CB_SETCURSEL, currentIndex, 0);
        currentIndex++;
      }
  }
  break;
    // case WM_USER + 1024:
    //   if (wParam > 1 && lParam)
    //   {
    //     char tmp[512];

    //     int indev = -1, outdev = -1;
    //     int r = SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
    //     if (r != CB_ERR)
    //       indev = SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_GETITEMDATA, r, 0);
    //     r = SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_GETCURSEL, 0, 0);
    //     if (r != CB_ERR)
    //       outdev = SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_GETITEMDATA, r, 0);

    //     snprintf(tmp, 100, "0 0 %d %d", indev, outdev);
    //     lstrcpyn((char *)lParam, tmp, wParam);
    //   }
    //   break;
  }
  return 0;
}

static HWND configFunc(const char *type_string, HWND parent, const char *initConfigString)
{
  (void)type_string;
  return CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE), parent, dlgProc, (LPARAM)initConfigString);
}

reaper_csurf_reg_t csurf_faderport_reg =
    {
        "REASONUS_FADERPORT",
        "ReaSonus FaderPort",
        createFunc,
        configFunc,
};
