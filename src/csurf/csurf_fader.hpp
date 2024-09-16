#ifndef CSURF_FADER_H_
#define CSURF_FADER_H_

#include <reaper_plugin.h>
#include "csurf_fader_resources.hpp"

using namespace std;

class CSurf_Fader
{
protected:
    int value;
    int channel;
    midi_Output *m_midiout;

    void SendValue()
    {
        m_midiout->Send(0xe0 + channel, value & 0x7f, (value >> 7) & 0x7f, -1);
    };

public:
    CSurf_Fader(int channel, double value, midi_Output *m_midiout) : value(value), channel(channel), m_midiout(m_midiout) {};

    ~CSurf_Fader() {};

    void SetValue(int _value)
    {
        if (value == _value)
        {
            return;
        }
        value = _value;
        this->SendValue();
    };
};

#endif // CSURF_FADER_H_