#include "../shared/csurf_daw.hpp"
#include <vector>
#include <string>
#include "../shared/csurf_utils.hpp"

int DAW::sendModes[3] = {0, 1, 3};

int csurfRound(double val)
{
    return (int)(val + 0.5);
}

/************************************************************************
 * Track
 ************************************************************************/
void DAW::UnSelectAllTracks()
{
    for (int i = 0; i < ::CountTracks(0); i++)
    {
        MediaTrack *media_track = ::GetTrack(0, i);
        ::SetTrackSelected(media_track, false);
    }
    MediaTrack *media_track = ::GetMasterTrack(0);
    SetTrackSelected(media_track, false);
};

bool DAW::IsTrackArmed(MediaTrack *media_track)
{
    int flagsOut;
    ::GetTrackState(media_track, &flagsOut);

    return hasBit(flagsOut, 6);
};

bool DAW::IsTrackMuted(MediaTrack *media_track)
{
    int flagsOut;
    GetTrackState(media_track, &flagsOut);

    return hasBit(flagsOut, 3);
};

bool DAW::IsTrackSelected(MediaTrack *media_track)
{
    return ::IsTrackSelected(media_track);
}

bool DAW::IsTrackSoloed(MediaTrack *media_track)
{
    int flagsOut;
    GetTrackState(media_track, &flagsOut);

    return hasBit(flagsOut, 4);
};

int DAW::GetTrackPanMode(MediaTrack *media_track)
{
    int panMode = 0;
    double pan1, pan2 = 0.0;

    GetTrackUIPan(media_track, &pan1, &pan2, &panMode);

    return panMode;
}

std::string DAW::GetTrackName(MediaTrack *media_track)
{
    if (!media_track)
    {
        return "";
    }

    char trackName[256];
    ::GetTrackName(media_track, trackName, sizeof(trackName));

    return std::string(trackName);
}

std::string DAW::GetTrackIndex(MediaTrack *media_track)
{
    return std::to_string((int)GetMediaTrackInfo_Value(media_track, "IP_TRACKNUMBER"));
}

std::string DAW::GetTrackInputName(MediaTrack *media_track)
{
    int inputCode = int(::GetMediaTrackInfo_Value(media_track, "I_RECINPUT"));
    if (hasBit(inputCode, 12))
    {
        char midiDeviceName[256];
        int midiDeviceId = (inputCode >> 5) & ((1 << 6) - 1);
        ::GetMIDIInputName(midiDeviceId, midiDeviceName, sizeof(midiDeviceName));
        return std::string(midiDeviceName);
    }
    else
    {
        int inputChannel = inputCode & ((1 << 5) - 1);
        const char *inputName = ::GetInputChannelName(inputChannel);
        return std::string(inputName);
    }
}

std::string DAW::GetTrackMonitorMode(MediaTrack *media_track)
{
    int monitorMode = int(::GetMediaTrackInfo_Value(media_track, "I_RECMON"));

    switch (monitorMode)
    {
    case 0:
        return "Mon OFF";

    case 1:
        return "Mon ON";

    case 2:
        return "Mon !Play";

    default:
        return "";
    }
}

std::string DAW::GetTrackRecordingMode(MediaTrack *media_track)
{
    int recordingMode = int(::GetMediaTrackInfo_Value(media_track, "I_RECMODE"));

    switch (recordingMode)
    {
    case 0:
        return "Rec inp";

    case 1:
        return "Rec Stereo";

    case 2:
        return "Rec None";

    case 3:
        return "Rec St Comp";

    case 4:
        return "Rec Midi";

    case 5:
        return "Rec Mono";

    case 6:
        return "Rec Mn Comp";

    case 7:
        return "Rec Midi Od";

    case 8:
        return "Rec Midi Rep";

    default:
        return "";
    }
}

ButtonColor DAW::GetTrackColor(MediaTrack *media_track)
{
    ButtonColor color;
    int red = 0x7f;
    int green = 0x7f;
    int blue = 0x7f;

    int track_color = ::GetTrackColor(media_track);
    if (track_color != 0)
    {
        ColorFromNative(track_color, &red, &green, &blue);
    }
    color.SetColor(red / 2, green / 2, blue / 2);

    return color;
}

void DAW::SetSelectedTracksRange(MediaTrack *media_track)
{
    int index = (int)GetMediaTrackInfo_Value(media_track, "IP_TRACKNUMBER");
    int selected_track_count = CountSelectedTracks(0);
    int first_index = (int)GetMediaTrackInfo_Value(GetSelectedTrack(0, 0), "IP_TRACKNUMBER");
    int last_index = (int)GetMediaTrackInfo_Value(GetSelectedTrack(0, selected_track_count - 1), "IP_TRACKNUMBER");
    int start_index = index < first_index
                          ? index
                      : index < last_index
                          ? first_index
                          : last_index;
    int end_index = index < first_index ? first_index : index;

    for (int i = start_index - 1; i < end_index; i++)
    {
        MediaTrack *selectable_track = GetTrack(0, i);
        SetTrackSelected(selectable_track, true);
    }
}

