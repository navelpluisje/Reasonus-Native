#ifndef CSURF_FP_8_TRACK_MANAGER_C_
#define CSURF_FP_8_TRACK_MANAGER_C_

#include <array>
#include "csurf_fp_8_channel_manager.hpp"

extern const int MOMENTARY_TIMEOUT;

class CSurf_FP_8_TrackManager : public CSurf_FP_8_ChannelManager {
    int mute_start[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int solo_start[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int touch_start[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    bool has_last_touched_fx_enabled = false;
    bool has_touch_mode = false;

protected:
    void GetFaderValue(
        MediaTrack *media_track,
        int *faderValue,
        int *valueBarValue,
        std::string *_pan1,
        std::string *_pan2
    ) const {
        int pan_mode = 0;
        double volume = 0.0;
        double pan1 = 0.0;
        double pan2 = 0.0;

        GetTrackUIVolPan(media_track, &volume, &pan1);
        GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);
        if (pan_mode < PAN_MODE_STEREO_PAN) {
            pan_mode = PAN_MODE_BALANCE_PAN;
        }

        *_pan1 = GetPan1String(pan1, pan_mode);
        if (pan_mode != PAN_MODE_BALANCE_PAN) {
            *_pan2 = GetPan2String(pan2, pan_mode);
        } else {
            *_pan2 = *_pan1;
        }

        // get the valuebar value to determine the value to display
        int valuebar_value;
        switch (stoi(context->GetSetting("displays", "track-value-bar-value"))) {
            case VALUEBAR_VALUE_VOLUME:
                valuebar_value = volToNormalized(volume);
                break;
            case VALUEBAR_VALUE_PAN_1:
                valuebar_value = panToNormalized(pan1);
                break;
            case VALUEBAR_VALUE_PAN_2:
                valuebar_value = panToNormalized(pan2);
                break;
            default:
                valuebar_value = volToNormalized(volume);
        }

        *faderValue = static_cast<int>(volToNormalized(volume) * 16383.0);
        *valueBarValue = valuebar_value * 127;
    }

    static std::string GetDisplayLineValue(MediaTrack *media_track, const DisplayValue display_value) {
        double pan1 = 0.0;
        double pan2 = 0.0;
        int pan_mode;

        switch (display_value) {
            case DISPLAY_VALUE_NAME:
                return DAW::GetTrackName(media_track);

            case DISPLAY_VALUE_VOLUME:
                return GetVolumeString(DAW::GetTrackVolume(media_track));

            case DISPLAY_VALUE_PAN_1:
                GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);
                return GetPan1String(pan1, pan_mode);

            case DISPLAY_VALUE_PAN_2:
                GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);
                return GetPan2String(pan2, pan_mode);

            case DISPLAY_VALUE_TRACK_NB:
                return DAW::GetTrackIndex(media_track);

            case DISPLAY_VALUE_PHASE:
                return GetPhaseString(DAW::GetTrackPhase(media_track));

            case DISPLAY_VALUE_ARMED:
                return GetArmedString(DAW::IsTrackArmed(media_track));

            case DISPLAY_VALUE_AUTOMATION:
                return GetAutomationString(DAW::GetTrackAutomationMode(media_track));

            case DISPLAY_VALUE_FX_STATE:
                return GetTrackFXString(!DAW::GetTrackFxBypassed(media_track));

            default:
                return "";
        }
    }

public:
    CSurf_FP_8_TrackManager(
        const std::vector<CSurf_FP_8_Track *> &tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout) {
        CSurf_FP_8_TrackManager::UpdateTracks(true);
    }

    ~CSurf_FP_8_TrackManager() override = default;


