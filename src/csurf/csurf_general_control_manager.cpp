#ifndef CSURF_GENERAL_CONTROL_MANAGER_C_
#define CSURF_GENERAL_CONTROL_MANAGER_C_

#include "csurf_context.cpp"
#include "csurf_button.hpp"
#include "csurf_utils.hpp"
#include "csurf_navigator.cpp"
#include "csurf_color_button.hpp"

class CSurf_GeneralControlManager
{
protected:
    CSurf_Button *soloClearButton;
    CSurf_Button *muteClearButton;
    CSurf_ColorButton *bypassButton;
    CSurf_ColorButton *macroButton;
    CSurf_ColorButton *linkButton;

    CSurf_Context *context;
    CSurf_Navigator *trackNavigator;
    midi_Output *m_midiout;

    bool hasSolo;
    bool hasMute;
    bool hasSelectedBypass;
    bool hasGlobalBypass;
    bool followCursor;

    void SetButtonValue()
    {
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
        linkButton->SetValue(followCursor ? BTN_VALUE_ON : BTN_VALUE_OFF);
    };

    void SetButtonColors() {};

public:
    CSurf_GeneralControlManager(
        CSurf_Context *context,
        CSurf_Navigator *trackNavigator,
        midi_Output *m_midiout) : context(context), trackNavigator(trackNavigator), m_midiout(m_midiout)
    {
        soloClearButton = new CSurf_Button(BTN_SOLO_CLEAR, BTN_VALUE_OFF, m_midiout);
        muteClearButton = new CSurf_Button(BTN_MUTE_CLEAR, BTN_VALUE_OFF, m_midiout);
        bypassButton = new CSurf_ColorButton(ButtonColorRed, ButtonColorWhiteDim, BTN_BYPASS, BTN_VALUE_OFF, m_midiout);
        linkButton = new CSurf_ColorButton(ButtonColorGreen, ButtonColorGreenDim, BTN_LINK, BTN_VALUE_OFF, m_midiout);
    };

    ~CSurf_GeneralControlManager() {};

    void Update()
    {
        hasSolo = trackNavigator->HasTracksWithSolo();
        hasMute = trackNavigator->HasTracksWithMute();
        hasSelectedBypass = (bool)GetToggleCommandState(8);
        hasGlobalBypass = (bool)GetToggleCommandState(40344);
        followCursor = GetToggleCommandStringState("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND");

        SetButtonValue();
    };

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
        Main_OnCommandStringEx("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND");
    };
};

#endif