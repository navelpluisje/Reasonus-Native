#ifndef CSURF_NAVIGATOR_FILTERS_H_
#define CSURF_NAVIGATOR_FILTERS_H_

#include <map>

enum NavigatorFilter
{
    TrackAllFilter,
    TrackSendsFilter,
    TrackReceivesFilter,
    TrackHarwareOutputFilter,
    TrackInstrumentsFilter,
    TrackTopFoldersFilter,
    TrackAllFoldersFilter,
};

static void GetAllVisibleTracks(WDL_PtrList<MediaTrack> &tracks)
{
    tracks.Empty();

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool visible = (bool)GetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER");
        if (visible)
        {
            tracks.Add(media_track);
        }
    }
}

static void SetTrackUIVisibility(map<int, bool> tracks)
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

static void HandleAllTracksFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        tracks[i] = true;
    }

    SetTrackUIVisibility(tracks);
}

static void HandleTracksWithSendsFilter()
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

static void HandleTracksWithReceivesFilter()
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

static void HandleTracksWithHardwareOutputsFilter()
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

static void HandleTracksWithinstrumentsFilter()
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

static void HandleTracksTopFoldersFilter()
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

static void HandleTracksAllFoldersFilter()
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

#endif
