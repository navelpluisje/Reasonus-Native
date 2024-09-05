#ifndef CSURF_BUTTON_H_
#define CSURF_BUTTON_H_

#include <reaper_plugin.h>
#include <reaper_plugin_functions.h>
#include "csurf_button_resources.hpp"

using namespace std;

class CSurf_Button
{
    // bool active = false;
    // bool blinking = false;
    Btn_Value value;
    Btn_Types type;

    void SendValue(midi_Output *m_midiout);

public:
    CSurf_Button(Btn_Types type, Btn_Value value);

    CSurf_Button(Btn_Types type, Btn_Value value, midi_Output *m_midiout);

    ~CSurf_Button() {};

    void SetValue(Btn_Value value, midi_Output *m_midiout);
};

struct ButtonColor
{
    int red;
    int green;
    int blue;

    void SetColor(int _red, int _green, int _blue)
    {
        red = _red;
        green = _green;
        blue = _blue;
    }
};

class CSurf_ColorButton
{
protected:
    // bool active = false;
    // bool blinking = false;
    Btn_Value value;
    Btn_Types type;

    ButtonColor colorActive = {
        0x7f,
        0x7f,
        0x7f,
    };
    ButtonColor colorDim = {
        0x7f,
        0x7f,
        0x7f,
    };

    void SendValue(midi_Output *m_midiout);

    void SendColor(midi_Output *m_midiout);

public:
    CSurf_ColorButton(ButtonColor colorActive, ButtonColor colorDim, Btn_Types type, Btn_Value value);

    CSurf_ColorButton(ButtonColor colorActive, ButtonColor colorDim, Btn_Types type, Btn_Value value, midi_Output *m_midiout);

    // void SetValue(Btn_Value value);
    void SetValue(Btn_Value value, midi_Output *m_midiout);

    void SetColor(ButtonColor colorActive, ButtonColor colorDim, midi_Output *m_midiout);
};

#endif // CSURF_BUTTON_H_