#include "../shared/csurf_daw.hpp"
#include <vector>
#include <string>
#include "../shared/csurf_utils.hpp"
#include "csurf.h"

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
}

bool DAW::IsTrackArmed(MediaTrack *media_track)
{
    int flagsOut;
    ::GetTrackState(media_track, &flagsOut);

    return hasBit(flagsOut, 6);
}

void DAW::ToggleTrackArmed(MediaTrack *media_track)
{
    ::CSurf_SetSurfaceRecArm(media_track, CSurf_OnRecArmChange(media_track, !DAW::IsTrackArmed(media_track)), NULL);
}

bool DAW::IsTrackMuted(MediaTrack *media_track)
{
    int flagsOut;
    GetTrackState(media_track, &flagsOut);

    return hasBit(flagsOut, 3);
}

void DAW::ToggleTrackMuted(MediaTrack *media_track)
{
    ::CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, !DAW::IsTrackMuted(media_track)), NULL);
}

bool DAW::IsTrackSoloed(MediaTrack *media_track)
{
    int flagsOut;
    GetTrackState(media_track, &flagsOut);

    return hasBit(flagsOut, 4);
}

void DAW::ToggleTrackSoloed(MediaTrack *media_track)
{
    ::CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, !DAW::IsTrackSoloed(media_track)), NULL);
}

bool DAW::IsTrackSelected(MediaTrack *media_track)
{
    return ::IsTrackSelected(media_track);
}

bool DAW::IsTrackParent(MediaTrack *media_track)
{
    return (int)GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH") == 1;
}

bool DAW::IsTrackPinned(MediaTrack *media_track)
{
    return (int)GetMediaTrackInfo_Value(media_track, "B_TCPPIN") == 1;
}

int DAW::GetTrackPanMode(MediaTrack *media_track)
{
    int panMode = 0;
    double pan1, pan2 = 0.0;

    GetTrackUIPan(media_track, &pan1, &pan2, &panMode);

    if (panMode < PAN_MODE_STEREO_PAN)
    {
        return PAN_MODE_BALANCE_PAN;
    }
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

        if (midiDeviceName[0] == '\0')
        {
            return "";
        }

        return std::string(midiDeviceName);
    }
    else
    {
        int inputChannel = inputCode & ((1 << 5) - 1);
        const char *inputName = ::GetInputChannelName(inputChannel);

        if (!inputName)
        {
            return "";
        }

        return std::string(inputName);
    }
}

std::string DAW::GetTrackMonitorMode(MediaTrack *media_track)
{
    int monitorMode = int(::GetMediaTrackInfo_Value(media_track, "I_RECMON"));

    switch (monitorMode)
    {
    case 0:
        return "Mon: OFF";

    case 1:
        return "Mon: ON";

    case 2:
        return "Mon: AUTO";

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

void DAW::ToggleSelectedTrack(MediaTrack *media_track)
{
    ::SetTrackSelected(media_track, !DAW::IsTrackSelected(media_track));
}

void DAW::SetUniqueSelectedTrack(MediaTrack *media_track)
{
    UnSelectAllTracks();
    ::SetTrackSelected(media_track, true);
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

void DAW::SetTrackPan1(MediaTrack *media_track, double value)
{
    CSurf_SetSurfacePan(media_track, CSurf_OnPanChange(media_track, value, false), NULL);
}

void DAW::SetTrackPan2(MediaTrack *media_track, double value)
{
    SetMediaTrackInfo_Value(media_track, "D_WIDTH", CSurf_OnWidthChange(media_track, value, false));
}

void DAW::SetTrackVolume(MediaTrack *media_track, double value)
{
    CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, value, false), NULL);
}

double DAW::GetTrackVolume(MediaTrack *media_track)
{
    double volume, pan1 = 0.0;
    GetTrackUIVolPan(media_track, &volume, &pan1);

    return volume;
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
    return GetSetTrackSendInfo(media_track, SEND_MODE_RECEIVE, receive, "P_SRCTRACK", 0) ? true : false;
}

std::string DAW::GetTrackReceiveSrcName(MediaTrack *media_track, int receive)
{
    MediaTrack *srcTrack = (MediaTrack *)GetSetTrackSendInfo(media_track, SEND_MODE_RECEIVE, receive, "P_SRCTRACK", 0);
    if (srcTrack)
    {
        return GetTrackName(srcTrack);
    }
    return "No Source";
}

int DAW::GetTrackReceiveMode(MediaTrack *media_track, int receive)
{
    return (int)GetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "I_SENDMODE");
}

int DAW::GetTrackReceiveAutoMode(MediaTrack *media_track, int receive)
{
    return (int)GetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "I_AUTOMODE");
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

void DAW::ToggleTrackReceiveMute(MediaTrack *media_track, int receive)
{
    SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "B_MUTE", !DAW::GetTrackReceiveMute(media_track, receive));
}

bool DAW::GetTrackReceivePhase(MediaTrack *media_track, int receive)
{
    return (bool)GetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "B_PHASE");
}

void DAW::ToggleTrackReceivePhase(MediaTrack *media_track, int receive)
{
    SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "B_PHASE", !DAW::GetTrackReceivePhase(media_track, receive));
}

bool DAW::GetTrackReceiveMono(MediaTrack *media_track, int receive)
{
    return (bool)GetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "B_Mono");
}

