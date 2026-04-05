#ifndef CSURF_DAW_H_
#define CSURF_DAW_H_

#include <reaper_plugin.h>
#include <string>
#include <map>
#include <vector>
#include "../controls/csurf_color_button_colors.hpp"

enum Features {
    FEATURE_PINNED_TRACKS
};

enum PAN_MODES {
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

enum SEND_MODES {
    SEND_MODE_RECEIVE = -1,
    SEND_MODE_SEND = 0,
    SEND_MODE_HARDWARE = 1,
};

static std::map<Features, double> feature_versions = { // NOLINT(*-statically-constructed-objects, *-throwing-static-initialization)
    {FEATURE_PINNED_TRACKS, 7.46},
};

class DAW {
public:
    static std::array<int, 3> sendModes;

    /**************************************************************************************************************
     *  Track related methods
     *************************************************************************************************************/

    /**
     * Do Unselect all tracks
     */
    static void UnSelectAllTracks();

    /**
     * Check if the given media track is armed
     * @param media_track The media track to check
     * @return Whether if the given track is armed
     */
    static bool IsTrackArmed(MediaTrack *media_track);

    /**
     * Toggle the armed state for the given track
     * @param media_track The track to toggle the armed value
     */
    static void ToggleTrackArmed(MediaTrack *media_track);

    /**
     * Check if the given track is muted
     * @param media_track The track to check for the muted state
     * @return Whether if the given track is muted
     */
    static bool IsTrackMuted(MediaTrack *media_track);

    /**
     * Toggle the mute state for the given track
     * @param media_track The track to toggle the muted state
     */
    static void ToggleTrackMuted(MediaTrack *media_track);

    /**
     * Check if the given track is soloed
     * @param media_track The track to check for the soloed state
     * @return Whether if the given track is soloed
     */
    static bool IsTrackSoloed(MediaTrack *media_track);

    /**
     * Toggle the solo state for the given track
     * @param media_track The track to toggle the soloed state
     */
    static void ToggleTrackSoloed(MediaTrack *media_track);

    /**
     * Unique solo the give track. This will un-solo all teh other tracks
     * @param media_track The track to unique solo
     */
    static void SetTrackSoloUnique(MediaTrack *media_track);

    /**
     * Check if the given track is selected
     * @param media_track The track to check for the selected state
     * @return Whether if the given track is selected
     */
    static bool IsTrackSelected(MediaTrack *media_track);

    /**
     * unique select the give track. This will unselect all teh other tracks
     * @param media_track The track to unique select
     */
    static void ToggleTrackSelected(MediaTrack *media_track);

    /**
     * unique select the give track. This will unselect all teh other tracks
     * @param media_track The track to unique select
     */
    static void SetUniqueSelectedTrack(MediaTrack *media_track);

    /**
     * unique select the give track. This will unselect all teh other tracks
     * @param media_track The track to unique select
     */
    static void SetSelectedTracksRange(MediaTrack *media_track);

    /**
     * Check if the given track is a parent track
     * @param media_track The track to check is track is a parent
     * @return Whether if the given track is a parent track
     */
    static bool IsTrackParent(MediaTrack *media_track);

    /**
     * Check if the given track is a pinned track
     * @param media_track The track to check is track is a pinned
     * @return Whether if the given track is a pinned track
     */
    static bool IsTrackPinned(MediaTrack *media_track);

    /**
     * Check if the given track is visible in the mixer (This does not check if the track is 'in view')
     * @param media_track The track to check is track is visible
     * @return Whether if the given track is visible in the mixer
     */
    static bool IsTrackVisible(MediaTrack *media_track);

    /**
     * Get the pan mode for the given track
     * @param media_track The track to get the pan mode for
     * @return The pan mode:\n
     *   3: Default balance\n
     *   5: Stereo Pan\n
     *   6: Dual Pan
     */
    static int GetTrackPanMode(MediaTrack *media_track);

    /**
     * Set pan value 1 the give track.
     * @param media_track The track to set pan 1 for
     * @param value the normalized value to set (0.0 - 1.0)
     */
    static void SetTrackPan1(MediaTrack *media_track, double value);

    /**
     * Set pan value 2 the give track.
     * @param media_track The track to set pan 1 for
     * @param value the normalized value to set (0.0 - 1.0)
     */
    static void SetTrackPan2(MediaTrack *media_track, double value);

    /**
     * Get the name for the given track
     * @param media_track The track to get the name for
     * @return The track name
     */
    static std::string GetTrackName(MediaTrack *media_track);

