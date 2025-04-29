#include "csurf_button.hpp"

void CSurf_Button::SendValue()
{
    if (m_midiout)
    {

        m_midiout->Send(MIDI_MESSAGE_BUTTON, type, value, -1);
    }
}

CSurf_Button::CSurf_Button(Btn_Types type, Btn_Value value, midi_Output *m_midiout) : value(value), type(type), m_midiout(m_midiout)
{
    this->SendValue();
};

void CSurf_Button::SetValue(Btn_Value _value, bool force)
{
    if (value == _value && !force)
    {
        return;
    }
    value = _value;
    this->SendValue();
}
