#include "csurf_valuebar.hpp"

void CSurf_Valuebar::SendValue()
{
    m_midiout->Send(0xb0, channelValuebar, value, -1);
};

void CSurf_Valuebar::SendMode()
{
    m_midiout->Send(0xb0, channelValuebarMode, valuebarMode, -1);
};

CSurf_Valuebar::CSurf_Valuebar(int channel, double value, midi_Output *m_midiout) : value(value), m_midiout(m_midiout)
{
    channelValuebar = channelValuebars[channel];
    channelValuebarMode = channelValuebarModes[channel];
    valuebarMode = VALUEBAR_MODE_NORMAL;
    SendValue();
    SendMode();
};

void CSurf_Valuebar::SetValue(int _value)
{
    if (value == _value)
    {
        return;
    }
    value = _value;
    this->SendValue();
};

void CSurf_Valuebar::SetMode(ValuebarMode _valuebarMode)
{
    if (valuebarMode == _valuebarMode)
    {
        return;
    }
    valuebarMode = _valuebarMode;
    this->SendMode();
};
