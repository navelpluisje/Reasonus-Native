#ifndef CSURF_FP_8_FADER_MANAGER_H_
#define CSURF_FP_8_FADER_MANAGER_H_

#include "csurf_fp_8_track_manager.cpp"
#include "csurf_fp_8_plugins_manager.cpp"
#include "csurf_fp_8_track_plugins_manager.cpp"
#include "csurf_fp_8_sends_manager.cpp"
#include "csurf_fp_8_track_sends_manager.cpp"
#include "csurf_fp_8_receives_manager.cpp"
#include "csurf_fp_8_track_receives_manager.cpp"
#include "../shared/csurf_context.cpp"
#include "../controls/csurf_button.hpp"
#include "csurf_fp_8_track.hpp"

class CSurf_FP_8_FaderManager
{
    CSurf_FP_8_ChannelManager *channelManager = NULL;
    std::vector<CSurf_FP_8_Track *> tracks;

    ChannelMode prevChannelMode = TrackMode;
    midi_Output *m_midiout;
    CSurf_FP_8_Navigator *navigator;
    CSurf_Context *context;

    CSurf_Button *trackButton;
    CSurf_Button *pluginsButton;
    CSurf_Button *sendButton;
    CSurf_Button *panButton;

    void SetButtonValues(ChannelMode mode, bool force = false);

    void SetChannelMode(ChannelMode mode, bool updateButtons);

public:
    CSurf_FP_8_FaderManager(
        CSurf_Context *context,
        CSurf_FP_8_Navigator *navigator,
        midi_Output *m_midiout);

    ~CSurf_FP_8_FaderManager();

    void Refresh(bool force = false);

    void HandleTrackButtonClick(int value);

    void HandlePluginsButtonClick(int value, bool track = false);

    void HandleSendButtonClick(int value, bool track = false);

    void HandlePanButtonClick(int value, bool track = false);

    void HandleMixAllButtonClick();

    void ResetMixButtonClick();

    void HandleLinkButtonClick();

    void HandleTouchButtonClick();

    void SetPluginControlMode();

    void HandleEncoderIncrement();

    void HandleEncoderDecrement();

    void HandleEncoderPush();

    // ADD ALL THE TRACKMANAGERS METHODS HERE TO PROXY THEM
    void UpdateTracks();
    void HandleMuteClick(int index, int value);
    void HandleSoloClick(int index, int value);
    void HandleSelectClick(int index, int value);
    void HandleFaderMove(int index, int msb, int lsb);
    void HandleFaderTouch(int index, int value);
};

#endif
