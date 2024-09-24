#include "csurf_channel_context_manager.hpp"

void CSurf_ChannelContextManager::SetButtonValues(ChannelMode mode)
{
    channelMode = mode;
    trackButton->SetValue(channelMode == TrackMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
    pluginsButton->SetValue(channelMode == PluginMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
    sendButton->SetValue(channelMode == SendMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
    panButton->SetValue(channelMode == PanMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
}

CSurf_ChannelContextManager::CSurf_ChannelContextManager(CSurf_Context *context, CSurf_Navigator *navigator, midi_Output *m_midiout) : m_midiout(m_midiout), navigator(navigator), context(context)
{
    trackButton = new CSurf_Button(BTN_TRACK, BTN_VALUE_ON, m_midiout);
    pluginsButton = new CSurf_Button(BTN_EDIT_PLUGINS, BTN_VALUE_OFF, m_midiout);
    sendButton = new CSurf_Button(BTN_SEND, BTN_VALUE_OFF, m_midiout);
    panButton = new CSurf_Button(BTN_PAN, BTN_VALUE_OFF, m_midiout);

    for (int i = 0; i < navigator->GetTrackCount(); i++)
    {
        CSurf_Track *track = new CSurf_Track(i, context, m_midiout);
        tracks.push_back(track);
    }

    channelManager = new CSurf_TrackManager(tracks, navigator, context, m_midiout);
}

void CSurf_ChannelContextManager::HandleTrackButtonClick()
{
    if (channelMode != TrackMode)
    {
        SetButtonValues(TrackMode);
        channelManager = new CSurf_TrackManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderPanMode);
        context->ResetPanPushMode();
    }
};

void CSurf_ChannelContextManager::HandlePluginsButtonClick()
{
    if (channelMode != PluginMode)
    {
        SetButtonValues(PluginMode);
        channelManager = new CSurf_PluginsManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderPluginMode);
        context->ResetPanPushMode();
    }
};

void CSurf_ChannelContextManager::HandleSendButtonClick()
{
    if (channelMode != SendMode)
    {
        SetButtonValues(SendMode);
        channelManager = new CSurf_SendsManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderSendMode);
    }
};

void CSurf_ChannelContextManager::HandlePanButtonClick()
{
    if (channelMode != PanMode)
    {
        SetButtonValues(PanMode);
        channelManager = new CSurf_ReceivesManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderReceiveMode);
    }
};

// ADD ALL THE TRACKMANAGERS METJODS HERE TO PROXY THEM
void CSurf_ChannelContextManager::UpdateTracks() { channelManager->UpdateTracks(); }
void CSurf_ChannelContextManager::HandleMuteClick(int index) { channelManager->HandleMuteClick(index); }
void CSurf_ChannelContextManager::HandleSoloClick(int index) { channelManager->HandleSoloClick(index); }
void CSurf_ChannelContextManager::HandleSelectClick(int index) { channelManager->HandleSelectClick(index); }
void CSurf_ChannelContextManager::HandleFaderMove(int index, int msb, int lsb) { channelManager->HandleFaderMove(index, msb, lsb); }