    /**
     * Get the index for the given track
     * @param media_track The track to get the index for
     * @return The track index
     */
    static std::string GetTrackIndex(MediaTrack *media_track);

    /**
     * Get the input name for the given track
     * @param media_track The track to get the input name for
     * @return The track input name
     */
    static std::string GetTrackInputName(MediaTrack *media_track);

    /**
     * Get the monitor mode for the given track
     * @param media_track The track to get the monitor mode for
     * @return The track monitor mode
     */
    static std::string GetTrackMonitorMode(MediaTrack *media_track);

    /**
     * Get the recording mode for the given track
     * @param media_track The track to get the recording mode for
     * @return The track recording mode
     */
    static std::string GetTrackRecordingMode(MediaTrack *media_track);

    /**
     * Get the track color for the given track
     * @param media_track The track to get the track color for
     * @return The track color
     */
    static ButtonColor GetTrackColor(MediaTrack *media_track);

    /**
     * Get the peak data for the given track to populate the vu-meter
     * @param media_track The track to get the peak data for
     * @return The value at this point in time
     */
    static double GetTrackPeakInfo(MediaTrack *media_track);

    /**
     * Get normalized peak data for the given track to populate the vu-meter
     * @param media_track The track to get the normalized peak data for
     * @return The value at this normalized point in time
     */
    static int GetTrackSurfacePeakInfo(MediaTrack *media_track);

    /**
     * Get the volume for the given track to populate the vu-meter
     * @param media_track The track to get the volume for
     * @return The volume
     */
    static double GetTrackVolume(MediaTrack *media_track);

    /**
     * Set the volume for the given track to populate the vu-meter
     * @param media_track The track to get the volume for
     * @param value The volume for the track
     */
    static void SetTrackVolume(MediaTrack *media_track, double value);

    /**************************************************************************************************************
     *  Track plugins related methods
     *************************************************************************************************************/

    /**
     * Check if the give track has an FX plugin
     * @param media_track The track to check for FX plugins
     */
    static bool TrackHasFx(MediaTrack *media_track);

    /**
     * Check if the given track ga an FX plugin at the given index
     * @param media_track The track to check for the FX plugin
     * @param fxIndex The index of the plugin to check
     * @return Whether the track has an FX plugin at the given index
     */
    static bool HasTrackFx(MediaTrack *media_track, int fxIndex);

    /**
     * Gat the name of the plugin at the given index
     * @param media_track The track to check for the plugin
     * @param fxIndex Index of the plugin
     * @param full Do we want the full plugin name or only the real name (plugin type and developer will be tripped by default)
     * @return The name of the plugin
     */
    static std::string GetTrackFxName(MediaTrack *media_track, int fxIndex, bool full);

    /**
     * Get the type of the plugin at the given index
     * @param media_track The track to check for the plugin
     * @param fxIndex Index of the plugin
     * @return The type of the plugin.\n
     *      This can be Clap, VST, etc.\n
     *      This will be unknown when not known.\n
     *      This will be No FX when the plugin could not be found
     */
    static std::string GetTrackFxType(MediaTrack *media_track, int fxIndex);

    /**
     * Get the developer of the plugin at the given index
     * @param media_track The track to check for the plugin
     * @param fxIndex Index of the plugin
     * @return The developer of the plugin
     */
    static std::string GetTrackFxDeveloper(MediaTrack *media_track, int fxIndex);

    /**
     * Get of the FX on the given index is enabled or not
     * @param media_track The track to check for the plugin
     * @param fxIndex Index of the plugin
     * @return Whether the FX is enabled or not
     */
    static bool GetTrackFxEnabled(MediaTrack *media_track, int fxIndex);

    /**
     * Get of the FX on the given index is offline or not
     * @param media_track The track to check for the plugin
     * @param fxIndex Index of the plugin
     * @return Whether the FX is offline or not
     */
    static bool GetTrackFxOffline(MediaTrack *media_track, int fxIndex);

    /**
     * Get the string value for the enabled state for the given fx
     * @param media_track  The track to check for the plugin
     * @param fxIndex  Index of the plugin
     * @return The string representation for the enabled state. Value:
     * - Enabled
     * - Disabled
     */
    static std::string GetTrackFxSurfaceEnabled(MediaTrack *media_track, int fxIndex);

    /**
     * Get if the panel for the FX on the given index is opened or not
     * @param media_track The track to check for the plugin
     * @param fxIndex Index of the plugin
     * @return Whether the panel is opened or not
     */
    static bool GetTrackFxPanelOpen(MediaTrack *media_track, int fxIndex);

