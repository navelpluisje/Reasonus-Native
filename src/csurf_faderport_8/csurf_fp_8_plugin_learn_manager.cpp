#ifndef CSURF_FP_8_PLUGIN_LEARN_MANAGER_C_
#define CSURF_FP_8_PLUGIN_LEARN_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"
#include "../ui/windows/csurf_ui_fp_8_control_panel.hpp"
#include "../shared/csurf_faderport_ui_imgui_utils.hpp"
#include "../shared/csurf_plugin_utils.hpp"

class CSurf_FP_8_PluginLearnManager : public CSurf_FP_8_ChannelManager {
    mINI::INIStructure plugin_mapping_ini;
    std::string file_name;
    int updateCount;

protected:
    [[nodiscard]] std::string getParamKey(const std::string &prefix, const int index) const {
        const std::string controlIndex = std::to_string(context->GetChannelManagerItemIndex() + index);
        return prefix + controlIndex;
    }

    void GetCurrentPlugin() {
        const int pluginId = context->GetPluginEditPluginId();
        MediaTrack *media_track = context->GetPluginEditTrack();

        const std::string orig_plugin_name = DAW::GetOrigTrackFxName(media_track, pluginId);
        PluginUtils::ReadCreatePluginMappingFileByOrigPluginName(orig_plugin_name, plugin_mapping_ini);

        file_name = PluginUtils::GetReaSonusPluginMappingFilePath(
            plugin_mapping_ini["Global"]["developer"],
            plugin_mapping_ini["Global"]["name"],
            plugin_mapping_ini["Global"]["type"],
            true
        );
    }

    void SaveIniFile() {
        const mINI::INIFile file(file_name);
        file.write(plugin_mapping_ini, true);
    }

public:
    CSurf_FP_8_PluginLearnManager(
        const std::vector<CSurf_FP_8_Track *> &tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout) {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerItemsCount(CSurf_Context::GetPluginMaxGroupCount());
        GetCurrentPlugin();
        CSurf_FP_8_PluginLearnManager::UpdateTracks(true);
        color = ButtonColorWhiteDim;
    }

    ~CSurf_FP_8_PluginLearnManager() override {
    }

