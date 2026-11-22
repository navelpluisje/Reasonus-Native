#ifndef CSURF_FP_V2_PAN_FADER_MANAGER_H_
#define CSURF_FP_V2_PAN_FADER_MANAGER_H_

#include "csurf_fp_v2_fader_manager.hpp"
#include "csurf_fp_v2_navigator.hpp"
#include "csurf_fp_v2_track.hpp"
#include "reaper_plugin.h"
#include "reaper_plugin_functions.h"
#include "../shared/csurf_daw.hpp"
#include "../shared/csurf_single_point_automation_item.hpp"
#include "fmt/chrono.h"

class CSurf_FP_V2_PanFaderManager : public CSurf_FP_V2_FaderManager {
    std::string GetChunkName(const int pan_mode) const {
        if (!context->GetShiftLeft()) {
            return pan_mode == PAN_MODE_DUAL_PAN ? "<DUALPANENVL2" : "<PANENV2";
        }
        if (context->GetShiftLeft() && pan_mode > PAN_MODE_BALANCE_PAN) {
            return pan_mode == PAN_MODE_DUAL_PAN ? "<DUALPANENV2" : "<WIDTHENV2";
        }
        return "";
    }

    void GetPanValue(int *panValue) const {
        double pan1 = 0.0;
        double pan2 = 0.0;
        int pan_mode = 0;

        MediaTrack *media_track = navigator->GetControllerTrack();

        if (context->GetMasterFaderMode()) {
            media_track = GetMasterTrack(nullptr);
        }

        GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);

        if (context->GetShiftLeft()) {
            *panValue = static_cast<int>(panToNormalized(pan2) * 16383.0);
        } else {
            *panValue = static_cast<int>(panToNormalized(pan1) * 16383.0);
        }
    }

public:
    CSurf_FP_V2_PanFaderManager(
        CSurf_FP_V2_Navigator *navigator,
        CSurf_Context *context,
        CSurf_FP_V2_Track *track,
        midi_Output *m_midiout
    ) : CSurf_FP_V2_FaderManager(navigator, context, track, m_midiout) {
        touch_start = false;
    }

    ~CSurf_FP_V2_PanFaderManager() override = default;

    void UpdateTracks(const bool force_update) override {
        int pan_value = 0;
        GetPanValue(&pan_value);

        if (!context->GetFaderDisabled() || !settings->GetCanDisableFader()) {
            track->SetFaderValue(pan_value, force_update);
        }
    }

    void HandleFaderTouch(const int value) override {
        MediaTrack *media_track = navigator->GetControllerTrack();
        const int pan_mode = DAW::GetTrackPanMode(media_track);

        if (GetTrackAutomationMode(media_track) == AUTOMATION_TOUCH) {
            touch_start = value > 0 ? 1 : 0;
            if (touch_start > 0) {
                if (context->GetShiftLeft()) {
                    DAW::SetTrackPan2(media_track, DAW::GetTrackPan1(media_track));
                } else {
                    DAW::SetTrackPan1(media_track, DAW::GetTrackPan2(media_track));
                }
            }
        }

        /**
         * Check if we have the proper conditions to continue with Single point automation
         */
        if (HasSinglePointAutomation(media_track, value, false)) {
            const std::string chunk_name = GetChunkName(pan_mode);
            double pan1 = 0.0;
            double pan2 = 0.0;
            int dummy = 0;

            if (value > 0) {
                // We need to set it to read first to get the actual volume of the envelope
                SetTrackAutomationMode(media_track, AUTOMATION_READ);
                GetTrackUIPan(media_track, &pan1, &pan2, &dummy);
                SetTrackAutomationMode(media_track, AUTOMATION_TRIM);
            } else {
                GetTrackUIPan(media_track, &pan1, &pan2, &dummy);
            }

            if (context->GetShiftLeft() && pan_mode == PAN_MODE_DUAL_PAN) {
                pan2 = 0 - pan2;
            }

            HandleSinglePointAutomation(
                media_track,
                value,
                chunk_name,
                !context->GetShiftLeft() ? 0 - pan1 : pan2,
                SPA_Track
            );
        }
    }

    void HandleFaderMove(const int msb, const int lsb) override {
        MediaTrack *media_track = navigator->GetControllerTrack();

        if (context->GetShiftLeft()) {
            DAW::SetTrackPan2(media_track, normalizedToPan(int14ToNormalized(msb, lsb)));
        } else {
            DAW::SetTrackPan1(media_track, normalizedToPan(int14ToNormalized(msb, lsb)));
        }
    }
};

#endif
