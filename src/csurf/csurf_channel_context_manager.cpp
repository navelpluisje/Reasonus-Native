#include "csurf_channel_context_manager.hpp"
#include "csurf_filter_manager.cpp"
#include "csurf_plugin_learn_manager.cpp"
#include "csurf_plugin_control_manager.cpp"
#include <reaper_plugin_functions.h>

void CSurf_ChannelContextManager::SetButtonValues(ChannelMode channelMode)
{
    context->SetChannelMode(channelMode);
    trackButton->SetValue(channelMode == TrackMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
    pluginsButton->SetValue(channelMode == PluginMode
                                ? BTN_VALUE_ON
                            : channelMode == TrackPluginMode ? BTN_VALUE_BLINK
                                                             : BTN_VALUE_OFF);
    sendButton->SetValue(channelMode == SendMode
                             ? BTN_VALUE_ON
                         : channelMode == TrackSendMode ? BTN_VALUE_BLINK
                                                        : BTN_VALUE_OFF);
    panButton->SetValue(channelMode == PanMode
                            ? BTN_VALUE_ON
                        : channelMode == TrackPanMode ? BTN_VALUE_BLINK
                                                      : BTN_VALUE_OFF);
}

CSurf_ChannelContextManager::CSurf_ChannelContextManager(
    CSurf_Context *context,
    CSurf_Navigator *navigator,
    midi_Output *m_midiout) : m_midiout(m_midiout), navigator(navigator), context(context)
{
    trackButton = new CSurf_Button(BTN_TRACK, BTN_VALUE_ON, m_midiout);
    pluginsButton = new CSurf_Button(BTN_EDIT_PLUGINS, BTN_VALUE_OFF, m_midiout);
    sendButton = new CSurf_Button(BTN_SEND, BTN_VALUE_OFF, m_midiout);
    panButton = new CSurf_Button(BTN_PAN, BTN_VALUE_OFF, m_midiout);

    for (int i = 0; i < context->GetNbChannels(); i++)
    {
        CSurf_Track *track = new CSurf_Track(i, context, m_midiout);
        tracks.push_back(track);
    }

    channelManager = new CSurf_TrackManager(tracks, navigator, context, m_midiout);
}

void CSurf_ChannelContextManager::HandleTrackButtonClick()
{
    if (context->GetChannelMode() != TrackMode)
    {
        SetButtonValues(TrackMode);
        channelManager = new CSurf_TrackManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderPanMode);
        context->ResetPanPushMode();
    }
};

void CSurf_ChannelContextManager::HandlePluginsButtonClick(bool track)
{
    if (context->GetChannelMode() != PluginMode && !track)
    {
        SetButtonValues(PluginMode);
        channelManager = new CSurf_PluginsManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderPluginMode);
        context->ResetPanPushMode();
    }
    else
    {
        SetButtonValues(TrackPluginMode);
        channelManager = new CSurf_TrackPluginsManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderPluginMode);
        context->ResetPanPushMode();
    }
};

void CSurf_ChannelContextManager::HandleSendButtonClick(bool track)
{
    if (context->GetChannelMode() != SendMode && !track)
    {
        SetButtonValues(SendMode);
        channelManager = new CSurf_SendsManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderSendMode);
    }
    else
    {
        SetButtonValues(TrackSendMode);
        channelManager = new CSurf_TrackSendsManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderSendMode);
    }
};

void CSurf_ChannelContextManager::HandlePanButtonClick(bool track)
{
    if (context->GetChannelMode() != PanMode && !track)
    {
        SetButtonValues(PanMode);
        channelManager = new CSurf_ReceivesManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderReceiveMode);
    }
    else
    {
        SetButtonValues(TrackPanMode);
        channelManager = new CSurf_TrackReceivesManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderReceiveMode);
    }
};

void CSurf_ChannelContextManager::HandleMixButtonClick()
{
    if (context->GetChannelMode() != MixMode)
    {
        prevChannelMode = context->GetChannelMode();
        context->SetChannelMode(MixMode);
        channelManager = new CSurf_FilterManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderReceiveMode);
    }
    else
    {
        ResetMixButtonClick();
    }
};

void CSurf_ChannelContextManager::ResetMixButtonClick()
{
    if (context->GetChannelMode() == MixMode)
    {
        HandleTrackButtonClick();
    }
};

void CSurf_ChannelContextManager::HandleLinkButtonClick()
{
    if (context->GetChannelMode() == PluginEditMode)
    {
        context->SetChannelMode(PluginControlMode);
        channelManager = new CSurf_PluginControlManager(tracks, navigator, context, m_midiout);
    }
    else
    {
        prevChannelMode = context->GetChannelMode();
        context->SetChannelMode(PluginEditMode);
        channelManager = new CSurf_PluginLearnManager(tracks, navigator, context, m_midiout);
    }
};

// ADD ALL THE TRACKMANAGERS METHODS HERE TO PROXY THEM
void CSurf_ChannelContextManager::UpdateTracks()
{
    // if (context->GetChannelMode() == TrackPluginMode || context->GetChannelMode() == PluginMode)
    // {
    //     int track, fx, _;
    //     if (GetFocusedFX2(&track, &_, &fx))
    //     {
    //         // TODO: Implement fx control
    //         // We can do some fx magic, but not now
    //     }
    // }
    channelManager->UpdateTracks();
}
void CSurf_ChannelContextManager::HandleMuteClick(int index) { channelManager->HandleMuteClick(index); }
void CSurf_ChannelContextManager::HandleSoloClick(int index) { channelManager->HandleSoloClick(index); }
void CSurf_ChannelContextManager::HandleSelectClick(int index)
{
    channelManager->HandleSelectClick(index);
    ResetMixButtonClick();
}
void CSurf_ChannelContextManager::HandleFaderMove(int index, int msb, int lsb) { channelManager->HandleFaderMove(index, msb, lsb); }
void CSurf_ChannelContextManager::HandleFaderTouch(int index) { channelManager->HandleFaderTouch(index); }