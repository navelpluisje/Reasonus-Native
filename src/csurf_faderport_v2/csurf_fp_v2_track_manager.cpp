#include "csurf_fp_v2_track_manager.hpp"
#include "../shared/csurf_daw.hpp"

constexpr int MOMENTARY_TIMEOUT = 500;

void CSurf_FP_V2_TrackManager::GetFaderValue(MediaTrack *media_track, int *faderValue) const {
    double volume = 0.0;
    double pan1 = 0.0;

    if (context->GetMasterFaderMode())
    {
        media_track = GetMasterTrack(nullptr);
    }

    GetTrackUIVolPan(media_track, &volume, &pan1);

    if (context->GetShiftLeft())
    {
        *faderValue = static_cast<int>(panToNormalized(pan1) * 16383.0);
    }
    else
    {
        *faderValue = static_cast<int>(volToNormalized(volume) * 16383.0);
    }
}

CSurf_FP_V2_TrackManager::CSurf_FP_V2_TrackManager(
    CSurf_Context *context,
    CSurf_FP_V2_Navigator *navigator,
    midi_Output *m_midiout) : navigator(navigator), context(context), m_midiout(m_midiout), color() {
    track = new CSurf_FP_V2_Track(context, m_midiout);
}

void CSurf_FP_V2_TrackManager::UpdateTrack()
{
    MediaTrack *media_track = navigator->GetControllerTrack();

    int faderValue = 0;

    if (media_track == nullptr || CountTracks(nullptr) < 1)
    {
        ClearTrack();
        forceUpdate = false;
        return;
    }

    const bool isArmed = DAW::IsTrackArmed(media_track);

    GetFaderValue(media_track, &faderValue);

    if (context->GetShiftLeft())
    {
        track->SetMuteButtonValue(navigator->HasTracksWithMute() ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
        track->SetSoloButtonValue(navigator->HasTracksWithSolo() ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
    }
    else
    {
        track->SetMuteButtonValue(DAW::IsTrackMuted(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
        track->SetSoloButtonValue(DAW::IsTrackSoloed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
    }

    track->SetArmButtonValue(navigator->HasAllArmedTracks()
                                 ? BTN_VALUE_BLINK
                             : isArmed // NOLINT(*-avoid-nested-conditional-operator)
                                 ? BTN_VALUE_ON
                                 : BTN_VALUE_OFF,
                             forceUpdate);
    track->SetBypassButtonValue(DAW::GetTrackFxBypassed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);

    if (context->GetLastTouchedFxMode())
    {
        double paramValue = 0.0;
        int trackNumber = -1;
        int fxNumber = -1;
        int paramNumber = -1;

        GetLastTouchedFX(&trackNumber, &fxNumber, &paramNumber);
        if (MediaTrack *_media_track = GetTrack(nullptr, trackNumber - 1))
        {
            paramValue = TrackFX_GetParamNormalized(_media_track, fxNumber, paramNumber);
            track->SetFaderValue(static_cast<int>(paramValue * 16383.0), forceUpdate);
        }
    }
    else
    {
        if (!context->GetFaderDisabled() || !settings->GetCanDisableFader())
        {
            track->SetFaderValue(faderValue, forceUpdate);
        }
    }

    hasLastTouchedFxEnabled = context->GetLastTouchedFxMode();
    forceUpdate = false;
}

void CSurf_FP_V2_TrackManager::ClearTrack() const {
    track->SetMuteButtonValue(BTN_VALUE_OFF);
    track->SetSoloButtonValue(BTN_VALUE_OFF);
    track->SetArmButtonValue(BTN_VALUE_OFF);

    track->SetFaderValue(0, forceUpdate);
}

// ReSharper disable once CppParameterMayBeConst
void CSurf_FP_V2_TrackManager::HandleMuteClick(int index, const int value)
{
    (void)index;

    if (context->GetShiftLeft())
    {
        if (value == 0)
        {
            return;
        }

        Main_OnCommandAsyncEx(40339, 0, nullptr); // Track: Unmute all tracks
        return;
    }

    const int now = static_cast<int>(GetTickCount());
    MediaTrack *media_track = navigator->GetControllerTrack();

    if (value == 0 && settings->GetMuteSoloMomentary())
    {
        if (now - mute_start > MOMENTARY_TIMEOUT)
        {
            CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, static_cast<int>(!DAW::IsTrackMuted(media_track))), nullptr);
        }
        mute_start = 0;
    }
    else if (value > 0)
    {
        mute_start = now;
        CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, static_cast<int>(!DAW::IsTrackMuted(media_track))), nullptr);
    }
}

// ReSharper disable once CppParameterMayBeConst
void CSurf_FP_V2_TrackManager::HandleSoloClick(int index, const int value)
{
    (void)index;
    if (context->GetShiftLeft())
    {
        if (value == 0)
        {
            return;
        }
        Main_OnCommandAsyncEx(40340, 0, nullptr); // Track: Un solo all tracks
        return;
    }

    const int now = static_cast<int>(GetTickCount());
    MediaTrack *media_track = navigator->GetControllerTrack();

    if (value == 0 && settings->GetMuteSoloMomentary())
    {
        if (now - solo_start > MOMENTARY_TIMEOUT)
        {
            CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, static_cast<int>(!DAW::IsTrackSoloed(media_track))), nullptr);
        }
        solo_start = 0;
    }
    else if (value > 0)
    {
        solo_start = now;
        CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, static_cast<int>(!DAW::IsTrackSoloed(media_track))), nullptr);
    }
}

