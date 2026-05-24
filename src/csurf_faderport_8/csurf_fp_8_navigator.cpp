#include "csurf_fp_8_navigator.hpp"
#include "../shared/csurf_daw.hpp"
#include "csurf_fp_8_navigator_filters.hpp"

void CSurf_FP_8_Navigator::UpdateMixerPosition() {
    const WDL_PtrList<MediaTrack> bank = GetBankTracks();
    MediaTrack *media_track = bank.Get(0);
    SetMixerScroll(media_track);
    DAW::SetTcpScroll(media_track);
}

void CSurf_FP_8_Navigator::GetAllVisibleTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute) {
    tracks.Empty();
    bool _solo = false;
    bool _mute = false;

    for (int i = 0; i < CountTracks(nullptr); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        const bool visible = DAW::IsTrackVisible(media_track);
        const int solo = static_cast<int>(GetMediaTrackInfo_Value(media_track, "I_SOLO"));
        const bool mute = static_cast<bool>(GetMediaTrackInfo_Value(media_track, "B_MUTE"));

        if (solo > 0 && !_solo) {
            _solo = true;
        }

        if (mute && !_mute) {
            _mute = true;
        }

        if (visible) {
            tracks.Add(media_track);
        }
    }
    hasSolo = _solo;
    hasMute = _mute;
}

void CSurf_FP_8_Navigator::SetTrackUIVisibility(const std::map<int, bool> &tracks) const {
    PreventUIRefresh(1);

    bool set_next_visible_track_selected = false;

    for (const auto &[index, visible]: tracks) {
        MediaTrack *media_track = GetTrack(nullptr, index);
        SetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER", static_cast<double>(visible));

        if (DAW::IsTrackSelected(media_track) && !visible) {
            SetMediaTrackInfo_Value(media_track, "I_SELECTED", 0);
            set_next_visible_track_selected = true;
        }

        if (set_next_visible_track_selected && visible) {
            SetMediaTrackInfo_Value(media_track, "I_SELECTED", 1);
            set_next_visible_track_selected = false;
        }
    }

    PreventUIRefresh(-1);
    TrackList_AdjustWindows(false);
    UpdateArrange();
}

