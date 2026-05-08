// ReSharper disable CppRedundantParentheses
#include "../shared/csurf_daw.hpp"
#include <utility>
#include <vector>
#include <regex>
#include <string>
#include "../shared/csurf_utils.hpp"
#include "csurf.h"
#include "csurf_plugin_utils.hpp"

int DAW::sendModes[3] = {0, 1, 3};

/************************************************************************
 * Track
 ************************************************************************/
void DAW::UnSelectAllTracks() {
    for (int i = 0; i < CountTracks(nullptr); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        SetTrackSelected(media_track, false);
    }
    MediaTrack *media_track = GetMasterTrack(nullptr);
    SetTrackSelected(media_track, false);
}

bool DAW::IsTrackArmed(MediaTrack *media_track) {
    int flagsOut;
    GetTrackState(media_track, &flagsOut);

    return hasBit(flagsOut, 6);
}

void DAW::ToggleTrackArmed(MediaTrack *media_track) {
    CSurf_SetSurfaceRecArm(
        media_track,
        CSurf_OnRecArmChange(media_track, static_cast<int>(!IsTrackArmed(media_track))),
        nullptr
    );
}

bool DAW::IsTrackMuted(MediaTrack *media_track) {
    int flagsOut;
    GetTrackState(media_track, &flagsOut);

    return hasBit(flagsOut, 3);
}

void DAW::ToggleTrackMuted(MediaTrack *media_track) {
    CSurf_SetSurfaceMute(
        media_track,
        CSurf_OnMuteChange(media_track, static_cast<int>(!IsTrackMuted(media_track))),
        nullptr
    );
}

void DAW::SetTrackMuted(MediaTrack *media_track, const bool mute) {
    CSurf_SetSurfaceMute(
        media_track,
        CSurf_OnMuteChange(media_track, static_cast<int>(mute)),
        nullptr
    );
}

bool DAW::IsTrackSoloed(MediaTrack *media_track) {
    int flagsOut;
    GetTrackState(media_track, &flagsOut);

    return hasBit(flagsOut, 4);
}

void DAW::ToggleTrackSoloed(MediaTrack *media_track) {
    CSurf_SetSurfaceSolo(
        media_track,
        CSurf_OnSoloChange(media_track, static_cast<int>(!IsTrackSoloed(media_track))),
        nullptr
    );
}

bool DAW::IsTrackSelected(MediaTrack *media_track) {
    return ::IsTrackSelected(media_track);
}

bool DAW::IsTrackParent(MediaTrack *media_track) {
    return doubleToBool(GetMediaTrackInfo_Value(media_track, "I_FOLDERDEPTH"));
}

bool DAW::IsTrackPinned(MediaTrack *media_track) {
    return doubleToBool(GetMediaTrackInfo_Value(media_track, "B_TCPPIN"));
}

