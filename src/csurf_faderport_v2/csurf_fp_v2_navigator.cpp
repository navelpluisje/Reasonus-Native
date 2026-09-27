#include "csurf_fp_v2_navigator.hpp"
#include "../shared/csurf.h"
#include "../shared/csurf_daw.hpp"

void CSurf_FP_V2_Navigator::UpdateMixerPosition()
{
    MediaTrack *media_track = GetControllerTrack();
    if (media_track)
    {
        SetMixerScroll(media_track);
        DAW::SetTcpScroll(media_track);
    }
}

void CSurf_FP_V2_Navigator::GetAllControllableTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute)
{
    tracks.Empty();
    bool _solo = false;
    bool _mute = false;
    bool _arm = false;
    bool _all_armed = true;

    for (int i = 0; i < CountTracks(nullptr); i++)
    {
        MediaTrack *media_track = GetTrack(nullptr, i);
        const bool visible = DAW::IsTrackVisible(media_track);
        const int solo = DAW::IsTrackSoloed(media_track);
        const bool mute = DAW::IsTrackMuted(media_track);
        const bool armed = DAW::IsTrackArmed(media_track);

        if (solo > 0 && !_solo)
        {
            _solo = true;
        }

        if (mute && !_mute)
        {
            _mute = true;
        }

        if (armed && !_arm)
        {
            _arm = true;
        }

        if (!armed && _all_armed)
        {
            _all_armed = false;
        }

        if (visible || settings->GetControlHiddenTracks())
        {
            tracks.Add(media_track);
        }
    }

    hasSolo = _solo;
    hasMute = _mute;
    hasArmed = _arm;
    hasAllArmed = _all_armed;
}

CSurf_FP_V2_Navigator::CSurf_FP_V2_Navigator(
    CSurf_Context *context
    ) : context(context), hasSolo(false), hasMute(false), hasArmed(false), hasAllArmed(false), isTouched(false) {}

MediaTrack *CSurf_FP_V2_Navigator::GetControllerTrack()
{
    if (context->GetMasterFaderMode())
    {
        DAW::SetExtState(FP_V2_CONTROLLED_TRACK, -1, false);
        return GetMasterTrack(nullptr);
    }

    DAW::SetExtState(FP_V2_CONTROLLED_TRACK, track_offset, false);
    GetAllControllableTracks(tracks, hasSolo, hasMute);

    if (DAW::IsTrackSelected(tracks.Get(track_offset)))
    {
        return tracks.Get(track_offset);
    }

    return nullptr;
}

bool CSurf_FP_V2_Navigator::IsTrackTouched(const MediaTrack *media_track, const int is_pan)
{
    if (media_track != GetControllerTrack() || context->GetLastTouchedFxMode())
    {
        return false;
    }

    if ((!context->GetShiftLeft() && is_pan == 0) || (context->GetShiftLeft() && is_pan == 1))
    {
        return isTouched;
    }

    return false;
}

void CSurf_FP_V2_Navigator::SetOffset(const int offset)
{
    if (tracks.GetSize() == 0 || offset < 0)
    {
        track_offset = 0;
    }
    else if (offset > tracks.GetSize() - context->GetNbChannels())
    {
        track_offset = tracks.GetSize() - context->GetNbChannels();
    }
    else
    {
        track_offset = offset;
    }
}

void CSurf_FP_V2_Navigator::SetOffsetByTrack(MediaTrack *media_track)
{
    const int trackId = stoi(DAW::GetTrackIndex(media_track));

    for (int i = 0; tracks.GetSize(); i++)
    {
        const int id = stoi(DAW::GetTrackIndex(tracks.Get(i)));

        if (trackId == id)
        {
            SetOffset(i);
            break;
        }
    }
}

int CSurf_FP_V2_Navigator::GetOffset() const {
    return track_offset;
}

void CSurf_FP_V2_Navigator::IncrementOffset(const int count)
{
    if (track_offset + count <= tracks.GetSize() - context->GetNbChannels())
    {
        track_offset += count;
    }
    else if (tracks.GetSize() < context->GetNbChannels())
    {
        track_offset = 0;
    }
    else
    {
        track_offset = tracks.GetSize() - context->GetNbChannels();
    }
    UpdateMixerPosition();
}

void CSurf_FP_V2_Navigator::DecrementOffset(const int count)
{
    if (track_offset - count >= 0)
    {
        track_offset -= count;
    }
    else
    {
        track_offset = 0;
    }
    UpdateMixerPosition();
}

void CSurf_FP_V2_Navigator::UpdateOffset()
{
    GetAllControllableTracks(tracks, hasSolo, hasMute);

    MediaTrack *media_track = GetSelectedTrack(nullptr, 0);
    track_offset = static_cast<int>(GetMediaTrackInfo_Value(media_track, "IP_TRACKNUMBER")) - 1;
}

MediaTrack *CSurf_FP_V2_Navigator::GetNextTrack()
{
    if (track_offset + 1 > tracks.GetSize() - 1)
    {
        if (settings->GetEndlessTrackScroll())
        {
            track_offset = 0;
        }
    }
    else
    {
        track_offset++;
    }

    MediaTrack *media_track = tracks.Get(track_offset);
    SetMixerScroll(media_track);
    DAW::SetTcpScroll(media_track);
    return media_track;
}

MediaTrack *CSurf_FP_V2_Navigator::GetPreviousTrack()
{
    if (track_offset - 1 < 0)
    {
        if (settings->GetEndlessTrackScroll())
        {
            track_offset = tracks.GetSize() - 1;
        }
    }
    else
    {
        track_offset--;
    }

    MediaTrack *media_track = tracks.Get(track_offset);
    SetMixerScroll(media_track);
    DAW::SetTcpScroll(media_track);
    return media_track;
}

bool CSurf_FP_V2_Navigator::HasTracksWithSolo() const {
    return hasSolo;
}

bool CSurf_FP_V2_Navigator::HasTracksWithMute() const {
    return hasMute;
}

bool CSurf_FP_V2_Navigator::HasArmedTracks() const {
    return hasArmed;
}

bool CSurf_FP_V2_Navigator::HasAllArmedTracks() const {
    return hasAllArmed;
}

void CSurf_FP_V2_Navigator::SetIsTouched(const bool value)
{
    isTouched = value;
}
