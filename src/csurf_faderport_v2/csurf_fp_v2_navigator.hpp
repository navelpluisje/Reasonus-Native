#ifndef CSURF_NAVIGATOR_V2_H_
#define CSURF_NAVIGATOR_V2_H_

#include <stdlib.h>
#include <WDL/ptrlist.h>
#include "../shared/csurf_context.cpp"
#include <map>
#include <reaper_plugin.h>

class CSurf_FP_V2_Navigator
{
    int track_offset = 0;
    CSurf_Context *context;

    WDL_PtrList<MediaTrack> tracks;
    bool hasSolo;
    bool hasMute;
    bool hasArmed;
    bool hasAllArmed;
    bool isTouched;

    void UpdateMixerPosition();

    void GetAllControllableTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute);

public:
    CSurf_FP_V2_Navigator(CSurf_Context *context);

    MediaTrack *GetControllerTrack();

    bool IsTrackTouched(MediaTrack *media_track, int is_pan);

    void SetOffset(int offset);

    void SetOffsetByTrack(MediaTrack *media_track);

    int GetOffset();

    void IncrementOffset(int count);

    void DecrementOffset(int count);

    void UpdateOffset();

    MediaTrack *GetNextTrack();

    MediaTrack *GetPreviousTrack();

    bool HasTracksWithSolo();

    bool HasTracksWithMute();

    bool HasArmedTracks();

    bool HasAllArmedTracks();

    void SetIsTouched(bool value);
};

#endif
