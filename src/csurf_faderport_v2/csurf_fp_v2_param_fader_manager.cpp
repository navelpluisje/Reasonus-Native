#ifndef CSURF_FP_V2_PARAM_FADER_MANAGER_H_
#define CSURF_FP_V2_PARAM_FADER_MANAGER_H_

#include "csurf_fp_v2_fader_manager.hpp"
#include "csurf_fp_v2_navigator.hpp"
#include "reaper_plugin_functions.h"

class CSurf_FP_V2_ParamFaderManager : public CSurf_FP_V2_FaderManager {
protected:
    int previous_automation_mode;

public:
    CSurf_FP_V2_ParamFaderManager(
        CSurf_FP_V2_Navigator *navigator,
        CSurf_Context *context,
        CSurf_FP_V2_Track *track,
        midi_Output *m_midiout
    ) : CSurf_FP_V2_FaderManager(navigator, context, track, m_midiout) {
        previous_automation_mode = 0;
        touch_start = false;
    }

    ~CSurf_FP_V2_ParamFaderManager() override = default;

    void UpdateTracks(const bool force_update) override {
        int track_number = -1;
        int plugin_index = -1;
        int param_index = -1;

        GetLastTouchedFX(&track_number, &plugin_index, &param_index);
        if (MediaTrack *_media_track = GetTrack(nullptr, track_number - 1)) {
            const double param_value = TrackFX_GetParamNormalized(_media_track, plugin_index, param_index);
            track->SetFaderValue(static_cast<int>(param_value * 16383.0), force_update);
        }
    }

    void HandleFaderTouch(const int value) override {
        int track_number = -1;
        int plugin_index = -1;
        int plugin_param_index = -1;

        if (!GetLastTouchedFX(&track_number, &plugin_index, &plugin_param_index)) {
            return;
        }

        MediaTrack *media_track = GetTrack(nullptr, track_number - 1);
        if (media_track == nullptr) {
            return;
        }

        /**
         * Check if we have the proper conditions to continue with Single point automation
         * All pan related values (except for width) are displayed reversed.
         * Therefore some fo the values are inverted as well, to make it mork as it should
         */
        if (HasSinglePointAutomation(media_track, value, true)) {
            logInteger("HandleFaderTouch", value);
            double parameter_value;
            double dummy;

            // Get the value to write for the automation
            if (value > 0) {
                previous_automation_mode = GetTrackAutomationMode(media_track);
                // We need to set it to read first to get the actual volume of the envelope
                SetTrackAutomationMode(media_track, AUTOMATION_READ);
                parameter_value = TrackFX_GetParam(media_track, plugin_index, plugin_param_index, &dummy, &dummy);
                SetTrackAutomationMode(media_track, AUTOMATION_TRIM);
            } else {
                parameter_value = TrackFX_GetParam(media_track, plugin_index, plugin_param_index, &dummy, &dummy);
                SetTrackAutomationMode(media_track, previous_automation_mode);
            }

            HandleSinglePointAutomation(
                media_track,
                value,
                fmt::format("{}_{}_{}", 0, plugin_index, plugin_param_index),
                parameter_value,
                SPA_Plugin,
                plugin_index,
                plugin_param_index
            );
        } else {
            if (value > 0 || media_track == nullptr) {
                return;
            }
            // Tell REAPER we do not alter the automation anymore
            TrackFX_EndParamEdit(media_track, plugin_index, plugin_param_index);
        }
    }

    void HandleFaderMove(const int msb, const int lsb) override {
        int track_number = -1;
        int plugin_index = -1;
        int param_index = -1;

        if (GetLastTouchedFX(&track_number, &plugin_index, &param_index)) {
            if (MediaTrack *media_track = GetTrack(nullptr, track_number - 1)) {
                TrackFX_SetParamNormalized(media_track, plugin_index, param_index, int14ToNormalized(msb, lsb));
            }
        }
    }
};

#endif
