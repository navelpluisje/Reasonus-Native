#ifndef CSURF_NAVIGATOR_H_
#define CSURF_NAVIGATOR_H_

#include <WDL/ptrlist.h>
#include "csurf_context.cpp"
#include <map>
#include <reaper_plugin.h>

using namespace std;

enum NavigatorFilter
{
    /**
     * @brief Select all tracks in the project
     */
    TrackAllFilter,
    /**
     * @brief Select all tracks that have sends
     */
    TrackSendsFilter,
    /**
     * @brief Select all tracks that have receives
     */
    TrackReceivesFilter,
    /**
     * @brief Select all tracks that have a hardware output
     */
    TrackHarwareOutputFilter,
    /**
     * @brief Select all tracks that have a virtual instrument
     */
    TrackInstrumentsFilter,
    /**
     * @brief Select all tracks that have one or more effects
     */
    TrackEffectsFilter,
    /**
     * @brief Select all tracks that are a folder on top level
     */
    TrackTopFoldersFilter,
    /**
     * @brief Select all tracks that are a folder
     */
    TrackAllFoldersFilter,
    /**
     * @brief Select all tracks that have a media item with MIDI
     */
    TrackWithMidiFilter,
    /**
     * @brief Select all tracks that have a media item with Audio
     */
    TrackWithAudioFilter,
    /**
     * @brief Select all tracks that are a VCA Lead
     */
    TrackIsVcaFilter,
    TrackCustomFilter,
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

    void HandleTracksWithInstrumentsFilter();

    void HandleTracksWithEffectsFilter();

    void HandleTracksWithMidiFilter();

    void HandleTracksWithAudioFilter();

    void HandleTracksTopFoldersFilter();

    void HandleTracksAllFoldersFilter();

    void HandleTracksAreVcaFilter();

    void HandleTracksCustomFilter(string filter);

public:
    CSurf_Navigator(CSurf_Context *context);

    MediaTrack *GetTrackByIndex(int index);

    WDL_PtrList<MediaTrack> GetBankTracks();

    void SetOffset(int offset);

    int GetOffset();

    void IncrementOffset(int count);

    void DecrementOffset(int count);

    void HandlePanEncoderChange(int value);

    bool HasTracksWithSolo();

    bool HasTracksWithMute();

    void HandleFilter(NavigatorFilter filter);

    void HandleCustomFilter(string filterName);
};

#endif
