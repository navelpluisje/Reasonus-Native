#ifndef CSURF_NAVIGATOR_H_
#define CSURF_NAVIGATOR_H_

#include <WDL/ptrlist.h>
#include "csurf_context.cpp"
#include <map>
#include <reaper_plugin.h>

using namespace std;

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

class CSurf_Navigator
{
    int track_offset = 0;
    CSurf_Context *context;

    WDL_PtrList<MediaTrack> tracks;
    bool hasSolo;
    bool hasMute;

    void UpdateMixerPosition();

    void GetAllVisibleTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute);

    void SetTrackUIVisibility(map<int, bool> &tracks);

    void HandleAllTracksFilter();

    void HandleTracksWithSendsFilter();

    void HandleTracksWithReceivesFilter();

    void HandleTracksWithHardwareOutputsFilter();

    void HandleTracksWithinstrumentsFilter();

    void HandleTracksTopFoldersFilter();

    void HandleTracksAllFoldersFilter();

public:
    CSurf_Navigator(CSurf_Context *context);

    MediaTrack *GetTrackByIndex(int index);

    WDL_PtrList<MediaTrack> GetBankTracks();

    void SetOffset(int offset);

    void IncrementOffset(int count);

    void DecrementOffset(int count);

    void HandlePanEncoderChange(int value);

    bool HasTracksWithSolo();

    bool HasTracksWithMute();

    void handleFilter(NavigatorFilter filter);

    void UpdateTrackCount();

    int GetTrackCount();
};

#endif
