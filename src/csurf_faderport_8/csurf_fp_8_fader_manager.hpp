#ifndef CSURF_FP_8_FADER_MANAGER_H_
#define CSURF_FP_8_FADER_MANAGER_H_

#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_filter_manager.cpp"
#include "csurf_fp_8_plugin_learn_manager.cpp"
#include "csurf_fp_8_plugin_control_manager.cpp"
#include "csurf_fp_8_menu_manager.cpp"
#include "csurf_fp_8_track_manager.cpp"
#include "csurf_fp_8_hui_plugins_manager.cpp"
#include "csurf_fp_8_track_plugins_manager.cpp"
#include "csurf_fp_8_hui_sends_manager.cpp"
#include "csurf_fp_8_track_sends_manager.cpp"
#include "csurf_fp_8_hui_receives_manager.cpp"
#include "csurf_fp_8_track_receives_manager.cpp"
#include "csurf_fp_8_track_pan_manager.cpp"

class CSurf_FP_8_FaderManager
{
    CSurf_FP_8_ChannelManager *channelManager = NULL;
    std::vector<CSurf_FP_8_Track *> tracks;

    ChannelMode prevChannelMode = TrackMode;
    midi_Output *m_midiout;
    CSurf_FP_8_Navigator *navigator;
    CSurf_Context *context;

    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_8);

    CSurf_Button *trackButton;
    CSurf_Button *pluginsButton;
    CSurf_Button *sendButton;
    CSurf_Button *panButton;

    void SetButtonValues(bool force = false);

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
    void HandleFaderTouch(int index, int value = 0);
};

#endif
