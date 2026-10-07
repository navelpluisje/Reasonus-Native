#ifndef CSURF_FP_8_PLUGINS_MANAGER_C_
#define CSURF_FP_8_PLUGINS_MANAGER_C_

#include <algorithm>

#include "csurf_fp_8_channel_manager.hpp"
#include "../shared/csurf_plugin_utils.hpp"

class CSurf_FP_8_PluginsManager : public CSurf_FP_8_ChannelManager {
    int nb_plugins = 0;
    int current_plugin = 0;

protected:
    void GetFaderValue(MediaTrack *media_track, int *fader_value, int *value_bar_value) const { // NOLINT(*-convert-member-functions-to-static)
        int panMode = 0;
        double volume = 0.0;
        double pan1 = 0.0;
        double pan2 = 0.0;

        GetTrackUIVolPan(media_track, &volume, &pan1);
        GetTrackUIPan(media_track, &pan1, &pan2, &panMode);

        *fader_value = static_cast<int>(volToNormalized(volume) * 16383.0);
        *value_bar_value = static_cast<int>(panToNormalized(pan1) * 127);
    }

    [[nodiscard]] std::string GetBypassedText(const bool bypassed) const { // NOLINT(*-convert-member-functions-to-static)
        return bypassed ? "Bypassed" : "Enabled";
    }

    [[nodiscard]] int GetPluginIndex(const int index) const {
        const bool control_input_plugins = settings->HasPluginInputControl() && context->GetArm();

        return context->GetChannelManagerItemIndex(nb_track_items[index] - 1) + (
                   control_input_plugins ? 0x1000000 : 0
               );
    }

    std::string GetLine4Content(
        const int slot_index,
        const int display_index
    ) const {
        const bool respect_slots = settings->SendsShouldRespectSlots();
        if (!respect_slots) {
            if (nb_track_items[display_index] == 0) {
                return "";
            }
            return Progress(slot_index + 1, nb_track_items[display_index]);
        }
        if (display_index < context->GetNbChannels() - 2) {
            return "";
        }
        if (display_index == context->GetNbChannels() - 2) {
            return "Slot:";
        }

        return Progress(slot_index + 1, nb_plugins);
    }

public:
    CSurf_FP_8_PluginsManager(
        const std::vector<CSurf_FP_8_Track *> &tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout) {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerType(Hui);
        CSurf_FP_8_PluginsManager::UpdateTracks(true);
    }

    ~CSurf_FP_8_PluginsManager() override = default;