bool DAW::IsTrackVisible(MediaTrack *media_track) {
    return doubleToBool(GetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER"));
}

void DAW::SetMixerTrackVisible(MediaTrack *media_track, const bool visible) {
    SetMediaTrackInfo_Value(media_track, "B_SHOWINMIXER", boolToDouble(visible));
}

void DAW::SetTCPTrackVisible(MediaTrack *media_track, const bool visible) {
    SetMediaTrackInfo_Value(media_track, "B_SHOWINTCP", boolToDouble(visible));
}

int DAW::GetTrackPanMode(MediaTrack *media_track) {
    int panMode = 0;
    double pan1 = 0.0;
    double pan2 = 0.0;

    GetTrackUIPan(media_track, &pan1, &pan2, &panMode);

    if (panMode < PAN_MODE_STEREO_PAN) {
        return PAN_MODE_BALANCE_PAN;
    }
    return panMode;
}

std::string DAW::GetTrackName(MediaTrack *media_track) {
    if (media_track == nullptr) {
        return "";
    }

    char trackName[256]; // NOLINT(*-avoid-c-arrays)
    ::GetTrackName(media_track, trackName, sizeof(trackName));

    return {trackName};
}

std::string DAW::GetTrackIndex(MediaTrack *media_track) {
    return std::to_string(static_cast<int>(GetMediaTrackInfo_Value(media_track, "IP_TRACKNUMBER")));
}

std::string DAW::GetTrackInputName(MediaTrack *media_track) {
    int const input_code = static_cast<int>(GetMediaTrackInfo_Value(media_track, "I_RECINPUT"));

    if (hasBit(input_code, 12)) {
        char midi_device_name[256]; // NOLINT(*-avoid-c-arrays)
        int const midi_device_id = (input_code >> 5) & ((1 << 6) - 1);
        GetMIDIInputName(midi_device_id, midi_device_name, sizeof(midi_device_name));

        if (midi_device_name[0] == '\0') {
            return "";
        }

        return {midi_device_name};
    }

    // ReSharper disable once CppRedundantParentheses
    int const input_channel = input_code & ((1 << 5) - 1);
    const char *input_name = GetInputChannelName(input_channel);

    if (input_name == nullptr) {
        return "";
    }

    return {input_name};
}

std::string DAW::GetTrackMonitorMode(MediaTrack *media_track) {
    int const monitor_mode = static_cast<int>(GetMediaTrackInfo_Value(media_track, "I_RECMON"));

    switch (monitor_mode) {
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

std::string DAW::GetTrackRecordingMode(MediaTrack *media_track) {
    const int recording_mode = static_cast<int>(GetMediaTrackInfo_Value(media_track, "I_RECMODE"));

    switch (recording_mode) {
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

bool DAW::GetTrackPhase(MediaTrack *media_track) {
    return static_cast<bool>(GetMediaTrackInfo_Value(media_track, "B_PHASE"));
}

int DAW::GetTrackAutomationMode(MediaTrack *media_track) {
    return static_cast<int>(GetMediaTrackInfo_Value(media_track, "I_AUTOMODE"));
}

ButtonColor DAW::GetTrackColor(MediaTrack *media_track) {
    ButtonColor color{};
    int red = 0x7f;
    int green = 0x7f;
    int blue = 0x7f;

    const int track_color = ::GetTrackColor(media_track);
    if (track_color != 0) {
        ColorFromNative(track_color, &red, &green, &blue);
    }
    color.SetColor(red / 2, green / 2, blue / 2);

    return color;
}

void DAW::ToggleTrackSelected(MediaTrack *media_track) {
    SetTrackSelected(media_track, !IsTrackSelected(media_track));
}

void DAW::SetUniqueSelectedTrack(MediaTrack *media_track) {
    if (media_track != nullptr) {
        UnSelectAllTracks();
        SetTrackSelected(media_track, true);
        CSurf_OnSelectedChange(media_track, 1); // this will automatically update the surface
        if (!::IsTrackVisible(media_track, true)) {
            SetMixerScroll(media_track);
        }
    }
}

void DAW::SetTrackSoloUnique(MediaTrack *media_track) {
    Main_OnCommandEx(40340, 0, nullptr); // Track: Un-solo all tracks
    ToggleTrackSoloed(media_track);
}

void DAW::SetSelectedTracksRange(MediaTrack *media_track) {
    int const index = static_cast<int>(GetMediaTrackInfo_Value(media_track, "IP_TRACKNUMBER"));
    const int selected_track_count = CountSelectedTracks(nullptr);
    const int first_index = static_cast<int>(
        GetMediaTrackInfo_Value(GetSelectedTrack(nullptr, 0), "IP_TRACKNUMBER")
    );
    const int last_index = static_cast<int>(
        GetMediaTrackInfo_Value(GetSelectedTrack(nullptr, selected_track_count - 1), "IP_TRACKNUMBER")
    );
    const int start_index = index < first_index
                                ? index
                                : index < last_index // NOLINT(*-avoid-nested-conditional-operator)
                                      ? last_index
                                      : first_index;
    const int end_index = index < first_index ? first_index : index;

    for (int i = start_index - 1; i < end_index; i++) {
        MediaTrack *selectable_track = GetTrack(nullptr, i);
        SetTrackSelected(selectable_track, true);
    }
}

double DAW::GetTrackPeakInfo(MediaTrack *media_track) {
    if (media_track != nullptr) {
        return (Track_GetPeakInfo(media_track, 0) + Track_GetPeakInfo(media_track, 1)) / 2;
    }
    return 0.0;
}

int DAW::GetTrackSurfacePeakInfo(MediaTrack *media_track) {
    return static_cast<int>(volToNormalized(GetTrackPeakInfo(media_track)) * 127.0);
}

void DAW::SetTrackPan1(MediaTrack *media_track, const double value) {
    CSurf_SetSurfacePan(media_track, CSurf_OnPanChange(media_track, value, false), nullptr);
}

void DAW::SetTrackPan2(MediaTrack *media_track, const double value) {
    SetMediaTrackInfo_Value(media_track, "D_WIDTH", CSurf_OnWidthChange(media_track, value, false));
}

void DAW::SetTrackVolume(MediaTrack *media_track, const double value) {
    CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, value, false), nullptr);
}

double DAW::GetTrackVolume(MediaTrack *media_track) {
    double volume;
    double pan1 = 0.0;
    GetTrackUIVolPan(media_track, &volume, &pan1);

    return volume;
}

/************************************************************************
 * Track FX
 ************************************************************************/
bool DAW::TrackHasFx(MediaTrack *media_track) {
    bool has_fx = false;
    char plugin_name[256]; // NOLINT(*-avoid-c-arrays)

    for (int i = 0; i < TrackFX_GetCount(media_track); i++) {
        TrackFX_GetFXName(media_track, i, plugin_name, std::size(plugin_name));
        has_fx = PluginUtils::IsPluginFX(plugin_name);
        if (has_fx) {
            break;
        }
    }

    return has_fx;
}

bool DAW::HasTrackFx(MediaTrack *media_track, const int fxIndex) {
    char plugin_name[256] = ""; // NOLINT(*-avoid-c-arrays)
    return TrackFX_GetFXName(media_track, fxIndex, plugin_name, std::size(plugin_name));
}

std::string DAW::GetTrackFxName(MediaTrack *media_track, const int fxIndex, const bool full) {
    char plugin_name[256] = ""; // NOLINT(*-avoid-c-arrays)
    if (!TrackFX_GetFXName(media_track, fxIndex, plugin_name, std::size(plugin_name))) {
        return "No FX";
    }

    if (full) {
        return PluginUtils::StripPluginNamePrefixes(plugin_name);
    }

    std::vector<std::string> plugin_name_parts = split(
        PluginUtils::StripPluginNamePrefixes(PluginUtils::StripPluginChannelPostfix(plugin_name).data()), " (");

    if (plugin_name_parts.size() > 1) {
        plugin_name_parts.pop_back();
    }

    return join(plugin_name_parts, " (");
}

std::string DAW::GetTrackFxType(MediaTrack *media_track, const int fxIndex) {
    char plugin_name[256] = ""; // NOLINT(*-avoid-c-arrays)

    if (!TrackFX_GetFXName(media_track, fxIndex, plugin_name, std::size(plugin_name))) {
        return "No FX";
    }

    const std::vector<std::string> plugin_name_parts = split(plugin_name, PREFIX_SEPARATOR);

    if (plugin_name_parts.size() > 1) {
        return toLowerCase(plugin_name_parts.at(0));
    }

    return "unknown";
}

std::string DAW::GetTrackFxDeveloper(MediaTrack *media_track, const int fxIndex) {
    char plugin_name[256] = ""; // NOLINT(*-avoid-c-arrays)

    if (!TrackFX_GetFXName(media_track, fxIndex, plugin_name, std::size(plugin_name))) {
        return "No Dev";
    }

    return PluginUtils::ExtractDeveloperName(plugin_name);
}

bool DAW::GetTrackFxEnabled(MediaTrack *media_track, const int fxIndex) {
    return TrackFX_GetEnabled(media_track, fxIndex);
}

bool DAW::GetTrackFxOffline(MediaTrack *media_track, const int fxIndex) {
    return TrackFX_GetOffline(media_track, fxIndex);
}

std::string DAW::GetTrackFxSurfaceEnabled(MediaTrack *media_track, const int fxIndex) {
    return TrackFX_GetEnabled(media_track, fxIndex) ? "Enabled" : "Bypassed";
}

bool DAW::GetTrackFxPanelOpen(MediaTrack *media_track, const int fxIndex) {
    if (TrackFX_GetCount(media_track) < 1) {
        return false;
    }
    return TrackFX_GetOpen(media_track, fxIndex);
}

bool DAW::GetTrackFxBypassed(MediaTrack *media_track) {
    return !doubleToBool(GetMediaTrackInfo_Value(media_track, "I_FXEN"));
}

void DAW::ToggleTrackFxBypass(MediaTrack *media_track) {
    if (GetTrackFxBypassed(media_track)) {
        SetMediaTrackInfo_Value(media_track, "I_FXEN", 1.0);
    } else {
        SetMediaTrackInfo_Value(media_track, "I_FXEN", 0.0);
    }
}

/************************************************************************
 * Track FX Param
 ************************************************************************/
std::string DAW::GetTrackFxParamName(MediaTrack *media_track, const int fxIndex, const int param) {
    char param_name[256] = ""; // NOLINT(*-avoid-c-arrays)

    if (TrackFX_GetParamName(media_track, fxIndex, param, param_name, std::size(param_name))) {
        return param_name;
    }

    return "No FX";
}

int DAW::GetTrackFxParamNbSteps(MediaTrack *media_track, const int fxIndex, const int param) {
    int nb_steps = 1;
    double step_out;
    double _dummy;
    bool is_toggle;

    if (TrackFX_GetParameterStepSizes(media_track, fxIndex, param, &step_out, &_dummy, &_dummy, &is_toggle)) {
        if (step_out > 0) {
            nb_steps = static_cast<int>(std::lround(1.0 / step_out)) + 1;
        }
    }

    return nb_steps;
}

void DAW::SetTrackFXParamUntouched(MediaTrack *media_track, const int fxIndex) {
    TrackFX_SetNamedConfigParm(media_track, fxIndex, "last_touched", "-1");
}

/************************************************************************
 * Track Receive
 ************************************************************************/
bool DAW::HasTrackReceive(MediaTrack *media_track, const int receive) {
    return GetSetTrackSendInfo(media_track, SEND_MODE_RECEIVE, receive, "P_SRCTRACK", nullptr) != nullptr;
}

std::string DAW::GetTrackReceiveSrcName(MediaTrack *media_track, const int receive) {
    auto *src_track = static_cast<MediaTrack *>(GetSetTrackSendInfo(media_track, SEND_MODE_RECEIVE, receive,
                                                                    "P_SRCTRACK", nullptr));

    if (src_track != nullptr) {
        return GetTrackName(src_track);
    }

    return "No Source";
}

int DAW::GetTrackReceiveMode(MediaTrack *media_track, const int receive) {
    return static_cast<int>(GetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "I_SENDMODE"));
}

int DAW::GetTrackReceiveAutoMode(MediaTrack *media_track, const int receive) {
    return static_cast<int>(GetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "I_AUTOMODE"));
}

std::string DAW::GetTrackSurfaceReceiveMode(MediaTrack *media_track, const int receive) {
    return GetSendModeString(GetTrackReceiveMode(media_track, receive));
}

std::string DAW::GetTrackSurfaceReceiveAutoMode(MediaTrack *media_track, const int receive) {
    return GetAutomationString(GetTrackReceiveAutoMode(media_track, receive));
}

bool DAW::GetTrackReceiveMute(MediaTrack *media_track, const int receive) {
    bool receive_mute = false;
    GetTrackReceiveUIMute(media_track, receive, &receive_mute);

    return receive_mute;
}

void DAW::ToggleTrackReceiveMute(MediaTrack *media_track, const int receive) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "B_MUTE",
                           boolToDouble(!GetTrackReceiveMute(media_track, receive)));
}

