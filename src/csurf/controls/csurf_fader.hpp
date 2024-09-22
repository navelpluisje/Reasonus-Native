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

    void SendValue();

public:
    CSurf_Fader(int channel, double value, midi_Output *m_midiout) : value(value), channel(channel), m_midiout(m_midiout) {};

    ~CSurf_Fader() {};

    void SetValue(int _value, bool force = false);
};

#endif // CSURF_FADER_H_