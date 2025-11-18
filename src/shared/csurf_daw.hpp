#ifndef CSURF_DAW_H_
#define CSURF_DAW_H_

#include <reaper_plugin.h>
#include <string>
#include <map>
#include <vector>
#include "../controls/csurf_color_button_colors.hpp"

enum Features
{
    FEATURE_PINNED_TRACKS
};

enum PAN_MODES
{
    /**
     * @brief Default single encoder pan
     */
    PAN_MODE_BALANCE_PAN = 3,
    /**
     * @brief Width and pan control
     */
    PAN_MODE_STEREO_PAN = 5,
    /**
     * @brief Left and right pan
     */
    PAN_MODE_DUAL_PAN = 6,
};

static std::map<Features, double>
    feature_versions = {
        {FEATURE_PINNED_TRACKS, 7.46},
};

class DAW
{

public:
    static int sendModes[3];

    static void UnSelectAllTracks();

    static bool IsTrackArmed(MediaTrack *media_track);
    static void ToggleTrackArmed(MediaTrack *media_track);
    static bool IsTrackMuted(MediaTrack *media_track);
    static void ToggleTrackMuted(MediaTrack *media_track);
    static bool IsTrackSoloed(MediaTrack *media_track);
    static void ToggleTrackSoloed(MediaTrack *media_track);
    static bool IsTrackSelected(MediaTrack *media_track);
    static bool IsTrackParent(MediaTrack *media_track);
    static bool IsTrackPinned(MediaTrack *media_track);
    static bool IsTrackVisible(MediaTrack *media_track);

    static int GetTrackPanMode(MediaTrack *media_track);
    static std::string GetTrackName(MediaTrack *media_track);
    static std::string GetTrackIndex(MediaTrack *media_track);
    static std::string GetTrackInputName(MediaTrack *media_track);
    static std::string GetTrackMonitorMode(MediaTrack *media_track);
    static std::string GetTrackRecordingMode(MediaTrack *media_track);
    static ButtonColor GetTrackColor(MediaTrack *media_track);
    static void SetUniqueSelectedTrack(MediaTrack *media_track);
    static void ToggleSelectedTrack(MediaTrack *media_track);
    static void SetSelectedTracksRange(MediaTrack *media_track);
    static double GetTrackPeakInfo(MediaTrack *media_track);
    static int GetTrackSurfacePeakInfo(MediaTrack *media_track);
    static void SetTrackPan1(MediaTrack *media_track, double value);
    static void SetTrackPan2(MediaTrack *media_track, double value);
    static void SetTrackVolume(MediaTrack *media_track, double value);
    static double GetTrackVolume(MediaTrack *media_track);

    /**
     * Track plugins
     */
    static bool TrackHasFx(MediaTrack *media_track);
    static bool HasTrackFx(MediaTrack *media_track, int fx);
    static std::string GetTrackFxName(MediaTrack *media_track, int fx, bool full = false);
    static std::string GetTrackFxDeveloper(MediaTrack *media_track, int fx);
    static bool GetTrackFxEnabled(MediaTrack *media_track, int fx);
    static bool GetTrackFxOffline(MediaTrack *media_track, int fx);
    static std::string GetTrackFxSurfceEnabled(MediaTrack *media_track, int fx);
    static bool GetTrackFxPanelOpen(MediaTrack *media_track, int fx);
    static bool GetTrackFxBypassed(MediaTrack *media_track);
    static void ToggleTrackFxBypass(MediaTrack *media_track);

    static std::string GetTrackFxParamName(MediaTrack *media_track, int fx, int param);
    static int GetTrackFxParamNbSteps(MediaTrack *media_track, int fx, int param);
    static void SetTrackFXParamUntouched(MediaTrack *media_track, int fx);

    /**
     * Track receives
     */
    static bool HasTrackReceive(MediaTrack *media_track, int receive);
    static std::string GetTrackReceiveSrcName(MediaTrack *media_track, int receive);
    static int GetTrackReceiveMode(MediaTrack *media_track, int receive);
    static int GetTrackReceiveAutoMode(MediaTrack *media_track, int receive);
    static std::string GetTrackSurfaceReceiveMode(MediaTrack *media_track, int receive);
    static std::string GetTrackSurfaceReceiveAutoMode(MediaTrack *media_track, int receive);

    static bool GetTrackReceiveMute(MediaTrack *media_track, int receive);
    static void ToggleTrackReceiveMute(MediaTrack *media_track, int receive);

    static bool GetTrackReceivePhase(MediaTrack *media_track, int receive);
    static void ToggleTrackReceivePhase(MediaTrack *media_track, int receive);

    static bool GetTrackReceiveMono(MediaTrack *media_track, int receive);
    static void ToggleTrackReceiveMono(MediaTrack *media_track, int receive);

    static int GetNextTrackReceiveMode(MediaTrack *media_track, int receive);
    static void SetNextTrackReceiveMode(MediaTrack *media_track, int receive);

    static void SetTrackReceiveVolume(MediaTrack *media_track, int receive, double volume);
    static void SetTrackReceivePan(MediaTrack *media_track, int receive, double pan);

    /**
     * Track sends
     */
    static bool HasTrackSend(MediaTrack *media_track, int send);
    static std::string GetTrackSendDestName(MediaTrack *media_track, int send);
    static int GetTrackSendMode(MediaTrack *media_track, int send);
    static int GetTrackSendAutoMode(MediaTrack *media_track, int send);
    static std::string GetTrackSurfaceSendMode(MediaTrack *media_track, int send);
    static std::string GetTrackSurfaceSendAutoMode(MediaTrack *media_track, int send);

    static bool GetTrackSendMute(MediaTrack *media_track, int send);
    static void ToggleTrackSendMute(MediaTrack *media_track, int send);

    static bool GetTrackSendPhase(MediaTrack *media_track, int send);
    static void ToggleTrackSendPhase(MediaTrack *media_track, int send);

    static bool GetTrackSendMono(MediaTrack *media_track, int send);
    static void ToggleTrackSendMono(MediaTrack *media_track, int send);

    static int GetNextTrackSendMode(MediaTrack *media_track, int send);
    static void SetNextTrackSendMode(MediaTrack *media_track, int send);

    static void SetTrackSendVolume(MediaTrack *media_track, int send, double volume);
    static void SetTrackSendPan(MediaTrack *media_track, int send, double pan);

    /**
     * Project related
     */
    static int GetProjectTimeMode();
    static int GetProjectMeasureOffset();
    static double GetProjectTimeOffset();
    static std::vector<std::string> GetProjectTime(bool overwrite_time_code, int new_time_code);

    /**
     * Media item related
     */
    static bool MediaItemHasMidi(MediaItem *media_item);
    static bool MediaItemHasAudio(MediaItem *media_item);

    static void EditSave();
    static void EditUndo();
    static void EditRedo();

    /**
     * Window related
     */
    static void SetTcpScroll(MediaTrack *media_track);

    static int GetPinnedTracksHeight();

    static bool VersionHasFeature(Features feature);

    /**
     * Ext State
     */
    static std::string GetExtState(std::string key, std::string default_value);
    static void SetExtState(std::string key, std::string value, bool persist);
};

#endif