bool DAW::GetTrackReceivePhase(MediaTrack *media_track, const int receive) {
    return doubleToBool(GetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "B_PHASE"));
}

void DAW::ToggleTrackReceivePhase(MediaTrack *media_track, const int receive) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "B_PHASE",
                           boolToDouble(!GetTrackReceivePhase(media_track, receive)));
}

bool DAW::GetTrackReceiveMono(MediaTrack *media_track, const int receive) {
    return doubleToBool(GetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "B_MONO"));
}

void DAW::ToggleTrackReceiveMono(MediaTrack *media_track, const int receive) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "B_MONO",
                           boolToDouble(!GetTrackReceiveMono(media_track, receive)));
}

int DAW::GetNextTrackReceiveMode(MediaTrack *media_track, const int receive) {
    return sendModes[(GetTrackReceiveMode(media_track, receive) + 1) % 4];
}

void DAW::SetNextTrackReceiveMode(MediaTrack *media_track, const int receive) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "I_SENDMODE",
                           GetNextTrackReceiveMode(media_track, receive));
}

void DAW::SetTrackReceiveVolume(MediaTrack *media_track, const int receive, const double volume) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "D_VOL",
                           CSurf_OnRecvVolumeChange(media_track, receive, volume, false));
}

void DAW::SetTrackReceivePan(MediaTrack *media_track, const int receive, const double pan) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_RECEIVE, receive, "D_PAN",
                           CSurf_OnRecvPanChange(media_track, receive, pan, false));
}

