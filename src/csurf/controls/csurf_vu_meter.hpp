

#ifndef CSURF_VU_METER_H_
#define CSURF_VU_METER_H_

#include <reaper_plugin.h>
#include "csurf_vu_meter_resources.hpp"

using namespace std;

static ChannelVuMeter channelVuMeters[16] = {
    VU_METER_1,
    VU_METER_2,
    VU_METER_3,
    VU_METER_4,
    VU_METER_5,
    VU_METER_6,
    VU_METER_7,
    VU_METER_8,
    VU_METER_9,
    VU_METER_10,
    VU_METER_11,
    VU_METER_12,
    VU_METER_13,
    VU_METER_14,
    VU_METER_15,
    VU_METER_16,
};

class CSurf_VuMeter
{
protected:
    ChannelVuMeter channelVuMeter;
    int value = 0;
    midi_Output *m_midiout;

    void SendValue();

public:
    CSurf_VuMeter(int channel, midi_Output *m_midiout);

    ~CSurf_VuMeter() {};

    void SetValue(int _value, bool force = false);
};

#endif // CSURF_VU_METER_H_