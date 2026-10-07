#include "csurf_fp_8_last_touched_fx_manager.hpp"
#include "../shared/csurf_plugin_utils.hpp"

CSurf_FP_8_LastTouchedFXManager::CSurf_FP_8_LastTouchedFXManager(
    CSurf_FP_8_Track *track,
    CSurf_Context *context,
    midi_Output *m_midiout) : track(track), context(context), m_midiout(m_midiout), colorActive(), colorDim() {
    track->SetMuteButtonValue(BTN_VALUE_OFF);
    track->SetSoloButtonValue(BTN_VALUE_OFF);
    track->SetSelectButtonValue(BTN_VALUE_OFF);
};

void CSurf_FP_8_LastTouchedFXManager::UpdateTrack(bool force_update) {
    int track_number = -1;
    int plugin_index = -1;
    int param_index = -1;
    auto track_name = "";
    char fxName[256];
    char paramName[256];
    char paramValueString[256];
    double paramValue = 0.0;

    if (hasLastTouchedFxEnabled != context->GetLastTouchedFxMode() && context->GetLastTouchedFxMode()) {
        force_update = true;
    }

    if (GetLastTouchedFX(&track_number, &plugin_index, &param_index)) {
        // Somehow this prevents reaper from crashing while selecting link mode.
        // TODO: finsd a better way for this
        char buffer[250];
        snprintf(buffer, sizeof(buffer), "Zone -- %d\n", param_index);

        if (MediaTrack *media_track = GetTrack(nullptr, track_number - 1)) {
            track_name = static_cast<const char *>(GetSetMediaTrackInfo(media_track, "P_NAME", nullptr));
            TrackFX_GetFXName(media_track, plugin_index, fxName, sizeof(fxName));
            TrackFX_GetParamName(media_track, plugin_index, param_index, paramName, std::size(paramName));
            TrackFX_GetFormattedParamValue(
                media_track, plugin_index,
                param_index,
                paramValueString,
                std::size(paramValueString)
            );
            paramValue = TrackFX_GetParamNormalized(media_track, plugin_index, param_index);
        }
    } else {
        track_name = "No Track";
    }

    track->SetDisplayMode(DISPLAY_MODE_2, force_update);
    track->SetDisplayLine(
        0,
        ALIGN_LEFT,
        track_name,
        NON_INVERT,
        force_update
    );
    track->SetDisplayLine(
        1,
        ALIGN_LEFT,
        param_index < 0 ? "No Param" : PluginUtils::StripPluginNamePrefixes(fxName).c_str(),
        INVERT,
        force_update
    );
    track->SetDisplayLine(
        2,
        ALIGN_LEFT,
        param_index < 0 ? "  " : std::string(paramName).c_str(),
        NON_INVERT,
        force_update
    );
    track->SetDisplayLine(
        3,
        ALIGN_CENTER,
        param_index < 0 ? "  " : paramValueString,
        NON_INVERT,
        force_update
    );
    track->SetFaderValue(static_cast<int>(paramValue * 16383.0), force_update);

    track->SetMuteButtonValue(BTN_VALUE_OFF, force_update);
    track->SetSoloButtonValue(BTN_VALUE_OFF, force_update);
    track->SetSelectButtonValue(BTN_VALUE_OFF, force_update);

    hasLastTouchedFxEnabled = context->GetLastTouchedFxMode();
};

void CSurf_FP_8_LastTouchedFXManager::HandleSelectClick(const int index) const {
    (void) index;
};

void CSurf_FP_8_LastTouchedFXManager::HandleMuteClick(const int index) const {
    (void) index;
};

void CSurf_FP_8_LastTouchedFXManager::HandleSoloClick(const int index) const {
    (void) index;
};

void CSurf_FP_8_LastTouchedFXManager::HandleFaderTouch(int _, const int value) const {
    if (value > 0) {
        return;
    }
    if (context->GetLastTouchedFxMode()) {
        int track_number = -1;
        int plugin_index = -1;
        int param_index = -1;

        // Tell REAPER we do not alter the automatiuon anymore
        if (GetLastTouchedFX(&track_number, &plugin_index, &param_index)) {
            if (MediaTrack *media_track = GetTrack(nullptr, track_number - 1)) {
                TrackFX_EndParamEdit(media_track, plugin_index, param_index);
            }
        }
    }
};

void CSurf_FP_8_LastTouchedFXManager::HandleFaderMove(const int msb, const int lsb) const {
    int track_number = -1;
    int plugin_index = -1;
    int param_index = -1;

    if (GetLastTouchedFX(&track_number, &plugin_index, &param_index)) {
        if (MediaTrack *media_track = GetTrack(0, track_number - 1)) {
            TrackFX_SetParamNormalized(media_track, plugin_index, param_index, int14ToNormalized(msb, lsb));
        }
    }
};
