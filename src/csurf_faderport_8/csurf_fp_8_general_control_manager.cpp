#ifndef CSURF_FP_8_GENERAL_CONTROL_MANAGER_C_
#define CSURF_FP_8_GENERAL_CONTROL_MANAGER_C_

#include "../shared/csurf_context.cpp"
#include "../shared/csurf_utils.hpp"
#include "../controls/csurf_button.hpp"
#include "../controls/csurf_color_button.hpp"
#include "csurf_fp_8_navigator.hpp"
#include "csurf_fp_8_fader_manager.hpp"
#include "csurf_fp_8_menu_manager.cpp"
#include "csurf_fp_8_ui_control_panel.hpp"
#include "../shared/csurf_faderport_ui_imgui_utils.hpp"

class CSurf_FP_8_GeneralControlManager
{
protected:
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

    void SetButtonValue(bool force = false)
    {
        armButton->SetValue(context->GetArm() ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        if (context->GetShiftLeft())
        {
            bypassButton->SetValue(hasGlobalBypass
                                       ? BTN_VALUE_ON
                                       : BTN_VALUE_OFF,
                                   force);
            macroButton->SetColor(ButtonColorYellow, force);
            macroButton->SetValue(ReaSonus8ControlPanel::control_panel_open ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        }
        else
        {
            bypassButton->SetValue(hasSelectedBypass
                                       ? BTN_VALUE_ON
                                       : BTN_VALUE_OFF,
                                   force);
            macroButton->SetColor(ButtonColorWhite, force);
            macroButton->SetValue(ReaSonus8ControlPanel::control_panel_open ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        }

        soloClearButton->SetValue(hasSolo && !context->GetSettings()->GetDistractionFreeMode() ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        muteClearButton->SetValue(hasMute && !context->GetSettings()->GetDistractionFreeMode() ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        linkButton->SetValue(
            ButtonOnBlinkOff(
                context->GetLastTouchedFxMode(),
                context->GetChannelMode() == PluginEditMode,
                context->GetSettings()->GetDistractionFreeMode()),
            force);
        shiftLeftButton->SetValue(((!context->GetSettings()->GetSwapShiftButtons() && context->GetShiftLeft()) ||
                                   (context->GetShiftRight() && context->GetSettings()->GetSwapShiftButtons()))
                                      ? BTN_VALUE_ON
                                      : BTN_VALUE_OFF,
                                  force);
    };

    void UpdatePanValue(int val)
    {
        double pan1, pan2 = 0.0;
        int pan_mode;
        MediaTrack *media_track = GetSelectedTrack(0, 0);
        GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);
        if (pan_mode < PAN_MODE_STEREO_PAN)
        {
            pan_mode = PAN_MODE_BALANCE_PAN;
        }

        if (pan_mode == PAN_MODE_BALANCE_PAN)
        {
            double newValue = int(panToNormalized(pan1) * 127.0) + val;
            newValue = minmax(0.0, newValue, 127.0);
            SetMediaTrackInfo_Value(media_track, "D_PAN", normalizedToPan(newValue / 127));
        }
        else if (context->GetPanPushMode())
        {
            double newValue = int(panToNormalized(pan1) * 127.0) + val;
            newValue = minmax(0.0, newValue, 127.0);
            SetMediaTrackInfo_Value(media_track, pan_mode < 6 ? "D_PAN" : "D_DUALPANL", normalizedToPan(newValue / 127));
        }
        else
        {
            double newValue = int(panToNormalized(pan2) * 127.0) + val;
            newValue = minmax(0.0, newValue, 127.0);
            SetMediaTrackInfo_Value(media_track, pan_mode < 6 ? "D_WIDTH" : "D_DUALPANR", normalizedToPan(newValue / 127));
        }
    }

    void IncrementPan(int val)
    {
        UpdatePanValue(val);
    }

    void DecrementPan(int val)
    {
        UpdatePanValue(val * -1);
    }

    void resetPan()
    {
        MediaTrack *media_track = GetSelectedTrack(0, 0);

        switch (DAW::GetTrackPanMode(media_track))
        {
        case PAN_MODE_STEREO_PAN:
        {
            if (context->GetPanPushMode())
            {
                SetMediaTrackInfo_Value(media_track, "D_PAN", 0);
            }
            else
            {
                SetMediaTrackInfo_Value(media_track, "D_WIDTH", 1);
            }
            break;
        }
        case PAN_MODE_DUAL_PAN:
        {
            if (context->GetPanPushMode())
            {
                SetMediaTrackInfo_Value(media_track, "D_DUALPANL", -1);
            }
            else
            {
                SetMediaTrackInfo_Value(media_track, "D_DUALPANR", 1);
            }
            break;
        }
        default:
        {
            SetMediaTrackInfo_Value(media_track, "D_PAN", 0);
        }
        }
    }

    void SetButtonColors(bool force = false)
    {
        bypassButton->SetColor(ButtonColorRed, force);
        macroButton->SetColor(ButtonColorWhite, force);
        linkButton->SetColor(ButtonColorGreen, force);
    };

public:
    CSurf_FP_8_GeneralControlManager(
        CSurf_Context *context,
        CSurf_FP_8_Navigator *trackNavigator,
        CSurf_FP_8_FaderManager *faderManager,
        midi_Output *m_midiout) : context(context), trackNavigator(trackNavigator), faderManager(faderManager), m_midiout(m_midiout)
    {
        armButton = new CSurf_Button(BTN_ARM, BTN_VALUE_OFF, m_midiout);
        soloClearButton = new CSurf_Button(BTN_SOLO_CLEAR, BTN_VALUE_OFF, m_midiout);
        muteClearButton = new CSurf_Button(BTN_MUTE_CLEAR, BTN_VALUE_OFF, m_midiout);
        bypassButton = new CSurf_ColorButton(ButtonColorRed, BTN_BYPASS, BTN_VALUE_OFF, m_midiout);
        macroButton = new CSurf_ColorButton(ButtonColorWhite, BTN_MACRO, BTN_VALUE_OFF, m_midiout);
        linkButton = new CSurf_ColorButton(ButtonColorGreen, BTN_LINK, BTN_VALUE_OFF, m_midiout);
        shiftLeftButton = new CSurf_Button(BTN_SHIFT_LEFT, BTN_VALUE_OFF, m_midiout);
    };

    ~CSurf_FP_8_GeneralControlManager() {};

    void Update()
    {
        hasSolo = trackNavigator->HasTracksWithSolo();
        hasMute = trackNavigator->HasTracksWithMute();
        hasSelectedBypass = (bool)GetToggleCommandState(8);
        hasGlobalBypass = (bool)GetToggleCommandState(40344);
        followCursor = GetToggleCommandStringState("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND");
        last_touched_fx_mode = context->GetLastTouchedFxMode();
        functionsDialogOpen = ReaSonus8ControlPanel::control_panel_open && ReaSonus8ControlPanel::current_page == ReaSonus8ControlPanel::FUNCTIONS_PAGE;

        SetButtonValue();
        SetButtonColors();
    };

    void Refresh(bool force = false)
    {
        SetButtonValue(force);
        SetButtonColors(force);
    }

    void HandleEncoderClick(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetAddSendReceiveMode() > -1)
        {
            bool is_receive_mode = context->GetChannelMode() == ReceiveMode || context->GetChannelMode() == TrackReceiveMode;
            MediaTrack *src_track = is_receive_mode ? GetTrack(0, context->GetCurrentSelectedSendReceive()) : trackNavigator->GetTrackByIndex(context->GetAddSendReceiveMode());
            MediaTrack *dest_track = is_receive_mode ? trackNavigator->GetTrackByIndex(context->GetAddSendReceiveMode()) : GetTrack(0, context->GetCurrentSelectedSendReceive());

            if (CreateTrackSend(src_track, dest_track) > -1)
            {
                context->SetAddSendReceiveMode(-1);
            }
            return;
        }

        if (context->GetChannelMode() == MenuMode)
        {
            faderManager->HandleEncoderPush();
            return;
        }

        if (context->GetShiftLeft())
        {
            resetPan();
        }
        else
        {
            context->TogglePanPushMode();
        }
    }

    void HandleEncoderChange(int value)
    {
        if (context->GetAddSendReceiveMode() > -1)
        {
            hasBit(value, 6)
                ? context->DecrementCurrentSelectedSendReceive()
                : context->IncrementCurrentSelectedSendReceive();
            return;
        }

        if (context->GetChannelMode() == MenuMode)
        {
            hasBit(value, 6)
                ? faderManager->HandleEncoderDecrement()
                : faderManager->HandleEncoderIncrement();
            return;
        }

        switch (context->GetPanEncoderMode())
        {
        case PanEncoderTrackPanMode:
            hasBit(value, 6)
                ? DecrementPan(value - 64)
                : IncrementPan(value);
            break;

        case PanEncoderPluginMode:
        case PanEncoderSendMode:
        case PanEncoderReceiveMode:
            context->UpdateChannelManagerItemIndex(hasBit(value, 6) ? -1 : 1);
            break;
        }
    }

    void HandleArmButton(int value)
    {
        if (context->GetShiftChannelLeft())
        {
            Main_OnCommandEx(40490, 0, 0); // Track: Arm all tracks for recording
        }
        else if (context->GetShiftChannelRight())
        {
            Main_OnCommandEx(40491, 0, 0); // Track: Unarm all tracks for recording
        }
        else
        {
            armState.SetValue(value > 0);
            context->SetArm(armState.IsActive());
        }

        SetButtonValue();
    }

    void HandleSoloClearButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        Main_OnCommandEx(40340, 0, 0); // Track: Unsolo all tracks
    };

    void HandleMuteClearButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        Main_OnCommandEx(40339, 0, 0); // Track: Unmute all tracks
    };

    void HandleBypassButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        context->GetShiftLeft() ? Main_OnCommandEx(40344, 0, 0) // Track: Toggle FX bypass on all tracks
                                : Main_OnCommandEx(8, 0, 0);    // Track: Toggle FX bypass for selected tracks
    };

    void HandleMacroButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        int current_page = context->GetShiftLeft() ? ReaSonus8ControlPanel::FUNCTIONS_PAGE : ReaSonus8ControlPanel::FILTERS_PAGE;

        ToggleFP8ControlPanel(current_page);
    };

    void HandleLinkButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            Main_OnCommandStringEx("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND");
        }
        else
        {
            if (::CountTracks(0) == 0)
            {
                return;
            }

            if (context->GetSettings()->GetPluginControl() &&
                (context->IsChannelMode(PluginMode) ||
                 context->IsChannelMode(TrackPluginMode) ||
                 context->IsChannelMode(PluginControlMode) ||
                 context->IsChannelMode(PluginEditMode)) &&

                (context->GetPluginEditPluginId() > -1))
            {
                faderManager->HandleLinkButtonClick();
            }
            else
            {
                context->ToggleLastTouchedFxMode();
            }
        }
    };

    void HandleShiftButton(int value)
    {
        shiftState.SetValue(value > 0);
        context->SetShiftLeft(shiftState.IsActive());
        context->SetShiftLeftLocked(shiftState.IsLocked());

        SetButtonValue();
    }
};

#endif