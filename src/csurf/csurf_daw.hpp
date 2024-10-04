#ifndef CSURF_DAW_H_
#define CSURF_DAW_H_

#include <reaper_plugin.h>
#include <string>

using namespace std;

class DAW
{

public:
    static int sendModes[3];

    static void UnSelectAllTracks();

    static bool IsTrackArmed(MediaTrack *media_track);
    static bool IsTrackMuted(MediaTrack *media_track);
    static bool IsTrackSoloed(MediaTrack *media_track);
    static bool IsTrackSelected(MediaTrack *media_track);

    static int GetTrackPanMode(MediaTrack *media_track);
    static string GetTrackName(MediaTrack *media_track);
    static string GetTrackIndex(MediaTrack *media_track);

    static double GetTrackPeakInfo(MediaTrack *media_track);
    static int GetTrackSurfacePeakInfo(MediaTrack *media_track);

    static bool HasTrackFx(MediaTrack *media_track, int fx);
    static string GetTrackFxName(MediaTrack *media_track, int fx);
    static bool GetTrackFxEnabled(MediaTrack *media_track, int fx);
    static bool GetTrackFxOffline(MediaTrack *media_track, int fx);
    static string GetTrackFxSurfceEnabled(MediaTrack *media_track, int fx);
    static bool GetTrackFxPanelOpen(MediaTrack *media_track, int fx);

    static bool HasTrackReceive(MediaTrack *media_track, int receive);
    static string GetTrackReceiveSrcName(MediaTrack *media_track, int receive);
    static int GetTrackReceiveMode(MediaTrack *media_track, int receive);
    static int GetTrackReceiveAutoMode(MediaTrack *media_track, int receive);
    static string GetTrackSurfaceReceiveMode(MediaTrack *media_track, int receive);
    static string GetTrackSurfaceReceiveAutoMode(MediaTrack *media_track, int receive);
    static bool GetTrackReceiveMute(MediaTrack *media_track, int receive);
    static bool GetTrackReceivePhase(MediaTrack *media_track, int receive);
    static bool GetTrackReceiveMono(MediaTrack *media_track, int receive);
    static int GetNextTrackReceiveMode(MediaTrack *media_track, int receive);

    static bool HasTrackSend(MediaTrack *media_track, int send);
    static string GetTrackSendDestName(MediaTrack *media_track, int send);
    static int GetTrackSendMode(MediaTrack *media_track, int send);
    static int GetTrackSendAutoMode(MediaTrack *media_track, int send);
    static string GetTrackSurfaceSendMode(MediaTrack *media_track, int send);
    static string GetTrackSurfaceSendAutoMode(MediaTrack *media_track, int send);
    static bool GetTrackSendMute(MediaTrack *media_track, int send);
    static bool GetTrackSendPhase(MediaTrack *media_track, int send);
    static bool GetTrackSendMono(MediaTrack *media_track, int send);
    static int GetNextTrackSendMode(MediaTrack *media_track, int receive);
};

#endif
