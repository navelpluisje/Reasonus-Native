#ifndef CSURF_FP_8_GENERAL_CONTROL_MANAGER_C_
#define CSURF_FP_8_GENERAL_CONTROL_MANAGER_C_

#include "csurf_fp_8_fader_manager.hpp"
#include "../shared/csurf_reasonus_settings.hpp"
#include "../ui/windows/csurf_ui_fp_8_control_panel.hpp"

class CSurf_FP_8_GeneralControlManager {
    CSurf_Button *armButton;
    CSurf_Button *soloClearButton;
    CSurf_Button *muteClearButton;
    CSurf_ColorButton *bypassButton;
    CSurf_ColorButton *macroButton;
    CSurf_ColorButton *linkButton;
    CSurf_Button *shiftLeftButton;

    CSurf_Context *context;
    CSurf_FP_8_Navigator *trackNavigator;
    CSurf_FP_8_FaderManager *faderManager;
    midi_Output *m_midiout;
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_8);

    ShiftState armState;
    ShiftState shiftState;
    DoubleClickState dbcState;
    bool hasSolo;
    bool hasMute;
    bool hasSelectedBypass;
    bool hasGlobalBypass;
    bool followCursor;
    bool last_touched_fx_mode;

    bool functionsDialogOpen;

protected:
    void SetButtonValue(const bool force = false) const {
        armButton->SetValue(context->GetArm() ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        if (context->GetShiftLeft()) {
            bypassButton->SetValue(
                hasGlobalBypass
                    ? BTN_VALUE_ON
                    : BTN_VALUE_OFF,
                force
            );

            macroButton->SetColor(ButtonColorYellow, force);

            macroButton->SetValue(ReaSonus8ControlPanel::control_panel_open ? BTN_VALUE_ON : BTN_VALUE_OFF, force);

            soloClearButton->SetValue(
                GetToggleCommandIdState(40745)
                    ? BTN_VALUE_ON
                    : BTN_VALUE_OFF,
                force
            );
        } else {
            bypassButton->SetValue(
                hasSelectedBypass
                    ? BTN_VALUE_ON
                    : BTN_VALUE_OFF,
                force
            );

            macroButton->SetColor(ButtonColorWhite, force);

            macroButton->SetValue(ReaSonus8ControlPanel::control_panel_open ? BTN_VALUE_ON : BTN_VALUE_OFF, force);

            soloClearButton->SetValue(
                hasSolo && !settings->GetDistractionFreeMode()
                    ? BTN_VALUE_ON
                    : BTN_VALUE_OFF,
                force
            );
        }

        muteClearButton->SetValue(hasMute && !settings->GetDistractionFreeMode() ? BTN_VALUE_ON : BTN_VALUE_OFF, force);

        linkButton->SetValue(
            ButtonOnBlinkOff(
                context->GetLastTouchedFxMode(),
                context->GetChannelMode() == PluginEditMode,
                settings->GetDistractionFreeMode()),
            force
        );

        shiftLeftButton->SetValue(
            (!settings->GetSwapShiftButtons() && context->GetShiftLeft())
            || (context->GetShiftRight() && settings->GetSwapShiftButtons())
                ? BTN_VALUE_ON
                : BTN_VALUE_OFF,
            force
        );
    }

    void UpdatePanValue(const int val) const {
        double pan1 = 0.0;
        double pan2 = 0.0;
        int pan_mode;
        MediaTrack *media_track = GetSelectedTrack(nullptr, 0);
        GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);

        if (pan_mode < PAN_MODE_STEREO_PAN) {
            pan_mode = PAN_MODE_BALANCE_PAN;
        }

        if (pan_mode == PAN_MODE_BALANCE_PAN) {
            double newValue = static_cast<int>(panToNormalized(pan1) * 127.0) + val;
            newValue = minmax(0.0, newValue, 127.0);
            SetMediaTrackInfo_Value(media_track, "D_PAN", normalizedToPan(newValue / 127));
        } else if (context->GetPanPushMode()) {
            double newValue = static_cast<int>(panToNormalized(pan1) * 127.0) + val;
            newValue = minmax(0.0, newValue, 127.0);
            SetMediaTrackInfo_Value(media_track, pan_mode < 6 ? "D_PAN" : "D_DUALPANL",
                                    normalizedToPan(newValue / 127));
        } else {
            double newValue = static_cast<int>(panToNormalized(pan2) * 127.0) + val;
            newValue = minmax(0.0, newValue, 127.0);
            SetMediaTrackInfo_Value(media_track, pan_mode < 6 ? "D_WIDTH" : "D_DUALPANR",
                                    normalizedToPan(newValue / 127));
        }
    }

    void IncrementPan(const int val) const {
        UpdatePanValue(val);
    }

    void DecrementPan(const int val) const {
        UpdatePanValue(val * -1);
    }

    static void UpdateSoloInFrontVolume(const int val) {
        const int current_value = GetIntConfigVar("solodimdb10");

        if (current_value < 999) {
            SetIntConfigVar("solodimdb10", minmax(-1500, current_value + val, -10));
        }
    }

    static void IncrementSoloInFrontVolume(const int val) {
        UpdateSoloInFrontVolume(val);
    }

    static void DecrementSoloInFrontVolume(const int val) {
        UpdateSoloInFrontVolume(val);
    }

    void resetPan() const {
        MediaTrack *media_track = GetSelectedTrack(nullptr, 0);

        switch (DAW::GetTrackPanMode(media_track)) {
            case PAN_MODE_STEREO_PAN: {
                if (context->GetPanPushMode()) {
                    SetMediaTrackInfo_Value(media_track, "D_PAN", 0);
                } else {
                    SetMediaTrackInfo_Value(media_track, "D_WIDTH", 1);
                }
                break;
            }
            case PAN_MODE_DUAL_PAN: {
                if (context->GetPanPushMode()) {
                    SetMediaTrackInfo_Value(media_track, "D_DUALPANL", -1);
                } else {
                    SetMediaTrackInfo_Value(media_track, "D_DUALPANR", 1);
                }
                break;
            }
            default: {
                SetMediaTrackInfo_Value(media_track, "D_PAN", 0);
            }
        }
    }

    void SetButtonColors(const bool force = false) const {
        bypassButton->SetColor(ButtonColorRed, force);
        macroButton->SetColor(ButtonColorWhite, force);
        linkButton->SetColor(ButtonColorGreen, force);
    }

