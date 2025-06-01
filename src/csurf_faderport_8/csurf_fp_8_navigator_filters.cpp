#include "csurf_fp_8_navigator_filters.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"
#include <mini/ini.h>

std::map<int, bool> GetSibblingTracks(std::map<int, bool> tracks)
{
    std::map<int, bool> response;

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

std::map<int, bool> GetParentTracks(std::map<int, bool> tracks)
{
    std::map<int, bool> response;

    for (auto const &track : tracks)
    {
        MediaTrack *media_track = GetTrack(0, track.first);
        int track_depth = GetTrackDepth(media_track);
        int track_id = track.first - 1;

        if (track_depth == 0)
        {
            continue;
        }

        while (track_depth > 0)
        {
            MediaTrack *current_track = GetTrack(0, track_id);
            int current_track_depth = GetTrackDepth(current_track);

            if ((int)GetMediaTrackInfo_Value(current_track, "I_FOLDERDEPTH") == 1 && current_track_depth < track_depth)
            {
                response[track_id] = true;
                track_depth = current_track_depth;
            }
            track_id -= 1;
        }
    }

    return response;
}

std::map<int, bool> GetChildTracks(std::map<int, bool> tracks)
{
    std::map<int, bool> response;

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

bool FuzzyMatch(std::string trackName, std::string matches)
{
    bool match = false;

    for (const std::string &val : split(matches, ","))
    {
        std::string::size_type found = trackName.find(val);
        match = match || found != std::string::npos;
    }

    return match;
}
