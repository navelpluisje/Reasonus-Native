#ifndef CSURF_FP_V2_FADER_MANAGER_H_
#define CSURF_FP_V2_FADER_MANAGER_H_

#include "csurf_fp_v2_navigator.hpp"
#include "csurf_fp_v2_track.hpp"
#include "reaper_plugin_functions.h"
#include "../shared/csurf_project_state.hpp"
#include "../shared/csurf_reasonus_settings.hpp"
#include "../shared/csurf_single_point_automation_item.hpp"

class CSurf_FP_V2_FaderManager {
protected:
    CSurf_FP_V2_Navigator *navigator;
    CSurf_Context *context;
    midi_Output *m_midiout;
    CSurf_FP_V2_Track *track;

    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_8);
    ProjectState *project_state = ProjectState::GetInstance();
    SinglePointAutomationItem *single_point_automation = nullptr;
    bool touch_start;

    bool HasSinglePointAutomation(
        MediaTrack *media_track,
        const int value,
        const bool last_touched_param
    ) const {
        return (last_touched_param || GetTrackAutomationMode(media_track) == AUTOMATION_TRIM)
               && settings->UseAutomationSinglePoint()
               && (last_touched_param || context->IsSinglePointAutomationEnabled())
               && (value != 0 || single_point_automation != nullptr);
    }

    void HandleSinglePointAutomation(
        MediaTrack *media_track,
        const int value,
        const std::string &chunk_name,
        const double point_value,
        const SinglePointAutomationType spa_type,
        const int item_index = -1,
        const int sub_item_index = -1
    ) {
        if (single_point_automation == nullptr && value > 0) {
            switch (spa_type) {
                case SPA_Plugin: {
                    single_point_automation = new SinglePointAutomationItem(
                        media_track,
                        chunk_name,
                        point_value,
                        spa_type,
                        item_index,
                        sub_item_index
                    );
                    break;
                }

                default:
                    single_point_automation = new SinglePointAutomationItem(
                        media_track,
                        chunk_name,
                        point_value,
                        spa_type
                    );
            }
        }

        if (single_point_automation != nullptr && value > 0) {
            single_point_automation->InsertStartPoint(point_value);
            return;
        }

        if (single_point_automation != nullptr) {
            single_point_automation->InsertEndPoint(point_value, chunk_name);
        }

        single_point_automation = nullptr;
    }

public:
    CSurf_FP_V2_FaderManager(
        CSurf_FP_V2_Navigator *navigator,
        CSurf_Context *context,
        CSurf_FP_V2_Track *track,
        midi_Output *m_midiout
    ) : navigator(navigator), context(context), m_midiout(m_midiout), track(track), touch_start(false) {
    }

    virtual ~CSurf_FP_V2_FaderManager() {
        delete navigator;
        delete context;
        delete track;
        delete m_midiout;
    }

    virtual void UpdateTracks(const bool force_update) {
        (void) force_update;
    }

    virtual void HandleFaderTouch(const int value) {
        (void) value;
    }

    virtual void HandleFaderMove(const int msb, const int lsb) {
        (void) msb;
        (void) lsb;
    }
};

#endif
