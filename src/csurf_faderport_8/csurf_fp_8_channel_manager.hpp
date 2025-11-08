#ifndef CSURF_FP_8_CHANNEL_MANAGER_H_
#define CSURF_FP_8_CHANNEL_MANAGER_H_

#include "../shared/csurf_context.cpp"
#include "../controls/csurf_button.hpp"
#include "csurf_fp_8_navigator.hpp"
#include "csurf_fp_8_channel_manager_resources.hpp"
#include "csurf_fp_8_track.hpp"
#include <vector>

class CSurf_FP_8_ChannelManager
{
protected:
    std::vector<CSurf_FP_8_Track *> tracks;
    CSurf_FP_8_Navigator *navigator;
    CSurf_Context *context;
    midi_Output *m_midiout;

    ButtonColor color;

    bool forceUpdate = false;

    virtual void SetTrackColors(MediaTrack *media_track)
    {
        (void)media_track;
    };

public:
    CSurf_FP_8_ChannelManager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : tracks(tracks), navigator(navigator), context(context), m_midiout(m_midiout) {};

    virtual ~CSurf_FP_8_ChannelManager()
    {
        delete navigator;
        delete context;
        delete m_midiout;
    };

    virtual void UpdateTracks() {};

    virtual void HandleSelectClick(int index, int value)
    {
        (void)index;
        (void)value;
    };

    virtual void HandleMuteClick(int index, int value)
    {
        (void)index;
        (void)value;
    };

    virtual void HandleSoloClick(int index, int value)
    {
        (void)index;
        (void)value;
    };

    virtual void HandleFaderTouch(int index, int value)
    {
        (void)index;
        (void)value;
    };

    virtual void HandleFaderMove(int index, int msb, int lsb)
    {
        (void)index;
        (void)msb;
        (void)lsb;
    };
};

#endif