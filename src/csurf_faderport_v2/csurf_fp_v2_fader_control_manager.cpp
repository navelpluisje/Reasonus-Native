#include "csurf_fp_v2_fader_control_manager.hpp"
#include "../shared/csurf_plugin_utils.hpp"

void CSurf_FP_V2_FaderControlManager::SetChannelMode(const ChannelMode channelMode) {
    context->SetChannelMode(channelMode);
    if (updateButtons) {
        SetButtonValues(true);
    }

    switch (channelMode) {
        case TrackMode:
            context->SetPanEncoderMode(PanEncoderTrackPanMode);
            context->ResetPanPushMode();
            context->SetShowTimeCode(false);
            SetTracks();
            channelManager = new CSurf_FP_8_TrackManager(tracks, navigator, context, m_midiout);
            break;

        case PluginMode:
            context->SetPanEncoderMode(PanEncoderPluginMode);
            context->ResetPanPushMode();
            context->SetLastTouchedFxMode(false);
            SetTracks();
            channelManager = new CSurf_FP_8_PluginsManager(tracks, navigator, context, m_midiout);
            break;

        case TrackPluginMode:
            context->SetPanEncoderMode(PanEncoderPluginMode);
            context->ResetPanPushMode();
            SetTracks();
            channelManager = new CSurf_FP_8_TrackPluginsManager(tracks, navigator, context, m_midiout);
            break;

        case SendMode:
            context->SetPanEncoderMode(PanEncoderSendMode);
            SetTracks();
            channelManager = new CSurf_FP_8_SendsManager(tracks, navigator, context, m_midiout);
            break;

        case TrackSendMode:
            context->SetPanEncoderMode(PanEncoderSendMode);
            SetTracks();
            channelManager = new CSurf_FP_8_TrackSendsManager(tracks, navigator, context, m_midiout);
            break;

        case ReceiveMode:
            context->SetPanEncoderMode(PanEncoderReceiveMode);
            SetTracks();
            channelManager = new CSurf_FP_8_ReceivesManager(tracks, navigator, context, m_midiout);
            break;

        case TrackReceiveMode:
            context->SetPanEncoderMode(PanEncoderReceiveMode);
            SetTracks();
            channelManager = new CSurf_FP_8_TrackReceivesManager(tracks, navigator, context, m_midiout);
            break;

        case PanMode1:
        case PanMode2:
            context->SetPanEncoderMode(PanEncoderPanMode);
            SetTracks();
            channelManager = new CSurf_FP_8_PanManager(tracks, navigator, context, m_midiout);
            break;

        case MixMode:
            context->SetPanEncoderMode(PanEncoderMixMode);
            SetTracks();
            channelManager = new CSurf_FP_8_FilterManager(tracks, navigator, context, m_midiout);
            break;

        case PluginControlMode:
            context->SetPanEncoderMode(PanEncoderPluginControlMode);
            context->SetLastTouchedFxMode(false);
            SetTracks();
            channelManager = new CSurf_FP_8_PluginControlManager(tracks, navigator, context, m_midiout);
            break;

        case PluginEditMode:
            context->SetPanEncoderMode(PanEncoderPluginEditMode);
            context->SetLastTouchedFxMode(false);
            SetTracks();
            channelManager = new CSurf_FP_8_PluginLearnManager(tracks, navigator, context, m_midiout);
            break;

        case MenuMode:
            context->SetPanEncoderMode(PanEncoderMenuMode);
            SetTracks();
            channelManager = new CSurf_FP_8_Menu_Manager(tracks, navigator, context, m_midiout);
            break;
    }
}

void CSurf_FP_V2_FaderControlManager::SetTracks() {
    for (int i = 0; i < context->GetNbChannels(); i++) {
        auto track = new CSurf_FP_8_Track(i, context, m_midiout);
        tracks.push_back(track);
    }
}

CSurf_FP_V2_FaderControlManager::CSurf_FP_V2_FaderControlManager(
    CSurf_Context *context,
    CSurf_FP_8_Navigator *navigator,
    midi_Output *m_midiout) : m_midiout(m_midiout), navigator(navigator), context(context) {
    trackButton = new CSurf_Button(BTN_TRACK, BTN_VALUE_ON, m_midiout);
    pluginsButton = new CSurf_Button(BTN_EDIT_PLUGINS, BTN_VALUE_OFF, m_midiout);
    sendButton = new CSurf_Button(BTN_SEND, BTN_VALUE_OFF, m_midiout);
    panButton = new CSurf_Button(BTN_PAN, BTN_VALUE_OFF, m_midiout);

    SetTracks();

    channelManager = new CSurf_FP_8_TrackManager(tracks, navigator, context, m_midiout);
}

CSurf_FP_V2_FaderControlManager::~CSurf_FP_V2_FaderControlManager() {
    delete channelManager;
    delete navigator;
    delete context;
    tracks.clear();
}

void CSurf_FP_V2_FaderControlManager::Refresh(const bool force) {
    SetChannelMode(TrackMode, true);
}

// ADD ALL THE TRACKMANAGERS METHODS HERE TO PROXY THEM
void CSurf_FP_V2_FaderControlManager::UpdateTracks(const bool force_update) const {
    if (force_update) {
        SetButtonValues(force_update);
    }

    channelManager->UpdateTracks(force_update);
}

void CSurf_FP_V2_FaderControlManager::HandleFaderMove(const int index, const int msb, const int lsb) const {
    channelManager->HandleFaderMove(index, msb, lsb);
}

void CSurf_FP_V2_FaderControlManager::HandleFaderTouch(const int index, const int value) const {
    channelManager->HandleFaderTouch(index, value);
}
