#ifndef CSURF_CHANNEL_MANAGER_H_
#define CSURF_CHANNEL_MANAGER_H_

#include "csurf_context.cpp"
#include "csurf_navigator.hpp"
#include "csurf_channel_manager_resources.hpp"
#include "controls/csurf_button.hpp"
#include "csurf_track.hpp"
#include <vector>

class CSurf_ChannelManager
{
protected:
    std::vector<CSurf_Track *> tracks;
    CSurf_Navigator *navigator;
    CSurf_Context *context;
    midi_Output *m_midiout;

    ButtonColor color;

    bool forceUpdate = false;

    virtual void SetTrackColors(MediaTrack *media_track)
    {
        (void)media_track;
    };

public:
    CSurf_ChannelManager(
        std::vector<CSurf_Track *> tracks,
        CSurf_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : tracks(tracks), navigator(navigator), context(context), m_midiout(m_midiout) {};

    virtual ~CSurf_ChannelManager()
    {
        delete navigator;
        delete context;
        delete m_midiout;
    };

    virtual void UpdateTracks() {};

    virtual void HandleSelectClick(int index)
    {
        (void)index;
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

    virtual void HandleFaderTouch(int index)
    {
        (void)index;
    };

    virtual void HandleFaderMove(int index, int msb, int lsb)
    {
        (void)index;
        (void)msb;
        (void)lsb;
    };
};

#endif