    /**
     * Get the FX's are bypassed for the given track
     * @param media_track The track to check for the plugin
     * @return Whether the FX is disabled or not
     */
    static bool GetTrackFxBypassed(MediaTrack *media_track);

    /**
     * Toggle bypass of the FX's for the given track
     * @param media_track The track to disable the FX for
     */
    static void ToggleTrackFxBypass(MediaTrack *media_track);

    /**
     * Get the parameter name of the parameter with the given values
     * @param media_track The track where we want the plugin param for
     * @param fxIndex The index on the plugin
     * @param param The parameter index to get the name for
     * @return The name of the given parameter
     */
    static std::string GetTrackFxParamName(MediaTrack *media_track, int fxIndex, int param);

    /**
     * Get the number of steps of the parameter with the given values
     * @param media_track The track where we want the plugin param for
     * @param fxIndex The index on the plugin
     * @param param The parameter index to get the number of steps for
     * @return The number of steps of the given parameter
     */
    static int GetTrackFxParamNbSteps(MediaTrack *media_track, int fxIndex, int param);

    /**
     * Untouch the last touched parameter for the given plugin
     * @param media_track The track for the plugin we need
     * @param fxIndex The index on the plugin
     */
    static void SetTrackFXParamUntouched(MediaTrack *media_track, int fxIndex);

    /**************************************************************************************************************
     *  Track Receives related methods
     *************************************************************************************************************/
    /**
     * Check if the give track has a receive at the given index
     * @param media_track The track where we want the receive for
     * @param receive The index of the receive track
     * @return Whether the receive is available
     */
    static bool HasTrackReceive(MediaTrack *media_track, int receive);

    /**
     * Get the name of the source of the receive at the given index for the given track
     * @param media_track The track where we want the receive for
     * @param receive The index of the receive track
     * @return The name of the source of the receive
     */
    static std::string GetTrackReceiveSrcName(MediaTrack *media_track, int receive);

    /**
     * Get the receive mode for the given receive
     * @param media_track The track where we want the receive for
     * @param receive The index of the receive track
     * @return The receive mode:
     * - 0 = post-fader,
     * - 1 = pre-fx,
     * - 2 = post-fx (deprecated),
     * - 3 = post-fx
     */
    static int GetTrackReceiveMode(MediaTrack *media_track, int receive);

    /**
     * Get the string representation for receive mode for the given receive
     * @param media_track The track where we want the receive for
     * @param receive The index of the receive track
     * @return The receive mode:
     * - 0 = Post-Fdr,
     * - 1 = Pre-FX,
     * - 2 = Post-FX,
     * - 3 = Post-FX,
     * - others: Post-Fdr
     */
    static std::string GetTrackSurfaceReceiveMode(MediaTrack *media_track, int receive);

    /**
     * Get the receive automation mode for the given receive
     * @param media_track The track where we want the receive for
     * @param receive The index of the receive track
     * @return The receive automation mode:
     * - -1 = use track automode,
     * - 0 = trim/off,
     * - 1 = read,
     * - 2 = touch,
     * - 3 = write,
     * - 4 = latch
     */
    static int GetTrackReceiveAutoMode(MediaTrack *media_track, int receive);

    /**
     * Get the string representation of the receive automation mode for the given receive
     * @param media_track The track where we want the receive for
     * @param receive The index of the receive track
     * @return The receive automation mode:
     * - -1 = Track,
     * - 0 = Trim,
     * - 1 = Read,
     * - 2 = Touch,
     * - 3 = Write,
     * - 4 = Latch,
     */
    static std::string GetTrackSurfaceReceiveAutoMode(MediaTrack *media_track, int receive);

    /**
     * Get the mute state for the given receive
     * @param media_track The track where we want the receive for
     * @param receive The mute state of the receive track
     * @return Whether mute is engaged
     */
    static bool GetTrackReceiveMute(MediaTrack *media_track, int receive);

    /**
     * Toggle the mute state for the given receive
     * @param media_track The track for the receive we need
     * @param receive The index on the receive
     */
    static void ToggleTrackReceiveMute(MediaTrack *media_track, int receive);

    /**
     * Get the phase state for the given receive
     * @param media_track The track where we want the receive for
     * @param receive The phase state of the receive track
     * @return whether phase is engaged
     */
    static bool GetTrackReceivePhase(MediaTrack *media_track, int receive);

