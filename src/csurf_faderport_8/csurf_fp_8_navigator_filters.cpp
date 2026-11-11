#include "csurf_fp_8_navigator_filters.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"

std::map<int, bool> GetSibblingTracks(std::map<int, bool> tracks)
{
    std::map<int, bool> response;

    for (const auto &[track_index, _] : tracks)
    {
        MediaTrack *media_track = GetTrack(nullptr, track_index);
        const int nb_tracks = CountTracks(nullptr);
        const int start_depth = GetTrackDepth(media_track);
        int track_depth = start_depth;
        int track_id;

        if (track_depth == 0)
        {
            track_id = 0;
        }
        else
        {
            MediaTrack *parent_track = GetParentTrack(media_track);
            track_id = static_cast<int>(GetMediaTrackInfo_Value(parent_track, "IP_TRACKNUMBER"));
        }

        while (track_depth >= start_depth && track_id < nb_tracks)
        {
            MediaTrack *current_track = GetTrack(nullptr, track_id);
            track_depth = GetTrackDepth(current_track);
            if (track_depth == start_depth)
            {
                response[track_id] = true;
            }
            track_id += 1;
        }
    }

    return response;
}

std::map<int, bool> GetParentTracks(std::map<int, bool> tracks)
{
    std::map<int, bool> response;

    for (const auto &[track_index, _] : tracks)
    {
        MediaTrack *media_track = GetTrack(nullptr, track_index);
        int track_depth = GetTrackDepth(media_track);
        int track_id = track_index - 1;

        if (track_depth == 0)
        {
            continue;
        }

        while (track_depth > 0)
        {
            MediaTrack *current_track = GetTrack(nullptr, track_id);
            const int current_track_depth = GetTrackDepth(current_track);

            if (static_cast<int>(GetMediaTrackInfo_Value(current_track, "I_FOLDERDEPTH")) == 1 && current_track_depth < track_depth)
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

    for (const auto &[track_index, _] : tracks)
    {
        MediaTrack *media_track = GetTrack(nullptr, track_index);

        // The track is a parent, so has children. Now add all the children to the result
        if (static_cast<int>(GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH")) == 1)
        {
            int folderDepth = 1;
            int trackId = track_index + 1;

            while (folderDepth > 0)
            {
                MediaTrack *child_track = GetTrack(nullptr, trackId);
                const int depth = static_cast<int>(GetMediaTrackInfo_Value(child_track, "I_FOLDERDEPTH"));
                response[trackId] = true;
                folderDepth += depth;
                trackId += 1;
            }
        }
    }
    return response;
}

std::map<int, bool> GetCustomFilterTracks(mINI::INIMap<std::string> filter)
{
    std::map<int, bool> tracks;
    std::map<int, bool> filterTracks;
    std::map<int, bool> childTracks;
    std::map<int, bool> parentTracks;
    std::map<int, bool> sibblingTracks;

    bool stop = false;
    for (int i = 0; i < CountTracks(nullptr); i++)
    {
        if (!stop)
        {
            MediaTrack *media_track = GetTrack(nullptr, i);
            std::string trackName = DAW::GetTrackName(media_track);
            bool isChild = GetTrackDepth(media_track) > 0;

            if (FuzzyMatch(trackName, filter["text"], filter["case-insensitive"] == "1") && !(isChild && filter["top-level"] == "1"))
            {
                filterTracks[i] = true;
                if (filter["match-multiple"] == "0")
                {
                    stop = true;
                }
            }
        }
    }

    tracks.insert(filterTracks.begin(), filterTracks.end());

    if (filter["children"] == "1")
    {
        childTracks = GetChildTracks(filterTracks);
        tracks.insert(childTracks.begin(), childTracks.end());
    }

    if (filter["parents"] == "1")
    {
        parentTracks = GetParentTracks(filterTracks);
        tracks.insert(parentTracks.begin(), parentTracks.end());
    }

    if (filter["sibblings"] == "1")
    {
        sibblingTracks = GetSibblingTracks(filterTracks);
        tracks.insert(sibblingTracks.begin(), sibblingTracks.end());
    }

    return tracks;
}

bool FuzzyMatch(std::string track_name, std::string matches, const bool case_insensitive)
{
    bool match = false;

    if (case_insensitive)
    {
        track_name = toLowerCase(track_name);
        matches = toLowerCase(matches);
    }

    for (const std::string &val : split(matches, ","))
    {
        const std::string::size_type found = track_name.find(val);
        match = match || found != std::string::npos;
    }

    return match;
}

std::map<int, bool> GetAllTracksBase()
{
    std::map<int, bool> result;

    for (int i = 0; i < CountTracks(nullptr); i++)
    {
        result[i] = false;
    }

    return result;
}