    void UpdateTracks(const bool force_update) override {
        int trackId, itemNumber, takeId, pluginId, paramId;
        if (GetTouchedOrFocusedFX(0, &trackId, &itemNumber, &takeId, &pluginId, &paramId)) {
            context->SetPluginEditParamId(paramId);
        }

        if (updateCount > 100) {
            const mINI::INIFile file(file_name);
            file.read(plugin_mapping_ini);
        }
        updateCount += 1;

        for (int i = 0; i < context->GetNbChannels(); i++) {
            const int paramIndex = context->GetChannelManagerItemIndex() + i;
            const CSurf_FP_8_Track *track = tracks.at(i);
            track->SetTrackColor(color);

            // If the track is armed always blink as an indication it is armed
            track->SetSelectButtonValue(BTN_VALUE_OFF, force_update);
            track->SetMuteButtonValue(BTN_VALUE_OFF, force_update);
            track->SetSoloButtonValue(BTN_VALUE_OFF, force_update);
            track->SetFaderValue(0, force_update);
            track->SetValueBarMode(VALUEBAR_MODE_NORMAL);
            track->SetValueBarValue(0);

            track->SetDisplayMode(DISPLAY_MODE_2, force_update);

            std::string paramKey = getParamKey("Select_", paramIndex);

            if (plugin_mapping_ini.has(paramKey)) {
                track->SetDisplayLine(0, ALIGN_CENTER, plugin_mapping_ini[paramKey]["name"].c_str(), INVERT,
                                      force_update);
            } else {
                track->SetDisplayLine(0, ALIGN_CENTER, "Free", INVERT, force_update);
            }

            paramKey = getParamKey("Fader_", paramIndex);
            if (plugin_mapping_ini.has(paramKey)) {
                track->SetDisplayLine(2, ALIGN_CENTER, plugin_mapping_ini[paramKey]["name"].c_str(), INVERT,
                                      force_update);
            } else {
                track->SetDisplayLine(2, ALIGN_CENTER, "Free", INVERT, force_update);
            }

            track->SetDisplayLine(1, ALIGN_CENTER, "", NON_INVERT, true);
            track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, true);
        }
    }

    void OpenMappingUi(const int plugin_id, const int channel) const {
        MediaTrack *media_track = context->GetPluginEditTrack();
        const std::string plugin_name = DAW::GetTrackFxName(media_track, plugin_id, false);
        const std::string developer_name = DAW::GetTrackFxDeveloper(media_track, plugin_id);

        if (!ReaSonus8ControlPanel::control_panel_open) {
            ToggleFP8ControlPanel(ReaSonus8ControlPanel::MAPPING_PAGE);
        } else if (ReaSonus8ControlPanel::current_page != ReaSonus8ControlPanel::MAPPING_PAGE) {
            ReaSonus8ControlPanel::SetCurrentPage(ReaSonus8ControlPanel::MAPPING_PAGE);
        } else if (
            ReaSonus8ControlPanel::GetPageStringProperty(0).compare(developer_name) == 0 &&
            ReaSonus8ControlPanel::GetPageStringProperty(1).compare(plugin_name + ".ini") == 0 &&
            ReaSonus8ControlPanel::GetPageProperty(0) == channel) {
            ToggleFP8ControlPanel(ReaSonus8ControlPanel::MAPPING_PAGE);
        }

        if (ReaSonus8ControlPanel::control_panel_open) {
            ReaSonus8ControlPanel::SetPageStringProperty(0, developer_name);
            ReaSonus8ControlPanel::SetPageStringProperty(1, plugin_name + ".ini");
            ReaSonus8ControlPanel::SetPageProperty(0, channel);
        }
    }

    void HandleSelectClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }
        const int controlIndex = context->GetChannelManagerItemIndex() + index;
        const int pluginId = context->GetPluginEditPluginId();
        int trackId, itemNumber, takeId, _pluginId, paramId;

        const std::string paramKey = getParamKey("Select_", controlIndex);

        if (context->GetShiftLeft()) {
            OpenMappingUi(pluginId, controlIndex);
        } else if (context->GetShiftRight()) {
            if (plugin_mapping_ini.has(paramKey)) {
                plugin_mapping_ini.remove(paramKey);
                SaveIniFile();
            }
        } else {
            if (!GetTouchedOrFocusedFX(0, &trackId, &itemNumber, &takeId, &_pluginId, &paramId)) {
                return;
            }

            context->SetPluginEditParamId(paramId);

            MediaTrack *media_track = context->GetPluginEditTrack();
            const std::string paramName = DAW::GetTrackFxParamName(media_track, pluginId, paramId);
            const int nbSteps = DAW::GetTrackFxParamNbSteps(media_track, pluginId, paramId);

            if (!plugin_mapping_ini.has(paramKey)) {
                plugin_mapping_ini[paramKey];
            }
            plugin_mapping_ini[paramKey]["origName"] = paramName;
            plugin_mapping_ini[paramKey]["name"] = paramName;
            plugin_mapping_ini[paramKey]["param"] = std::to_string(paramId);
            plugin_mapping_ini[paramKey]["steps"] = std::to_string(nbSteps);

            SaveIniFile();
            if (settings->GetUntouchAfterLearn()) {
                DAW::SetTrackFXParamUntouched(media_track, pluginId);
            }
        }
        // Open the dialog
    }

    void HandleMuteClick(const int index, const int value) override {
        (void) index;
        (void) value;
        // If in edit mode
        // Edit the current assignment
        // Else do nothing
    }

    void HandleSoloClick(const int index, const int value) override {
        (void) index;
        (void) value;
    }

    void HandleFaderTouch(const int index, const int value) override {
        (void) value;
        const int controlIndex = context->GetChannelManagerItemIndex() + index;
        const int pluginId = context->GetPluginEditPluginId();
        int trackId, itemNumber, takeId, _pluginId, paramId;

        const std::string paramKey = getParamKey("Fader_", controlIndex);

        if (context->GetShiftLeft()) {
            OpenMappingUi(pluginId, controlIndex);
        } else if (context->GetShiftRight()) {
            if (plugin_mapping_ini.has(paramKey)) {
                plugin_mapping_ini.remove(paramKey);
                SaveIniFile();
            }
        } else {
            if (!GetTouchedOrFocusedFX(0, &trackId, &itemNumber, &takeId, &_pluginId, &paramId)) {
                return;
            }

            context->SetPluginEditParamId(paramId);

            MediaTrack *media_track = context->GetPluginEditTrack();
            const std::string paramName = DAW::GetTrackFxParamName(media_track, pluginId, paramId);

            if (!plugin_mapping_ini.has(paramKey)) {
                plugin_mapping_ini[paramKey];
            }

            plugin_mapping_ini[paramKey]["origName"] = paramName;
            plugin_mapping_ini[paramKey]["name"] = paramName;
            plugin_mapping_ini[paramKey]["param"] = std::to_string(paramId);

            SaveIniFile();
            if (settings->GetUntouchAfterLearn()) {
                DAW::SetTrackFXParamUntouched(media_track, pluginId);
            }
        }
    }
};

#endif
