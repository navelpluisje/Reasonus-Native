#include "csurf_fader_manager.hpp"
#include "csurf_filter_manager.cpp"
#include "csurf_plugin_learn_manager.cpp"
#include "csurf_plugin_control_manager.cpp"
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>

void CSurf_FaderManager::SetButtonValues(ChannelMode channelMode)
{
    context->SetChannelMode(channelMode);
    trackButton->SetValue(channelMode == TrackMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
    pluginsButton->SetValue(ButtonOnBlinkOff(channelMode == PluginMode, channelMode == TrackPluginMode));
    sendButton->SetValue(ButtonOnBlinkOff(channelMode == SendMode, channelMode == TrackSendMode));
    panButton->SetValue(ButtonOnBlinkOff(channelMode == PanMode, channelMode == TrackPanMode));
}

CSurf_FaderManager::CSurf_FaderManager(
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

CSurf_FaderManager::~CSurf_FaderManager()
{
    delete channelManager;
    delete navigator;
    delete context;
    tracks.clear();
};

void CSurf_FaderManager::HandleTrackButtonClick(int value)
{
    if (context->GetChannelMode() != TrackMode && value > 0)
    {
        SetButtonValues(TrackMode);
        channelManager = new CSurf_TrackManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderPanMode);
        context->ResetPanPushMode();
    }
};

void CSurf_FaderManager::HandlePluginsButtonClick(int value, bool track)
{
    if (value == 0)
    {
        return;
    }

    bool cameFromPlugin = (context->GetChannelMode() == PluginEditMode || context->GetChannelMode() == PluginControlMode);

    if ((context->GetChannelMode() != PluginMode && !track && !cameFromPlugin) || (prevChannelMode == PluginMode && cameFromPlugin))
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

void CSurf_FaderManager::HandleSendButtonClick(int value, bool track)
{
    if (value == 0)
    {
        return;
    }

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

void CSurf_FaderManager::HandlePanButtonClick(int value, bool track)
{
    if (value == 0)
    {
        return;
    }

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

void CSurf_FaderManager::HandleMixButtonClick()
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

void CSurf_FaderManager::ResetMixButtonClick()
{
    if (context->GetChannelMode() == MixMode)
    {
        HandleTrackButtonClick(BTN_VALUE_ON);
    }
};

void CSurf_FaderManager::HandleLinkButtonClick()
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

void CSurf_FaderManager::SetPluginControlMode()
{
    context->SetChannelMode(PluginControlMode);
    channelManager = new CSurf_PluginControlManager(tracks, navigator, context, m_midiout);
}

// ADD ALL THE TRACKMANAGERS METHODS HERE TO PROXY THEM
void CSurf_FaderManager::UpdateTracks()
{
    channelManager->UpdateTracks();
}

void CSurf_FaderManager::HandleMuteClick(int index, int value)
{
    channelManager->HandleMuteClick(index, value);
}

void CSurf_FaderManager::HandleSoloClick(int index, int value)
{
    channelManager->HandleSoloClick(index, value);

    if (value == 0)
    {
        return;
    }

    prevChannelMode = context->GetChannelMode();

    if (context->GetPluginControl() &&
        (context->GetChannelMode() == TrackPluginMode || context->GetChannelMode() == PluginMode) &&
        (context->GetPluginEditPluginId() > -1))
    {
        if (!hasPluginConfigFile(context->GetPluginEditTrack(), context->GetPluginEditPluginId()))
        {
            if (context->GetShiftLeft() && MB("Do you want to configure this plugin?", "No plugin configuration", 1) == 1)
            {
                context->SetChannelMode(PluginEditMode);
                channelManager = new CSurf_PluginLearnManager(tracks, navigator, context, m_midiout);
            }
        }
        else
        {
            context->SetChannelMode(PluginControlMode);
            channelManager = new CSurf_PluginControlManager(tracks, navigator, context, m_midiout);
        }
    }
}

void CSurf_FaderManager::HandleSelectClick(int index, int value)
{
    if (value == 0)
    {
        return;
    }

    channelManager->HandleSelectClick(index);
    ResetMixButtonClick();
}

void CSurf_FaderManager::HandleFaderMove(int index, int msb, int lsb)
{
    channelManager->HandleFaderMove(index, msb, lsb);
}

void CSurf_FaderManager::HandleFaderTouch(int index, int value)
{
    if (value == 0)
    {
        return;
    }

    channelManager->HandleFaderTouch(index);
}
