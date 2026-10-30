#include "csurf_api.hpp"
#include <reaper_plugin_functions.h>
#include <string>
#include <vector>

void ReaSonusApi::SetPluginControl(const int track_index, const int plugin_index) const {
    MediaTrack *media_track = GetTrack(nullptr, track_index);

    // First clean up all open fx windows and then open the plugin in a floating window
    Main_OnCommandStringEx("_REASONUS_CLOSE_ALL_FLOATING_FX_WINDOWS_COMMAND", 0, nullptr);
    // SWS/S&M: Close all floating FX and chain windows
    TrackFX_Show(media_track, plugin_index, SHOW_FLAG_SHOW_WINDOW);

    context->SetPluginEditTrack(media_track);
    context->SetPluginEditPluginId(plugin_index);
    if (PluginUtils::hasPluginConfigFile(context->GetPluginEditTrack(), context->GetPluginEditPluginId())) {
        faderManager->ChangeChannelMode(PluginControlMode);
    }
}

void ReaSonusApi::HandlePluginControl() const {
    if (!DAW::GetExtState(EXT_STATE_FP_PLUGIN_CONTROL, "").empty()) {
        // Set the correct values to open the plugin control
        const std::vector<std::string> plugin_ids = split(
            DAW::GetExtState(EXT_STATE_FP_PLUGIN_CONTROL, ""),
            ":"
        );

        if (plugin_ids[0] == "true") {
            faderManager->ChangeChannelMode(PluginMode);
        } else {
            SetPluginControl(stoi(plugin_ids[0]), stoi(plugin_ids[1]));
        }
        DAW::SetExtState(EXT_STATE_FP_PLUGIN_CONTROL, "", false);
    }
}

void ReaSonusApi::HandleSendMode() const {
    if (!DAW::GetExtState(EXT_STATE_FP_SENDS_CONTROL, "").empty()) {
        if (DAW::GetExtState(EXT_STATE_FP_SENDS_CONTROL, "") == "true") {
            faderManager->ChangeChannelMode(SendMode);
        }
        DAW::SetExtState(EXT_STATE_FP_SENDS_CONTROL, "", false);
    }
}

void ReaSonusApi::HandlePanMode() const {
    if (!DAW::GetExtState(EXT_STATE_FP_PAN_CONTROL, "").empty()) {
        if (DAW::GetExtState(EXT_STATE_FP_PAN_CONTROL, "") == "true") {
            if (context->GetChannelMode() == PanMode1) {
                faderManager->ChangeChannelMode(PanMode2);
            } else {
                faderManager->ChangeChannelMode(PanMode1);
            }
        }
        DAW::SetExtState(EXT_STATE_FP_PAN_CONTROL, "", false);
    }
}

void ReaSonusApi::HandleFilterByName() const {
    if (!DAW::GetExtState(EXT_STATE_FP_FILTER_BY_NAME, "").empty()) {
        const std::string filter = DAW::GetExtState(EXT_STATE_FP_FILTER_BY_NAME, "");

        trackNavigator->HandleCustomFilterByName(filter);

        DAW::SetExtState(EXT_STATE_FP_FILTER_BY_NAME, "", false);
    }
}

void ReaSonusApi::HandleFilterByType() const {
    if (!DAW::GetExtState(EXT_STATE_FP_FILTER_BY_TYPE, "").empty()) {
        const std::string filter = DAW::GetExtState(EXT_STATE_FP_FILTER_BY_TYPE, "");

        trackNavigator->HandleFilter(static_cast<NavigatorFilter>(stoi(filter)));

        DAW::SetExtState(EXT_STATE_FP_FILTER_BY_TYPE, "", false);
    }
}

void ReaSonusApi::HandleSetFaderMode() const {
    if (!DAW::GetExtState(EXT_STATE_FP_FADER_MODE, "").empty()) {
        // Set the correct values to open the plugin control
        const std::vector<std::string> fader_mode_ids = split(
            DAW::GetExtState(EXT_STATE_FP_FADER_MODE, ""),
            ":"
        );

        if (fader_mode_ids[0] == "0") {
            faderManager->ChangeChannelMode(static_cast<ChannelMode>(0));
        } else if (stoi(fader_mode_ids[0]) > 0 && fader_mode_ids.size() > 1) {
            MediaTrack *media_track = GetTrack(nullptr, stoi(fader_mode_ids[1]));
            DAW::SetUniqueSelectedTrack(media_track);
            faderManager->ChangeChannelMode(static_cast<ChannelMode>(stoi(fader_mode_ids[0])));
        }
        DAW::SetExtState(EXT_STATE_FP_FADER_MODE, "", false);
    }
}

ReaSonusApi::ReaSonusApi(CSurf_Context *context) : context(context), faderManager(nullptr), trackNavigator(nullptr) {
}

ReaSonusApi::ReaSonusApi(
    CSurf_Context *context,
    CSurf_FP_8_FaderManager *faderManager,
    CSurf_FP_8_Navigator *trackNavigator
) : context(context), faderManager(faderManager), trackNavigator(trackNavigator) {
}

ReaSonusApi::~ReaSonusApi() {
}

void ReaSonusApi::HandleApi() const {
    HandlePluginControl();
    HandleSendMode();
    HandlePanMode();
    HandleFilterByName();
    HandleFilterByType();
    HandleSetFaderMode();
}