void DAW::ToggleTrackReceiveMono(MediaTrack *media_track, int receive)
{
    SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "B_MONO", !DAW::GetTrackReceiveMono(media_track, receive));
}

int DAW::GetNextTrackReceiveMode(MediaTrack *media_track, int receive)
{
    return sendModes[(GetTrackReceiveMode(media_track, receive) + 1) % 4];
}

void DAW::SetNextTrackReceiveMode(MediaTrack *media_track, int receive)
{
    ::SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "I_SENDMODE", DAW::GetNextTrackReceiveMode(media_track, receive));
}

void DAW::SetTrackReceiveVolume(MediaTrack *media_track, int receive, double volume)
{
    ::SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "D_VOL", CSurf_OnRecvVolumeChange(media_track, receive, volume, false));
}

void DAW::SetTrackReceivePan(MediaTrack *media_track, int receive, double pan)
{
    ::SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "D_PAN", CSurf_OnRecvPanChange(media_track, receive, pan, false));
}

/************************************************************************
 * Track Send
 ************************************************************************/
bool DAW::HasTrackSend(MediaTrack *media_track, int send)
{
    return GetSetTrackSendInfo(media_track, SEND_MODE_SEND, send, "P_DESTTRACK", 0) ? true : false;
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
    return (int)GetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "I_SENDMODE");
}

int DAW::GetTrackSendAutoMode(MediaTrack *media_track, int send)
{
    return (int)GetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "I_AUTOMODE");
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

void DAW::ToggleTrackSendMute(MediaTrack *media_track, int send)
{
    SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "B_MUTE", !DAW::GetTrackSendMute(media_track, send));
}

bool DAW::GetTrackSendPhase(MediaTrack *media_track, int send)
{
    return (bool)GetTrackSendInfo_Value(media_track, 0, send, "B_PHASE");
}

void DAW::ToggleTrackSendPhase(MediaTrack *media_track, int send)
{
    SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "B_PHASE", !DAW::GetTrackSendPhase(media_track, send));
}

bool DAW::GetTrackSendMono(MediaTrack *media_track, int send)
{
    return (bool)GetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "B_MONO");
}

void DAW::ToggleTrackSendMono(MediaTrack *media_track, int send)
{
    SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "B_MONO", !DAW::GetTrackSendMono(media_track, send));
}

int DAW::GetNextTrackSendMode(MediaTrack *media_track, int send)
{
    return sendModes[(GetTrackSendMode(media_track, send) + 1) % 4];
}

void DAW::SetNextTrackSendMode(MediaTrack *media_track, int send)
{
    ::SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "I_SENDMODE", DAW::GetNextTrackSendMode(media_track, send));
}

void DAW::SetTrackSendVolume(MediaTrack *media_track, int send, double volume)
{
    ::SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "D_VOL", CSurf_OnSendVolumeChange(media_track, send, volume, false));
}

void DAW::SetTrackSendPan(MediaTrack *media_track, int send, double pan)
{
    ::SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "D_PAN", CSurf_OnSendPanChange(media_track, send, pan, false));
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

/************************************************************************
 * Window Related
 ************************************************************************/
void DAW::SetTcpScroll(MediaTrack *media_track)
{
    PreventUIRefresh(1);
    // Get the y position of the provideed track relative to the top of the Arrange view
    int track_tcpy = GetMediaTrackInfo_Value(media_track, "I_TCPY");
    // Get the main window
    HWND mainHWND = GetMainHwnd();
    // Get the Arrange view
    HWND arrangeHWND = findWindowChildByID(mainHWND, 1000);
    // Get the scroll position and other data of the Arrange view
    int scroll_position, scroll_pageSize, scroll_min, scroll_max, scroll_track_pos;

    int pinned_tracks_height = VersionHasFeature(FEATURE_PINNED_TRACKS) ? GetPinnedTracksHeight() : 0;
    bool done = getWindowScrollInfo(arrangeHWND, "v", &scroll_position, &scroll_pageSize, &scroll_min, &scroll_max, &scroll_track_pos);

    if (done)
    {
        // Set the new Arrange scroll position
        setWindowScrollPos(arrangeHWND, "v", track_tcpy + scroll_position - pinned_tracks_height);
    }

    PreventUIRefresh(-1);
}

int DAW::GetPinnedTracksHeight()
{
    MediaTrack *media_track;
    int pinned_height = 0;

    for (int i = 0; i < ::GetNumTracks(); i++)
    {
        media_track = ::GetTrack(0, i);
        if (IsTrackPinned(media_track))
        {
            pinned_height += GetMediaTrackInfo_Value(media_track, "I_TCPH");
        }
    }

    // Return the actual height plus the gap
    return pinned_height + 10;
}

bool DAW::VersionHasFeature(Features feature)
{
    double current_version = std::stod(::GetAppVersion());
    return current_version >= feature_versions[feature];
}

std::string DAW::GetExtState(std::string key, std::string default_value)
{
    return ::HasExtState(EXT_STATE_SECTION, key.c_str()) ? ::GetExtState(EXT_STATE_SECTION, key.c_str()) : default_value;
}

void DAW::SetExtState(std::string key, std::string value, bool persist)
{
    ::SetExtState(EXT_STATE_SECTION, key.c_str(), value.c_str(), persist);
}
