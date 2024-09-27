#include "csurf_daw.hpp"
#include <reaper_plugin.h>
#include <reaper_plugin_functions.h>

void unSelectAllTracks()
{
    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        SetTrackSelected(media_track, false);
    }
};
