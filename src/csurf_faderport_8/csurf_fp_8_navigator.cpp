#include "csurf_fp_8_navigator.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"
#include <mini/ini.h>
#include "csurf_fp_8_navigator_filters.hpp"

void CSurf_FP_8_Navigator::GetAllVisibleTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute)
{
    tracks.Empty();
    bool _solo = false;
    bool _mute = false;

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool visible = (bool)GetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER");
        int solo = (int)GetMediaTrackInfo_Value(media_track, "I_SOLO");
        bool mute = (bool)GetMediaTrackInfo_Value(media_track, "B_MUTE");

        if (solo > 0 && !_solo)
        {
            _solo = true;
        }

        if (mute && !_mute)
        {
            _mute = true;
        }

        if (visible)
        {
            tracks.Add(media_track);
        }
    }
    hasSolo = _solo;
    hasMute = _mute;
}

void CSurf_FP_8_Navigator::SetTrackUIVisibility(std::map<int, bool> &tracks)
{
    PreventUIRefresh(1);

    bool set_next_visible_track_selected = false;

    for (auto const &track : tracks)
    {
        MediaTrack *media_track = GetTrack(0, track.first);
        SetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER", track.second);

        if (DAW::IsTrackSelected(media_track) && !track.second)
        {
            SetMediaTrackInfo_Value(media_track, "I_SELECTED", 0);
            set_next_visible_track_selected = true;
        }

        if (set_next_visible_track_selected && track.second)
        {
            SetMediaTrackInfo_Value(media_track, "I_SELECTED", 1);
            set_next_visible_track_selected = false;
        }
    }

    PreventUIRefresh(-1);
    TrackList_AdjustWindows(false);
    UpdateArrange();
}

