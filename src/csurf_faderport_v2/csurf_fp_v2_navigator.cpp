#include "csurf_fp_v2_navigator.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"
#include <mini/ini.h>

void CSurf_FP_V2_Navigator::GetAllControllableTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute)
{
    tracks.Empty();
    bool _solo = false;
    bool _mute = false;
    bool _arm = false;

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool visible = DAW::IsTrackVisible(media_track);
        int solo = DAW::IsTrackSoloed(media_track);
        bool mute = DAW::IsTrackMuted(media_track);
        bool armed = DAW::IsTrackArmed(media_track);

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

        if (visible || context->GetControlHiddenTracks())
        {
            tracks.Add(media_track);
        }
    }
    hasSolo = _solo;
    hasMute = _mute;
    hasArmed = _arm;
}

void CSurf_FP_V2_Navigator::UpdateMixerPosition()
{
    MediaTrack *media_track = GetControllerTrack();
    if (media_track)
    {
        SetMixerScroll(media_track);
        DAW::SetTcpScroll(media_track);
    }
};

CSurf_FP_V2_Navigator::CSurf_FP_V2_Navigator(CSurf_Context *context) : context(context) {};

MediaTrack *CSurf_FP_V2_Navigator::GetControllerTrack()
{
    if (context->GetMasterFaderMode())
    {
        return GetMasterTrack(0);
    }

    GetAllControllableTracks(tracks, hasSolo, hasMute);

    if (DAW::IsTrackSelected(tracks.Get(track_offset)))
    {
        return tracks.Get(track_offset);
    }

    return nullptr;
}

bool CSurf_FP_V2_Navigator::IsTrackTouched(MediaTrack *media_track, int is_pan)
{
    if (media_track != GetControllerTrack() || context->GetLastTouchedFxMode())
    {
        return false;
    }

    if (!context->GetShiftLeft() && is_pan == 0 || context->GetShiftLeft() && is_pan == 1)
    {
        return isTouched;
    }

    return false;
}

void CSurf_FP_V2_Navigator::SetOffset(int offset)
{
    if (tracks.GetSize() == 0 || offset < 0)
    {
        track_offset = 0;
    }
    else if (offset > (tracks.GetSize() - context->GetNbChannels()))
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
    int trackId = stoi(DAW::GetTrackIndex(media_track));

    for (int i = 0; tracks.GetSize(); i++)
    {
        int id = stoi(DAW::GetTrackIndex(tracks.Get(i)));

        if (trackId == id)
        {
            SetOffset(i);
            break;
        }
    }
}

int CSurf_FP_V2_Navigator::GetOffset()
{
    return track_offset;
}

void CSurf_FP_V2_Navigator::IncrementOffset(int count)
{
    if ((track_offset + count) <= (tracks.GetSize() - context->GetNbChannels()))
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

void CSurf_FP_V2_Navigator::DecrementOffset(int count)
{
    if ((track_offset - count) >= 0)
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

    MediaTrack *media_track = ::GetSelectedTrack(0, 0);
    track_offset = (int)::GetMediaTrackInfo_Value(media_track, "IP_TRACKNUMBER") - 1;
}

MediaTrack *CSurf_FP_V2_Navigator::GetNextTrack()
{
    if (track_offset + 1 > tracks.GetSize() - 1)
    {
        if (context->GetEndlessTrackScroll())
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
        if (context->GetEndlessTrackScroll())
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

bool CSurf_FP_V2_Navigator::HasTracksWithSolo()
{
    return hasSolo;
};

bool CSurf_FP_V2_Navigator::HasTracksWithMute()
{
    return hasMute;
};

bool CSurf_FP_V2_Navigator::HasArmedTracks()
{
    return hasArmed;
};

void CSurf_FP_V2_Navigator::SetIsTouched(bool value)
{
    isTouched = value;
}
