#ifndef CSURF_V2_GENERAL_CONTROL_MANAGER_V2_C_
#define CSURF_V2_GENERAL_CONTROL_MANAGER_V2_C_

#include "../shared/csurf_context.cpp"
#include "../controls/csurf_button.hpp"
#include "csurf_fp_v2_navigator.hpp"

class CSurf_FP_V2_GeneralControlManager
{
protected:
    CSurf_Button *shiftLeftButton;

    CSurf_Context *context;
    CSurf_FP_V2_Navigator *trackNavigator;
    midi_Output *m_midiout;

    ShiftState shiftState;

    void SetButtonValue()
    {
        shiftLeftButton->SetValue(context->GetShiftLeft() ? BTN_VALUE_ON : BTN_VALUE_OFF);
    };

    void SetButtonColors() {};

public:
    CSurf_FP_V2_GeneralControlManager(
        CSurf_Context *context,
        CSurf_FP_V2_Navigator *trackNavigator,
        midi_Output *m_midiout) : context(context), trackNavigator(trackNavigator), m_midiout(m_midiout)
    {
        shiftLeftButton = new CSurf_Button(BTN_SHIFT_LEFT, BTN_VALUE_OFF, m_midiout);
    };

    ~CSurf_FP_V2_GeneralControlManager() {};

    void Update()
    {
        SetButtonValue();
    };

    void HandleShiftButton(int value)
    {
        shiftState.SetValue(value > 0);
        context->SetShiftLeft(shiftState.IsActive());

        SetButtonValue();
    }
};

#endif