#include "csurf_vu_meter.hpp"

void CSurf_VuMeter::SendValue()
{
    m_midiout->Send(channelVuMeter, value, 0, -1);
};

CSurf_VuMeter::CSurf_VuMeter(int channel, midi_Output *m_midiout) : m_midiout(m_midiout)
{
    channelVuMeter = channelVuMeters[channel];
    SendValue();
};

void CSurf_VuMeter::SetValue(int _value, bool force)
{
    if (value == _value && !force)
    {
        return;
    }
    value = _value;
    this->SendValue();
};
