#ifndef CSURF_FADER_H_
#define CSURF_FADER_H_

#include <reaper_plugin.h>
#include "csurf_fader_resources.hpp"
#include "csurf_utils.h"

using namespace std;

class CSurf_Fader
{
protected:
    double value;
    int channel;
    midi_Output *m_midiout;

    void SendValue()
    {
        int volint = int(volToNormalized(value) * 16383.0);

        m_midiout->Send(0xe0 + channel, volint & 0x7f, (volint >> 7) & 0x7f, -1);
    };

public:
    CSurf_Fader(int channel, double value, midi_Output *m_midiout) : value(value), channel(channel), m_midiout(m_midiout) {};

    ~CSurf_Fader() {};

    void SetValue(double _value)
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