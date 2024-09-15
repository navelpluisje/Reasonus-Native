#ifndef CSURF_CHANNEL_CONTEXT_H_
#define CSURF_CHANNEL_CONTEXT_H_

#include "csurf_track_manager.cpp"
#include "csurf_sends_manager.cpp"
#include "csurf_receives_manager.cpp"
#include "csurf_button.hpp"
#include "csurf_track.hpp"

class CSurf_ChannelContext
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

    void SetButtonValues(ChannelMode mode)
    {
        channelMode = mode;
        trackButton->SetValue(channelMode == TrackMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
        pluginsButton->SetValue(channelMode == PluginMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
        sendButton->SetValue(channelMode == SendMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
        panButton->SetValue(channelMode == PanMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
    }

public:
    CSurf_ChannelContext(CSurf_Context *context, CSurf_Navigator *navigator, midi_Output *m_midiout) : m_midiout(m_midiout), navigator(navigator), context(context)
    {
        trackButton = new CSurf_Button(BTN_TRACK, BTN_VALUE_ON, m_midiout);
        pluginsButton = new CSurf_Button(BTN_EDIT_PLUGINS, BTN_VALUE_OFF, m_midiout);
        sendButton = new CSurf_Button(BTN_SEND, BTN_VALUE_OFF, m_midiout);
        panButton = new CSurf_Button(BTN_PAN, BTN_VALUE_OFF, m_midiout);

        for (int i = 0; i < 8; i++)
        {
            CSurf_Track *track = new CSurf_Track(i, context, m_midiout);
            tracks.push_back(track);
        }

        channelManager = new CSurf_TrackManager(tracks, navigator, context, m_midiout);
    }
    ~CSurf_ChannelContext() {}

    void handleTrackButtonClick()
    {
        if (channelMode != TrackMode)
        {
            SetButtonValues(TrackMode);
            channelManager = new CSurf_TrackManager(tracks, navigator, context, m_midiout);
            context->SetPanEncoderMode(PanEncoderPanMode);
        }
    };
    void handlePluginsButtonClick() {};
    void handleSendButtonClick()
    {
        if (channelMode != SendMode)
        {
            SetButtonValues(SendMode);
            channelManager = new CSurf_SendsManager(tracks, navigator, context, m_midiout);
            context->SetPanEncoderMode(PanEncoderNavigateMode);
        }
    };
    void handlePanButtonClick()
    {
        if (channelMode != PanMode)
        {
            SetButtonValues(PanMode);
            channelManager = new CSurf_ReceivesManager(tracks, navigator, context, m_midiout);
            context->SetPanEncoderMode(PanEncoderNavigateMode);
        }
    };

    // ADD ALL THE TRACKMANAGERS METJODS HERE TO PROXY THEM
    void UpdateTracks() { channelManager->UpdateTracks(); }
    void HandleMuteClick(int index) { channelManager->HandleMuteClick(index); }
    void HandleSoloClick(int index) { channelManager->HandleSoloClick(index); }
    void HandleSelectClick(int index) { channelManager->HandleSelectClick(index); }
    void HandleFaderMove(int index, int msb, int lsb) { channelManager->HandleFaderMove(index, msb, lsb); }
};

#endif
