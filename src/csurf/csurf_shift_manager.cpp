#ifndef CSURF_SHIFT_MANAGER_C_
#define CSURF_SHIFT_MANAGER_C_

#include "controls/csurf_button.hpp"
#include "csurf_context.cpp"

const int TOGGLE_SPEED = 150;

class CSurf_ShiftManager
{
protected:
    CSurf_Context *context;

    CSurf_Button *shiftLeftButton;
    CSurf_Button *shiftRightButton;
    CSurf_Button *armButton;

    bool shiftLeft = false;
    bool shiftLeftInvert = false;
    int shiftLeftStart = 0;
    bool shiftRight = false;
    bool shiftRightInvert = false;
    int shiftRightStart = 0;
    bool arm = false;
    bool armInvert = false;
    int armStart = 0;

    void SetButtonValue()
    {
        shiftLeftButton->SetValue(context->GetShiftLeft() ? BTN_VALUE_ON : BTN_VALUE_OFF);
        shiftRightButton->SetValue(context->GetShiftRight() ? BTN_VALUE_ON : BTN_VALUE_OFF);
        armButton->SetValue(context->GetArm() ? BTN_VALUE_ON : BTN_VALUE_OFF);
    }

public:
    CSurf_ShiftManager(CSurf_Context *_context, midi_Output *m_midiout)
    {
        context = _context;
        shiftLeftButton = new CSurf_Button(BTN_SHIFT_LEFT, BTN_VALUE_OFF, m_midiout);
        shiftRightButton = new CSurf_Button(BTN_SHIFT_RIGHT, BTN_VALUE_OFF, m_midiout);
        armButton = new CSurf_Button(BTN_ARM, BTN_VALUE_OFF, m_midiout);
    }
    ~CSurf_ShiftManager() {};

    void HandleShiftLeftButton(int val)
    {
        int time = timeGetTime();
        shiftLeft = val > 0;

        if (shiftLeftStart == 0)
        {
            shiftLeftStart = time;
        }
        else
        {
            if (time - shiftLeftStart < TOGGLE_SPEED)
            {
                shiftLeftInvert = !shiftLeftInvert;
            }
            shiftLeftStart = 0;
        }
        context->SetShiftLeft(shiftLeftInvert ? !shiftLeft : shiftLeft);

        SetButtonValue();
    }

    void HandleShiftRightButton(int val)
    {
        int time = timeGetTime();
        shiftRight = val > 0;

        if (shiftRightStart == 0)
        {
            shiftRightStart = time;
        }
        else
        {
            if (time - shiftRightStart < TOGGLE_SPEED)
            {
                shiftRightInvert = !shiftRightInvert;
            }
            shiftRightStart = 0;
        }
        context->SetShiftRight(shiftRightInvert ? !shiftRight : shiftRight);

        SetButtonValue();
    }

    void HandleArmButton(int val)
    {
        int time = timeGetTime();
        arm = val > 0;

        if (armStart == 0)
        {
            armStart = time;
        }
        else
        {
            if (time - armStart < TOGGLE_SPEED)
            {
                armInvert = !armInvert;
            }
            armStart = 0;
        }
        context->SetArm(armInvert ? !arm : arm);

        SetButtonValue();
    }
};

#endif