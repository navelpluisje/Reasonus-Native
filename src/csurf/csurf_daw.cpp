#include "csurf_daw.hpp"
#include <vector>
#include <string>
#include <reaper_plugin_functions.h>
#include "csurf_utils.hpp"

int DAW::sendModes[3] = {0, 1, 3};

using namespace std;

/************************************************************************
 * Track
 ************************************************************************/
void DAW::UnSelectAllTracks()
{
    for (int i = 0; i < CountTracks(0); i++)
    {
        MediaTrack *media_track = GetTrack(0, i);
        SetTrackSelected(media_track, false);
    }
};

bool DAW::IsTrackArmed(MediaTrack *media_track)
{
    int flagsOut;
    GetTrackState(media_track, &flagsOut);

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

string DAW::GetTrackName(MediaTrack *media_track)
{
    if (!media_track)
    {
        return "";
    }

    char trackName[256];
    ::GetTrackName(media_track, trackName, sizeof(trackName));

    return string(trackName);
}

string DAW::GetTrackIndex(MediaTrack *media_track)
{
    return to_string((int)GetMediaTrackInfo_Value(media_track, "IP_TRACKNUMBER"));
}

string DAW::GetTrackInputName(MediaTrack *media_track)
{
    int inputCode = int(::GetMediaTrackInfo_Value(media_track, "I_RECINPUT"));
    if (hasBit(inputCode, 12))
    {
        char midiDeviceName[256];
        int midiDeviceId = (inputCode >> 5) & ((1 << 6) - 1);
        ::GetMIDIInputName(midiDeviceId, midiDeviceName, sizeof(midiDeviceName));
        return string(midiDeviceName);
    }
    else
    {
        int inputChannel = inputCode & ((1 << 5) - 1);
        const char *inputName = ::GetInputChannelName(inputChannel);
        return string(inputName);
    }
}

string DAW::GetTrackMonitorMode(MediaTrack *media_track)
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

string DAW::GetTrackRecordingMode(MediaTrack *media_track)
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
        ::TrackFX_GetFXName(media_track, i, pluginName, size(pluginName));
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
    return TrackFX_GetFXName(media_track, fx, pluginName, size(pluginName));
}

string DAW::GetTrackFxName(MediaTrack *media_track, int fx, bool full)
{
    char pluginName[256] = "";
    if (!::TrackFX_GetFXName(media_track, fx, pluginName, size(pluginName)))
    {
        return "No FX";
    }
    if (full)
    {
        return StripPluginNamePrefixes(pluginName);
    }
    vector<string> pluginNameParts = split(StripPluginNamePrefixes(StripPluginChannelPostfix(pluginName).data()), " (");
    if (pluginNameParts.size() > 1)
    {
        pluginNameParts.pop_back();
    }

    return join(pluginNameParts, " (");
}

string DAW::GetTrackFxDeveloper(MediaTrack *media_track, int fx)
{
    char pluginName[256] = "";
    if (!::TrackFX_GetFXName(media_track, fx, pluginName, size(pluginName)))
    {
        return "No Dev";
    }
    vector<string> pluginNameParts = split(StripPluginNamePrefixes(StripPluginChannelPostfix(pluginName).data()), " (");
    string developer = pluginNameParts.at(pluginNameParts.size() - 1);
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

string DAW::GetTrackFxSurfceEnabled(MediaTrack *media_track, int fx)
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

/************************************************************************
 * Track FX Param
 ************************************************************************/
string DAW::GetTrackFxParamName(MediaTrack *media_track, int fx, int param)
{
    char paramName[256] = "";
    if (TrackFX_GetParamName(media_track, fx, param, paramName, size(paramName)))
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
            nbSteps = (int)round(1.0 / stepOut) + 1;
        }
    }
    return nbSteps;
}

/************************************************************************
 * Track Receive
 ************************************************************************/
bool DAW::HasTrackReceive(MediaTrack *media_track, int receive)
{
    return GetSetTrackSendInfo(media_track, -1, receive, "P_SRCTRACK", 0) ? true : false;
}

string DAW::GetTrackReceiveSrcName(MediaTrack *media_track, int receive)
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

string DAW::GetTrackSurfaceReceiveMode(MediaTrack *media_track, int receive)
{
    return GetSendModeString(GetTrackReceiveMode(media_track, receive));
}

string DAW::GetTrackSurfaceReceiveAutoMode(MediaTrack *media_track, int receive)
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

string DAW::GetTrackSendDestName(MediaTrack *media_track, int send)
{
    MediaTrack *destTrack = (MediaTrack *)GetSetTrackSendInfo(media_track, 0, send, "P_SRCTRACK", 0);
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

string DAW::GetTrackSurfaceSendMode(MediaTrack *media_track, int send)
{
    return GetSendModeString(GetTrackSendMode(media_track, send));
}

string DAW::GetTrackSurfaceSendAutoMode(MediaTrack *media_track, int send)
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
