#include "csurf_navigator.hpp"
#include "csurf_utils.hpp"
#include "csurf_daw.hpp"
#include <reaper_plugin_functions.h>
#include <mINI/ini.h>
#include "csurf_navigator_filters.hpp"

void CSurf_Navigator::GetAllVisibleTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute)
{
    tracks.Empty();
    bool _solo = false;
    bool _mute = false;

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool visible = (bool)GetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER");
        int solo = GetMediaTrackInfo_Value(media_track, "I_SOLO");
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

void CSurf_Navigator::SetTrackUIVisibility(map<int, bool> &tracks)
{
    PreventUIRefresh(1);

    for (auto const &track : tracks)
    {
        MediaTrack *media_track = GetTrack(0, track.first);
        SetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER", track.second);
    }

    PreventUIRefresh(-1);
    TrackList_AdjustWindows(false);
    UpdateArrange();
}

void CSurf_Navigator::HandleAllTracksFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        tracks[i] = true;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksCustomFilter(string filterName)
{
    mINI::INIFile file(GetReaSonusIniPath());
    mINI::INIStructure ini;
    file.read(ini);
    mINI::INIMap<string> filter = ini[filterName];

    map<int, bool> tracks;
    map<int, bool> filterTracks;
    map<int, bool> allTracks;
    map<int, bool> childTracks;
    map<int, bool> parentTracks;
    map<int, bool> sibblingTracks;

    bool stop = false;
    for (int i = 0; i < CountTracks(0); i++)
    {
        allTracks[i] = false;
        if (!stop)
        {
            MediaTrack *media_track = GetTrack(0, i);
            string trackName = DAW::GetTrackName(media_track);
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
}

void CSurf_Navigator::HandleTracksWithSendsFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool hasSends = (bool)::GetTrackNumSends(media_track, 0);
        tracks[i] = hasSends;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksWithReceivesFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool hasReceives = (bool)::GetTrackNumSends(media_track, -1);
        tracks[i] = hasReceives;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksWithHardwareOutputsFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool hasHardwareOutputs = (bool)::GetTrackNumSends(media_track, 1);
        tracks[i] = hasHardwareOutputs;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksWithInstrumentsFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        int hasInstruments = ::TrackFX_GetInstrument(media_track);

        tracks[i] = hasInstruments > -1;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksWithEffectsFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        tracks[i] = DAW::TrackHasFx(media_track);
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksWithMidiFilter()
{
    map<int, bool> tracks = {};

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
}

void CSurf_Navigator::HandleTracksWithAudioFilter()
{
    map<int, bool> tracks = {};

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
}

void CSurf_Navigator::HandleTracksAreVcaFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        bool isVCA = GetSetTrackGroupMembership(media_track, "VOLUME_VCA_LEAD", 0, 0) == 1;
        tracks[i] = isVCA;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::HandleTracksTopFoldersFilter()
{
    map<int, bool> tracks = {};
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
}

void CSurf_Navigator::HandleTracksAllFoldersFilter()
{
    map<int, bool> tracks = {};

    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        int folderDepth = (int)GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH");
        tracks[i] = folderDepth == 1;
    }

    SetTrackUIVisibility(tracks);
}

void CSurf_Navigator::UpdateMixerPosition()
{
    MediaTrack *media_track = GetTrack(0, track_offset);
    SetMixerScroll(media_track);
};

CSurf_Navigator::CSurf_Navigator(CSurf_Context *context) : context(context)
{
    HandleAllTracksFilter();
};

MediaTrack *CSurf_Navigator::GetTrackByIndex(int index)
{
    WDL_PtrList<MediaTrack> bank = GetBankTracks();
    return bank.Get(index);
}

WDL_PtrList<MediaTrack> CSurf_Navigator::GetBankTracks()
{
    WDL_PtrList<MediaTrack> bank;
    GetAllVisibleTracks(tracks, hasSolo, hasMute);
    int channelCount = context->GetNbChannels() > tracks.GetSize() ? context->GetNbChannels() : tracks.GetSize();
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

void CSurf_Navigator::SetOffset(int offset)
{
    if (tracks.GetSize() < context->GetNbChannels())
    {
        track_offset = 0;
    }
    else if (offset > (tracks.GetSize() - context->GetNbChannels()))
    {
        track_offset = tracks.GetSize() - context->GetNbChannels();
    }
    else
    {
        track_offset = offset;
    }
}

int CSurf_Navigator::GetOffset()
{
    return track_offset;
}

void CSurf_Navigator::IncrementOffset(int count)
{
    if ((track_offset + count) <= (tracks.GetSize() - context->GetNbChannels()))
    {
        track_offset += count;
    }
    else if (tracks.GetSize() < context->GetNbChannels())
    {
        track_offset = 0;
    }
    else
    {
        track_offset = tracks.GetSize() - context->GetNbChannels();
    }
    UpdateMixerPosition();
}

void CSurf_Navigator::DecrementOffset(int count)
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

void CSurf_Navigator::HandlePanEncoderChange(int value)
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

bool CSurf_Navigator::HasTracksWithSolo()
{
    return hasSolo;
};

bool CSurf_Navigator::HasTracksWithMute()
{
    return hasMute;
};

void CSurf_Navigator::HandleFilter(NavigatorFilter filter)
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

void CSurf_Navigator::HandleCustomFilter(string filterName)
{
    HandleTracksCustomFilter(filterName);
}
