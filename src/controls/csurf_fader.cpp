#include "csurf_fader.hpp"
#include "../shared/csurf_log.hpp"

void CSurf_Fader::SendValue()
{
    if (m_midiout)
    {
        LOG_DEBUG("CSurf_Fader::SendValue: [0x%02x, 0x%02x, 0x%02x]", 0xe0 + channel, value & 0x7f, (value >> 7) & 0x7f);
        m_midiout->Send(0xe0 + channel, value & 0x7f, (value >> 7) & 0x7f, -1);
    }
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
