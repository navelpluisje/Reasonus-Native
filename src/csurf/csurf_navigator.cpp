
#include "csurf_navigator.hpp"
#include "csurf_utils.hpp"
#include <reaper_plugin_functions.h>

void CSurf_Navigator::GetAllVisibleTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute)
{
    tracks.Empty();
    bool _solo = false;
    bool _mute = false;

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool visible = (bool)GetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER");
        int solo = GetMediaTrackInfo_Value(media_track, "I_SOLO");
        bool mute = (bool)GetMediaTrackInfo_Value(media_track, "B_MUTE");

        if (solo > 0 && !_solo)
        {
            _solo = true;
        }

        if (mute && !_mute)
        {
            _mute = true;
        }

        if (visible)
        {
            tracks.Add(media_track);
        }
    }
    hasSolo = _solo;
    hasMute = _mute;
}

void CSurf_Navigator::SetTrackUIVisibility(map<int, bool> &tracks)
{
    PreventUIRefresh(1);

    for (auto const &track : tracks)
    {
        MediaTrack *media_track = GetTrack(0, track.first);
        SetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER", track.second);
    }

    PreventUIRefresh(-1);
    TrackList_AdjustWindows(false);
    UpdateArrange();
}

void CSurf_Navigator::HandleAllTracksFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        tracks[i] = true;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksWithSendsFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool hasSends = (bool)GetTrackNumSends(media_track, 0);
        tracks[i] = hasSends;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksWithReceivesFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool hasReceives = (bool)GetTrackNumSends(media_track, -1);
        tracks[i] = hasReceives;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksWithHardwareOutputsFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool hasHardwareOutputs = (bool)GetTrackNumSends(media_track, 1);
        tracks[i] = hasHardwareOutputs;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksWithinstrumentsFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        int hasInstruments = TrackFX_GetInstrument(media_track);

        tracks[i] = hasInstruments > -1;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksTopFoldersFilter()
{
    map<int, bool> tracks = {};
    int depth = 0;

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        int folderDepth = (int)GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH");
        depth += folderDepth;

        tracks[i] = false;
        if (depth == 1 && folderDepth == 1)
        {
            tracks[i] = true;
        }
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksAllFoldersFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        int folderDepth = (int)GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH");
        tracks[i] = folderDepth == 1;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::UpdateMixerPosition()
{
    MediaTrack *media_track = GetTrack(0, track_offset);
    SetMixerScroll(media_track);
};

CSurf_Navigator::CSurf_Navigator(CSurf_Context *context) : context(context)
{
    HandleAllTracksFilter();
};

MediaTrack *CSurf_Navigator::GetTrackByIndex(int index)
{
    WDL_PtrList<MediaTrack> bank = GetBankTracks();
    return bank.Get(index);
}

WDL_PtrList<MediaTrack> CSurf_Navigator::GetBankTracks()
{
    WDL_PtrList<MediaTrack> bank;
    GetAllVisibleTracks(tracks, hasSolo, hasMute);
    for (int i = track_offset; i < track_offset + nb_channels; i++)
    {
        bank.Add(tracks.Get(i));
    }
    return bank;
}

void CSurf_Navigator::SetOffset(int offset)
{
    if (offset > (tracks.GetSize() - nb_channels))
    {
        track_offset = tracks.GetSize() - nb_channels;
    }
    else
    {
        track_offset = offset;
    }
}

void CSurf_Navigator::IncrementOffset(int count)
{
    if ((track_offset + count) <= (tracks.GetSize() - nb_channels))
    {
        track_offset += count;
    }
    else if (tracks.GetSize() < nb_channels)
    {
        track_offset = 0;
    }
    else
    {
        track_offset = tracks.GetSize() - nb_channels;
    }
    UpdateMixerPosition();
}

void CSurf_Navigator::DecrementOffset(int count)
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

void CSurf_Navigator::HandlePanEncoderChange(int value)
{
    if (hasBit(value, 6))
    {
        DecrementOffset(1);
    }
    if (!hasBit(value, 6) && track_offset < (tracks.GetSize() - nb_channels))
    {
        track_offset += 1;
    }
}

bool CSurf_Navigator::HasTracksWithSolo()
{
    return hasSolo;
};

bool CSurf_Navigator::HasTracksWithMute()
{
    return hasMute;
};

void CSurf_Navigator::handleFilter(NavigatorFilter filter)
{
    switch (filter)
    {
    case TrackSendsFilter:
        HandleTracksWithSendsFilter();
        break;
    case TrackReceivesFilter:
        HandleTracksWithReceivesFilter();
        break;
    case TrackHarwareOutputFilter:
        HandleTracksWithHardwareOutputsFilter();
        break;
    case TrackInstrumentsFilter:
        HandleTracksWithinstrumentsFilter();
        break;
    case TrackTopFoldersFilter:
        HandleTracksTopFoldersFilter();
        break;
    case TrackAllFoldersFilter:
        HandleTracksAllFoldersFilter();
        break;
    default:
        HandleAllTracksFilter();
    }
}

void CSurf_Navigator::UpdateTrackCount()
{
    bool hasLastTouchedFx = context->GetLastTouchedFxMode();
    nb_channels = hasLastTouchedFx ? 7 : 8;
}

// char buffer[250];
// snprintf(buffer, sizeof(buffer), "Zone -- %s\n\n", sourceFilePath_.c_str());
// ShowConsoleMsg(buffer);
