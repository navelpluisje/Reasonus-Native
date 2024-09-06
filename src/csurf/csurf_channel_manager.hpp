#ifndef CSURF_CHANNEL_MANAGER_H_
#define CSURF_CHANNEL_MANAGER_H_

#include "csurf_context.cpp"
#include "csurf_navigator.cpp"
#include "csurf_button.hpp"
#include "csurf_track.hpp"
#include <vector>
#include <db2val.h>

/**
 * @brief Check if the bit with index key is set in val
 *
 * @param val The value to check
 * @param key The key to search for
 * @return true
 * @return false
 */
static bool hasBit(int val, int key)
{
    return val & (1 << key);
};

/**
 * @brief get the midi messages of the fader and translate it to volume
 *
 * @param msb Midi message 2
 * @param lsb Midi message 1
 * @return double
 */
static double int14ToVol(unsigned char msb, unsigned char lsb)
{
    int val = lsb | (msb << 7);
    double pos = ((double)val * 1000.0) / 16383.0;
    pos = SLIDER2DB(pos);

    return DB2VAL(pos);
}

class CSurf_ChannelManager
{
protected:
    CSurf_Context *context;
    CSurf_Navigator *navigator;
    midi_Output *m_midiout;

    ButtonColor colorActive;
    ButtonColor colorDim;

    std::vector<CSurf_Track *> tracks;

    virtual void SetTrackColors(MediaTrack *media_track)
    {
        (void)media_track;
    };

public:
    CSurf_ChannelManager(std::vector<CSurf_Track *> tracks, CSurf_Navigator *navigator, CSurf_Context *context, midi_Output *m_midiout)
    {
        this->context = context;
        this->navigator = navigator;
        this->tracks = tracks;
        this->m_midiout = m_midiout;
    };
    virtual ~CSurf_ChannelManager() {};

    virtual void UpdateTracks() {};

    virtual void HandleSelectClick(int index)
    {
        (void)index;
    };

    virtual void HandleMuteClick(int index)
    {
        (void)index;
    };

    virtual void HandleSoloClick(int index)
    {
        (void)index;
    };

    virtual void HandleFaderTouch() {};

    virtual void HandleFaderMove(int index, int msb, int lsb)
    {
        (void)index;
        (void)msb;
        (void)lsb;
    };
};

#endif