bool DAW::HasTrackSend(MediaTrack *media_track, const int send) {
    return GetSetTrackSendInfo(media_track, SEND_MODE_SEND, send, "P_DESTTRACK", nullptr) != nullptr;
}

std::string DAW::GetTrackSendDestName(MediaTrack *media_track, const int send) {
    auto *dest_track = static_cast<MediaTrack *>(GetSetTrackSendInfo(media_track, 0, send, "P_DESTTRACK", nullptr));

    if (dest_track != nullptr) {
        return GetTrackName(dest_track);
    }

    return "No Dest";
}

int DAW::GetTrackSendMode(MediaTrack *media_track, const int send) {
    return static_cast<int>(GetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "I_SENDMODE"));
}

int DAW::GetTrackSendAutoMode(MediaTrack *media_track, const int send) {
    return static_cast<int>(GetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "I_AUTOMODE"));
}

std::string DAW::GetTrackSurfaceSendMode(MediaTrack *media_track, const int send) {
    return GetSendModeString(GetTrackSendMode(media_track, send));
}

std::string DAW::GetTrackSurfaceSendAutoMode(MediaTrack *media_track, const int send) {
    return GetAutomationString(GetTrackSendAutoMode(media_track, send));
}

bool DAW::GetTrackSendMute(MediaTrack *media_track, const int send) {
    bool send_mute = false;
    GetTrackSendUIMute(media_track, send, &send_mute);

    return send_mute;
}