void CSurf_FP_8_Navigator::HandleAllTracksFilter()
{
    std::map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        tracks[i] = true;
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksCustomFilter(std::string filterName)
{
    mINI::INIFile file(GetReaSonusIniPath(FP_8));
    mINI::INIStructure ini;
    file.read(ini);
    mINI::INIMap<std::string> filter = ini[filterName];

    std::map<int, bool> tracks;
    std::map<int, bool> filterTracks;
    std::map<int, bool> allTracks;
    std::map<int, bool> childTracks;
    std::map<int, bool> parentTracks;
    std::map<int, bool> sibblingTracks;

    bool stop = false;
    for (int i = 0; i < CountTracks(0); i++)
    {
        allTracks[i] = false;
        if (!stop)
        {
            MediaTrack *media_track = GetTrack(0, i);
            std::string trackName = DAW::GetTrackName(media_track);
            bool isChild = GetTrackDepth(media_track) > 0;

            if (FuzzyMatch(trackName, filter["text"]) && !(isChild && filter["top-level"] == "1"))
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

    tracks.insert(allTracks.begin(), allTracks.end());

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithSendsFilter()
{
    std::map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool hasSends = (bool)::GetTrackNumSends(media_track, 0);
        tracks[i] = hasSends;
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithReceivesFilter()
{
    std::map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool hasReceives = (bool)::GetTrackNumSends(media_track, -1);
        tracks[i] = hasReceives;
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithHardwareOutputsFilter()
{
    std::map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool hasHardwareOutputs = (bool)::GetTrackNumSends(media_track, 1);
        tracks[i] = hasHardwareOutputs;
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithInstrumentsFilter()
{
    std::map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        int hasInstruments = ::TrackFX_GetInstrument(media_track);

        tracks[i] = hasInstruments > -1;
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithEffectsFilter()
{
    std::map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        tracks[i] = DAW::TrackHasFx(media_track);
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithMidiFilter()
{
    std::map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        bool hasMidi = false;
        MediaTrack *media_track = GetTrack(0, i);

        for (int i = 0; i < ::GetTrackNumMediaItems(media_track); i++)
        {
            MediaItem *media_item = ::GetTrackMediaItem(media_track, i);
            hasMidi = DAW::MediaItemHasMidi(media_item);
            if (hasMidi)
            {
                break;
            }
        }

        tracks[i] = hasMidi;
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksWithAudioFilter()
{
    std::map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        bool hasAudio = false;
        MediaTrack *media_track = GetTrack(0, i);

        for (int i = 0; i < ::GetTrackNumMediaItems(media_track); i++)
        {
            MediaItem *media_item = ::GetTrackMediaItem(media_track, i);
            hasAudio = DAW::MediaItemHasAudio(media_item);
            if (hasAudio)
            {
                break;
            }
        }

        tracks[i] = hasAudio;
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksAreVcaFilter()
{
    std::map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool isVCA = GetSetTrackGroupMembership(media_track, "VOLUME_VCA_LEAD", 0, 0) == 1;
        tracks[i] = isVCA;
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksTopFoldersFilter()
{
    std::map<int, bool> tracks = {};
    int depth = 0;

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        int folderDepth = (int)GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH");
        depth += folderDepth;

        tracks[i] = false;
        if (depth == 1 && folderDepth == 1)
        {
            tracks[i] = true;
        }
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::HandleTracksAllFoldersFilter()
{
    std::map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        int folderDepth = (int)GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH");
        tracks[i] = folderDepth == 1;
    }

    SetTrackUIVisibility(tracks);
    SetOffset(0);
}

void CSurf_FP_8_Navigator::UpdateMixerPosition()
{
    WDL_PtrList<MediaTrack> bank = GetBankTracks();
    MediaTrack *media_track = bank.Get(0);
    SetMixerScroll(media_track);
    DAW::SetTcpScroll(media_track);
};

CSurf_FP_8_Navigator::CSurf_FP_8_Navigator(CSurf_Context *context) : context(context)
{
    HandleAllTracksFilter();
};

MediaTrack *CSurf_FP_8_Navigator::GetTrackByIndex(int index)
{
    if (context->GetMasterFaderMode() && index == context->GetNbChannels() - 1)
    {
        return ::GetMasterTrack(0);
    }
    WDL_PtrList<MediaTrack> bank = GetBankTracks();
    return bank.Get(index);
}

WDL_PtrList<MediaTrack> CSurf_FP_8_Navigator::GetBankTracks()
{
    WDL_PtrList<MediaTrack> bank;
    GetAllVisibleTracks(tracks, hasSolo, hasMute);

    int channelCount = context->GetNbBankChannels() > tracks.GetSize() ? context->GetNbBankChannels() : tracks.GetSize();

    for (int i = track_offset; i < track_offset + channelCount; i++)
    {
        if (i > track_offset + channelCount)
        {
            bank.Add(NULL);
        }
        else
        {
            bank.Add(tracks.Get(i));
        }
    }
    return bank;
}

bool CSurf_FP_8_Navigator::IsTrackTouched(MediaTrack *media_track)
{
    int index = -1;
    WDL_PtrList<MediaTrack> bank = GetBankTracks();
    std::string searchId = DAW::GetTrackIndex(media_track);

    for (int i = 0; i < context->GetNbChannels(); i++)
    {
        std::string id = DAW::GetTrackIndex(bank.Get(i));
        if (id.compare(searchId) == 0)
        {
            index = i;
        }
    }

    return trackTouched[index];
}

void CSurf_FP_8_Navigator::SetOffset(int offset)
{
    if (tracks.GetSize() < context->GetNbBankChannels())
    {
        track_offset = 0;
    }
    else if (offset > (tracks.GetSize() - context->GetNbBankChannels()))
    {
        track_offset = tracks.GetSize() - context->GetNbBankChannels();
    }
    else
    {
        track_offset = offset;
    }
}

int CSurf_FP_8_Navigator::GetOffset()
{
    return track_offset;
}

void CSurf_FP_8_Navigator::SetOffsetByTrack(MediaTrack *media_track)
{
    int trackId = stoi(DAW::GetTrackIndex(media_track));

    for (int i = 0; tracks.GetSize(); i++)
    {
        int id = stoi(DAW::GetTrackIndex(media_track));

        if (trackId == id)
        {
            SetOffset(i);
            break;
        }
    }
}

void CSurf_FP_8_Navigator::IncrementOffset(int count)
{
    if ((track_offset + count) <= (tracks.GetSize() - context->GetNbBankChannels()))
    {
        track_offset += count;
    }
    else if (tracks.GetSize() < context->GetNbBankChannels())
    {
        track_offset = 0;
    }
    else
    {
        track_offset = tracks.GetSize() - context->GetNbBankChannels();
    }
    UpdateMixerPosition();
}

void CSurf_FP_8_Navigator::DecrementOffset(int count)
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

void CSurf_FP_8_Navigator::HandlePanEncoderChange(int value)
{
    if (hasBit(value, 6))
    {
        DecrementOffset(1);
    }
    if (!hasBit(value, 6) && track_offset < (tracks.GetSize() - context->GetNbChannels()))
    {
        track_offset += 1;
    }
}

bool CSurf_FP_8_Navigator::HasTracksWithSolo()
{
    return hasSolo;
};

bool CSurf_FP_8_Navigator::HasTracksWithMute()
{
    return hasMute;
};

void CSurf_FP_8_Navigator::HandleFilter(NavigatorFilter filter)
{
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
    default:
        HandleAllTracksFilter();
    }
}

void CSurf_FP_8_Navigator::HandleCustomFilter(std::string filterName)
{
    HandleTracksCustomFilter(filterName);
}

void CSurf_FP_8_Navigator::SetTrackTouched(int index, bool value)
{
    trackTouched[index] = value;
}
