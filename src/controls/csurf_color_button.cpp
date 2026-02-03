#include "csurf_color_button.hpp"
#include "../shared/csurf_log.hpp"

CSurf_ColorButton::CSurf_ColorButton(ButtonColor _color, Btn_Types type, Btn_Value value, midi_Output *m_midiout) : CSurf_Button(type, value, m_midiout)
{
    color = _color;
    SendValue();
    SendColor();
};

void CSurf_ColorButton::SendValue()
{
    if (m_midiout)
    {
        LOG_DEBUG("CSurf_ColorButton::SendValue: [0x%02x, 0x%02x, 0x%02x]", MIDI_MESSAGE_BUTTON, type, value);
        m_midiout->Send(MIDI_MESSAGE_BUTTON, type, value, -1);
    }
};

void CSurf_ColorButton::SendColor()
{
    if (m_midiout)
    {
        LOG_DEBUG("CSurf_ColorButton::SendColor: [0x%02x, 0x%02x, 0x%02x]", MIDI_MESSAGE_COLOR_RED, type, color.red);
        LOG_DEBUG("CSurf_ColorButton::SendColor: [0x%02x, 0x%02x, 0x%02x]", MIDI_MESSAGE_COLOR_GREEN, type, color.green);
        LOG_DEBUG("CSurf_ColorButton::SendColor: [0x%02x, 0x%02x, 0x%02x]", MIDI_MESSAGE_COLOR_BLUE, type, color.blue);
        m_midiout->Send(MIDI_MESSAGE_COLOR_RED, type, color.red, -1);
        m_midiout->Send(MIDI_MESSAGE_COLOR_GREEN, type, color.green, -1);
        m_midiout->Send(MIDI_MESSAGE_COLOR_BLUE, type, color.blue, -1);
    }
};

void CSurf_ColorButton::SetValue(Btn_Value _value, bool force)
{
    if (value == _value && !force)
    {
        return;
    }
    value = _value;
    this->SendValue();
};

void CSurf_ColorButton::SetColor(ButtonColor _color, bool force)
{
    if (!color.IsColor(_color) || force)
    {
        color = _color;
        SendColor();
    }
};