#ifndef CSURF_FP_V2_TRACK_FADER_MANAGER_H_
#define CSURF_FP_V2_TRACK_FADER_MANAGER_H_

#include <vector>
#include <WDL/db2val.h>
#include "csurf_fp_v2_fader_manager.hpp"
#include "csurf_fp_v2_navigator.hpp"
#include "csurf_fp_v2_track.hpp"
#include "reaper_plugin_functions.h"
#include "../shared/csurf_daw.hpp"
#include "../shared/csurf_single_point_automation_item.hpp"

class CSurf_FP_V2_TrackFaderManager : public CSurf_FP_V2_FaderManager {
    void GetFaderValue(int *faderValue) const {
        double volume = 0.0;
        double pan1 = 0.0;

        MediaTrack *media_track = navigator->GetControllerTrack();
        if (context->GetMasterFaderMode()) {
            media_track = GetMasterTrack(nullptr);
        }

        GetTrackUIVolPan(media_track, &volume, &pan1);

        if (context->GetShiftLeft()) {
            *faderValue = static_cast<int>(panToNormalized(pan1) * 16383.0);
        } else {
            *faderValue = static_cast<int>(volToNormalized(volume) * 16383.0);
        }
    }

public:
    CSurf_FP_V2_TrackFaderManager(
        CSurf_FP_V2_Navigator *navigator,
        CSurf_Context *context,
        CSurf_FP_V2_Track *track,
        midi_Output *m_midiout
    ) : CSurf_FP_V2_FaderManager(navigator, context, track, m_midiout) {
        touch_start = false;
    }

    ~CSurf_FP_V2_TrackFaderManager() override = default;

    void UpdateTracks(const bool force_update) override {
        int fader_value = 0;
        GetFaderValue(&fader_value);

        if (!context->GetFaderDisabled() || !settings->GetCanDisableFader()) {
            track->SetFaderValue(fader_value, force_update);
        }
    }

    void HandleFaderTouch(const int value) override {
        MediaTrack *media_track = navigator->GetControllerTrack();

        if (
            GetTrackAutomationMode(media_track) == AUTOMATION_TOUCH
            || GetTrackAutomationMode(media_track) == AUTOMATION_LATCH
            || GetTrackAutomationMode(media_track) == AUTOMATION_PREVIEW
        ) {
            touch_start = value > 0 ? 1 : 0;
            if (touch_start > 0) {
                DAW::SetTrackVolume(media_track, DAW::GetTrackVolume(media_track));
            }
        }

        /**
         * Check if we have the proper conditions to continue with Single point automation
         */
        if (HasSinglePointAutomation(media_track, value, false)) {
            // Get the value to write for the automation
            double volume;
            // The slider max value in the REAPER settings has effect on the value to get and write for the automation
            // That's why we need the decibel offset and use it to determine the actual value to write to the automation.
            const double fader_offset = 12 - DAW::GetSliderMaxVolume();

            if (value > 0) {
                // We need to set it to read first to get the actual volume of the envelope
                SetTrackAutomationMode(media_track, AUTOMATION_READ);
                volume = DB2SLIDER(VAL2DB(DAW::GetTrackVolume(media_track)) - fader_offset);
                SetTrackAutomationMode(media_track, AUTOMATION_TRIM);
            } else {
                volume = DB2SLIDER(VAL2DB(DAW::GetTrackVolume(media_track)) - fader_offset);
            }

            HandleSinglePointAutomation(media_track, value, "<VOLENV2", volume, SPA_Track);
        }
    }

    void HandleFaderMove(const int msb, const int lsb) override {
        MediaTrack *media_track = navigator->GetControllerTrack();

        if (context->GetShiftLeft()) {
            CSurf_SetSurfacePan(
                media_track,
                CSurf_OnPanChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false),
                nullptr
            );
        } else {
            CSurf_SetSurfaceVolume(
                media_track,
                CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false),
                nullptr
            );
        }
    }
};

#endif
