#include "csurf_fader.hpp"

void CSurf_Fader::SendValue()
{
    m_midiout->Send(0xe0 + channel, value & 0x7f, (value >> 7) & 0x7f, -1);
};

void CSurf_Fader::SetValue(int _value)
{
    if (value == _value)
    {
        return;
    }
    value = _value;
    this->SendValue();
};
