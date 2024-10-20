#include "csurf_daw.hpp"
#include <reaper_plugin_functions.h>
#include "csurf_utils.hpp"

int DAW::sendModes[3] = {0, 1, 3};

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
    char trackName[256];
    ::GetTrackName(media_track, trackName, sizeof(trackName));

    return string(trackName);
}

string DAW::GetTrackIndex(MediaTrack *media_track)
{
    return to_string((int)GetMediaTrackInfo_Value(media_track, "IP_TRACKNUMBER"));
}

double DAW::GetTrackPeakInfo(MediaTrack *media_track)
{
    if (media_track)
    {
        return (Track_GetPeakInfo(media_track, 0) + Track_GetPeakInfo(media_track, 1)) / 2;
    }
    return 0.0;
}

int DAW::GetTrackSurfacePeakInfo(MediaTrack *media_track)
{
    return int(volToNormalized(GetTrackPeakInfo(media_track)) * 127.0);
}

bool DAW::HasTrackFx(MediaTrack *media_track, int fx)
{
    char pluginName[256] = "";
    return TrackFX_GetFXName(media_track, fx, pluginName, size(pluginName));
}

string DAW::GetTrackFxName(MediaTrack *media_track, int fx)
{
    char pluginName[256] = "";
    if (TrackFX_GetFXName(media_track, fx, pluginName, size(pluginName)))
    {
        return StripFxType(pluginName);
    }
    return "No FX";
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
    return TrackFX_GetEnabled(media_track, fx) ? "Enabled" : "Bypassed";
}

bool DAW::GetTrackFxPanelOpen(MediaTrack *media_track, int fx)
{
    if (TrackFX_GetCount(media_track) < 1)
    {
        return false;
    }
    return ::TrackFX_GetOpen(media_track, fx);
}

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
    int nbSteps = 0;
    double stepOut, _;
    bool isToggle;
    if (TrackFX_GetParameterStepSizes(media_track, fx, param, &stepOut, &_, &_, &isToggle))
    {
        nbSteps = (int)round(1.0 / stepOut) + 1;
    }
    return nbSteps;
}

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
