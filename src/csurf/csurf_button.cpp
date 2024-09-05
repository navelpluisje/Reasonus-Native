#include <reaper_plugin.h>
#include <reaper_plugin_functions.h>
#include "csurf_button.hpp"

void CSurf_Button::SendValue(midi_Output *m_midiout)
{
    m_midiout->Send(MIDI_MESSAGE_BUTTON, type, value, -1);
};

CSurf_Button::CSurf_Button(Btn_Types type, Btn_Value value) : value(value), type(type) {};

CSurf_Button::CSurf_Button(Btn_Types type, Btn_Value value, midi_Output *m_midiout) : value(value), type(type)
{
    this->SendValue(m_midiout);
};

void CSurf_Button::SetValue(Btn_Value _value, midi_Output *m_midiout)
{
    if (value == _value)
    {
        return;
    }
    value = _value;
    this->SendValue(m_midiout);
};

void CSurf_ColorButton::SendValue(midi_Output *m_midiout)
{
    m_midiout->Send(MIDI_MESSAGE_BUTTON, type, value, -1);
};

void CSurf_ColorButton::SendColor(midi_Output *m_midiout)
{
    ButtonColor color = this->value == 0 ? colorDim : colorActive;

    m_midiout->Send(MIDI_MESSAGE_COLOR_RED, type, color.red, -1);
    m_midiout->Send(MIDI_MESSAGE_COLOR_GREEN, type, color.green, -1);
    m_midiout->Send(MIDI_MESSAGE_COLOR_BLUE, type, color.blue, -1);
};

CSurf_ColorButton::CSurf_ColorButton(ButtonColor colorActive, ButtonColor colorDim, Btn_Types type, Btn_Value value) : value(value), type(type)
{
    this->colorActive = colorActive;
    this->colorDim = colorDim;
};

CSurf_ColorButton::CSurf_ColorButton(ButtonColor colorActive, ButtonColor colorDim, Btn_Types type, Btn_Value value, midi_Output *m_midiout) : value(value), type(type)
{
    this->colorActive = colorActive;
    this->colorDim = colorDim;
    this->SendValue(m_midiout);
    SendColor(m_midiout);
};

void CSurf_ColorButton::SetValue(Btn_Value _value, midi_Output *m_midiout)
{
    if (value == _value)
    {
        return;
    }
    value = _value;
    SendValue(m_midiout);
};

void CSurf_ColorButton::SetColor(ButtonColor colorActive, ButtonColor colorDim, midi_Output *m_midiout)
{
    this->colorActive = colorActive;
    this->colorDim = colorDim;
    SendColor(m_midiout);
};
