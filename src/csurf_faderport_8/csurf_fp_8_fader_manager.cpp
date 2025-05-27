#include "csurf_fp_8_fader_manager.hpp"
#include "csurf_fp_8_filter_manager.cpp"
#include "csurf_fp_8_plugin_learn_manager.cpp"
#include "csurf_fp_8_plugin_control_manager.cpp"
#include "csurf_fp_8_menu_manager.cpp"

void CSurf_FP_8_FaderManager::SetButtonValues(ChannelMode channelMode)
{
    trackButton->SetValue(channelMode == TrackMode ? BTN_VALUE_ON : BTN_VALUE_OFF);
    pluginsButton->SetValue(ButtonOnBlinkOff(channelMode == PluginMode, channelMode == TrackPluginMode));
    sendButton->SetValue(ButtonOnBlinkOff(channelMode == SendMode, channelMode == TrackSendMode));
    panButton->SetValue(ButtonOnBlinkOff(channelMode == PanMode, channelMode == TrackPanMode));
}

void CSurf_FP_8_FaderManager::SetChannelMode(ChannelMode channelMode, bool updateButtons = false)
{
    context->SetChannelMode(channelMode);
    if (updateButtons)
    {
        SetButtonValues(channelMode);
    }

    switch (channelMode)
    {
    case TrackMode:
        channelManager = new CSurf_FP_8_TrackManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderPanMode);
        context->ResetPanPushMode();
        context->SetShowTimeCode(false);
        break;

    case PluginMode:
        channelManager = new CSurf_FP_8_PluginsManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderPluginMode);
        context->ResetPanPushMode();
        break;

    case TrackPluginMode:
        channelManager = new CSurf_FP_8_TrackPluginsManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderPluginMode);
        context->ResetPanPushMode();
        break;

    case SendMode:
        channelManager = new CSurf_FP_8_SendsManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderSendMode);
        break;

    case TrackSendMode:
        channelManager = new CSurf_FP_8_TrackSendsManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderSendMode);
        break;

    case PanMode:
        channelManager = new CSurf_FP_8_ReceivesManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderReceiveMode);
        break;

    case TrackPanMode:
        channelManager = new CSurf_FP_8_TrackReceivesManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderReceiveMode);
        break;

    case MixMode:
        channelManager = new CSurf_FP_8_FilterManager(tracks, navigator, context, m_midiout);
        context->SetPanEncoderMode(PanEncoderReceiveMode);
        break;

    case PluginControlMode:
        channelManager = new CSurf_FP_8_PluginControlManager(tracks, navigator, context, m_midiout);
        break;

    case PluginEditMode:
        channelManager = new CSurf_FP_8_PluginLearnManager(tracks, navigator, context, m_midiout);
        break;

    case MenuMode:
        channelManager = new CSurf_FP_8_Menu_Manager(tracks, navigator, context, m_midiout);
        break;
    }
}

CSurf_FP_8_FaderManager::CSurf_FP_8_FaderManager(
    CSurf_Context *context,
    CSurf_FP_8_Navigator *navigator,
    midi_Output *m_midiout) : m_midiout(m_midiout), navigator(navigator), context(context)
{
    trackButton = new CSurf_Button(BTN_TRACK, BTN_VALUE_ON, m_midiout);
    pluginsButton = new CSurf_Button(BTN_EDIT_PLUGINS, BTN_VALUE_OFF, m_midiout);
    sendButton = new CSurf_Button(BTN_SEND, BTN_VALUE_OFF, m_midiout);
    panButton = new CSurf_Button(BTN_PAN, BTN_VALUE_OFF, m_midiout);

    for (int i = 0; i < context->GetNbChannels(); i++)
    {
        CSurf_FP_8_Track *track = new CSurf_FP_8_Track(i, context, m_midiout);
        tracks.push_back(track);
    }

    channelManager = new CSurf_FP_8_TrackManager(tracks, navigator, context, m_midiout);
}

CSurf_FP_8_FaderManager::~CSurf_FP_8_FaderManager()
{
    delete channelManager;
    delete navigator;
    delete context;
    tracks.clear();
};

void CSurf_FP_8_FaderManager::HandleTrackButtonClick(int value)
{
    if (value == 0)
    {
        return;
    }

    context->SetPluginEditPluginId(-1);

    if (context->GetChannelMode() != TrackMode)
    {
        SetChannelMode(TrackMode, true);
    }
    else
    {
        context->ToggleShowTimeCode();
    }
};

