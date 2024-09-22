#ifndef CSURF_GENERAL_CONTROL_MANAGER_C_
#define CSURF_GENERAL_CONTROL_MANAGER_C_

#include "csurf_context.cpp"
#include "controls/csurf_button.hpp"
#include "csurf_utils.hpp"
#include "csurf_navigator.hpp"
#include "controls/csurf_color_button.hpp"

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
    midi_Output *m_midiout;

    ShiftState armState;
    ShiftState shiftState;
    bool hasSolo;
    bool hasMute;
    bool hasSelectedBypass;
    bool hasGlobalBypass;
    bool followCursor;
    bool lastTouchedFxMode;

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
        shiftLeftButton->SetValue(context->GetShiftLeft() ? BTN_VALUE_ON : BTN_VALUE_OFF);
    };

    void SetButtonColors() {};

public:
    CSurf_GeneralControlManager(
        CSurf_Context *context,
        CSurf_Navigator *trackNavigator,
        midi_Output *m_midiout) : context(context), trackNavigator(trackNavigator), m_midiout(m_midiout)
    {
        armButton = new CSurf_Button(BTN_ARM, BTN_VALUE_OFF, m_midiout);
        soloClearButton = new CSurf_Button(BTN_SOLO_CLEAR, BTN_VALUE_OFF, m_midiout);
        muteClearButton = new CSurf_Button(BTN_MUTE_CLEAR, BTN_VALUE_OFF, m_midiout);
        bypassButton = new CSurf_ColorButton(ButtonColorRed, ButtonColorWhiteDim, BTN_BYPASS, BTN_VALUE_OFF, m_midiout);
        linkButton = new CSurf_ColorButton(ButtonColorGreen, ButtonColorGreenDim, BTN_LINK, BTN_VALUE_OFF, m_midiout);
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

        SetButtonValue();
    };

    void HandleArmButton(int val)
    {
        int time = timeGetTime();
        armState.active = val > 0;

        if (armState.start == 0)
        {
            armState.start = time;
        }
        else
        {
            if (time - armState.start < TOGGLE_SPEED)
            {
                armState.ToggleInvert();
            }
            armState.start = 0;
        }
        context->SetArm(armState.invert ? !armState.active : armState.active);

        SetButtonValue();
    }

    void handleSoloClearButton()
    {
        Main_OnCommandEx(40340, 0, 0); // Track: Unsolo all tracks
    };

    void handleMuteClearButton()
    {
        Main_OnCommandEx(40339, 0, 0); // Track: Unmute all tracks
    };

    void handleBypassButton()
    {
        context->GetShiftLeft() ? Main_OnCommandEx(40344, 0, 0) // Track: Toggle FX bypass on all tracks
                                : Main_OnCommandEx(8, 0, 0);    // Track: Toggle FX bypass for selected tracks
    };

    void handleMacroButton() {};

    void handleLinkButton()
    {
        if (context->GetShiftLeft())
        {
            Main_OnCommandStringEx("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND");
        }
        else
        {
            context->ToggleLastTouchedFxMode();
            trackNavigator->UpdateTrackCount();
        }
    };

    void HandleShiftButton(int val)
    {
        int time = timeGetTime();
        shiftState.active = val > 0;

        if (shiftState.start == 0)
        {
            shiftState.start = time;
        }
        else
        {
            if (time - shiftState.start < TOGGLE_SPEED)
            {
                shiftState.ToggleInvert();
            }
            shiftState.start = 0;
        }
        context->SetShiftLeft(shiftState.invert ? !shiftState.active : shiftState.active);

        SetButtonValue();
    }
};

#endif