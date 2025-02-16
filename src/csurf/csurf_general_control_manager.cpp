#ifndef CSURF_GENERAL_CONTROL_MANAGER_C_
#define CSURF_GENERAL_CONTROL_MANAGER_C_

#include "csurf_context.cpp"
#include "controls/csurf_button.hpp"
#include "csurf_utils.hpp"
#include "csurf_navigator.hpp"
#include "csurf_faderport_ui_functions.hpp"
#include "csurf_faderport_ui_filters.hpp"
#include "controls/csurf_color_button.hpp"
#include "csurf_fader_manager.hpp"

using namespace CSURF_FADERPORT_UI_FUNCTIONS;
using namespace CSURF_FADERPORT_UI_FILTERS;

class CSurf_GeneralControlManager
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
    CSurf_Navigator *trackNavigator;
    CSurf_FaderManager *faderManager;
    midi_Output *m_midiout;

    ShiftState armState;
    ShiftState shiftState;
    bool hasSolo;
    bool hasMute;
    bool hasSelectedBypass;
    bool hasGlobalBypass;
    bool followCursor;
    bool lastTouchedFxMode;
    bool functionsDialogOpen;

    void SetButtonValue()
    {
        armButton->SetValue(context->GetArm() ? BTN_VALUE_ON : BTN_VALUE_OFF);
        if (context->GetShiftLeft())
        {
            bypassButton->SetValue(hasGlobalBypass ? BTN_VALUE_ON
                                                   : BTN_VALUE_OFF);
        }
        else
        {
            bypassButton->SetValue(hasSelectedBypass ? BTN_VALUE_ON
                                                     : BTN_VALUE_OFF);
        }

        soloClearButton->SetValue(hasSolo ? BTN_VALUE_ON : BTN_VALUE_OFF);
        muteClearButton->SetValue(hasMute ? BTN_VALUE_ON : BTN_VALUE_OFF);
        linkButton->SetValue(context->GetLastTouchedFxMode() ? BTN_VALUE_ON : BTN_VALUE_OFF);
        shiftLeftButton->SetValue((context->GetShiftLeft() || (context->GetShiftRight() && context->GetSwapShiftButtons()))
                                      ? BTN_VALUE_ON
                                      : BTN_VALUE_OFF);
    };

    void UpdatePanValue(int val)
    {
        double pan1, pan2 = 0.0;
        int panMode;
        MediaTrack *media_track = GetSelectedTrack(0, 0);
        GetTrackUIPan(media_track, &pan1, &pan2, &panMode);

        if (context->GetPanPushMode())
        {
            double newValue = int(panToNormalized(pan1) * 127.0) + val;
            newValue = newValue < 0 ? 0 : newValue > 127 ? 127
                                                         : newValue;
            SetMediaTrackInfo_Value(media_track, "D_PAN", normalizedToPan(newValue / 127));
        }
        else
        {
            double newValue = int(panToNormalized(pan2) * 127.0) + val;
            newValue = newValue < 0 ? 0 : newValue > 127 ? 127
                                                         : newValue;
            SetMediaTrackInfo_Value(media_track, "D_WIDTH", normalizedToPan(newValue / 127));
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

    void SetButtonColors() {};

public:
    CSurf_GeneralControlManager(
        CSurf_Context *context,
        CSurf_Navigator *trackNavigator,
        CSurf_FaderManager *faderManager,
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

    ~CSurf_GeneralControlManager() {};

    void Update()
    {
        hasSolo = trackNavigator->HasTracksWithSolo();
        hasMute = trackNavigator->HasTracksWithMute();
        hasSelectedBypass = (bool)GetToggleCommandState(8);
        hasGlobalBypass = (bool)GetToggleCommandState(40344);
        followCursor = GetToggleCommandStringState("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND");
        lastTouchedFxMode = context->GetLastTouchedFxMode();
        functionsDialogOpen = IsFunctionsDialogOpen();

        SetButtonValue();
    };

    void HandleEncoderClick(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            MediaTrack *media_track = GetSelectedTrack(0, 0);
            SetMediaTrackInfo_Value(media_track, "D_PAN", 0);
            SetMediaTrackInfo_Value(media_track, "D_WIDTH", 0);
        }
        else
        {
            context->TogglePanPushMode();
        }
    }

    void HandleEncoderChange(int value)
    {
        switch (context->GetPanEncoderMode())
        {
        case PanEncoderPanMode:
            hasBit(value, 6) ? DecrementPan(1) : IncrementPan(1);
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
        if (context->GetShiftLeft())
        {
            Main_OnCommandEx(40490, 0, 0); // Track: Arm all tracks for recording
        }
        else if (context->GetShiftRight())
        {
            Main_OnCommandEx(40491, 0, 0); // Track: Unarm all tracks for recording
        }
        else
        {
            armState.SetValue(value > 0);
            context->SetArm(armState.invert ? !armState.active : armState.active);
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

        if (context->GetShiftLeft())
        {
            IsFunctionsDialogOpen() ? HideFunctionsDialog() : ShowFunctionsDialog();
        }
        else
        {
            IsFiltersDialogOpen() ? HideFiltersDialog() : ShowFiltersDialog();
        }
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
            if (context->GetPluginControl() &&
                (context->GetChannelMode() == PluginControlMode ||
                 context->GetChannelMode() == PluginEditMode))
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
        context->SetShiftLeft(shiftState.invert ? !shiftState.active : shiftState.active);

        SetButtonValue();
    }
};

#endif