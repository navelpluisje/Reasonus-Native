#ifndef CSURF_V2_GENERAL_CONTROL_MANAGER_V2_C_
#define CSURF_V2_GENERAL_CONTROL_MANAGER_V2_C_

#include "../shared/csurf_context.cpp"
#include "../controls/csurf_button.hpp"
#include "../shared/csurf_utils.hpp"
#include "csurf_fp_v2_navigator.hpp"
#include "csurf_fp_v2_ui_functions.hpp"
#include "../controls/csurf_color_button.hpp"

class CSurf_FP_V2_GeneralControlManager
{
protected:
    CSurf_ColorButton *linkButton;
    CSurf_Button *shiftLeftButton;

    CSurf_Context *context;
    CSurf_FP_V2_Navigator *trackNavigator;
    midi_Output *m_midiout;

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
        linkButton->SetValue(context->GetLastTouchedFxMode() ? BTN_VALUE_ON : BTN_VALUE_OFF);
        shiftLeftButton->SetValue((context->GetShiftLeft() || (context->GetShiftRight() && context->GetSwapShiftButtons()))
                                      ? BTN_VALUE_ON
                                      : BTN_VALUE_OFF);
    };

    void SetButtonColors() {};

public:
    CSurf_FP_V2_GeneralControlManager(
        CSurf_Context *context,
        CSurf_FP_V2_Navigator *trackNavigator,
        midi_Output *m_midiout) : context(context), trackNavigator(trackNavigator), m_midiout(m_midiout)
    {
        linkButton = new CSurf_ColorButton(ButtonColorGreen, BTN_LINK, BTN_VALUE_OFF, m_midiout);
        shiftLeftButton = new CSurf_Button(BTN_SHIFT_LEFT, BTN_VALUE_OFF, m_midiout);
    };

    ~CSurf_FP_V2_GeneralControlManager() {};

    void Update()
    {
        followCursor = GetToggleCommandStringState("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND");
        lastTouchedFxMode = context->GetLastTouchedFxMode();
        functionsDialogOpen = CSURF_FP_V2_UI_FUNCTIONS::IsFunctionsDialogOpen();

        SetButtonValue();
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
                context->ToggleLastTouchedFxMode();
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