double DAW::GetTrackPeakInfo(MediaTrack *media_track)
{
    if (media_track)
    {
        return (::Track_GetPeakInfo(media_track, 0) + ::Track_GetPeakInfo(media_track, 1)) / 2;
    }
    return 0.0;
}

int DAW::GetTrackSurfacePeakInfo(MediaTrack *media_track)
{
    return int(volToNormalized(GetTrackPeakInfo(media_track)) * 127.0);
}

/************************************************************************
 * Track FX
 ************************************************************************/
bool DAW::TrackHasFx(MediaTrack *media_track)
{
    bool hasFx = false;
    char pluginName[256];

    for (int i = 0; i < ::TrackFX_GetCount(media_track); i++)
    {
        ::TrackFX_GetFXName(media_track, i, pluginName, std::size(pluginName));
        hasFx = IsPluginFX(pluginName);
        if (hasFx)
        {
            break;
        }
    }

    return hasFx;
}
bool DAW::HasTrackFx(MediaTrack *media_track, int fx)
{
    char pluginName[256] = "";
    return TrackFX_GetFXName(media_track, fx, pluginName, std::size(pluginName));
}

std::string DAW::GetTrackFxName(MediaTrack *media_track, int fx, bool full)
{
    char pluginName[256] = "";
    if (!::TrackFX_GetFXName(media_track, fx, pluginName, std::size(pluginName)))
    {
        return "No FX";
    }
    if (full)
    {
        return StripPluginNamePrefixes(pluginName);
    }
    std::vector<std::string> pluginNameParts = split(StripPluginNamePrefixes(StripPluginChannelPostfix(pluginName).data()), " (");
    if (pluginNameParts.size() > 1)
    {
        pluginNameParts.pop_back();
    }

    return join(pluginNameParts, " (");
}

std::string DAW::GetTrackFxDeveloper(MediaTrack *media_track, int fx)
{
    char pluginName[256] = "";
    if (!::TrackFX_GetFXName(media_track, fx, pluginName, std::size(pluginName)))
    {
        return "No Dev";
    }
    std::vector<std::string> pluginNameParts = split(StripPluginNamePrefixes(StripPluginChannelPostfix(pluginName).data()), " (");
    std::string developer = pluginNameParts.at(pluginNameParts.size() - 1);
    if (!developer.empty())
    {
        developer.pop_back();
    }

    return developer;
}

bool DAW::GetTrackFxEnabled(MediaTrack *media_track, int fx)
{
    return ::TrackFX_GetEnabled(media_track, fx);
}

bool DAW::GetTrackFxOffline(MediaTrack *media_track, int fx)
{
    return ::TrackFX_GetOffline(media_track, fx);
}

std::string DAW::GetTrackFxSurfceEnabled(MediaTrack *media_track, int fx)
{
    return ::TrackFX_GetEnabled(media_track, fx) ? "Enabled" : "Bypassed";
}

bool DAW::GetTrackFxPanelOpen(MediaTrack *media_track, int fx)
{
    if (::TrackFX_GetCount(media_track) < 1)
    {
        return false;
    }
    return ::TrackFX_GetOpen(media_track, fx);
}

bool DAW::GetTrackFxBypassed(MediaTrack *media_track)
{
    return !(bool)GetMediaTrackInfo_Value(media_track, "I_FXEN");
}

void DAW::ToggleTrackFxBypass(MediaTrack *media_track)
{
    if (DAW::GetTrackFxBypassed(media_track))
    {
        SetMediaTrackInfo_Value(media_track, "I_FXEN", 1.0);
    }
    else
    {
        SetMediaTrackInfo_Value(media_track, "I_FXEN", 0.0);
    }
}

/************************************************************************
 * Track FX Param
 ************************************************************************/
std::string DAW::GetTrackFxParamName(MediaTrack *media_track, int fx, int param)
{
    char paramName[256] = "";
    if (TrackFX_GetParamName(media_track, fx, param, paramName, std::size(paramName)))
    {
        return paramName;
    }
    return "No FX";
}

int DAW::GetTrackFxParamNbSteps(MediaTrack *media_track, int fx, int param)
{
    int nbSteps = 1;
    double stepOut, _;
    bool isToggle;
    if (TrackFX_GetParameterStepSizes(media_track, fx, param, &stepOut, &_, &_, &isToggle))
    {
        if (stepOut > 0)
        {
            nbSteps = csurfRound(1.0 / stepOut) + 1;
        }
    }
    return nbSteps;
}

