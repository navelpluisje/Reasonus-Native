#ifndef CSURF_COLOR_BUTTON_H_
#define CSURF_COLOR_BUTTON_H_

#include "csurf_button.hpp"
#include "csurf_color_button_colors.hpp"

class CSurf_ColorButton : public CSurf_Button
{
protected:
    ButtonColor colorActive = ButtonColorWhite;
    ButtonColor colorDim = ButtonColorWhiteDim;

    void SendValue();

    void SendColor();

public:
    CSurf_ColorButton(ButtonColor colorActive, ButtonColor colorDim, Btn_Types type, Btn_Value value, midi_Output *m_midiout);
    ~CSurf_ColorButton() {};

    virtual void SetValue(Btn_Value value, bool force = false) override;

    void SetColor(ButtonColor colorActive, ButtonColor colorDim, bool force = false);
};

#endif // CSURF_COLOR_BUTTON_H_