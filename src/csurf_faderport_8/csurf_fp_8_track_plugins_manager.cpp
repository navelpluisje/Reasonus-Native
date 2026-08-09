#ifndef CSURF_FP_8_TRACK_PLUGINS_MANAGER_C_
#define CSURF_FP_8_TRACK_PLUGINS_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"
#include "../shared/csurf_plugin_utils.hpp"

class CSurf_FP_8_TrackPluginsManager : public CSurf_FP_8_ChannelManager {
    int current_plugin = 0;

protected:
    void GetFaderValue(MediaTrack *media_track, int *fader_value, int *valuebar_value) const { // NOLINT(*-convert-member-functions-to-static)
        int panMode = 0;
        double volume = 0.0;
        double pan1 = 0.0;
        double pan2 = 0.0;

        GetTrackUIVolPan(media_track, &volume, &pan1);
        GetTrackUIPan(media_track, &pan1, &pan2, &panMode);

        *fader_value = static_cast<int>(volToNormalized(volume) * 16383.0);
        *valuebar_value = static_cast<int>(panToNormalized(pan1) * 127);
    }

    [[nodiscard]] std::string GetBypassedText(const bool bypassed) const { // NOLINT(*-convert-member-functions-to-static)
        return bypassed ? "Bypassed" : "Enabled";
    }

public:
    CSurf_FP_8_TrackPluginsManager(
        const std::vector<CSurf_FP_8_Track *> &tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout
    ) {
        context->ResetChannelManagerItemIndex();
        context->ResetChannelManagerItemsCount();
        CSurf_FP_8_TrackPluginsManager::UpdateTracks(true);
    }

    ~CSurf_FP_8_TrackPluginsManager() override = default;

    void UpdateTracks(const bool force_update) override {
        current_plugin = context->GetChannelManagerItemIndex();

        const WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *plugin_track = GetSelectedTrack(nullptr, 0);
        if (plugin_track == nullptr
            && context->GetMasterFaderMode()
            && DAW::IsTrackSelected(GetMasterTrack(nullptr))
        ) {
            plugin_track = GetMasterTrack(nullptr);
        }
        context->SetChannelManagerItemsCount(TrackFX_GetCount(plugin_track));

        for (int i = 0; i < context->GetNbChannels(); i++) {
            MediaTrack *media_track;
            const int plugin_index = context->GetChannelManagerItemIndex() + i;
            int fader_value = 0;
            int valuebar_value = 0;

            const CSurf_FP_8_Track *track = tracks.at(i);
            if (context->GetMasterFaderMode() && i == context->GetNbChannels() - 1) {
                media_track = GetMasterTrack(nullptr);
            } else {
                media_track = media_tracks.Get(i);
            }

            SetTrackColors(media_track, DAW::IsTrackSelected(media_track), false);
            GetFaderValue(media_track, &fader_value, &valuebar_value);

            if (media_track == nullptr) {
                track->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT);
            } else {
                track->SetDisplayLine(
                    0,
                    ALIGN_LEFT,
                    DAW::GetTrackName(media_track).c_str(),
                    plugin_track == media_track ? INVERT : NON_INVERT,
                    force_update
                );
            }

            if (DAW::HasTrackFx(plugin_track, plugin_index)) {
                track->SetDisplayLine(
                    1,
                    ALIGN_LEFT,
                    DAW::GetTrackFxName(plugin_track, plugin_index, false).c_str(),
                    INVERT,
                    force_update
                );
                track->SetDisplayLine(
                    2,
                    ALIGN_CENTER,
                    DAW::GetTrackFxSurfaceEnabled(plugin_track, plugin_index).c_str(),
                    NON_INVERT,
                    force_update
                );
                track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, force_update);
                track->SetMuteButtonValue(
                    ButtonBlinkOnOff(
                        context->GetShiftChannelLeft() && !DAW::GetTrackFxEnabled(plugin_track, plugin_index),
                        !DAW::GetTrackFxEnabled(plugin_track, plugin_index),
                        settings->GetDistractionFreeMode()),
                    force_update);
                track->SetSoloButtonValue(
                    ButtonBlinkOnOff(
                        DAW::GetTrackFxPanelOpen(plugin_track, plugin_index),
                        PluginUtils::hasPluginConfigFile(plugin_track, plugin_index),
                        settings->GetDistractionFreeMode()),
                    force_update);
            } else {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Fx", INVERT, force_update);
                track->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, force_update);
                track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, force_update);
                track->SetMuteButtonValue(BTN_VALUE_OFF, force_update);
                track->SetSoloButtonValue(BTN_VALUE_OFF, force_update);
            }

            track->SetTrackColor(color, force_update);
            track->SetSelectButtonValue(BTN_VALUE_ON, force_update);
            track->SetFaderValue(fader_value, force_update);
            track->SetValueBarMode(VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(valuebar_value);

            track->SetDisplayMode(DISPLAY_MODE_2, force_update);
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
                CSurf_OnRecArmChange(media_track, static_cast<int>(!DAW::IsTrackArmed(media_track))),
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

        MediaTrack *media_track = GetSelectedTrack(nullptr, 0);
        if (media_track == nullptr
            && context->GetMasterFaderMode()
            && DAW::IsTrackSelected(GetMasterTrack(nullptr))
        ) {
            media_track = GetMasterTrack(nullptr);
        }

        const int plugin_index = context->GetChannelManagerItemIndex() + index;

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

        MediaTrack *media_track = GetSelectedTrack(nullptr, 0);
        if (media_track == nullptr
            && context->GetMasterFaderMode()
            && DAW::IsTrackSelected(GetMasterTrack(nullptr))
        ) {
            media_track = GetMasterTrack(nullptr);
        }

        const int plugin_index = context->GetChannelManagerItemIndex() + index;

        if (DAW::GetTrackFxPanelOpen(media_track, plugin_index)) {
            TrackFX_Show(media_track, plugin_index, 0);
            TrackFX_Show(media_track, plugin_index, 2);
            context->SetPluginEditTrack(nullptr);
            context->SetPluginEditPluginId(-1);
        } else {
            // First clean up all open fx windows and then open the plugin in a floating window
            Main_OnCommandStringEx("_REASONUS_CLOSE_ALL_FLOATING_FX_WINDOWS_COMMAND", 0, nullptr);
            // SWS/S&M: Close all floating FX and chain windows
            TrackFX_Show(media_track, plugin_index, 3);
            context->SetPluginEditTrack(media_track);
            context->SetPluginEditPluginId(plugin_index);
        }
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        // Because it is the fx navigation, the fader will only change the channels volume
        CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), nullptr);
    }
};

#endif
