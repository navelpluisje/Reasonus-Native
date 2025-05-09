#ifndef csurf_fader_manager_H_
#define csurf_fader_manager_H_

#include "csurf_track_manager.cpp"
#include "csurf_plugins_manager.cpp"
#include "csurf_track_plugins_manager.cpp"
#include "csurf_sends_manager.cpp"
#include "csurf_track_sends_manager.cpp"
#include "csurf_receives_manager.cpp"
#include "csurf_track_receives_manager.cpp"
#include "controls/csurf_button.hpp"
#include "csurf_track.hpp"

class CSurf_FaderManager
{
    CSurf_ChannelManager *channelManager = NULL;
    std::vector<CSurf_Track *> tracks;

    ChannelMode prevChannelMode = TrackMode;
    midi_Output *m_midiout;
    CSurf_Navigator *navigator;
    CSurf_Context *context;

    CSurf_Button *trackButton;
    CSurf_Button *pluginsButton;
    CSurf_Button *sendButton;
    CSurf_Button *panButton;

    void SetButtonValues(ChannelMode mode);

public:
    CSurf_FaderManager(
        CSurf_Context *context,
        CSurf_Navigator *navigator,
        midi_Output *m_midiout);

    ~CSurf_FaderManager();

    void HandleTrackButtonClick(int value);

    void HandlePluginsButtonClick(int value, bool track = false);

    void HandleSendButtonClick(int value, bool track = false);

    void HandlePanButtonClick(int value, bool track = false);

    void HandleMixButtonClick();

    void ResetMixButtonClick();

    void HandleLinkButtonClick();

    void SetPluginControlMode();

    // ADD ALL THE TRACKMANAGERS METHODS HERE TO PROXY THEM
    void UpdateTracks();
    void HandleMuteClick(int index, int value);
    void HandleSoloClick(int index, int value);
    void HandleSelectClick(int index, int value);
    void HandleFaderMove(int index, int msb, int lsb);
    void HandleFaderTouch(int index, int value);
};

#endif
