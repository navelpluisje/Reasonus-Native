#ifndef CSURF_DAW_H_
#define CSURF_DAW_H_

#include <reaper_plugin.h>
#include <string>
#include <vector>

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
    static std::string GetTrackName(MediaTrack *media_track);
    static std::string GetTrackIndex(MediaTrack *media_track);
    static std::string GetTrackInputName(MediaTrack *media_track);
    static std::string GetTrackMonitorMode(MediaTrack *media_track);
    static std::string GetTrackRecordingMode(MediaTrack *media_track);
    static bool GetTrackFxBypassed(MediaTrack *media_track);
    static void ToggleTrackFxBypass(MediaTrack *media_track);

    static double GetTrackPeakInfo(MediaTrack *media_track);
    static int GetTrackSurfacePeakInfo(MediaTrack *media_track);

    static bool TrackHasFx(MediaTrack *media_track);
    static bool HasTrackFx(MediaTrack *media_track, int fx);
    static std::string GetTrackFxName(MediaTrack *media_track, int fx, bool full = false);
    static std::string GetTrackFxDeveloper(MediaTrack *media_track, int fx);
    static bool GetTrackFxEnabled(MediaTrack *media_track, int fx);
    static bool GetTrackFxOffline(MediaTrack *media_track, int fx);
    static std::string GetTrackFxSurfceEnabled(MediaTrack *media_track, int fx);
    static bool GetTrackFxPanelOpen(MediaTrack *media_track, int fx);

    static std::string GetTrackFxParamName(MediaTrack *media_track, int fx, int param);
    static int GetTrackFxParamNbSteps(MediaTrack *media_track, int fx, int param);

    static bool HasTrackReceive(MediaTrack *media_track, int receive);
    static std::string GetTrackReceiveSrcName(MediaTrack *media_track, int receive);
    static int GetTrackReceiveMode(MediaTrack *media_track, int receive);
    static int GetTrackReceiveAutoMode(MediaTrack *media_track, int receive);
    static std::string GetTrackSurfaceReceiveMode(MediaTrack *media_track, int receive);
    static std::string GetTrackSurfaceReceiveAutoMode(MediaTrack *media_track, int receive);
    static bool GetTrackReceiveMute(MediaTrack *media_track, int receive);
    static bool GetTrackReceivePhase(MediaTrack *media_track, int receive);
    static bool GetTrackReceiveMono(MediaTrack *media_track, int receive);
    static int GetNextTrackReceiveMode(MediaTrack *media_track, int receive);

    static bool HasTrackSend(MediaTrack *media_track, int send);
    static std::string GetTrackSendDestName(MediaTrack *media_track, int send);
    static int GetTrackSendMode(MediaTrack *media_track, int send);
    static int GetTrackSendAutoMode(MediaTrack *media_track, int send);
    static std::string GetTrackSurfaceSendMode(MediaTrack *media_track, int send);
    static std::string GetTrackSurfaceSendAutoMode(MediaTrack *media_track, int send);
    static bool GetTrackSendMute(MediaTrack *media_track, int send);
    static bool GetTrackSendPhase(MediaTrack *media_track, int send);
    static bool GetTrackSendMono(MediaTrack *media_track, int send);
    static int GetNextTrackSendMode(MediaTrack *media_track, int receive);

    static int GetProjectTimeMode();
    static int GetProjectMeasureOffset();
    static double GetProjectTimeOffset();
    static std::vector<std::string> GetProjectTime(bool overwrite_time_code, int new_time_code);

    static bool MediaItemHasMidi(MediaItem *media_item);
    static bool MediaItemHasAudio(MediaItem *media_item);

    static void EditSave();
    static void EditUndo();
    static void EditRedo();
};

#endif
