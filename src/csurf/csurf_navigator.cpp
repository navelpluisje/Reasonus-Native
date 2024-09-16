#ifndef CSURF_NAVIGATOR_C_
#define CSURF_NAVIGATOR_C_

#include <WDL/ptrlist.h>
#include <reaper_plugin.h>
#include <reaper_plugin_functions.h>
#include "csurf_channel_manager_resources.hpp"
#include "csurf_navigator_filters.hpp"
#include "csurf_utils.hpp"

class CSurf_Navigator
{
    int nb_channels = 8;
    int track_offset = 0;
    NavigatorFilter trackFilter = TrackAllFilter;

    WDL_PtrList<MediaTrack> tracks;

    void UpdateMixerPosition()
    {
        MediaTrack *media_track = GetTrack(0, track_offset);
        SetMixerScroll(media_track);
    }

public:
    CSurf_Navigator()
    {
        HandleAllTracksFilter();
    };

    MediaTrack *GetTrackByIndex(int index)
    {
        WDL_PtrList<MediaTrack> bank = GetBankTracks();
        return bank.Get(index);
    }

    WDL_PtrList<MediaTrack> GetBankTracks()
    {
        WDL_PtrList<MediaTrack> bank;
        GetAllVisibleTracks(tracks);
        for (int i = track_offset; i < track_offset + nb_channels; i++)
        {
            bank.Add(tracks.Get(i));
        }
        return bank;
    }

    void SetOffset(int offset)
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

    void IncrementOffset(int count)
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

    void DecrementOffset(int count)
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

    void HandlePanEncoderChange(int value)
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

    void handleFilter(NavigatorFilter filter)
    {
        trackFilter = filter;
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
};

#endif
// char buffer[250];
// snprintf(buffer, sizeof(buffer), "Zone -- %s\n\n", sourceFilePath_.c_str());
// ShowConsoleMsg(buffer);
