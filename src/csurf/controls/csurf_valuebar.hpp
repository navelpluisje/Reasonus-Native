

#ifndef CSURF_VALUEBAR_H_
#define CSURF_VALUEBAR_H_

#include <reaper_plugin.h>
#include "csurf_valuebar_resources.hpp"

using namespace std;

static ChannelValueBar channelValuebars[8] = {
    VALUEBAR_1,
    VALUEBAR_2,
    VALUEBAR_3,
    VALUEBAR_4,
    VALUEBAR_5,
    VALUEBAR_6,
    VALUEBAR_7,
    VALUEBAR_8,
};

static ChannelValueBarMode channelValuebarModes[8] = {
    VALUEBAR_MODE_1,
    VALUEBAR_MODE_2,
    VALUEBAR_MODE_3,
    VALUEBAR_MODE_4,
    VALUEBAR_MODE_5,
    VALUEBAR_MODE_6,
    VALUEBAR_MODE_7,
    VALUEBAR_MODE_8,
};

class CSurf_Valuebar
{
protected:
    ChannelValueBar channelValuebar;
    ChannelValueBarMode channelValuebarMode;
    ValuebarMode valuebarMode;
    int value;
    midi_Output *m_midiout;

    void SendValue();

    void SendMode();

public:
    CSurf_Valuebar(int channel, double value, midi_Output *m_midiout);

    ~CSurf_Valuebar() {};

    void SetValue(int _value);

    void SetMode(ValuebarMode _valuebarMode);
};

#endif // CSURF_VALUEBAR_H_