#ifndef CSURF_NAVIGATOR_V2_H_
#define CSURF_NAVIGATOR_V2_H_

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

    void UpdateMixerPosition();

    void GetAllVisibleTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute);

public:
    CSurf_FP_V2_Navigator(CSurf_Context *context);

    MediaTrack *GetControllerTrack();

    void SetOffset(int offset);

    int GetOffset();

    void IncrementOffset(int count);

    void DecrementOffset(int count);

    void UpdateOffset();

    bool HasTracksWithSolo();

    bool HasTracksWithMute();

    bool HasArmedTracks();
};

#endif