void CSurf_FP_8_FaderManager::HandlePluginsButtonClick(int value, bool track)
{
    if (value == 0)
    {
        return;
    }

    context->SetPluginEditPluginId(-1);
    bool cameFromPlugin = (context->IsChannelMode(PluginEditMode) || context->IsChannelMode(PluginControlMode));

    if ((!context->IsChannelMode(PluginMode) && !track && !cameFromPlugin) || (prevChannelMode == PluginMode && cameFromPlugin))
    {
        SetChannelMode(PluginMode, true);
    }
    else
    {
        SetChannelMode(TrackPluginMode, true);
    }
};

void CSurf_FP_8_FaderManager::HandleSendButtonClick(int value, bool track)
{
    if (value == 0)
    {
        return;
    }

    context->SetPluginEditPluginId(-1);

    if (!context->IsChannelMode(SendMode) && !track)
    {
        SetChannelMode(SendMode, true);
    }
    else
    {
        SetChannelMode(TrackSendMode, true);
    }
};

void CSurf_FP_8_FaderManager::HandlePanButtonClick(int value, bool track)
{
    if (value == 0)
    {
        return;
    }

    context->SetPluginEditPluginId(-1);

    if (context->IsChannelMode(PanMode) && !track)
    {
        SetChannelMode(PanMode, true);
    }
    else
    {
        SetChannelMode(TrackPanMode, true);
    }
};

void CSurf_FP_8_FaderManager::HandleMixAllButtonClick()
{
    if (!context->IsChannelMode(MixMode))
    {
        prevChannelMode = context->GetChannelMode();
        SetChannelMode(MixMode);
    }
    else
    {
        ResetMixButtonClick();
    }
};

void CSurf_FP_8_FaderManager::ResetMixButtonClick()
{
    if (context->IsChannelMode(MixMode))
    {
        HandleTrackButtonClick(BTN_VALUE_ON);
    }
};

void CSurf_FP_8_FaderManager::HandleLinkButtonClick()
{
    if (context->IsChannelMode(PluginEditMode))
    {
        SetChannelMode(PluginControlMode);
    }
    else
    {
        prevChannelMode = context->GetChannelMode();
        SetChannelMode(PluginEditMode);
    }
};

void CSurf_FP_8_FaderManager::HandleTouchButtonClick()
{
    if (context->IsChannelMode(MenuMode))
    {
        ChannelMode mode = context->GetPreviousChannelMode();
        SetChannelMode(mode);
    }
    else
    {
        SetChannelMode(MenuMode);
    }
}

void CSurf_FP_8_FaderManager::SetPluginControlMode()
{
    SetChannelMode(PluginControlMode);
}

// ADD ALL THE TRACKMANAGERS METHODS HERE TO PROXY THEM
void CSurf_FP_8_FaderManager::UpdateTracks()
{
    channelManager->UpdateTracks();
}

void CSurf_FP_8_FaderManager::HandleMuteClick(int index, int value)
{
    channelManager->HandleMuteClick(index, value);
}

void CSurf_FP_8_FaderManager::HandleEncoderIncrement()
{
    if (context->IsChannelMode(MenuMode))
    {
        channelManager->HandleMuteClick(0, 0);
    }
}

void CSurf_FP_8_FaderManager::HandleEncoderDecrement()
{
    if (context->IsChannelMode(MenuMode))
    {
        channelManager->HandleSoloClick(0, 0);
    }
}

void CSurf_FP_8_FaderManager::HandleEncoderPush()
{
    if (context->IsChannelMode(MenuMode))
    {
        channelManager->HandleSelectClick(0);
    }
}

void CSurf_FP_8_FaderManager::HandleSoloClick(int index, int value)
{
    channelManager->HandleSoloClick(index, value);

    if (value == 0)
    {
        return;
    }

    prevChannelMode = context->GetChannelMode();

    if (context->GetPluginControl() &&
        (context->IsChannelMode(TrackPluginMode) || context->IsChannelMode(PluginMode)) &&
        (context->GetPluginEditPluginId() > -1))
    {
        if (hasPluginConfigFile(context->GetPluginEditTrack(), context->GetPluginEditPluginId()))
        {
            SetChannelMode(PluginControlMode);
        }
    }
}

void CSurf_FP_8_FaderManager::HandleSelectClick(int index, int value)
{
    if (value == 0)
    {
        return;
    }

    channelManager->HandleSelectClick(index);
    ResetMixButtonClick();
}

void CSurf_FP_8_FaderManager::HandleFaderMove(int index, int msb, int lsb)
{
    channelManager->HandleFaderMove(index, msb, lsb);
}

void CSurf_FP_8_FaderManager::HandleFaderTouch(int index, int value)
{
    if (value == 0)
    {
        return;
    }

    channelManager->HandleFaderTouch(index);
}