    /**
     * Toggle the phase state for the given receive
     * @param media_track The track for the receive we need
     * @param receive The index on the receive
     */
    static void ToggleTrackReceivePhase(MediaTrack *media_track, int receive);

    /**
     * Get the mono state for the given receive
     * @param media_track The track where we want the receive for
     * @param receive The mute state of the receive track
     * @return Whether mono is engaged
     */
    static bool GetTrackReceiveMono(MediaTrack *media_track, int receive);

    /**
     * Toggle the mono state for the given receive
     * @param media_track The track for the receive we need
     * @param receive The index on the receive
     */
    static void ToggleTrackReceiveMono(MediaTrack *media_track, int receive);

    /**
     * ??
     * @param media_track The track where we want the receive for
     * @param receive The mute state of the receive track
     * @return Whether mono ois engaged
     */
    static int GetNextTrackReceiveMode(MediaTrack *media_track, int receive);

    /**
     * ??
     * @param media_track The track for the receive we need
     * @param receive The index on the receive
     */
    static void SetNextTrackReceiveMode(MediaTrack *media_track, int receive);

    /**
     * Set the volume for the given receive
     * @param media_track The track for the receive we need
     * @param receive The index on the receive
     * @param volume The value to set
     */
    static void SetTrackReceiveVolume(MediaTrack *media_track, int receive, double volume);

    /**
     * Set the volume for the given receive
     * @param media_track The track for the receive we need
     * @param receive The index on the receive
     * @param pan The pan value to set
     */
    static void SetTrackReceivePan(MediaTrack *media_track, int receive, double pan);

    /**************************************************************************************************************
     *  Track Sends related methods
     *************************************************************************************************************/

    /**
     * Check if the give track has a send at the given index
     * @param media_track The track where we want the send for
     * @param send The index of the send track
     * @return Whether the send is available
     */
    static bool HasTrackSend(MediaTrack *media_track, int send);

    /**
     * Get the name of the destination of the send at the given index for the given track
     * @param media_track The track where we want the send for
     * @param send The index of the send track
     * @return The name of the destination of the send
     */
    static std::string GetTrackSendDestName(MediaTrack *media_track, int send);

    /**
     * Get the send mode for the given send
     * @param media_track The track where we want the send for
     * @param send The index of the send track
     * @return The send mode:
     * - 0 = post-fader,
     * - 1 = pre-fx,
     * - 2 = post-fx (deprecated),
     * - 3 = post-fx
     */
    static int GetTrackSendMode(MediaTrack *media_track, int send);

    /**
     * Get the send automation mode for the given send
     * @param media_track The track where we want the send for
     * @param send The index of the send track
     * @return The send automation mode:
     * - -1 = use track automode,
     * - 0 = trim/off,
     * - 1 = read,
     * - 2 = touch,
     * - 3 = write,
     * - 4 = latch
     */
    static int GetTrackSendAutoMode(MediaTrack *media_track, int send);

    /**
     * Get the string representation for send mode for the given send
     * @param media_track The track where we want the send for
     * @param send The index of the send track
     * @return The send mode:
     * - 0 = Post-Fdr,
     * - 1 = Pre-FX,
     * - 2 = Post-FX,
     * - 3 = Post-FX,
     * - others: Post-Fdr
     */
    static std::string GetTrackSurfaceSendMode(MediaTrack *media_track, int send);

    /**
     * Get the string representation of the send automation mode for the given send
     * @param media_track The track where we want the send for
     * @param send The index of the send track
     * @return The send automation mode:
     * - -1 = Track,
     * - 0 = Trim,
     * - 1 = Read,
     * - 2 = Touch,
     * - 3 = Write,
     * - 4 = Latch,
     */
    static std::string GetTrackSurfaceSendAutoMode(MediaTrack *media_track, int send);

    /**
     * Get the mute state for the given send
     * @param media_track The track where we want the send for
     * @param send The mute state of the send track
     * @return Whether mute is engaged
     */
    static bool GetTrackSendMute(MediaTrack *media_track, int send);

    /**
     * Toggle the mute state for the given send
     * @param media_track The track for the send we need
     * @param send The index on the send
     */
    static void ToggleTrackSendMute(MediaTrack *media_track, int send);

    /**
     * Get the phase state for the given send
     * @param media_track The track where we want the send for
     * @param send The phase state of the send track
     * @return whether phase is engaged
     */
    static bool GetTrackSendPhase(MediaTrack *media_track, int send);

