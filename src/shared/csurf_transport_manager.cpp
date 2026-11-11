#include "csurf_transport_manager.hpp"

#include "csurf_daw.hpp"
#include "csurf_faderport_ui_imgui_utils.hpp"
#include "../controls/csurf_button.hpp"
#include "../shared/csurf_context.hpp"
#include "../shared/csurf_utils.hpp"
#include "../ui/windows/csurf_ui_fp_v2_control_panel.hpp"
#include "../ui/windows/csurf_ui_fp_8_control_panel.hpp"

void CSurf_TransportManager::SetButtonValues(const bool force) const {
    play_button->SetValue(ButtonBlinkOnOff(is_paused, is_playing), force);
    stop_button->SetValue(is_stopped ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
    record_button->SetValue(is_recording ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
    repeat_button->SetValue(is_repeat ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
    rewind_button->SetValue(ButtonBlinkOnOff(is_rewinding && is_fast_fwdRwd, is_rewinding), force);
    forward_button->SetValue(ButtonBlinkOnOff(is_forwarding && is_fast_fwdRwd, is_forwarding), force);
}

void CSurf_TransportManager::StopRewindOrForward() {
    is_fast_fwdRwd = false;
    is_forwarding = false;
    is_rewinding = false;

    if (context->GetSetting("surface", "mute-master-on-fwd-rwd") != "0") {
        DAW::SetTrackMuted(GetMasterTrack(nullptr), false);
    }
}

void CSurf_TransportManager::HandleRewind() const {
    CSurf_OnRew(0);

    if (is_fast_fwdRwd) {
        CSurf_OnRew(0);
        CSurf_OnRew(0);
        CSurf_OnRew(0);
    }
}

void CSurf_TransportManager::HandleForward() const {
    CSurf_OnFwd(0);

    if (is_fast_fwdRwd) {
        CSurf_OnFwd(0);
        CSurf_OnFwd(0);
        CSurf_OnFwd(0);
    }
}

void CSurf_TransportManager::SetPause() const {
    if (is_playing) {
        CSurf_OnPlay();
    }
}

void CSurf_TransportManager::SetRewindingState() {
    if (context->GetSetting("surface", "mute-master-on-fwd-rwd") != "0") {
        DAW::SetTrackMuted(GetMasterTrack(nullptr), true);
    }

    if (is_rewinding) {
        is_fast_fwdRwd = !is_fast_fwdRwd;
        return;
    }
    is_fast_fwdRwd = false;
    is_forwarding = false;
    is_rewinding = true;
}

void CSurf_TransportManager::SetForwardingState() {
    if (context->GetSetting("surface", "mute-master-on-fwd-rwd") != "0") {
        DAW::SetTrackMuted(GetMasterTrack(nullptr), true);
    }

    if (is_forwarding) {
        is_fast_fwdRwd = !is_fast_fwdRwd;
        return;
    }
    is_fast_fwdRwd = false;
    is_rewinding = false;
    is_forwarding = true;
}

void CSurf_TransportManager::handleFunctionKey(const std::string &key, const FunctionTypes type) const {
    const std::string device = context->GetNbChannels() > 1 ? FP_8 : FP_V2;
    const std::string action_id = settings->GetFunction(key, type);

    if (action_id == "0") {
        const int result = MB(
        "There is no action assigned to this function.\nDo you want to assign an action?",
          "No action assigned",
          MB_TYPE_OK_CANCEL
        );

        if (result == MB_RETURN_OK) {
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

void CSurf_TransportManager::SetPreviousPlayState() {
    if (is_playing) {
        previous_play_state = PLAY_STATE_PLAYING;
    } else if (is_paused) {
        previous_play_state = PLAY_STATE_PAUSED;
    } else if (is_recording) {
        previous_play_state = PLAY_STATE_RECORDING;
    } else {
        previous_play_state = PLAY_STATE_STOPPED;
    }
}

void CSurf_TransportManager::ResetPlayState() const {
    switch (previous_play_state) {
        case PLAY_STATE_PLAYING:
            CSurf_OnPlay();
            break;

        case PLAY_STATE_PAUSED:
            CSurf_OnPause();
            break;

        case PLAY_STATE_RECORDING:
            CSurf_OnRecord();
            break;

        default:
            CSurf_OnStop();
    }
}

CSurf_TransportManager::CSurf_TransportManager(CSurf_Context *context, midi_Output *m_midiout) : context(context) {
    play_button = new CSurf_Button(BTN_PLAY, BTN_VALUE_OFF, m_midiout);
    stop_button = new CSurf_Button(BTN_STOP, BTN_VALUE_OFF, m_midiout);
    record_button = new CSurf_Button(BTN_RECORD, BTN_VALUE_OFF, m_midiout);
    repeat_button = new CSurf_Button(BTN_LOOP, BTN_VALUE_OFF, m_midiout);
    rewind_button = new CSurf_Button(BTN_REWIND, BTN_VALUE_OFF, m_midiout);
    forward_button = new CSurf_Button(BTN_FORWARD, BTN_VALUE_OFF, m_midiout);

    settings = ReaSonusSettings::GetInstance(context->GetNbChannels() > 1 ? FP_8 : FP_V2);

    Update();
}

void CSurf_TransportManager::Update(const bool force_update) {
    const auto playState = static_cast<PLAY_STATES>(GetPlayStateEx(nullptr));
    is_playing = hasBit(playState, 0);
    is_paused = hasBit(playState, 1);
    is_recording = hasBit(playState, 2);
    is_stopped = !is_playing && !is_paused;
    is_repeat = GetSetRepeatEx(nullptr, -1) > 0;

    if (is_rewinding) {
        HandleRewind();
    }

    if (is_forwarding) {
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
    if (context->GetShiftRight() && value > 0) {
        handleFunctionKey("3", FUNCTION_TRANSPORT);
        return;
    }

    if (context->GetShiftLeft() && value > 0) {
        const std::string action_id = settings->GetFunction("1", FUNCTION_TRANSPORT);

        if (action_id == "0") {
            CSurf_GoStart();
        } else {
            handleFunctionKey("1", FUNCTION_TRANSPORT);
        }

        return;
    }

    const int now = static_cast<int>(GetTickCount());

    if (settings->HasMomentaryFwdRwd()) {
        if (value == 0) {
            StopRewindOrForward();
            ResetPlayState();

            if (now - rwd_started < 200) {
                Main_OnCommandAsyncEx(41041, 0, nullptr);
            }
            rwd_started = 0;
        } else {
            rwd_started = now;
            SetPreviousPlayState();
            SetPause();
            SetRewindingState();
        }
        return;
    }

    if (value > 0) {
        SetPause();
        SetRewindingState();
    }
}

void CSurf_TransportManager::HandleForwardButton(const int value) {
    if (context->GetShiftRight() && value > 0) {
        handleFunctionKey("4", FUNCTION_TRANSPORT);
        return;
    }

    if (context->GetShiftLeft() && value > 0) {
        const std::string action_id = settings->GetFunction("2", FUNCTION_TRANSPORT);

        if (action_id == "0") {
            CSurf_GoEnd();
        } else {
            handleFunctionKey("2", FUNCTION_TRANSPORT);
        }

        return;
    }

    const int now = static_cast<int>(GetTickCount());

    if (settings->HasMomentaryFwdRwd()) {
        if (value == 0) {
            StopRewindOrForward();
            ResetPlayState();

            if (now - fwd_started < 200) {
                Main_OnCommandAsyncEx(41040, 0, nullptr);
            }
            fwd_started = 0;
        } else {
            fwd_started = now;
            SetPreviousPlayState();
            SetPause();
            SetForwardingState();
        }
        return;
    }

    if (value > 0) {
        SetPause();
        SetForwardingState();
    }
}

void CSurf_TransportManager::HandleFootSwitchClick(const int value) const {
    if (value == 0) {
        return;
    }

    if (context->GetShiftChannelRight()) {
        handleFunctionKey("3", FUNCTION_FOOTSWITCH);
    } else if (context->GetShiftChannelLeft()) {
        handleFunctionKey("2", FUNCTION_FOOTSWITCH);
    } else {
        handleFunctionKey("1", FUNCTION_FOOTSWITCH);
    }
}

void CSurf_TransportManager::HandlePlayStateChange() {
    StopRewindOrForward();
}
