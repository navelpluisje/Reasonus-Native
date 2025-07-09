#include "csurf_fp_v2_navigator.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"
#include <mini/ini.h>

void CSurf_FP_V2_Navigator::GetAllVisibleTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute)
{
    tracks.Empty();
    bool _solo = false;
    bool _mute = false;
    bool _arm = false;

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool visible = (bool)GetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER");
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
    }
};

CSurf_FP_V2_Navigator::CSurf_FP_V2_Navigator(CSurf_Context *context) : context(context) {};

MediaTrack *CSurf_FP_V2_Navigator::GetControllerTrack()
{
    if (context->GetMasterFaderMode())
    {
        return GetMasterTrack(0);
    }

    GetAllVisibleTracks(tracks, hasSolo, hasMute);

    if (DAW::IsTrackSelected(tracks.Get(track_offset)))
    {
        return tracks.Get(track_offset);
    }

    return nullptr;
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