void DAW::SetTrackFXParamUntouched(MediaTrack *media_track, int fx)
{
    TrackFX_SetNamedConfigParm(media_track, fx, "last_touched", "-1");
}

/************************************************************************
 * Track Receive
 ************************************************************************/
bool DAW::HasTrackReceive(MediaTrack *media_track, int receive)
{
    return GetSetTrackSendInfo(media_track, -1, receive, "P_SRCTRACK", 0) ? true : false;
}

std::string DAW::GetTrackReceiveSrcName(MediaTrack *media_track, int receive)
{
    MediaTrack *srcTrack = (MediaTrack *)GetSetTrackSendInfo(media_track, -1, receive, "P_SRCTRACK", 0);
    if (srcTrack)
    {
        return GetTrackName(srcTrack);
    }
    return "No Source";
}

int DAW::GetTrackReceiveMode(MediaTrack *media_track, int receive)
{
    return (int)GetTrackSendInfo_Value(media_track, -1, receive, "I_SENDMODE");
}

int DAW::GetTrackReceiveAutoMode(MediaTrack *media_track, int receive)
{
    return (int)GetTrackSendInfo_Value(media_track, -1, receive, "I_AUTOMODE");
}

std::string DAW::GetTrackSurfaceReceiveMode(MediaTrack *media_track, int receive)
{
    return GetSendModeString(GetTrackReceiveMode(media_track, receive));
}

std::string DAW::GetTrackSurfaceReceiveAutoMode(MediaTrack *media_track, int receive)
{
    return GetAutomationString(GetTrackReceiveAutoMode(media_track, receive));
}

bool DAW::GetTrackReceiveMute(MediaTrack *media_track, int receive)
{
    bool receiveMute = false;
    GetTrackReceiveUIMute(media_track, receive, &receiveMute);
    return receiveMute;
}

bool DAW::GetTrackReceivePhase(MediaTrack *media_track, int receive)
{
    return (bool)GetTrackSendInfo_Value(media_track, -1, receive, "B_PHASE");
}

bool DAW::GetTrackReceiveMono(MediaTrack *media_track, int receive)
{
    return (bool)GetTrackSendInfo_Value(media_track, -1, receive, "B_Mono");
}

int DAW::GetNextTrackReceiveMode(MediaTrack *media_track, int send)
{
    return sendModes[(GetTrackReceiveMode(media_track, send) + 1) % 4];
}

/************************************************************************
 * Track Send
 ************************************************************************/
bool DAW::HasTrackSend(MediaTrack *media_track, int send)
{
    return GetSetTrackSendInfo(media_track, 0, send, "P_DESTTRACK", 0) ? true : false;
}

std::string DAW::GetTrackSendDestName(MediaTrack *media_track, int send)
{
    MediaTrack *destTrack = (MediaTrack *)GetSetTrackSendInfo(media_track, 0, send, "P_DESTTRACK", 0);
    if (destTrack)
    {
        return GetTrackName(destTrack);
    }
    return "No Dest";
}

int DAW::GetTrackSendMode(MediaTrack *media_track, int send)
{
    return (int)GetTrackSendInfo_Value(media_track, 0, send, "I_SENDMODE");
}

int DAW::GetTrackSendAutoMode(MediaTrack *media_track, int send)
{
    return (int)GetTrackSendInfo_Value(media_track, 0, send, "I_AUTOMODE");
}

std::string DAW::GetTrackSurfaceSendMode(MediaTrack *media_track, int send)
{
    return GetSendModeString(GetTrackSendMode(media_track, send));
}

std::string DAW::GetTrackSurfaceSendAutoMode(MediaTrack *media_track, int send)
{
    return GetAutomationString(GetTrackSendAutoMode(media_track, send));
}

bool DAW::GetTrackSendMute(MediaTrack *media_track, int send)
{
    bool sendMute = false;
    GetTrackSendUIMute(media_track, send, &sendMute);
    return sendMute;
}

bool DAW::GetTrackSendPhase(MediaTrack *media_track, int send)
{
    return (bool)GetTrackSendInfo_Value(media_track, 0, send, "B_PHASE");
}

bool DAW::GetTrackSendMono(MediaTrack *media_track, int send)
{
    return (bool)GetTrackSendInfo_Value(media_track, 0, send, "B_MONO");
}

int DAW::GetNextTrackSendMode(MediaTrack *media_track, int send)
{
    return sendModes[(GetTrackSendMode(media_track, send) + 1) % 4];
}

/************************************************************************
 * Media Item
 ************************************************************************/

