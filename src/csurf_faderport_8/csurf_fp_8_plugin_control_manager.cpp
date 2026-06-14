#ifndef CSURF_FP_8_PLUGIN_CONTROL_MANAGER_C_
#define CSURF_FP_8_PLUGIN_CONTROL_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"
#include "../shared/csurf_plugin_utils.hpp"

class CSurf_FP_8_PluginControlManager : public CSurf_FP_8_ChannelManager {
protected:
    mINI::INIStructure ini;
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_8);
    std::string fileName;
    int updateCount;
    bool editStepSize;
    int stepSize;

    [[nodiscard]] std::string getParamKey(const std::string &prefix, const int index) const { // NOLINT(*-convert-member-functions-to-static)
        return prefix + std::to_string(index);
    }

    void GetCurrentPlugin() {
        MediaTrack *media_track = context->GetPluginEditTrack();
        const int plugin_id = context->GetPluginEditPluginId();
        const std::string plugin_type = DAW::GetTrackFxType(media_track, plugin_id);
        const std::string plugin_name = DAW::GetTrackFxName(media_track, plugin_id, false);
        const std::string developer_name = DAW::GetTrackFxDeveloper(media_track, plugin_id);
        fileName = PluginUtils::GetReaSonusPluginMappingFilePath(developer_name, plugin_name, plugin_type, false);

        const mINI::INIFile file(fileName);
        if (!file.read(ini)) {
            ini["Global"];
            ini["Global"]["origName"] = plugin_name;
            ini["Global"]["name"] = plugin_name;
            ini["Global"]["type"] = plugin_type;
            ini["Global"]["developer"] = developer_name;
            (void) file.generate(ini, true);
        }
    }

    void SaveIniFile() {
        const mINI::INIFile file(fileName);
        file.write(ini, true);
    }

