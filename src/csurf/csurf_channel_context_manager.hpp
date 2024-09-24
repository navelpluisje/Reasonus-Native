#ifndef CSURF_CHANNEL_CONTEXT_MANAGER_H_
#define CSURF_CHANNEL_CONTEXT_MANAGER_H_

#include "csurf_track_manager.cpp"
#include "csurf_plugins_manager.cpp"
#include "csurf_sends_manager.cpp"
#include "csurf_receives_manager.cpp"
#include "controls/csurf_button.hpp"
#include "csurf_track.hpp"

class CSurf_ChannelContextManager
{
    CSurf_ChannelManager *channelManager = NULL;
    std::vector<CSurf_Track *> tracks;

    ChannelMode channelMode = TrackMode;
    midi_Output *m_midiout;
    CSurf_Navigator *navigator;
    CSurf_Context *context;

    CSurf_Button *trackButton;
    CSurf_Button *pluginsButton;
    CSurf_Button *sendButton;
    CSurf_Button *panButton;

    void SetButtonValues(ChannelMode mode);

public:
    CSurf_ChannelContextManager(CSurf_Context *context, CSurf_Navigator *navigator, midi_Output *m_midiout);

    ~CSurf_ChannelContextManager() {};

    void HandleTrackButtonClick();

    void HandlePluginsButtonClick();

    void HandleSendButtonClick();

    void HandlePanButtonClick();

    // ADD ALL THE TRACKMANAGERS METHODS HERE TO PROXY THEM
    void UpdateTracks();
    void HandleMuteClick(int index);
    void HandleSoloClick(int index);
    void HandleSelectClick(int index);
    void HandleFaderMove(int index, int msb, int lsb);
};

#endif