    void UpdateTracks(bool force_update) override {
        const WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        std::vector<std::string> time_code;
        const auto valuebar_mode = static_cast<ValuebarMode>(
            stoi(context->GetSetting("displays", "track-value-bar-mode"))
        );
        const std::array<int, 4> display_line_values = context->GetDisplayLineValues();
        const std::array<int, 4> display_align_values = context->GetDisplayAlignValues();
        const std::array<int, 4> display_invert_values = context->GetDisplayInvertValues();

        if (has_last_touched_fx_enabled != context->GetLastTouchedFxMode()) {
            force_update = true;
        }

        if (context->GetShowTimeCode()) {
            time_code = DAW::GetProjectTime(settings->GetOverwriteTimeCode(), settings->GetSurfaceTimeCode());
        }

        for (int i = 0; i < context->GetNbChannels(); i++) {
            MediaTrack *media_track;
            bool is_master_track = false;
            int fader_value = 0;
            int valuebar_value = 0;
            std::string strPan1;
            std::string strPan2;

            CSurf_FP_8_Track *track = tracks.at(i);
            if (context->GetMasterFaderMode() && i == context->GetNbChannels() - 1) {
                media_track = GetMasterTrack(nullptr);
                is_master_track = true;
            } else {
                media_track = media_tracks.Get(i);
            }

            if (media_track == nullptr || (CountTracks(nullptr) < i && !is_master_track)) {
                const int index = context->GetNbChannels() - (static_cast<int>(time_code.size()) + i);

                if (index < 1) {
                    track->ClearTrack(false);
                    track->SetDisplayMode(DISPLAY_MODE_0, force_update);
                    track->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT, force_update);
                    track->SetDisplayLine(1, ALIGN_CENTER, "", NON_INVERT, force_update);
                    track->SetDisplayLine(2, ALIGN_CENTER, time_code.at(abs(index)).c_str(), INVERT, force_update);
                } else {
                    track->ClearTrack(true, force_update);
                }
                continue;
            }

            const bool track_selected = DAW::IsTrackSelected(media_track);
            const bool is_armed = DAW::IsTrackArmed(media_track);

            GetFaderValue(media_track, &fader_value, &valuebar_value, &strPan1, &strPan2);
            const bool is_selected = (context->GetArm() && is_armed) || (!context->GetArm() && track_selected);

            SetTrackColors(media_track, is_selected, true);
            track->SetTrackColor(color);
            // If the track is armed always blink as an indication it is armed
            track->SetSelectButtonValue(
                !context->GetArm() && is_armed && !settings->GetDistractionFreeMode()
                    ? BTN_VALUE_BLINK
                    : BTN_VALUE_ON,
                force_update
            );
            track->SetMuteButtonValue(DAW::IsTrackMuted(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, force_update);
            track->SetSoloButtonValue(DAW::IsTrackSoloed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, force_update);

            track->SetFaderValue(fader_value, force_update || has_touch_mode);

            // Get value bar type and value type
            track->SetValueBarMode(context->GetArm() ? VALUEBAR_MODE_FILL : valuebar_mode);
            track->SetValueBarValue(valuebar_value);

            if (is_master_track) {
                track->SetDisplayMode(DISPLAY_MODE_2, force_update);
                track->SetDisplayLine(
                    DISPLAY_LINE_1,
                    ALIGN_CENTER,
                    DAW::GetTrackName(media_track).c_str(),
                    NON_INVERT,
                    force_update
                );
                track->SetDisplayLine(
                    DISPLAY_LINE_2,
                    ALIGN_CENTER,
                    DAW::GetTrackIndex(media_track).c_str(),
                    NON_INVERT,
                    force_update
                );
                track->SetDisplayLine(
                    DISPLAY_LINE_3,
                    ALIGN_CENTER,
                    strPan1.c_str(),
                    NON_INVERT,
                    force_update
                );
                track->SetDisplayLine(
                    DISPLAY_LINE_4,
                    ALIGN_CENTER,
                    strPan2.c_str(),
                    NON_INVERT,
                    force_update
                );
            } else if (context->GetArm() && is_armed) {
                track->SetDisplayMode(DISPLAY_MODE_2, force_update);
                track->SetDisplayLine(
                    DISPLAY_LINE_1,
                    ALIGN_CENTER,
                    DAW::GetTrackName(media_track).c_str(),
                    NON_INVERT,
                    force_update
                );
                track->SetDisplayLine(
                    DISPLAY_LINE_2,
                    ALIGN_CENTER,
                    DAW::GetTrackInputName(media_track).c_str(),
                    NON_INVERT,
                    force_update
                );
                track->SetDisplayLine(
                    DISPLAY_LINE_3,
                    ALIGN_CENTER,
                    DAW::GetTrackMonitorMode(media_track).c_str(),
                    NON_INVERT,
                    force_update
                );
                track->SetDisplayLine(
                    DISPLAY_LINE_4,
                    ALIGN_CENTER,
                    DAW::GetTrackRecordingMode(media_track).c_str(),
                    NON_INVERT,
                    force_update
                );
            } else {
                track->SetVuMeterValue(DAW::GetTrackSurfacePeakInfo(media_track), true);
                const int index = context->GetNbBankChannels() - (static_cast<int>(time_code.size()) + i);

                if (index < 1) {
                    track->SetDisplayMode(DISPLAY_MODE_0, force_update);
                    track->SetDisplayLine(
                        DISPLAY_LINE_1,
                        ALIGN_LEFT,
                        DAW::GetTrackName(media_track).c_str(),
                        NON_INVERT,
                        force_update
                    );
                    track->SetDisplayLine(
                        DISPLAY_LINE_2,
                        ALIGN_CENTER,
                        DAW::GetTrackIndex(media_track).c_str(),
                        NON_INVERT,
                        force_update
                    );
                    track->SetDisplayLine(
                        DISPLAY_LINE_3, ALIGN_CENTER,
                        time_code.at(abs(index)).c_str(),
                        INVERT,
                        force_update
                    );
                } else {
                    // Just the reular track layout.
                    // Get display mode and display line options.
                    track->SetDisplayMode(static_cast<DisplayMode>(settings->GetTrackDisplay()), force_update);
                    track->SetDisplayLine(
                        DISPLAY_LINE_1,
                        static_cast<Alignment>(display_align_values[DISPLAY_LINE_1]),
                        GetDisplayLineValue(
                            media_track,
                            static_cast<DisplayValue>(display_line_values[DISPLAY_LINE_1])
                        ).c_str(),
                        // DAW::GetTrackName(media_track).c_str(),
                        static_cast<Inverted>(display_invert_values[DISPLAY_LINE_1]),
                        force_update
                    );
                    track->SetDisplayLine(
                        DISPLAY_LINE_2,
                        static_cast<Alignment>(display_align_values[DISPLAY_LINE_2]),
                        GetDisplayLineValue(
                            media_track,
                            static_cast<DisplayValue>(display_line_values[DISPLAY_LINE_2])
                        ).c_str(),
                        static_cast<Inverted>(display_invert_values[DISPLAY_LINE_2]),
                        force_update
                    );
                    track->SetDisplayLine(
                        DISPLAY_LINE_3,
                        static_cast<Alignment>(display_align_values[DISPLAY_LINE_3]),
                        GetDisplayLineValue(
                            media_track,
                            static_cast<DisplayValue>(display_line_values[DISPLAY_LINE_3])
                        ).c_str(),
                        static_cast<Inverted>(display_invert_values[DISPLAY_LINE_3]),
                        force_update
                    );
                    track->SetDisplayLine(
                        DISPLAY_LINE_4,
                        static_cast<Alignment>(display_align_values[DISPLAY_LINE_4]),
                        GetDisplayLineValue(
                            media_track,
                            static_cast<DisplayValue>(display_line_values[DISPLAY_LINE_4])
                        ).c_str(),
                        static_cast<Inverted>(display_invert_values[DISPLAY_LINE_4]),
                        force_update
                    );
                }
            }
        }

        has_last_touched_fx_enabled = context->GetLastTouchedFxMode();
    }