void CSurf_FP_8_Navigator::HandleAllTracksFilter() {
    std::map<int, bool> all_tracks;

    for (int i = 0; i < CountTracks(nullptr); i++) {
        all_tracks[i] = true;
    }

    SetTrackUIVisibility(all_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithSendsFilter() {
    std::map<int, bool> filter_tracks = {};

    for (int i = 0; i < CountTracks(nullptr); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        const bool hasSends = static_cast<bool>(GetTrackNumSends(media_track, 0));
        filter_tracks[i] = hasSends;
    }

    SetTrackUIVisibility(filter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithReceivesFilter() {
    std::map<int, bool> fiter_tracks = {};

    for (int i = 0; i < CountTracks(nullptr); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        const bool hasReceives = static_cast<bool>(GetTrackNumSends(media_track, -1));
        fiter_tracks[i] = hasReceives;
    }

    SetTrackUIVisibility(fiter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithHardwareOutputsFilter() {
    std::map<int, bool> filter_tracks = {};

    for (int i = 0; i < CountTracks(nullptr); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        const bool hasHardwareOutputs = static_cast<bool>(GetTrackNumSends(media_track, 1));
        filter_tracks[i] = hasHardwareOutputs;
    }

    SetTrackUIVisibility(filter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithInstrumentsFilter() {
    std::map<int, bool> filter_tracks = {};

    for (int i = 0; i < CountTracks(nullptr); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        const int hasInstruments = TrackFX_GetInstrument(media_track);

        filter_tracks[i] = hasInstruments > -1;
    }

    SetTrackUIVisibility(filter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithEffectsFilter() {
    std::map<int, bool> filter_tracks = {};

    for (int i = 0; i < CountTracks(nullptr); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        filter_tracks[i] = DAW::TrackHasFx(media_track);
    }

    SetTrackUIVisibility(filter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithMidiFilter() {
    std::map<int, bool> filter_tracks = {};

    for (int i = 0; i < CountTracks(nullptr); i++) {
        bool has_midi = false;
        MediaTrack *media_track = GetTrack(nullptr, i);

        for (int itemidx = 0; itemidx < GetTrackNumMediaItems(media_track); itemidx++) {
            MediaItem *media_item = GetTrackMediaItem(media_track, itemidx);
            has_midi = DAW::MediaItemHasMidi(media_item);
            if (has_midi) {
                break;
            }
        }

        filter_tracks[i] = has_midi;
    }

    SetTrackUIVisibility(filter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithAudioFilter() {
    std::map<int, bool> filter_tracks = {};

    for (int i = 0; i < CountTracks(nullptr); i++) {
        bool has_audio = false;
        MediaTrack *media_track = GetTrack(nullptr, i);

        for (int itemidx = 0; itemidx < GetTrackNumMediaItems(media_track); itemidx++) {
            MediaItem *media_item = GetTrackMediaItem(media_track, itemidx);
            has_audio = DAW::MediaItemHasAudio(media_item);
            if (has_audio) {
                break;
            }
        }

        filter_tracks[i] = has_audio;
    }

    SetTrackUIVisibility(filter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksTopFoldersFilter() {
    std::map<int, bool> filter_tracks = {};
    int depth = 0;

    for (int i = 0; i < CountTracks(nullptr); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        const int folder_depth = static_cast<int>(GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH"));
        depth += folder_depth;

        filter_tracks[i] = false;
        if (depth == 1 && folder_depth == 1) {
            filter_tracks[i] = true;
        }
    }

    SetTrackUIVisibility(filter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksAllFoldersFilter() {
    std::map<int, bool> filter_tracks = {};

    for (int i = 0; i < CountTracks(nullptr); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        const int folder_depth = static_cast<int>(GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH"));
        filter_tracks[i] = folder_depth == 1;
    }

    SetTrackUIVisibility(filter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksAreVcaFilter() {
    std::map<int, bool> filter_tracks = {};

    for (int i = 0; i < CountTracks(nullptr); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        const bool is_vca = GetSetTrackGroupMembership(media_track, "VOLUME_VCA_LEAD", 0, 0) == 1;
        filter_tracks[i] = is_vca;
    }

    SetTrackUIVisibility(filter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksCustomFilter(const std::string &filter_name) {
    const mINI::INIMap<std::string> filter = settings->GetFilter(filter_name);

    std::map<int, bool> custom_filter_tracks = GetCustomFilterTracks(filter);
    std::map<int, bool> allTracks = GetAllTracksBase();

    custom_filter_tracks.insert(allTracks.begin(), allTracks.end());

    SetTrackUIVisibility(custom_filter_tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTrackCustomMultiSelectFilter() {
    std::map<int, bool> all_tracks = GetAllTracksBase();
    std::map<int, bool> filter_tracks;
    std::map<int, bool> tmp_tracks;

    for (const int filter_index: selected_filters) {
        tmp_tracks.clear();
        const mINI::INIMap<std::string> filter = settings->GetFilter(filter_index);
        tmp_tracks = GetCustomFilterTracks(filter);
        filter_tracks.insert(tmp_tracks.begin(), tmp_tracks.end());
    }

    filter_tracks.insert(all_tracks.begin(), all_tracks.end());

    SetTrackUIVisibility(filter_tracks);
    SetOffset(0);
}

bool CSurf_FP_8_Navigator::TrackIsInBankTracks(MediaTrack *media_track) {
    bool value = false;
    const int index = stoi(DAW::GetTrackIndex(media_track));
    const WDL_PtrList<MediaTrack> bank_tracks = GetBankTracks();

    for (int i = 0; i < bank_tracks.GetSize(); i++) {
        const int other_index = stoi(DAW::GetTrackIndex(bank_tracks.Get(i)));

        if (index == other_index) {
            value = true;
            break;
        }
    }
    return value;
}

CSurf_FP_8_Navigator::CSurf_FP_8_Navigator(CSurf_Context *context) : context(context) {
    HandleAllTracksFilter();
    has_mute = false;
    has_solo = false;
}

MediaTrack *CSurf_FP_8_Navigator::GetTrackByIndex(const int index) {
    if (context->GetMasterFaderMode() && index == context->GetNbChannels() - 1) {
        return GetMasterTrack(nullptr);
    }
    const WDL_PtrList<MediaTrack> bank = GetBankTracks();
    return bank.Get(index);
}

WDL_PtrList<MediaTrack> CSurf_FP_8_Navigator::GetBankTracks() {
    WDL_PtrList<MediaTrack> bank;
    GetAllVisibleTracks(tracks, has_solo, has_mute);

    const int channelCount = context->GetNbBankChannels();

    for (int i = track_offset; i < track_offset + channelCount; i++) {
        if (i > track_offset + channelCount) {
            bank.Add(nullptr);
        } else {
            bank.Add(tracks.Get(i));
        }
    }
    return bank;
}

bool CSurf_FP_8_Navigator::IsTrackTouched(const MediaTrack *media_track, const int is_pan) {
    int index = -1;
    const WDL_PtrList<MediaTrack> bank = GetBankTracks();

    for (int i = 0; i < context->GetNbChannels(); i++) {
        if (bank.Get(i) == media_track) {
            index = i;
        }
    }

    if (index == -1) {
        return false;
    }

    if (context->GetChannelMode() == TrackMode && is_pan == 0) {
        return track_touched[index];
    }

    if (context->GetChannelMode() == PanMode1 && is_pan == 1) {
        return track_touched[index];
    }

    if (context->GetChannelMode() == PanMode2 && is_pan == 2) {
        return track_touched[index];
    }

    return false;
}

void CSurf_FP_8_Navigator::SetOffset(const int offset) {
    if (tracks.GetSize() < context->GetNbBankChannels()) {
        track_offset = 0;
    } else if (offset > tracks.GetSize() - context->GetNbBankChannels()) {
        track_offset = tracks.GetSize() - context->GetNbBankChannels();
    } else {
        track_offset = offset;
    }
}

int CSurf_FP_8_Navigator::GetOffset() const {
    return track_offset;
}

void CSurf_FP_8_Navigator::SetOffsetByTrack(MediaTrack *media_track) {
    if (TrackIsInBankTracks(media_track)) {
        return;
    }

    const int trackId = stoi(DAW::GetTrackIndex(media_track));

    for (int i = 0; i < tracks.GetSize(); i++) {
        const int track_id = stoi(DAW::GetTrackIndex(tracks.Get(i)));

        if (trackId == track_id) {
            SetOffset(i / context->GetNbChannels() * context->GetNbChannels());
            break;
        }
    }
}

void CSurf_FP_8_Navigator::IncrementOffset(const int count) {
    if (track_offset + count <= tracks.GetSize() - context->GetNbBankChannels()) {
        track_offset += count;
    } else if (tracks.GetSize() < context->GetNbBankChannels()) {
        track_offset = 0;
    } else {
        track_offset = tracks.GetSize() - context->GetNbBankChannels();
    }
    UpdateMixerPosition();
}

void CSurf_FP_8_Navigator::DecrementOffset(const int count) {
    if (track_offset - count >= 0) {
        track_offset -= count;
    } else {
        track_offset = 0;
    }
    UpdateMixerPosition();
}

void CSurf_FP_8_Navigator::HandlePanEncoderChange(const int value) {
    if (hasBit(value, 6)) {
        DecrementOffset(1);
    }
    if (!hasBit(value, 6) && track_offset < tracks.GetSize() - context->GetNbChannels()) {
        track_offset += 1;
    }
}

bool CSurf_FP_8_Navigator::HasTracksWithSolo() const {
    return has_solo;
}

bool CSurf_FP_8_Navigator::HasTracksWithMute() const {
    return has_mute;
}

void CSurf_FP_8_Navigator::HandleFilter(const NavigatorFilter filter) {
    switch (filter) {
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
            HandleTracksWithInstrumentsFilter();
            break;
        case TrackEffectsFilter:
            HandleTracksWithEffectsFilter();
            break;
        case TrackTopFoldersFilter:
            HandleTracksTopFoldersFilter();
            break;
        case TrackAllFoldersFilter:
            HandleTracksAllFoldersFilter();
            break;
        case TrackWithMidiFilter:
            HandleTracksWithMidiFilter();
            break;
        case TrackWithAudioFilter:
            HandleTracksWithAudioFilter();
            break;
        case TrackIsVcaFilter:
            HandleTracksAreVcaFilter();
            break;
        case TrackCustomMultiSelectFilter:
            HandleTrackCustomMultiSelectFilter();
            break;
        default:
            HandleAllTracksFilter();
    }
}

void CSurf_FP_8_Navigator::HandleCustomFilter(const std::string &filterName) {
    HandleTracksCustomFilter(filterName);
}

void CSurf_FP_8_Navigator::SetTrackTouched(const int index, const bool value) {
    track_touched[index] = value;
}

void CSurf_FP_8_Navigator::SetMultiSelectFilter(const bool value) {
    multi_select_filter = value;

    if (!value) {
        selected_filters.clear();
    }
}

bool CSurf_FP_8_Navigator::GetMultiSelectFilter() const {
    return multi_select_filter;
}

void CSurf_FP_8_Navigator::AddFilter(const int filter_index) {
    selected_filters.push_back(filter_index);
}

void CSurf_FP_8_Navigator::ToggleFilter(const int filter_index) {
    if (HasFilter(filter_index)) {
        RemoveFilter(filter_index);
    } else {
        AddFilter(filter_index);
    }
}

void CSurf_FP_8_Navigator::RemoveFilter(const int filter_index) {
    selected_filters.erase(find(selected_filters.begin(), selected_filters.end(), filter_index));
}

bool CSurf_FP_8_Navigator::HasFilter(const int filter_index) {
    return distance(
               selected_filters.end(),
               find(selected_filters.begin(), selected_filters.end(), filter_index)
           ) != 0;
}
