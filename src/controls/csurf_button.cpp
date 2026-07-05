#include "csurf_button.hpp"

void CSurf_Button::SendValue() {
    if (m_midiout) {
        m_midiout->Send(MIDI_MESSAGE_BUTTON, type, value, -1);
    }
}

CSurf_Button::CSurf_Button(
    const Btn_Types type,
    const Btn_Value value,
    midi_Output *m_midiout
) : value(value), type(type), m_midiout(m_midiout) {
    this->CSurf_Button::SendValue();
}

void CSurf_Button::SetValue(const Btn_Value _value, const bool force) {
    if (value == _value && !force) {
        return;
    }
    value = _value;
    this->SendValue();
}
