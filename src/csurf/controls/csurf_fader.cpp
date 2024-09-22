#include "csurf_fader.hpp"

void CSurf_Fader::SendValue()
{
    m_midiout->Send(0xe0 + channel, value & 0x7f, (value >> 7) & 0x7f, -1);
};

void CSurf_Fader::SetValue(int _value, bool force)
{
    if (value == _value && !force)
    {
        return;
    }
    value = _value;
    this->SendValue();
};
