#include "csurf_color_button.hpp"

CSurf_ColorButton::CSurf_ColorButton(ButtonColor colorActive, ButtonColor colorDim, Btn_Types type, Btn_Value value, midi_Output *m_midiout) : CSurf_Button(type, value, m_midiout)
{
    this->colorActive = colorActive;
    this->colorDim = colorDim;
    this->SendValue();
    this->SendColor();
};

void CSurf_ColorButton::SendValue()
{
    m_midiout->Send(MIDI_MESSAGE_BUTTON, type, value, -1);
};

void CSurf_ColorButton::SendColor()
{
    ButtonColor color = this->value == 0 ? colorDim : colorActive;

    this->m_midiout->Send(MIDI_MESSAGE_COLOR_RED, type, color.red, -1);
    m_midiout->Send(MIDI_MESSAGE_COLOR_GREEN, type, color.green, -1);
    m_midiout->Send(MIDI_MESSAGE_COLOR_BLUE, type, color.blue, -1);
};

void CSurf_ColorButton::SetColor(ButtonColor colorActive, ButtonColor colorDim)
{
    this->colorActive = colorActive;
    this->colorDim = colorDim;
    SendColor();
};
