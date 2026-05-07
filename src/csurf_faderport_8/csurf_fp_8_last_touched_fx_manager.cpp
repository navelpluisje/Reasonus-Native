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
    int fx_number = -1;
    int param_number = -1;
    const char *track_name = "";
    char fxName[256];
    char paramName[256];
    char paramValueString[256];
    double paramValue = 0.0;

    if (hasLastTouchedFxEnabled != context->GetLastTouchedFxMode() && context->GetLastTouchedFxMode()) {
        force_update = true;
    }

    if (GetLastTouchedFX(&track_number, &fx_number, &param_number)) {
        // Somehow this prevents reaper from crashing while selecting link mode.
        // TODO: finsd a better way for this
        char buffer[250];
        snprintf(buffer, sizeof(buffer), "Zone -- %d\n", param_number);

        if (MediaTrack *media_track = GetTrack(nullptr, track_number - 1)) {
            track_name = static_cast<const char *>(GetSetMediaTrackInfo(media_track, "P_NAME", nullptr));
            TrackFX_GetFXName(media_track, fx_number, fxName, sizeof(fxName));
            TrackFX_GetParamName(media_track, fx_number, param_number, paramName, std::size(paramName));
            TrackFX_GetFormattedParamValue(
                media_track, fx_number,
                param_number,
                paramValueString,
                std::size(paramValueString)
            );
            paramValue = TrackFX_GetParamNormalized(media_track, fx_number, param_number);
        }
    } else {
        track_name = "No Track";
    }

    track->SetDisplayMode(DISPLAY_MODE_2, force_update);
    track->SetDisplayLine(0, ALIGN_LEFT, track_name, NON_INVERT, force_update);
    track->SetDisplayLine(1, ALIGN_LEFT,
                          param_number < 0 ? "No Param" : PluginUtils::StripPluginNamePrefixes(fxName).c_str(), INVERT,
                          force_update);
    track->SetDisplayLine(2, ALIGN_LEFT, param_number < 0 ? "  " : std::string(paramName).c_str(), NON_INVERT,
                          force_update);
    track->SetDisplayLine(3, ALIGN_CENTER, param_number < 0 ? "  " : paramValueString, NON_INVERT, force_update);
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

void CSurf_FP_8_LastTouchedFXManager::HandleFaderTouch() const {
};

void CSurf_FP_8_LastTouchedFXManager::HandleFaderMove(const int msb, const int lsb) const {
    int trackNumber = -1;
    int fxNumber = -1;
    int paramNumber = -1;

    if (GetLastTouchedFX(&trackNumber, &fxNumber, &paramNumber)) {
        if (MediaTrack *media_track = GetTrack(0, trackNumber - 1)) {
            TrackFX_SetParamNormalized(media_track, fxNumber, paramNumber, int14ToNormalized(msb, lsb));
        }
    }
};