    /**
     * Toggle the phase state for the given send
     * @param media_track The track for the send we need
     * @param send The index on the send
     */
    static void ToggleTrackSendPhase(MediaTrack *media_track, int send);

    /**
     * Get the mono state for the given send
     * @param media_track The track where we want the send for
     * @param send The mute state of the send track
     * @return Whether mono is engaged
     */
    static bool GetTrackSendMono(MediaTrack *media_track, int send);

    /**
     * Toggle the mono state for the given send
     * @param media_track The track for the send we need
     * @param send The index on the send
     */
    static void ToggleTrackSendMono(MediaTrack *media_track, int send);

    /**
     * ??
     * @param media_track The track where we want the send for
     * @param send The mute state of the send track
     * @return Whether mono ois engaged
     */
    static int GetNextTrackSendMode(MediaTrack *media_track, int send);

    /**
     * ??
     * @param media_track The track for the send we need
     * @param send The index on the send
     */
    static void SetNextTrackSendMode(MediaTrack *media_track, int send);

    /**
     * Set the volume for the given send
     * @param media_track The track for the send we need
     * @param send The index on the send
     * @param volume The volume to set
     */
    static void SetTrackSendVolume(MediaTrack *media_track, int send, double volume);

    /**
     * Set the volume for the given send
     * @param media_track The track for the send we need
     * @param send The index on the send
     * @param pan The pan value to set
     */
    static void SetTrackSendPan(MediaTrack *media_track, int send, double pan);

    /**************************************************************************************************************
     *  Project related methods
     *************************************************************************************************************/

    /**
     * Get the Time Mode. `projtimemode2` gets the time mode of the transport bar when not overwritten by the ruler
     * It returns -1 when overwritten by the ruler. In that case we get `projtimemode`, which is the ruler time code.
     * When the ruler time code has a secondary time code, it somehow always return 0
     * @return The projects time mode
     */
    static int GetProjectTimeMode();

    /**
     * Get the offset for the time measure
     * @return the time measure offset
     */
    static int GetProjectMeasureOffset();

    /**
     * Get the offset for the time
     * @return the time offset
     */
    static double GetProjectTimeOffset();

    /**
     *
     * @param tpos
     * @param proj_time_mode
     * @return
     */
    static std::vector<std::string> GetTimeSegments(double tpos, int proj_time_mode);

    /**
     * Get the projects current time
     * @param overwrite_time_code
     * @param new_time_code
     * @return
     */
    static std::vector<std::string> GetProjectTime(bool overwrite_time_code, int new_time_code);

    /**************************************************************************************************************
     *  Media Items related methods
     *************************************************************************************************************/

    /**
     * Check if the given media item is a media item
     * @param media_item The media item to check
     */
    static bool MediaItemHasMidi(MediaItem *media_item);

    /**
     * Check if the given media item is an audio item
     * @param media_item The media item to check
     */
    static bool MediaItemHasAudio(MediaItem *media_item);

    /**
     * Trigger save to store your latest changes
     */
    static void EditSave();

    /**
     * Trigger undo to undo your last saved changes
     */
    static void EditUndo();

    /**
     * Trigger redo to undo your undone changes
     */
    static void EditRedo();

    /**************************************************************************************************************
     *  Window related methods
     *************************************************************************************************************/

    /**
     * Be sure that after selecting a track and the setting to follow tcp is enabled,
     * the selected track is always in view
     * @param media_track The medis track that has to get into view
     */
    static void SetTcpScroll(MediaTrack *media_track);

    /**
     * Get the total height in pixels of the pinned tracks
     * @return
     */
    static int GetPinnedTracksHeight();

    /**
     * Check if the current installed version has a give feature
     * @param feature The feature to check. Currently only: FEATURE_PINNED_TRACKS
     * @return
     */
    static bool VersionHasFeature(Features feature);

    /**************************************************************************************************************
     *  Ext State related methods
     *************************************************************************************************************/

    /**
     * Get the value from the ext state with the given key.
     * If the key is not found, teh default value is returned
     * @param key The key to get the value from
     * @param default_value The value to return when the given key does not exist
     */
    static std::string GetExtState(const std::string &key, std::string default_value);

    /**
     * Set the value to the Ext state with the given key
     * @param key The key to store the value under
     * @param value The value to store
     * @param persist Set to true will store it persistent between projects. Otherwise, it will be stored for the current session
     */
    static void SetExtState(const std::string &key, const std::string &value, bool persist);
};

#endif
