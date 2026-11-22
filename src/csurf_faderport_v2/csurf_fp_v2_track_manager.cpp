#include "csurf_fp_v2_track_manager.hpp"

#include "csurf_fp_v2_pan_fader_manager.cpp"
#include "csurf_fp_v2_param_fader_manager.cpp"
#include "csurf_fp_v2_track_fader_manager.cpp"
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

bool CSurf_FP_V2_TrackManager::HasSinglePointAutomation(
    MediaTrack *media_track,
    const int value,
    const bool last_touched_param
) const {
    return (last_touched_param || GetTrackAutomationMode(media_track) == AUTOMATION_TRIM)
           && settings->UseAutomationSinglePoint()
           && (last_touched_param || context->IsSinglePointAutomationEnabled())
           && (value != 0 || single_point_automation != nullptr);
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
    if (single_point_automation == nullptr && value > 0) {
        switch (spa_type) {
            case SPA_Plugin: {
                single_point_automation = new SinglePointAutomationItem(
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
                single_point_automation = new SinglePointAutomationItem(
                    media_track,
                    chunk_name,
                    point_value,
                    spa_type
                );
        }
    }

    if (single_point_automation != nullptr && value > 0) {
        single_point_automation->InsertStartPoint(point_value);
        return;
    }

    if (single_point_automation != nullptr) {
        single_point_automation->InsertEndPoint(point_value, chunk_name);
    }

    single_point_automation = nullptr;
}

void CSurf_FP_V2_TrackManager::CheckFaderMode() {
    FaderModeV2 new_fader_mode = TrackFaderMode;

    if (context->GetLastTouchedFxMode()) {
        new_fader_mode = LastTouchedParamFaderMode;
    }

    if (context->GetPanFaderMode()) {
        new_fader_mode = PanFaderMode;
    }

    if (fader_mode == new_fader_mode) {
        return;
    }

    fader_mode = new_fader_mode;

    switch (fader_mode) {
        case TrackFaderMode:
            fader_manager = new CSurf_FP_V2_TrackFaderManager(navigator, context, track, m_midiout);
            break;

        case LastTouchedParamFaderMode:
            fader_manager = new CSurf_FP_V2_ParamFaderManager(navigator, context, track, m_midiout);
            break;

        case PanFaderMode:
            fader_manager = new CSurf_FP_V2_PanFaderManager(navigator, context, track, m_midiout);
            break;
    }
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
    previous_automation_mode = 0;
    fader_mode = TrackFaderMode;
    track = new CSurf_FP_V2_Track(context, m_midiout);
    fader_manager = new CSurf_FP_V2_TrackFaderManager(navigator, context, track, m_midiout);
}

void CSurf_FP_V2_TrackManager::UpdateTrack() {
    MediaTrack *media_track = navigator->GetControllerTrack();
    CheckFaderMode();

    if (context->GetMasterFaderMode()) {
        DAW::SetExtState(EXT_STATE_FP_V2_WIDGET_MODE, FP_V2_WIDGET_MASTER_MODE, false);
    } else if (context->GetLastTouchedFxMode()) {
        DAW::SetExtState(EXT_STATE_FP_V2_WIDGET_MODE, FP_V2_WIDGET_PARAM_MODE, false);
    } else {
        DAW::SetExtState(EXT_STATE_FP_V2_WIDGET_MODE, FP_V2_WIDGET_TRACK_MODE, false);
    }

    // int fader_value = 0;

    if (media_track == nullptr || CountTracks(nullptr) < 1) {
        ClearTrack();
        force_update = false;
        return;
    }

    const bool isArmed = DAW::IsTrackArmed(media_track);

    // GetFaderValue(media_track, &fader_value);

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

    fader_manager->UpdateTracks(force_update);

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
        if (now - mute_start > CSurf_Context::MOMENTARY_TIMEOUT) {
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
        if (now - solo_start > CSurf_Context::MOMENTARY_TIMEOUT) {
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

void CSurf_FP_V2_TrackManager::HandleFaderTouch(const int value) const {
    fader_manager->HandleFaderTouch(value);
}

void CSurf_FP_V2_TrackManager::HandleFaderMove(const int msb, const int lsb) const {
    fader_manager->HandleFaderMove(msb, lsb);
}