    void UpdateTracks(const bool force_update) override {
        nb_plugins = 0;
        const bool control_input_plugins = settings->HasPluginInputControl() && context->GetArm();
        const WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        const bool respect_slots = settings->PluginsShouldRespectSlots();

        for (int i = 0; i < context->GetNbChannels(); i++) {
            MediaTrack *media_track;

            if (context->GetMasterFaderMode() && i == context->GetNbChannels() - 1) {
                media_track = GetMasterTrack(nullptr);
            } else {
                media_track = media_tracks.Get(i);
            }

            const int _nb_track_plugins = control_input_plugins
                                              ? TrackFX_GetRecCount(media_track)
                                              : DAW::GetTrackFxCount(media_track, respect_slots);

            nb_track_items[i] = _nb_track_plugins;
            nb_plugins = std::max(_nb_track_plugins, nb_plugins);
        }

        context->SetChannelManagerItemsCount(nb_plugins);
        current_plugin = context->GetChannelManagerItemIndex();

        for (int i = 0; i < context->GetNbChannels(); i++) {
            MediaTrack *media_track;
            const CSurf_FP_8_Track *faderport_channel = tracks.at(i);

            int fader_value = 0;
            int value_bar_value = 0;
            if (context->GetMasterFaderMode() && i == context->GetNbChannels() - 1) {
                media_track = GetMasterTrack(nullptr);
            } else {
                media_track = media_tracks.Get(i);
            }

            const int slot_index = context->GetChannelManagerItemIndex(
                respect_slots ? nb_plugins : nb_track_items[i] - 1
            );

            if (media_track == nullptr) {
                faderport_channel->ClearTrack(true, force_update);

                if (i >= context->GetNbChannels() - 2) {
                    faderport_channel->SetDisplayMode(DISPLAY_MODE_2, force_update);
                    faderport_channel->SetDisplayLine(
                        DISPLAY_LINE_4,
                        ALIGN_CENTER,
                        GetLine4Content(slot_index, i).c_str(),
                        NON_INVERT,
                        force_update
                    );
                }
                continue;
            }

            const int plugin_index = respect_slots
                                         ? DAW::GetTrackFxIndexBySlotIndex(media_track, slot_index)
                                         : slot_index;
            SetTrackColors(media_track, DAW::IsTrackSelected(media_track), false);
            GetFaderValue(media_track, &fader_value, &value_bar_value);

            if (DAW::HasTrackFx(media_track, plugin_index)) {
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_1,
                    ALIGN_LEFT,
                    DAW::GetTrackName(media_track).c_str(),
                    NON_INVERT,
                    force_update
                );
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_2,
                    ALIGN_LEFT,
                    DAW::GetTrackFxName(media_track, plugin_index, false).c_str(),
                    INVERT,
                    force_update
                );
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_3,
                    ALIGN_CENTER,
                    DAW::GetTrackFxSurfaceEnabled(media_track, plugin_index).c_str(),
                    NON_INVERT,
                    force_update
                );
                faderport_channel->SetMuteButtonValue(
                    ButtonBlinkOnOff(
                        context->GetShiftChannelLeft() && !DAW::GetTrackFxEnabled(media_track, plugin_index),
                        !DAW::GetTrackFxEnabled(media_track, plugin_index),
                        settings->GetDistractionFreeMode()));
                faderport_channel->SetSoloButtonValue(
                    ButtonBlinkOnOff(
                        DAW::GetTrackFxPanelOpen(media_track, plugin_index),
                        PluginUtils::hasPluginConfigFile(media_track, plugin_index),
                        settings->GetDistractionFreeMode()));
            } else {
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_1,
                    ALIGN_LEFT,
                    DAW::GetTrackName(media_track).c_str(), NON_INVERT,
                    force_update
                );
                faderport_channel->SetDisplayLine(DISPLAY_LINE_2, ALIGN_LEFT, "", NON_INVERT, force_update);
                faderport_channel->SetDisplayLine(DISPLAY_LINE_3, ALIGN_CENTER, "", NON_INVERT, force_update);
                faderport_channel->SetMuteButtonValue(BTN_VALUE_OFF, force_update);
                faderport_channel->SetSoloButtonValue(BTN_VALUE_OFF, force_update);
            }

            faderport_channel->SetDisplayLine(
                DISPLAY_LINE_4,
                ALIGN_CENTER,
                GetLine4Content(slot_index, i).c_str(),
                NON_INVERT,
                force_update
            );

            faderport_channel->SetTrackColor(color, force_update);
            faderport_channel->SetSelectButtonValue(BTN_VALUE_ON, force_update);
            faderport_channel->SetFaderValue(fader_value, force_update);
            faderport_channel->SetValueBarMode(VALUEBAR_MODE_BIPOLAR);
            faderport_channel->SetValueBarValue(value_bar_value);

            faderport_channel->SetDisplayMode(DISPLAY_MODE_2, force_update);
        }
    }

    void HandleSelectClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetArm()) {
            CSurf_SetSurfaceRecArm(
                media_track,
                CSurf_OnRecArmChange(media_track, !DAW::IsTrackArmed(media_track)),
                nullptr
            );
            return;
        }

        DAW::SetUniqueSelectedTrack(media_track);
    }

    void HandleMuteClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        const int plugin_index = GetPluginIndex(index);

        if (context->GetShiftChannelLeft()) {
            TrackFX_SetOffline(media_track, plugin_index, !DAW::GetTrackFxOffline(media_track, plugin_index));
        } else {
            TrackFX_SetEnabled(media_track, plugin_index, !DAW::GetTrackFxEnabled(media_track, plugin_index));
        }
    }

    void HandleSoloClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }
        const bool respect_slots = settings->PluginsShouldRespectSlots();

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        const int slot_index = context->GetChannelManagerItemIndex(nb_track_items[index] - 1);
        const int plugin_index = respect_slots
                                     ? DAW::GetTrackFxIndexBySlotIndex(media_track, slot_index)
                                     : slot_index;

        // If the current plugin window is open, close it
        // Otherwise Close all other open windows and open the plugin window

        if (DAW::GetTrackFxPanelOpen(media_track, plugin_index)) {
            TrackFX_Show(media_track, plugin_index, 0);
            TrackFX_Show(media_track, plugin_index, 2);
            context->SetPluginEditTrack(nullptr);
            context->SetPluginEditPluginId(-1);
        } else {
            // First clean up all open fx windows and then open the plugin in a floating window
            Main_OnCommandStringEx("_REASONUS_CLOSE_ALL_FLOATING_FX_WINDOWS_COMMAND", 0, nullptr);
            // SWS/S&M: Close all floating FX and chanin windows
            TrackFX_Show(media_track, plugin_index, 3);
            context->SetPluginEditTrack(media_track);
            context->SetPluginEditPluginId(plugin_index);
            // We need to check if the plugin is available and then set the correct manager
        }
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        // Because it is the fx navigation, the fader will only change the channels volume
        CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), nullptr);
    }
};

#endif