public:
    CSurf_FP_8_PluginControlManager(
        const std::vector<CSurf_FP_8_Track *> &tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout
    ) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout) {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerItemsCount(CSurf_Context::GetPluginMaxGroupCount());
        GetCurrentPlugin();
        CSurf_FP_8_PluginControlManager::UpdateTracks(true);
        color = ButtonColorWhiteDim;
        editStepSize = false;
    }

    ~CSurf_FP_8_PluginControlManager() override = default;

    void UpdateTracks(const bool force_update) override {
        MediaTrack *media_track = context->GetPluginEditTrack();
        const int plugin_id = context->GetPluginEditPluginId();

        for (int i = 0; i < context->GetNbChannels(); i++) {
            const bool display_step_size = editStepSize && i == 0;
            const int filter_index = context->GetChannelManagerItemIndex() + i;
            std::string param_key = getParamKey("Select_", filter_index);
            const CSurf_FP_8_Track *track = tracks.at(i);

            const auto track_color = ini[param_key].has("color")
                                         ? stoi(ini[param_key]["color"])
                                         : 0x00ffffff;

            track->SetTrackColor(track_color, true);

            // If the track is armed always blink as an indication it is armed
            track->SetSelectButtonValue(BTN_VALUE_OFF, force_update);
            track->SetMuteButtonValue(BTN_VALUE_OFF, force_update);
            track->SetSoloButtonValue(BTN_VALUE_OFF, force_update);

            track->SetDisplayMode(DISPLAY_MODE_2, force_update);

            if (ini.has(param_key) && !ini[param_key]["param"].empty()) {
                const Inverted inverted = ini[param_key].has("uninvert-label")
                                          && ini[param_key]["uninvert-label"] == "1"
                                              ? NON_INVERT
                                              : INVERT;

                track->SetDisplayLine(0, ALIGN_CENTER, ini[param_key]["name"].c_str(), inverted, force_update);
                const std::string formatted_param_value = DAW::GetTrackFxFormattedParamValue(
                    media_track,
                    plugin_id,
                    stoi(ini[param_key]["param"])
                );
                track->SetSelectButtonValue(BTN_VALUE_ON, force_update);
                track->SetDisplayLine(1, ALIGN_CENTER, formatted_param_value.c_str(), NON_INVERT, force_update);
            } else {
                track->SetDisplayLine(0, ALIGN_CENTER, "", NON_INVERT, force_update);
                track->SetDisplayLine(1, ALIGN_CENTER, "", NON_INVERT, true);
                track->SetSelectButtonValue(BTN_VALUE_OFF, force_update);
            }

            param_key = getParamKey("Fader_", filter_index);
            if (ini.has(param_key) && !ini[param_key]["param"].empty()) {
                if (!display_step_size) {
                    const Inverted inverted = ini[param_key].has("uninvert-label")
                                              && ini[param_key]["uninvert-label"] == "1"
                                                  ? NON_INVERT
                                                  : INVERT;

                    track->SetDisplayLine(2, ALIGN_CENTER, ini[param_key]["name"].c_str(), inverted, force_update);
                    const std::string formatted_param_value = DAW::GetTrackFxFormattedParamValue(
                        media_track,
                        plugin_id,
                        stoi(ini[param_key]["param"])
                    );
                    track->SetDisplayLine(3, ALIGN_CENTER, formatted_param_value.c_str(), NON_INVERT, force_update);
                }

                const double value = TrackFX_GetParamNormalized(media_track, plugin_id, stoi(ini[param_key]["param"]));
                track->SetFaderValue(static_cast<int>(value * 16383.0), force_update);
                track->SetValueBarMode(VALUEBAR_MODE_FILL);
                track->SetValueBarValue(static_cast<int>(value * 127.0));
            } else {
                if (!display_step_size) {
                    track->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, force_update);
                    track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, true);
                }
                track->SetFaderValue(0, force_update);
                track->SetValueBarMode(VALUEBAR_MODE_OFF);
                track->SetValueBarValue(0);
            }

            if (display_step_size) {
                track->SetDisplayLine(2, ALIGN_CENTER, "Step size", INVERT, force_update);
                track->SetDisplayLine(3, ALIGN_CENTER, std::to_string(stepSize).c_str(), INVERT, force_update);
            }
        }
    }

    void HandleEndcoderPush(const int value) override {
        (void) value;
        if (!editStepSize) {
            stepSize = stoi(settings->GetSetting("surface", "plugin-step-size", "1"));
            context->SetPanEncoderMode(PanEncoderPluginStepSizeMode);
        } else {
            settings->SetAndSaveSetting("surface", "plugin-step-size", std::to_string(stepSize));
            context->SetPanEncoderMode(PanEncoderPluginControlMode);
        }
        editStepSize = !editStepSize;
    }

    void HandleEndcoderIncrement(const int value) override {
        (void) value;
        stepSize = minmax(1, stepSize + 1, context->GetNbChannels());
    }

    void HandleEndcoderDecrement(const int value) override {
        (void) value;
        stepSize = minmax(1, stepSize - 1, context->GetNbChannels());
    }

    void HandleSelectClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        const int control_index = context->GetChannelManagerItemIndex() + index;
        MediaTrack *media_track = context->GetPluginEditTrack();
        const int plugin_id = context->GetPluginEditPluginId();
        const std::string param_key = getParamKey("Select_", control_index);

        if (ini.has(param_key) && !ini[param_key]["param"].empty()) {
            const int param_id = stoi(ini[param_key]["param"]);

            int nb_steps = stoi(ini[param_key]["steps"]);
            if (nb_steps == 1) {
                nb_steps = 2;
            }
            const double param_value = TrackFX_GetParamNormalized(media_track, plugin_id, param_id);
            const double step_size = 1.0 / (nb_steps - 1);
            double new_value = static_cast<int>((param_value + step_size) * 100);

            if (new_value > 100) {
                new_value = 0;
            }

            TrackFX_SetParamNormalized(media_track, plugin_id, param_id, new_value / 100);
        }
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
        const int control_index = context->GetChannelManagerItemIndex() + index;
        const int plugin_id = context->GetPluginEditPluginId();

        const std::string param_key = getParamKey("Fader_", control_index);

        if (ini.has(param_key)) {
            MediaTrack *media_track = context->GetPluginEditTrack();
            if (value > 0 || media_track == nullptr) {
                return;
            }

            TrackFX_EndParamEdit(media_track, plugin_id, stoi(ini[param_key]["param"]));
        }
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        const int control_index = context->GetChannelManagerItemIndex() + index;
        const int plugin_id = context->GetPluginEditPluginId();

        const std::string param_key = getParamKey("Fader_", control_index);

        if (ini.has(param_key)) {
            MediaTrack *media_track = context->GetPluginEditTrack();
            const double value = int14ToNormalized(msb, lsb);

            TrackFX_SetParamNormalized(media_track, plugin_id, stoi(ini[param_key]["param"]), value);
        }
    }
};

#endif
