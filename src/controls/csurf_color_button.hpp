#ifndef CSURF_COLOR_BUTTON_H_
#define CSURF_COLOR_BUTTON_H_

#include "csurf_button.hpp"
#include "csurf_color_button_colors.hpp"

class CSurf_ColorButton : public CSurf_Button
{
protected:
    ButtonColor color = ButtonColorWhite;

    void SendValue();

    void SendColor();

public:
    CSurf_ColorButton(ButtonColor color, Btn_Types type, Btn_Value value, midi_Output *m_midiout);
    ~CSurf_ColorButton() {};

    virtual void SetValue(Btn_Value value, bool force = false) override;

    void SetColor(ButtonColor color, bool force = false);
};

#endif // CSURF_COLOR_BUTTON_H_