    void HandleSelectClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetArm()) {
            if (DAW::IsTrackParent(media_track)) {
                return;
            }
            DAW::ToggleTrackArmed(media_track);
            return;
        }

        if (context->GetShiftChannelRight()) {
            DAW::SetSelectedTracksRange(media_track);
            return;
        }

        if (context->GetShiftChannelLeft()) {
            DAW::ToggleTrackSelected(media_track);
            return;
        }

        DAW::SetUniqueSelectedTrack(media_track);
    }

    void HandleMuteClick(const int index, const int value) override {
        const int now = GetTickCount();
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (value == 0 && settings->GetMuteSoloMomentary()) {
            if (now - mute_start[index] > MOMENTARY_TIMEOUT) {
                DAW::ToggleTrackMuted(media_track);
            }
            mute_start[index] = 0;
        } else if (value > 0) {
            mute_start[index] = now;
            DAW::ToggleTrackMuted(media_track);
        }
    }

    void HandleSoloClick(const int index, const int value) override {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetShiftChannelLeft()) {
            if (value != 0) {
                DAW::SetTrackSoloUnique(media_track);
            }
            return;
        }

        const int now = GetTickCount();

        if (value == 0 && settings->GetMuteSoloMomentary()) {
            if (now - solo_start[index] > MOMENTARY_TIMEOUT) {
                DAW::ToggleTrackSoloed(media_track);
            }
            solo_start[index] = 0;
        } else if (value > 0) {
            solo_start[index] = now;
            DAW::ToggleTrackSoloed(media_track);
        }
    }

    void HandleFaderTouch(const int index, const int value) override {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetShiftChannelLeft() && settings->GetFaderReset()) {
            DAW::SetTrackVolume(media_track, 1.0);
        }

        if (GetTrackAutomationMode(media_track) == AUTOMATION_TOUCH) {
            touch_start[index] = value > 0 ? 1 : 0;
            if (touch_start[index] > 0) {
                DAW::SetTrackVolume(media_track, DAW::GetTrackVolume(media_track));
            }
        }
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        DAW::SetTrackVolume(media_track, int14ToVol(msb, lsb));
    }
};

#endif