public:
    CSurf_FP_8_GeneralControlManager(
        CSurf_Context *context,
        CSurf_FP_8_Navigator *trackNavigator,
        CSurf_FP_8_FaderManager *faderManager,
        midi_Output *m_midiout
    ) : context(context), trackNavigator(trackNavigator), faderManager(faderManager), m_midiout(m_midiout) {
        hasSolo = false;
        hasMute = false;
        hasSelectedBypass = false;
        hasGlobalBypass = false;
        followCursor = false;
        last_touched_fx_mode = false;
        functionsDialogOpen = false;

        armButton = new CSurf_Button(BTN_ARM, BTN_VALUE_OFF, this->m_midiout);
        soloClearButton = new CSurf_Button(BTN_SOLO_CLEAR, BTN_VALUE_OFF, this->m_midiout);
        muteClearButton = new CSurf_Button(BTN_MUTE_CLEAR, BTN_VALUE_OFF, this->m_midiout);
        bypassButton = new CSurf_ColorButton(ButtonColorRed, BTN_BYPASS, BTN_VALUE_OFF, this->m_midiout);
        macroButton = new CSurf_ColorButton(ButtonColorWhite, BTN_MACRO, BTN_VALUE_OFF, this->m_midiout);
        linkButton = new CSurf_ColorButton(ButtonColorGreen, BTN_LINK, BTN_VALUE_OFF, this->m_midiout);
        shiftLeftButton = new CSurf_Button(BTN_SHIFT_LEFT, BTN_VALUE_OFF, this->m_midiout);
    }

    ~CSurf_FP_8_GeneralControlManager() = default;

    void Update(const bool force_update = false) {
        hasSolo = trackNavigator->HasTracksWithSolo();
        hasMute = trackNavigator->HasTracksWithMute();
        hasSelectedBypass = static_cast<bool>(GetToggleCommandState(8));
        hasGlobalBypass = static_cast<bool>(GetToggleCommandState(40344));
        followCursor = GetToggleCommandStringState("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND");
        last_touched_fx_mode = context->GetLastTouchedFxMode();
        functionsDialogOpen = ReaSonus8ControlPanel::control_panel_open && ReaSonus8ControlPanel::current_page ==
                              ReaSonus8ControlPanel::FUNCTIONS_PAGE;

        SetButtonValue(force_update);
        SetButtonColors(force_update);
    }

    void Refresh(const bool force = false) const {
        SetButtonValue(force);
        SetButtonColors(force);
    }

    void HandleEncoderClick(const int value) const {
        if (value == 0) {
            return;
        }

        if (context->GetAddSendReceiveMode() > -1) {
            const bool is_receive_mode = context->GetChannelMode() == ReceiveMode
                                         || context->GetChannelMode() == TrackReceiveMode;
            MediaTrack *src_track = is_receive_mode
                                        ? GetTrack(nullptr, context->GetCurrentSelectedSendReceive())
                                        : trackNavigator->GetTrackByIndex(context->GetAddSendReceiveMode());
            MediaTrack *dest_track = is_receive_mode
                                         ? trackNavigator->GetTrackByIndex(context->GetAddSendReceiveMode())
                                         : GetTrack(nullptr, context->GetCurrentSelectedSendReceive());

            if (CreateTrackSend(src_track, dest_track) > -1) {
                context->SetAddSendReceiveMode(-1);
            }
            return;
        }

        if (context->IsChannelMode(MenuMode)) {
            faderManager->HandleEncoderPush();
            return;
        }

        if (context->IsChannelMode(PluginControlMode)) {
            faderManager->HandleEncoderPush();
            return;
        }

        if (context->GetShiftLeft()) {
            resetPan();
        } else {
            context->TogglePanPushMode();
        }
    }

    void HandleEncoderChange(const int value) const {
        if (context->GetAddSendReceiveMode() > -1) {
            hasBit(value, 6)
                ? context->DecrementCurrentSelectedSendReceive()
                : context->IncrementCurrentSelectedSendReceive();
            return;
        }

        switch (context->GetPanEncoderMode()) {
            case PanEncoderTrackPanMode: {
                if (context->GetShiftLeft() && GetToggleCommandIdState(40745)) {
                    hasBit(value, 6)
                        ? DecrementSoloInFrontVolume(-10)
                        : IncrementSoloInFrontVolume(10);
                } else {
                    hasBit(value, 6)
                        ? DecrementPan(value - 64)
                        : IncrementPan(value);
                }
                break;
            }

            case PanEncoderMenuMode:
            case PanEncoderPluginStepSizeMode:
                hasBit(value, 6)
                    ? faderManager->HandleEncoderDecrement()
                    : faderManager->HandleEncoderIncrement();
                break;

            case PanEncoderPluginMode:
            case PanEncoderSendMode:
            case PanEncoderReceiveMode:
            case PanEncoderPanMode:
            case PanEncoderMixMode:
            case PanEncoderPluginEditMode:
                context->UpdateChannelManagerItemIndex(hasBit(value, 6) ? -1 : 1);
                break;

            case PanEncoderPluginControlMode:
                const int stepSize = stoi(settings->GetSetting("surface", "plugin-step-size", "1"));
                context->UpdateChannelManagerItemIndex(hasBit(value, 6) ? 0 - stepSize : stepSize);
                break;
        }
    }

    void HandleArmButton(const int value) {
        if (context->GetShiftChannelLeft()) {
            Main_OnCommandAsyncEx(40490, 0, nullptr); // Track: Arm all tracks for recording
        } else if (context->GetShiftChannelRight()) {
            Main_OnCommandAsyncEx(40491, 0, nullptr); // Track: Unarm all tracks for recording
        } else {
            armState.SetValue(value > 0);
            context->SetArm(armState.IsActive());
        }

        SetButtonValue();
    }

    void HandleSoloClearButton(const int value) const {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            Main_OnCommandAsyncEx(40745, 0, nullptr); // Options: Solo in front
        } else {
            Main_OnCommandAsyncEx(40340, 0, nullptr); // Track: Unsolo all tracks
        }
    }

    static void HandleMuteClearButton(const int value) {
        if (value == 0) {
            return;
        }

        Main_OnCommandAsyncEx(40339, 0, nullptr); // Track: Unmute all tracks
    }

    void HandleBypassButton(const int value) const {
        if (value == 0) {
            return;
        }

        context->GetShiftLeft()
            ? Main_OnCommandAsyncEx(40344, 0, nullptr) // Track: Toggle FX bypass on all tracks
            : Main_OnCommandAsyncEx(8, 0, nullptr); // Track: Toggle FX bypass for selected tracks
    }

    void HandleMacroButton(const int value) const {
        if (value == 0) {
            return;
        }

        const int current_page = context->GetShiftLeft()
                                     ? ReaSonus8ControlPanel::FUNCTIONS_PAGE
                                     : ReaSonus8ControlPanel::FILTERS_PAGE;

        ToggleFP8ControlPanel(current_page);
    }

    void HandleLinkButton(const int value) const {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            Main_OnCommandStringEx("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND", 0, nullptr);
        } else {
            if (CountTracks(nullptr) == 0) {
                return;
            }

            if (!settings->GetDisablePluginControl()
                && (
                    context->IsChannelMode(PluginMode)
                    || context->IsChannelMode(TrackPluginMode)
                    || context->IsChannelMode(PluginControlMode)
                    || context->IsChannelMode(PluginEditMode)
                )
                && context->GetPluginEditPluginId() > -1
            ) {
                faderManager->HandleLinkButtonClick();
            } else {
                context->ToggleLastTouchedFxMode();
            }
        }
    }

    void HandleShiftButton(const int value) {
        shiftState.SetValue(value > 0);
        context->SetShiftLeft(shiftState.IsActive());
        context->SetShiftLeftLocked(shiftState.IsLocked());

        SetButtonValue();
    }
};

#endif
