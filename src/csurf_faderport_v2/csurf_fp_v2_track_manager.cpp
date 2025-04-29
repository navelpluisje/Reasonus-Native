#include <vector>
#include <WDL/ptrlist.h>
#include "../shared/csurf_context.cpp"
#include "../shared/csurf_daw.hpp"
#include "../shared/csurf_utils.hpp"
#include "csurf_fp_v2_track.hpp"
#include "csurf_fp_v2_track_manager.hpp"
#include "csurf_fp_v2_navigator.hpp"

const int MOMENTARY_TIMEOUT = 500;

void CSurf_FP_V2_TrackManager::GetFaderValue(MediaTrack *media_track, int *faderValue)
{
    double volume, pan1 = 0.0;

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
    soloButton = new CSurf_Button(BTN_SOLO_1, BTN_VALUE_OFF, m_midiout);
    muteButton = new CSurf_Button(BTN_MUTE_1, BTN_VALUE_OFF, m_midiout);
    fader = new CSurf_Fader(1, 0, m_midiout);

    UpdateTracks();
}

void CSurf_FP_V2_TrackManager::UpdateTracks()
{
    WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();

    if (hasLastTouchedFxEnabled != context->GetLastTouchedFxMode() && !context->GetLastTouchedFxMode())
    {
        forceUpdate = true;
    }

    int faderValue = 0;
    std::string strPan1, strPan2;

    MediaTrack *media_track = navigator->GetTrack();

    if (!media_track || CountTracks(0) < 1)
    {
        ClearTrack();
        forceUpdate = false;
        return;
    }

    GetFaderValue(media_track, &faderValue);

    if (context->GetShiftLeft())
    {
        SetMuteButtonValue(navigator->HasTracksWithMute() ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
        SetSoloButtonValue(navigator->HasTracksWithSolo() ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
    }
    else
    {
        SetMuteButtonValue(DAW::IsTrackMuted(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
        SetSoloButtonValue(DAW::IsTrackSoloed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
    }

    SetFaderValue(faderValue, forceUpdate);

    hasLastTouchedFxEnabled = context->GetLastTouchedFxMode();
    forceUpdate = false;
}

void CSurf_FP_V2_TrackManager::ClearTrack()
{
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

        Main_OnCommandEx(40344, 0, 0); // Track: Toggle FX bypass on all tracks
        return;
    }

    int now = GetTickCount();
    MediaTrack *media_track = navigator->GetTrack();

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
    MediaTrack *media_track = navigator->GetTrack();

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

void CSurf_FP_V2_TrackManager::HandleFaderMove(int msb, int lsb)
{
    MediaTrack *media_track = navigator->GetTrack();

    if (context->GetShiftLeft())
    {
        CSurf_SetSurfacePan(media_track, CSurf_OnPanChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false), NULL);
    }
    else
    {
        CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), NULL);
    }
}

void CSurf_FP_V2_TrackManager::SetSoloButtonValue(Btn_Value value, bool force)
{
    soloButton->SetValue(value, force);
}

void CSurf_FP_V2_TrackManager::SetMuteButtonValue(Btn_Value value, bool force)
{
    muteButton->SetValue(value, force);
}

void CSurf_FP_V2_TrackManager::SetFaderValue(int value, bool force)
{
    fader->SetValue(value, force);
}
