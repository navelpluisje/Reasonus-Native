#include "csurf_transport_manager.hpp"

#include "csurf_daw.hpp"
#include "../controls/csurf_button.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_context.cpp"
#include "../ui/windows/csurf_ui_fp_8_control_panel.hpp"
#include "csurf_faderport_ui_imgui_utils.hpp"
#include "../ui/pages/csurf_ui_fp_v2_control_panel.hpp"

void CSurf_TransportManager::SetButtonValues(const bool force) const {
    playButton->SetValue(ButtonBlinkOnOff(isPaused, isPlaying), force);
    stopButton->SetValue(isStopped ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
    recordButton->SetValue(isRecording ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
    repeatButton->SetValue(isRepeat ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
    rewindButton->SetValue(ButtonBlinkOnOff(isRewinding && isFastFwdRwd, isRewinding), force);
    forwardButton->SetValue(ButtonBlinkOnOff(isForwarding && isFastFwdRwd, isForwarding), force);
};

void CSurf_TransportManager::StopRewindOrForward() {
    isFastFwdRwd = false;
    isForwarding = false;
    isRewinding = false;

    if (context->GetSetting("surface", "mute-master-on-fwd-rwd") != "0") {
        DAW::SetTrackMuted(GetMasterTrack(nullptr), false);
    }
}

void CSurf_TransportManager::HandleRewind() const {
    CSurf_OnRew(0);

    if (isFastFwdRwd) {
        CSurf_OnRew(0);
        CSurf_OnRew(0);
        CSurf_OnRew(0);
    }
}

void CSurf_TransportManager::HandleForward() const {
    CSurf_OnFwd(0);

    if (isFastFwdRwd) {
        CSurf_OnFwd(0);
        CSurf_OnFwd(0);
        CSurf_OnFwd(0);
    }
}

void CSurf_TransportManager::SetPause() const {
    if (isPlaying) {
        CSurf_OnPlay();
    }
}

void CSurf_TransportManager::SetRewindingState() {
    if (context->GetSetting("surface", "mute-master-on-fwd-rwd") != "0") {
        DAW::SetTrackMuted(GetMasterTrack(nullptr), true);
    }

    if (isRewinding) {
        isFastFwdRwd = !isFastFwdRwd;
        return;
    }
    isFastFwdRwd = false;
    isForwarding = false;
    isRewinding = true;
}

void CSurf_TransportManager::SetForwardingState() {
    if (context->GetSetting("surface", "mute-master-on-fwd-rwd") != "0") {
        DAW::SetTrackMuted(GetMasterTrack(nullptr), true);
    }

    if (isForwarding) {
        isFastFwdRwd = !isFastFwdRwd;
        return;
    }
    isFastFwdRwd = false;
    isRewinding = false;
    isForwarding = true;
}

void CSurf_TransportManager::handleFootSwitchKey(const std::string &key) const {
    const std::string device = context->GetNbChannels() > 1 ? FP_8 : FP_V2;
    const std::string action_id = settings->GetFunction(key, true);

    if (action_id == "0") {
        const int result = MB("There is no action assigned to this function.\nDo you want to assign an action?",
                              "No action assigned", 1);
        if (result == 1) {
            if (device == FP_8) {
                if (!ReaSonus8ControlPanel::control_panel_open) {
                    ToggleFP8ControlPanel(ReaSonus8ControlPanel::FUNCTIONS_PAGE);
                }
            } else if (device == FP_V2) {
                if (!ReaSonusV2ControlPanel::control_panel_open) {
                    ToggleFPV2ControlPanel(ReaSonusV2ControlPanel::FUNCTIONS_PAGE);
                }
            }
        }
        return;
    }

    if (isInteger(action_id)) {
        Main_OnCommandAsyncEx(stoi(action_id), 0, nullptr);
    } else {
        Main_OnCommandStringEx(action_id, 0, nullptr);
    }
}

CSurf_TransportManager::CSurf_TransportManager(CSurf_Context *context, midi_Output *m_midiout) : context(context) {
    playButton = new CSurf_Button(BTN_PLAY, BTN_VALUE_OFF, m_midiout);
    stopButton = new CSurf_Button(BTN_STOP, BTN_VALUE_OFF, m_midiout);
    recordButton = new CSurf_Button(BTN_RECORD, BTN_VALUE_OFF, m_midiout);
    repeatButton = new CSurf_Button(BTN_LOOP, BTN_VALUE_OFF, m_midiout);
    rewindButton = new CSurf_Button(BTN_REWIND, BTN_VALUE_OFF, m_midiout);
    forwardButton = new CSurf_Button(BTN_FORWARD, BTN_VALUE_OFF, m_midiout);

    settings = ReaSonusSettings::GetInstance(context->GetNbChannels() > 1 ? FP_8 : FP_V2);

    Update();
}

void CSurf_TransportManager::Update(const bool force_update) {
    const int playState = GetPlayStateEx(nullptr);
    isPlaying = hasBit(playState, 0);
    isPaused = hasBit(playState, 1);
    isRecording = hasBit(playState, 2);
    isStopped = !isPlaying && !isPaused;
    isRepeat = GetSetRepeatEx(nullptr, -1) > 0;

    if (isRewinding) {
        HandleRewind();
    }

    if (isForwarding) {
        HandleForward();
    }

    SetButtonValues(force_update);
}

void CSurf_TransportManager::Refresh(const bool force) const {
    SetButtonValues(force);
}

void CSurf_TransportManager::HandlePlayButton(const int value) {
    if (value == 0) {
        return;
    }

    StopRewindOrForward();
    CSurf_OnPlay();
}

void CSurf_TransportManager::HandleStopButton(const int value) {
    if (value == 0) {
        return;
    }

    StopRewindOrForward();
    CSurf_OnStop();
}

void CSurf_TransportManager::HandleRepeatButton(const int value) const {
    if (value == 0) {
        return;
    }

    if (context->GetShiftLeft()) {
        GetSetRepeatEx(nullptr, 0);
    } else {
        // SendMessage(g_hwnd, WM_COMMAND, IDC_REPEAT, 0);
        GetSetRepeatEx(nullptr, 2);
    }
}

void CSurf_TransportManager::HandleRecordButton(const int value) {
    if (value == 0) {
        return;
    }

    StopRewindOrForward();
    CSurf_OnRecord();
}

void CSurf_TransportManager::HandleRewindButton(const int value) {
    if (value == 0) {
        return;
    }

    if (context->GetShiftLeft()) {
        CSurf_GoStart();
        return;
    }
    SetPause();
    SetRewindingState();
}

void CSurf_TransportManager::HandleForwardButton(const int value) {
    if (value == 0) {
        return;
    }

    if (context->GetShiftLeft()) {
        CSurf_GoEnd();
        return;
    }

    SetPause();
    SetForwardingState();
}

void CSurf_TransportManager::HandleFootSwitchClick(const int value) const {
    if (value == 0) {
        return;
    }

    if (context->GetShiftChannelRight()) {
        handleFootSwitchKey("3");
    } else if (context->GetShiftChannelLeft()) {
        handleFootSwitchKey("2");
    } else {
        handleFootSwitchKey("1");
    }
}

void CSurf_TransportManager::HandlePlayStateChange() {
    StopRewindOrForward();
}
