#ifndef CSURF_COLOR_BUTTON_H_
#define CSURF_COLOR_BUTTON_H_

#include "csurf_button.hpp"

using namespace std;

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

    bool IsColor(ButtonColor color)
    {
        return color.blue == blue && color.red == red && color.green == green;
    }
};

class CSurf_ColorButton : public CSurf_Button
{
protected:
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

    void SendValue();

    void SendColor();

public:
    CSurf_ColorButton(ButtonColor colorActive, ButtonColor colorDim, Btn_Types type, Btn_Value value, midi_Output *m_midiout);

    void SetColor(ButtonColor colorActive, ButtonColor colorDim);
};

#endif // CSURF_COLOR_BUTTON_H_