// ReSharper disable once CppParameterMayBeConst
void CSurf_FP_V2_TrackManager::HandleArmClick(int index, const int value) const {
    (void)index;
    if (value == 0)
    {
        return;
    }

    MediaTrack *media_track = navigator->GetControllerTrack();

    if (context->GetShiftLeft())
    {
        if (navigator->HasAllArmedTracks())
        {
            Main_OnCommandAsyncEx(40491, 0, nullptr); // Track: Unarm all tracks for recording
        }
        else
        {
            Main_OnCommandAsyncEx(40490, 0, nullptr); // Track: Arm all tracks for recording
        }
        return;
    }

    CSurf_SetSurfaceRecArm(media_track, CSurf_OnRecArmChange(media_track, static_cast<int>(!DAW::IsTrackArmed(media_track))), nullptr);
}

// ReSharper disable once CppParameterMayBeConst
void CSurf_FP_V2_TrackManager::HandleBypassClick(int index, const int value) const {
    (void)index;
    if (value == 0)
    {
        return;
    }

    MediaTrack *media_track = navigator->GetControllerTrack();

    context->GetShiftLeft() ? Main_OnCommandAsyncEx(40344, 0, nullptr)     // Track: Toggle FX bypass on all tracks
                            : DAW::ToggleTrackFxBypass(media_track); // Track: Toggle FX bypass for selected tracks
}

void CSurf_FP_V2_TrackManager::HandleFaderMove(const int msb, const int lsb) const {
    if (context->GetLastTouchedFxMode())
    {
        int trackNumber = -1;
        int fxNumber = -1;
        int paramNumber = -1;

        if (GetLastTouchedFX(&trackNumber, &fxNumber, &paramNumber))
        {
            if (MediaTrack *media_track = GetTrack(nullptr, trackNumber - 1))
            {
                TrackFX_SetParamNormalized(media_track, fxNumber, paramNumber, int14ToNormalized(msb, lsb));
            }
        }
        return;
    }

    MediaTrack *media_track = navigator->GetControllerTrack();

    if (context->GetShiftLeft())
    {
        CSurf_SetSurfacePan(media_track, CSurf_OnPanChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false), nullptr);
    }
    else
    {
        CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), nullptr);
    }
}
