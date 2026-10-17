#include "csurf_fp_v2_track_manager.hpp"
#include <fmt/format.h>
#include "../shared/csurf_daw.hpp"

constexpr int MOMENTARY_TIMEOUT = 500;

bool CSurf_FP_V2_TrackManager::HasSinglePointAutomation(
    MediaTrack *media_track,
    const int value
) const {
    return GetTrackAutomationMode(media_track) == AUTOMATION_TRIM
           && context->IsSinglePointAutomationEnabled()
           && (value != 0 || single_point_automation.at(0) != nullptr);
}

void CSurf_FP_V2_TrackManager::HandleSinglePointAutomation(
    MediaTrack *media_track,
    const int value,
    const std::string &chunk_name,
    const double point_value,
    const SinglePointAutomationType spa_type,
    const int item_index,
    const int sub_item_index
) {
    if (single_point_automation.at(0) == nullptr && value > 0) {
        switch (spa_type) {
            case SPA_Plugin: {
                single_point_automation.at(0) = new SinglePointAutomationItem(
                    media_track,
                    chunk_name,
                    point_value,
                    spa_type,
                    item_index,
                    sub_item_index
                );
                break;
            }

            default:
                single_point_automation.at(0) = new SinglePointAutomationItem(
                    media_track,
                    chunk_name,
                    point_value,
                    spa_type
                );
        }
    }

    if (single_point_automation.at(0) != nullptr && value > 0) {
        single_point_automation.at(0)->InsertStartPoint(point_value);
        return;
    }

    if (single_point_automation.at(0) != nullptr) {
        single_point_automation.at(0)->InsertEndPoint(point_value, chunk_name);
    }

    single_point_automation.at(0) = nullptr;
}

void CSurf_FP_V2_TrackManager::GetFaderValue(MediaTrack *media_track, int *faderValue) const {
    double volume = 0.0;
    double pan1 = 0.0;

    if (context->GetMasterFaderMode()) {
        media_track = GetMasterTrack(nullptr);
    }

    GetTrackUIVolPan(media_track, &volume, &pan1);

    if (context->GetShiftLeft()) {
        *faderValue = static_cast<int>(panToNormalized(pan1) * 16383.0);
    } else {
        *faderValue = static_cast<int>(volToNormalized(volume) * 16383.0);
    }
}

CSurf_FP_V2_TrackManager::CSurf_FP_V2_TrackManager(
    CSurf_Context *context,
    CSurf_FP_V2_Navigator *navigator,
    midi_Output *m_midiout
) : navigator(navigator), context(context), m_midiout(m_midiout), color() {
    track = new CSurf_FP_V2_Track(context, m_midiout);
}