void DAW::ToggleTrackSendMute(MediaTrack *media_track, const int send) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "B_MUTE",
                           boolToDouble(!GetTrackSendMute(media_track, send)));
}

bool DAW::GetTrackSendPhase(MediaTrack *media_track, const int send) {
    return doubleToBool(GetTrackSendInfo_Value(media_track, 0, send, "B_PHASE"));
}

void DAW::ToggleTrackSendPhase(MediaTrack *media_track, const int send) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "B_PHASE",
                           boolToDouble(!GetTrackSendPhase(media_track, send)));
}

bool DAW::GetTrackSendMono(MediaTrack *media_track, const int send) {
    return doubleToBool(GetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "B_MONO"));
}

void DAW::ToggleTrackSendMono(MediaTrack *media_track, const int send) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "B_MONO",
                           boolToDouble(!GetTrackSendMono(media_track, send)));
}

int DAW::GetNextTrackSendMode(MediaTrack *media_track, const int send) {
    return sendModes[(GetTrackSendMode(media_track, send) + 1) % 4];
}

void DAW::SetNextTrackSendMode(MediaTrack *media_track, const int send) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "I_SENDMODE",
                           GetNextTrackSendMode(media_track, send));
}

void DAW::SetTrackSendVolume(MediaTrack *media_track, const int send, const double volume) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "D_VOL",
                           CSurf_OnSendVolumeChange(media_track, send, volume, false));
}