bool DAW::MediaItemHasMidi(MediaItem *media_item)
{
    bool hasMidi = false;
    for (int i = 0; i < ::GetMediaItemNumTakes(media_item); i++)
    {
        MediaItem_Take *take = ::GetMediaItemTake(media_item, i);
        hasMidi = ::TakeIsMIDI(take);
        if (hasMidi)
        {
            break;
        }
    }

    return hasMidi;
}

bool DAW::MediaItemHasAudio(MediaItem *media_item)
{
    bool hasAudio = false;
    for (int i = 0; i < ::GetMediaItemNumTakes(media_item); i++)
    {
        MediaItem_Take *take = ::GetMediaItemTake(media_item, i);
        hasAudio = !::TakeIsMIDI(take);
        if (hasAudio)
        {
            break;
        }
    }

    return hasAudio;
}

/************************************************************************
 * Project
 ************************************************************************/
int DAW::GetProjectTimeMode()
{
    int TimeModeIndex = ::projectconfig_var_getoffs("projtimemode2", 0);
    int *timeMode2Ptr_ = (int *)projectconfig_var_addr(0, TimeModeIndex);
    // Get theh actrual value and not the pointer
    int val = *timeMode2Ptr_;

    // This will get rid of all the second measure data
    return minmax(0, val, 256) & 255;
}

int DAW::GetProjectMeasureOffset()
{
    int TimeModeIndex = ::projectconfig_var_getoffs("projmeasoffs", 0);
    int *timeMode2Ptr_ = (int *)projectconfig_var_addr(0, TimeModeIndex);
    // Get theh actrual value and not the pointer
    int val = *timeMode2Ptr_;

    return val;
}

double DAW::GetProjectTimeOffset()
{
    int TimeModeIndex = ::projectconfig_var_getoffs("projtimeoffs", 0);
    double *timeMode2Ptr_ = (double *)projectconfig_var_addr(0, TimeModeIndex);
    // Get theh actrual value and not the pointer
    double val = *timeMode2Ptr_;

    return val;
}

std::vector<std::string> GetTimeSegments(double tpos, int proj_time_mode)
{
    char beatTime[128];
    std::vector<std::string> value, hours, mins;

    ::format_timestr_len(tpos, beatTime, sizeof beatTime, 0, proj_time_mode > -1 ? proj_time_mode : 2);

    switch (proj_time_mode)
    {
    case -1: // Ruler, will be converted to Beat
    case 1:  // Beats + Time
    case 2:  // Beats
        value = split(std::string(beatTime), ".");
        // The first 2 items are 0-based so need an extra 1
        value[0] = std::to_string(std::stoi(value[0]) + 1);
        value[1] = std::to_string(std::stoi(value[1]) + 1);
        break;

    case 0: // Minutes:Seconds
        hours = split(std::string(beatTime), ":");
        mins = split(hours[1], ".");
        value.push_back(hours.at(0));
        value.push_back(mins.at(0));
        value.push_back(mins.at(1));
        break;

    case 5: // Hours:Minutes:Seconds:Frames
        value = split(std::string(beatTime), ":");
        break;

    case 3: // Seconds
        value = split(std::string(beatTime), ".");
        break;

    case 4: // Samples
    case 8: // Absolute Frames
        value = cutString(beatTime, 3);
        break;
    }

    return value;
}

std::vector<std::string> DAW::GetProjectTime(bool overwrite_time_code, int new_time_code)
{
    double play_position, play_offset;
    double beat_length = 0, __ = 0;
    int *_ = 0;

    int proj_time_mode = new_time_code;
    if (!overwrite_time_code)
    {
        proj_time_mode = DAW::GetProjectTimeMode();
    }

    int play_state = GetPlayStateEx(0);
    bool is_playing = hasBit(play_state, 0);

    if (is_playing)
    {
        play_position = ::GetPlayPositionEx(0);
    }
    else
    {
        play_position = ::GetCursorPositionEx(0);
    }

    switch (proj_time_mode)
    {
    case -1:
    case 1:
    case 2:
        ::TimeMap_GetMeasureInfo(0, 0, &__, &beat_length, _, _, &__);
        play_offset = ::TimeMap2_beatsToTime(0, (int)beat_length * GetProjectMeasureOffset(), _);
        break;

    default:
        play_offset = GetProjectTimeOffset();
    }

    return GetTimeSegments(play_position + play_offset, proj_time_mode);
}

/************************************************************************
 * Edit
 ************************************************************************/
void DAW::EditSave()
{
    ::Main_SaveProject(0, false);
}

void DAW::EditUndo()
{
    ::Undo_DoUndo2(0);
}

void DAW::EditRedo()
{
    ::Undo_DoRedo2(0);
}
