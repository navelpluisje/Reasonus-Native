#include "csurf_navigator_filters.hpp"
#include "csurf_utils.hpp"
#include "csurf_daw.hpp"
#include <reaper_plugin_functions.h>
#include <mINI/ini.h>

map<int, bool> GetSibblingTracks(map<int, bool> tracks)
{
    map<int, bool> response;

    for (auto const &track : tracks)
    {
        MediaTrack *media_track = GetTrack(0, track.first);
        int nbTracks = CountTracks(0);
        int startDepth = GetTrackDepth(media_track);
        int trackDepth = startDepth;
        int trackId;

        if (trackDepth == 0)
        {
            trackId = 0;
        }
        else
        {
            MediaTrack *parent_track = GetParentTrack(media_track);
            trackId = (int)GetMediaTrackInfo_Value(parent_track, "IP_TRACKNUMBER");
        }

        while (trackDepth >= startDepth && trackId < nbTracks)
        {
            MediaTrack *current_track = GetTrack(0, trackId);
            trackDepth = GetTrackDepth(current_track);
            if (trackDepth == startDepth)
            {
                response[trackId] = true;
            }
            trackId += 1;
        }
    }

    return response;
}

map<int, bool> GetParentTracks(map<int, bool> tracks)
{
    map<int, bool> response;

    for (auto const &track : tracks)
    {
        MediaTrack *media_track = GetTrack(0, track.first);
        int trackDepth = GetTrackDepth(media_track);
        int trackId = track.first - 1;

        if (trackDepth == 0)
        {
            continue;
        }

        while (trackDepth > 0)
        {
            MediaTrack *current_track = GetTrack(0, trackId);
            if ((int)GetMediaTrackInfo_Value(current_track, "I_FOLDERDEPTH") == 1)
            {
                response[trackId] = true;
            }
            trackId -= 1;
            trackDepth = GetTrackDepth(current_track);
        }
    }

    return response;
}

map<int, bool> GetChildTracks(map<int, bool> tracks)
{
    map<int, bool> response;

    for (auto const &track : tracks)
    {
        MediaTrack *media_track = GetTrack(0, track.first);
        // The track is a parent, so has children. Now add all teh children to the result
        if ((int)GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH") == 1)
        {
            int folderDepth = 1;
            int trackId = track.first + 1;

            while (folderDepth > 0)
            {
                MediaTrack *child_track = GetTrack(0, trackId);
                int depth = (int)GetMediaTrackInfo_Value(child_track, "I_FOLDERDEPTH");
                response[trackId] = true;
                folderDepth += depth;
                trackId += 1;
            }
        }
    }
    return response;
}

bool FuzzyMatch(string trackName, string matches)
{
    bool match = false;

    for (const string &val : split(matches, ","))
    {
        string::size_type found = trackName.find(val);
        match = match || found != string::npos;
    }

    return match;
}
