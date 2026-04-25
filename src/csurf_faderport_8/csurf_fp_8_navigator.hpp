#ifndef CSURF_FP_8_NAVIGATOR_H_
#define CSURF_FP_8_NAVIGATOR_H_

#include <WDL/ptrlist.h>
#include <map>
#include "../shared/csurf_reasonus_settings.hpp"
// #include <reaper_plugin.h>
// #include "../shared/csurf_utils.hpp"
#include "../shared/csurf_context.cpp"

enum NavigatorFilter {
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
    TrackCustomMultiSelectFilter,
};

class CSurf_FP_8_Navigator {
    int track_offset = 0;
    CSurf_Context *context;
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_8);

    WDL_PtrList<MediaTrack> tracks;
    bool has_solo;
    bool has_mute;
    bool track_touched[16] = {
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
    };
    bool multi_select_filter = false;
    std::vector<int> selected_filters;

    void UpdateMixerPosition();

    void GetAllVisibleTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute);

    void SetTrackUIVisibility(const std::map<int, bool> &tracks) const;

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

    void HandleTracksCustomFilter(const std::string &filter_name);

    void HandleTrackCustomMultiSelectFilter();

    bool TrackIsInBankTracks(MediaTrack *media_track);

public:
    explicit CSurf_FP_8_Navigator(CSurf_Context *context);

    MediaTrack *GetTrackByIndex(int index);

    WDL_PtrList<MediaTrack> GetBankTracks();

    bool IsTrackTouched(const MediaTrack *media_track, int is_pan);

    void SetOffset(int offset);

    [[nodiscard]] int GetOffset() const;

    void SetOffsetByTrack(MediaTrack *media_track);

    void IncrementOffset(int count);

    void DecrementOffset(int count);

    void HandlePanEncoderChange(int value);

    [[nodiscard]] bool HasTracksWithSolo() const;

    [[nodiscard]] bool HasTracksWithMute() const;

    void HandleFilter(NavigatorFilter filter);

    void HandleCustomFilter(const std::string &filterName);

    void SetTrackTouched(int index, bool value);

    void SetMultiSelectFilter(bool value);

    bool GetMultiSelectFilter() const;

    void AddFilter(int filter_index);

    void ToggleFilter(int filter_index);

    void RemoveFilter(int filter_index);

    bool HasFilter(int filter_index);
};

#endif