void DAW::SetTrackSendPan(MediaTrack *media_track, const int send, const double pan) {
    SetTrackSendInfo_Value(media_track, SEND_MODE_SEND, send, "D_PAN",
                           CSurf_OnSendPanChange(media_track, send, pan, false));
}

bool DAW::MediaItemHasMidi(MediaItem *media_item) {
    bool has_midi = false;

    for (int i = 0; i < GetMediaItemNumTakes(media_item); i++) {
        MediaItem_Take *take = GetMediaItemTake(media_item, i);
        has_midi = TakeIsMIDI(take);

        if (has_midi) {
            break;
        }
    }

    return has_midi;
}

bool DAW::MediaItemHasAudio(MediaItem *media_item) {
    bool has_audio = false;

    for (int i = 0; i < GetMediaItemNumTakes(media_item); i++) {
        MediaItem_Take *take = GetMediaItemTake(media_item, i);
        has_audio = !TakeIsMIDI(take);

        if (has_audio) {
            break;
        }
    }

    return has_audio;
}

int DAW::GetProjectTimeMode() {
    int size = 0;
    int time_mode_index = projectconfig_var_getoffs("projtimemode2", &size);

    const int *time_mode2_ptr_ = static_cast<int *>(projectconfig_var_addr(nullptr, time_mode_index));
    int val = *time_mode2_ptr_;

    if (val == -1) {
        time_mode_index = projectconfig_var_getoffs("projtimemode", &size);

        const int *time_mode2_ptr = static_cast<int *>(projectconfig_var_addr(nullptr, time_mode_index));
        val = *time_mode2_ptr;
    }

    // This will get rid of all the second measure data
    return val & 255;
}

int DAW::GetProjectMeasureOffset() {
    int const time_mode_index = projectconfig_var_getoffs("projmeasoffs", nullptr);
    const int *timeMode2Ptr_ = static_cast<int *>(projectconfig_var_addr(nullptr, time_mode_index));
    int const val = *timeMode2Ptr_;

    return val;
}

double DAW::GetProjectTimeOffset() {
    int const time_mode_index = projectconfig_var_getoffs("projtimeoffs", nullptr);
    double const *timeMode2Ptr_ = static_cast<double *>(projectconfig_var_addr(nullptr, time_mode_index));
    double const val = *timeMode2Ptr_;

    return val;
}

std::vector<std::string> DAW::GetTimeSegments(const double tpos, const int proj_time_mode) {
    char beat_time[128]; // NOLINT(*-avoid-c-arrays)
    std::vector<std::string> value;
    std::vector<std::string> hours;
    std::vector<std::string> mins;

    format_timestr_len(tpos, beat_time, sizeof beat_time, 0, proj_time_mode > -1 ? proj_time_mode : 2);

    switch (proj_time_mode) {
        case -1: // Ruler, will be converted to Beat
        case 1: // Beats + Time
        case 2: // Beats
        case 6: // Beats Minimal
        case 10: // Measure fractions
            value = split(std::string(beat_time), ".");
            // The first 2 items are 0-based so need an extra 1
            value[0] = std::to_string(std::stoi(value[0]) + 1);
            value[1] = std::to_string(std::stoi(value[1]) + 1);
            break;

        case 0: // Minutes:Seconds
            hours = split(std::string(beat_time), ":");
            mins = split(hours[1], ".");
            value.push_back(hours.at(0));
            value.push_back(mins.at(0));
            value.push_back(mins.at(1));
            break;

        case 5: // Hours:Minutes:Seconds:Frames
            value = split(std::string(beat_time), ":");
            break;

        case 3: // Seconds
            value = split(std::string(beat_time), ".");
            break;

        case 4: // Samples
        case 8: // Absolute Frames
            value = cutString(beat_time, 3);
            break;
        default: ;
    }

    return value;
}

