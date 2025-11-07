#include <vector>
#include "../shared/csurf_context.cpp"
#include "../shared/csurf_daw.hpp"
#include "../shared/csurf_utils.hpp"
#include "csurf_fp_v2_track_manager.hpp"
#include "csurf_fp_v2_navigator.hpp"

const int MOMENTARY_TIMEOUT = 500;

void CSurf_FP_V2_TrackManager::GetFaderValue(MediaTrack *media_track, int *faderValue)
{
    double volume, pan1 = 0.0;

    if (context->GetMasterFaderMode())
    {
        media_track = ::GetMasterTrack(0);
    }

    GetTrackUIVolPan(media_track, &volume, &pan1);

    if (context->GetShiftLeft())
    {
        *faderValue = int(panToNormalized(pan1) * 16383.0);
    }
    else
    {
        *faderValue = int(volToNormalized(volume) * 16383.0);
    }
}

CSurf_FP_V2_TrackManager::CSurf_FP_V2_TrackManager(
    CSurf_Context *context,
    CSurf_FP_V2_Navigator *navigator,
    midi_Output *m_midiout) : navigator(navigator), context(context), m_midiout(m_midiout)
{
    track = new CSurf_FP_V2_Track(context, m_midiout);
}

void CSurf_FP_V2_TrackManager::UpdateTrack()
{
    MediaTrack *media_track = navigator->GetControllerTrack();

    int faderValue = 0;
    std::string strPan1, strPan2;

    if (media_track == nullptr || CountTracks(0) < 1)
    {
        ClearTrack();
        forceUpdate = false;
        return;
    }

    bool isArmed = DAW::IsTrackArmed(media_track);

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

    track->SetArmButtonValue(isArmed ? BTN_VALUE_ON : BTN_VALUE_OFF);
    track->SetBypassButtonValue(DAW::GetTrackFxBypassed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);

    if (context->GetLastTouchedFxMode())
    {
        double paramValue = 0.0, min, max;
        int trackNumber = -1;
        int fxNumber = -1;
        int paramNumber = -1;

        GetLastTouchedFX(&trackNumber, &fxNumber, &paramNumber);
        if (MediaTrack *media_track = GetTrack(0, trackNumber - 1))
        {
            paramValue = TrackFX_GetParam(media_track, fxNumber, paramNumber, &min, &max);
            track->SetFaderValue((int)(paramValue * 16383.0), forceUpdate);
        }
    }
    else
    {
        if (!context->GetFaderDisabled())
        {
            track->SetFaderValue(faderValue, forceUpdate);
        }
    }

    hasLastTouchedFxEnabled = context->GetLastTouchedFxMode();
    forceUpdate = false;
}

void CSurf_FP_V2_TrackManager::ClearTrack()
{
    track->SetMuteButtonValue(BTN_VALUE_OFF);
    track->SetSoloButtonValue(BTN_VALUE_OFF);
    track->SetArmButtonValue(BTN_VALUE_OFF);

    track->SetFaderValue(0, forceUpdate);
}

void CSurf_FP_V2_TrackManager::HandleMuteClick(int index, int value)
{
    (void)index;

    if (context->GetShiftLeft())
    {
        if (value == 0)
        {
            return;
        }

        Main_OnCommandEx(40339, 0, 0); // Track: Unmute all tracks
        return;
    }

    int now = GetTickCount();
    MediaTrack *media_track = navigator->GetControllerTrack();

    if (value == 0 && context->GetMuteSoloMomentary())
    {
        if (now - mute_start > MOMENTARY_TIMEOUT)
        {
            CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, !DAW::IsTrackMuted(media_track)), NULL);
        }
        mute_start = 0;
    }
    else if (value > 0)
    {
        mute_start = now;
        CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, !DAW::IsTrackMuted(media_track)), NULL);
    }
}

void CSurf_FP_V2_TrackManager::HandleSoloClick(int index, int value)
{
    (void)index;
    if (context->GetShiftLeft())
    {
        if (value == 0)
        {
            return;
        }
        Main_OnCommandEx(40340, 0, 0); // Track: Unsolo all tracks
        return;
    }

    int now = GetTickCount();
    MediaTrack *media_track = navigator->GetControllerTrack();

    if (value == 0 && context->GetMuteSoloMomentary())
    {
        if (now - solo_start > MOMENTARY_TIMEOUT)
        {
            CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, !DAW::IsTrackSoloed(media_track)), NULL);
        }
        solo_start = 0;
    }
    else if (value > 0)
    {
        solo_start = now;
        CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, !DAW::IsTrackSoloed(media_track)), NULL);
    }
}

void CSurf_FP_V2_TrackManager::HandleArmClick(int index, int value)
{
    (void)index;
    if (value == 0)
    {
        return;
    }

    MediaTrack *media_track = navigator->GetControllerTrack();

    if (context->GetShiftLeft())
    {
        Main_OnCommandEx(40490, 0, 0); // Track: Arm all tracks for recording
        return;
    }

    CSurf_SetSurfaceRecArm(media_track, CSurf_OnRecArmChange(media_track, !DAW::IsTrackArmed(media_track)), NULL);
}

void CSurf_FP_V2_TrackManager::HandleBypassClick(int index, int value)
{
    (void)index;
    if (value == 0)
    {
        return;
    }

    MediaTrack *media_track = navigator->GetControllerTrack();

    if (value == 0)
    {
        return;
    }

    context->GetShiftLeft() ? Main_OnCommandEx(40344, 0, 0)          // Track: Toggle FX bypass on all tracks
                            : DAW::ToggleTrackFxBypass(media_track); // Track: Toggle FX bypass for selected tracks
}

void CSurf_FP_V2_TrackManager::HandleFaderMove(int msb, int lsb)
{
    if (context->GetLastTouchedFxMode())
    {
        int trackNumber = -1;
        int fxNumber = -1;
        int paramNumber = -1;

        if (GetLastTouchedFX(&trackNumber, &fxNumber, &paramNumber))
        {
            if (MediaTrack *media_track = GetTrack(0, trackNumber - 1))
            {
                TrackFX_SetParam(media_track, fxNumber, paramNumber, int14ToNormalized(msb, lsb));
            }
        }
        return;
    }

    MediaTrack *media_track = navigator->GetControllerTrack();

    if (context->GetShiftLeft())
    {
        CSurf_SetSurfacePan(media_track, CSurf_OnPanChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false), NULL);
    }
    else
    {
        CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), NULL);
    }
}