void CSurf_FP_V2_TrackManager::UpdateTrack() {
    MediaTrack *media_track = navigator->GetControllerTrack();

    int fader_value = 0;

    if (media_track == nullptr || CountTracks(nullptr) < 1) {
        ClearTrack();
        force_update = false;
        return;
    }

    const bool isArmed = DAW::IsTrackArmed(media_track);

    GetFaderValue(media_track, &fader_value);

    if (context->GetShiftLeft()) {
        track->SetMuteButtonValue(navigator->HasTracksWithMute() ? BTN_VALUE_ON : BTN_VALUE_OFF, force_update);
        track->SetSoloButtonValue(navigator->HasTracksWithSolo() ? BTN_VALUE_ON : BTN_VALUE_OFF, force_update);
    } else {
        track->SetMuteButtonValue(DAW::IsTrackMuted(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, force_update);
        track->SetSoloButtonValue(DAW::IsTrackSoloed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, force_update);
    }

    track->SetArmButtonValue(navigator->HasAllArmedTracks()
                                 ? BTN_VALUE_BLINK
                                 : isArmed // NOLINT(*-avoid-nested-conditional-operator)
                                       ? BTN_VALUE_ON
                                       : BTN_VALUE_OFF,
                             force_update);
    track->SetBypassButtonValue(DAW::GetTrackFxBypassed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);

    if (context->GetLastTouchedFxMode()) {
        double param_value = 0.0;
        int track_number = -1;
        int plugin_index = -1;
        int param_index = -1;

        GetLastTouchedFX(&track_number, &plugin_index, &param_index);
        if (MediaTrack *_media_track = GetTrack(nullptr, track_number - 1)) {
            param_value = TrackFX_GetParamNormalized(_media_track, plugin_index, param_index);
            track->SetFaderValue(static_cast<int>(param_value * 16383.0), force_update);
        }
    } else {
        if (!context->GetFaderDisabled() || !settings->GetCanDisableFader()) {
            track->SetFaderValue(fader_value, force_update);
        }
    }

    hasLastTouchedFxEnabled = context->GetLastTouchedFxMode();
    force_update = false;
}

void CSurf_FP_V2_TrackManager::ClearTrack() const {
    track->SetMuteButtonValue(BTN_VALUE_OFF);
    track->SetSoloButtonValue(BTN_VALUE_OFF);
    track->SetArmButtonValue(BTN_VALUE_OFF);

    track->SetFaderValue(0, force_update);
}

// ReSharper disable once CppParameterMayBeConst
void CSurf_FP_V2_TrackManager::HandleMuteClick(int index, const int value) {
    (void) index;

    if (context->GetShiftLeft()) {
        if (value == 0) {
            return;
        }

        Main_OnCommandAsyncEx(40339, 0, nullptr); // Track: Unmute all tracks
        return;
    }

    const int now = static_cast<int>(GetTickCount());
    MediaTrack *media_track = navigator->GetControllerTrack();

    if (value == 0 && settings->GetMuteSoloMomentary()) {
        if (now - mute_start > MOMENTARY_TIMEOUT) {
            CSurf_SetSurfaceMute(
                media_track,
                CSurf_OnMuteChange(media_track, static_cast<int>(!DAW::IsTrackMuted(media_track))),
                nullptr
            );
        }
        mute_start = 0;
    } else if (value > 0) {
        mute_start = now;
        CSurf_SetSurfaceMute(
            media_track,
            CSurf_OnMuteChange(media_track, static_cast<int>(!DAW::IsTrackMuted(media_track))),
            nullptr
        );
    }
}

// ReSharper disable once CppParameterMayBeConst
void CSurf_FP_V2_TrackManager::HandleSoloClick(int index, const int value) {
    (void) index;
    if (context->GetShiftLeft()) {
        if (value == 0) {
            return;
        }
        Main_OnCommandAsyncEx(40340, 0, nullptr); // Track: Un solo all tracks
        return;
    }

    const int now = static_cast<int>(GetTickCount());
    MediaTrack *media_track = navigator->GetControllerTrack();

    if (value == 0 && settings->GetMuteSoloMomentary()) {
        if (now - solo_start > MOMENTARY_TIMEOUT) {
            CSurf_SetSurfaceSolo(
                media_track,
                CSurf_OnSoloChange(media_track, static_cast<int>(!DAW::IsTrackSoloed(media_track))),
                nullptr
            );
        }
        solo_start = 0;
    } else if (value > 0) {
        solo_start = now;
        CSurf_SetSurfaceSolo(
            media_track,
            CSurf_OnSoloChange(media_track, static_cast<int>(!DAW::IsTrackSoloed(media_track))),
            nullptr
        );
    }
}

// ReSharper disable once CppParameterMayBeConst
void CSurf_FP_V2_TrackManager::HandleArmClick(int index, const int value) const {
    (void) index;
    if (value == 0) {
        return;
    }

    MediaTrack *media_track = navigator->GetControllerTrack();

    if (context->GetShiftLeft()) {
        if (navigator->HasAllArmedTracks()) {
            Main_OnCommandAsyncEx(40491, 0, nullptr); // Track: Unarm all tracks for recording
        } else {
            Main_OnCommandAsyncEx(40490, 0, nullptr); // Track: Arm all tracks for recording
        }
        return;
    }

    CSurf_SetSurfaceRecArm(
        media_track,
        CSurf_OnRecArmChange(media_track, static_cast<int>(!DAW::IsTrackArmed(media_track))),
        nullptr
    );
}

// ReSharper disable once CppParameterMayBeConst
void CSurf_FP_V2_TrackManager::HandleBypassClick(int index, const int value) const {
    (void) index;
    if (value == 0) {
        return;
    }

    MediaTrack *media_track = navigator->GetControllerTrack();

    context->GetShiftLeft()
        ? Main_OnCommandAsyncEx(40344, 0, nullptr) // Track: Toggle FX bypass on all tracks
        : DAW::ToggleTrackFxBypass(media_track);   // Track: Toggle FX bypass for selected tracks
}

// void CSurf_FP_V2_TrackManager::HandleFaderTouch(const int value) const {
//     if (value > 0) {
//         return;
//     }
//     if (context->GetLastTouchedFxMode()) {
//         int track_number = -1;
//         int plugin_index = -1;
//         int param_index = -1;
//
//         // Tell REAPER we do not alter the automatiuon anymore
//         if (GetLastTouchedFX(&track_number, &plugin_index, &param_index)) {
//             if (MediaTrack *media_track = GetTrack(nullptr, track_number - 1)) {
//                 TrackFX_EndParamEdit(media_track, plugin_index, param_index);
//             }
//         }
//     }
// }
//
void CSurf_FP_V2_TrackManager::HandleFaderTouch(const int value) {
    if (!context->GetLastTouchedFxMode()) {
        return;
    }

    int track_number = -1;
    int plugin_index = -1;
    int plugin_param_index = -1;

    if (!GetLastTouchedFX(&track_number, &plugin_index, &plugin_param_index)) {
        return;
    }

    MediaTrack *media_track = GetTrack(nullptr, track_number - 1);
    if (media_track == nullptr) {
        return;
    }

    /**
     * Check if we have the proper conditions to continue with Single point automation
     * All pan related values (except for width) are displayed reversed.
     * Therefore some fo the values are inverted as well, to make it mork as it should
     */
    if (HasSinglePointAutomation(media_track, value)) {
        double parameter_value = 0.0;
        double dummy;

        // Get the value to write for the automation
        if (value > 0) {
            // We need to set it to read first to get the actual volume of the envelope
            SetTrackAutomationMode(media_track, AUTOMATION_READ);
            parameter_value = TrackFX_GetParam(media_track, track_number, plugin_param_index, &dummy, &dummy);
            SetTrackAutomationMode(media_track, AUTOMATION_TRIM);
        } else {
            parameter_value = TrackFX_GetParam(media_track, track_number, plugin_param_index, &dummy, &dummy);
        }

        HandleSinglePointAutomation(
            media_track,
            value,
            fmt::format("{}_{}_{}", 0, track_number, plugin_param_index),
            parameter_value,
            SPA_Plugin,
            track_number,
            plugin_param_index
        );
    } else {
        if (value > 0 || media_track == nullptr) {
            return;
        }
        // Tell REAPER we do not alter the automatiuon anymore
        TrackFX_EndParamEdit(media_track, track_number, plugin_param_index);
    }
}

void CSurf_FP_V2_TrackManager::HandleFaderMove(const int msb, const int lsb) const {
    if (context->GetLastTouchedFxMode()) {
        int track_number = -1;
        int plugin_index = -1;
        int param_index = -1;

        if (GetLastTouchedFX(&track_number, &plugin_index, &param_index)) {
            if (MediaTrack *media_track = GetTrack(nullptr, track_number - 1)) {
                TrackFX_SetParamNormalized(media_track, plugin_index, param_index, int14ToNormalized(msb, lsb));
            }
        }
        return;
    }

    MediaTrack *media_track = navigator->GetControllerTrack();

    if (context->GetShiftLeft()) {
        CSurf_SetSurfacePan(
            media_track,
            CSurf_OnPanChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false),
            nullptr
        );
    } else {
        CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), nullptr);
    }
}
