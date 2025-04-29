#include "csurf_fp_v2_navigator.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"
// #include <WDL/wdltypes.h> // might be unnecessary in future
// #include <reaper_plugin_functions.h>
#include <mini/ini.h>

void CSurf_FP_V2_Navigator::GetAllVisibleTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute)
{
    tracks.Empty();
    bool _solo = false;
    bool _mute = false;
    bool _arm = false;

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack();
        bool visible = (bool)GetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER");
        int solo = GetMediaTrackInfo_Value(media_track, "I_SOLO");
        bool mute = (bool)GetMediaTrackInfo_Value(media_track, "B_MUTE");
        bool armed = (bool)GetMediaTrackInfo_Value(media_track, "I_RECARM");

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

        if (visible)
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
    MediaTrack *media_track = GetTrack();
    SetMixerScroll(media_track);
};

CSurf_FP_V2_Navigator::CSurf_FP_V2_Navigator(CSurf_Context *context) : context(context) {};

MediaTrack *CSurf_FP_V2_Navigator::GetTrack()
{
    WDL_PtrList<MediaTrack> bank = GetBankTracks();
    return bank.Get(0);
}

WDL_PtrList<MediaTrack> CSurf_FP_V2_Navigator::GetBankTracks()
{
    WDL_PtrList<MediaTrack> bank;
    GetAllVisibleTracks(tracks, hasSolo, hasMute);
    int channelCount = context->GetNbChannels() > tracks.GetSize() ? context->GetNbChannels() : tracks.GetSize();
    for (int i = track_offset; i < track_offset + channelCount; i++)
    {
        if (i > track_offset + channelCount)
        {
            bank.Add(NULL);
        }
        else
        {
            bank.Add(tracks.Get(i));
        }
    }
    return bank;
}

void CSurf_FP_V2_Navigator::SetOffset(int offset)
{
    if (tracks.GetSize() < context->GetNbChannels())
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