std::vector<std::string> DAW::GetProjectTime(const bool overwrite_time_code, const int new_time_code) {
    double play_position;
    double play_offset;
    double beat_length = 0;
    double _dummy2 = 0;
    int *_dummy = nullptr;

    int proj_time_mode = new_time_code;

    if (!overwrite_time_code) {
        proj_time_mode = GetProjectTimeMode();
    }

    int const play_state = GetPlayStateEx(nullptr);
    bool const is_playing = hasBit(play_state, 0);

    if (is_playing) {
        play_position = GetPlayPositionEx(nullptr);
    } else {
        play_position = GetCursorPositionEx(nullptr);
    }

    switch (proj_time_mode) {
        case -1:
        case 1:
        case 2:
            TimeMap_GetMeasureInfo(nullptr, 0, &_dummy2, &beat_length, _dummy, _dummy, &_dummy2);
            play_offset = TimeMap2_beatsToTime(nullptr, static_cast<int>(beat_length) * GetProjectMeasureOffset(),
                                               _dummy);
            break;

        default:
            play_offset = GetProjectTimeOffset();
    }

    return GetTimeSegments(play_position + play_offset, proj_time_mode);
}

void DAW::EditSave() {
    Main_SaveProject(nullptr, false);
}

void DAW::EditUndo() {
    Undo_DoUndo2(nullptr);
}

void DAW::EditRedo() {
    Undo_DoRedo2(nullptr);
}

void DAW::SetTcpScroll(MediaTrack *media_track) {
    PreventUIRefresh(1);
    // Get the y position of the provided track relative to the top of the Arrange view
    int const track_tcpy = static_cast<int>(GetMediaTrackInfo_Value(media_track, "I_TCPY"));
    // Get the main window
    // ReSharper disable once CppLocalVariableMayBeConst
    HWND mainHWND = GetMainHwnd();
    // Get the Arrange view
    // ReSharper disable once CppLocalVariableMayBeConst
    HWND arrangeHWND = findWindowChildByID(mainHWND, 1000);
    // Get the scroll position and other data of the Arrange view
    int scroll_position;
    int scroll_pageSize;
    int scroll_min;
    int scroll_max;
    int scroll_track_pos;

    int const pinned_tracks_height = VersionHasFeature(FEATURE_PINNED_TRACKS) ? GetPinnedTracksHeight() : 0;
    const bool done = getWindowScrollInfo(arrangeHWND, "v", &scroll_position, &scroll_pageSize, &scroll_min,
                                          &scroll_max,
                                          &scroll_track_pos);

    if (done) {
        // Set the new Arrange scroll position
        setWindowScrollPos(arrangeHWND, "v", track_tcpy + scroll_position - pinned_tracks_height);
    }

    PreventUIRefresh(-1);
}

int DAW::GetPinnedTracksHeight() {
    int pinned_height = 0;

    for (int i = 0; i < GetNumTracks(); i++) {
        MediaTrack *media_track = GetTrack(nullptr, i);
        if (IsTrackPinned(media_track)) {
            pinned_height += static_cast<int>(GetMediaTrackInfo_Value(media_track, "I_TCPH"));
        }
    }
    // Return the actual height plus the gap
    return pinned_height + 10;
}

bool DAW::VersionHasFeature(const Features feature) {
    double const current_version = std::stod(GetAppVersion());
    return current_version >= feature_versions[feature];
}

std::string DAW::GetExtState(const std::string &key, std::string default_value) {
    return HasExtState(EXT_STATE_SECTION, key.c_str())
               ? ::GetExtState(EXT_STATE_SECTION, key.c_str())
               : std::move(default_value);
}

void DAW::SetExtState(const std::string &key, const std::string &value, const bool persist) {
    ::SetExtState(EXT_STATE_SECTION, key.c_str(), value.c_str(), persist);
}
