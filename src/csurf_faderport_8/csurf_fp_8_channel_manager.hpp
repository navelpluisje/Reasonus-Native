#ifndef CSURF_FP_8_CHANNEL_MANAGER_H_
#define CSURF_FP_8_CHANNEL_MANAGER_H_

#include <vector>
#include "csurf_fp_8_channel_manager_resources.hpp"
#include "csurf_fp_8_navigator.hpp"
#include "csurf_fp_8_track.hpp"
#include "../shared/csurf_daw.hpp"
#include "../shared/csurf_project_state.hpp"
#include "../shared/csurf_reasonus_settings.hpp"
#include "../shared/csurf_single_point_automation_item.hpp"

class CSurf_FP_8_ChannelManager {
protected:
    std::vector<CSurf_FP_8_Track *> tracks;
    CSurf_FP_8_Navigator *navigator;
    CSurf_Context *context;
    midi_Output *m_midiout;
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_8);
    ProjectState *project_state = ProjectState::GetInstance();

    // is used as nb_sends, nb_receives or nb_plugins for the respective hui mode managers
    int nb_track_items[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<SinglePointAutomationItem *> single_point_automation = {
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr
    };

    ButtonColor color{};

    virtual void SetTrackColors(MediaTrack *media_track, const bool is_selected, const bool has_arm) {
        const double brightness = is_selected ? 1 : settings->GetTrackColorBrightnessPercentage();

        if (media_track == nullptr) {
            color.SetColor(ButtonColorWhite);
            return;
        }

        int red = 0xff;
        int green = 0x00;
        int blue = 0x00;

        if (!(context->GetArm() && has_arm) &&
            !(DAW::IsTrackArmed(media_track) && settings->GetDistractionFreeMode())
        ) {
            const int trackColor = GetTrackColor(media_track);
            if (trackColor == 0) {
                red = 0x7f;
                green = 0x7f;
                blue = 0x7f;
            } else {
                ColorFromNative(trackColor, &red, &green, &blue);
            }
        }
        color.SetColor(
            static_cast<int>(red * brightness / 2),
            static_cast<int>(green * brightness / 2),
            static_cast<int>(blue * brightness / 2)
        );
    }

    bool HasSinglePointAutomation(MediaTrack *media_track, const int index, const int value) const {
        return GetTrackAutomationMode(media_track) == AUTOMATION_TRIM
               && context->IsSinglePointAutomationEnabled()
               && (value != 0 || single_point_automation.at(index) != nullptr);
    }

    void HandleSinglePointAutomation(
        MediaTrack *media_track,
        const int index,
        const int value,
        const std::string &chunk_name,
        const double point_value,
        const SinglePointAutomationType spa_type,
        const int item_index = -1,
        const int sub_item_index = -1
    ) {
        if (single_point_automation.at(index) == nullptr && value > 0) {
            switch (spa_type) {
                case SPA_Plugin: {
                    single_point_automation.at(index) = new SinglePointAutomationItem(
                        media_track,
                        chunk_name,
                        point_value,
                        spa_type,
                        item_index,
                        sub_item_index
                    );
                    break;
                }

                case SPA_Send: {
                    single_point_automation.at(index) = new SinglePointAutomationItem(
                        media_track,
                        chunk_name,
                        point_value,
                        spa_type,
                        item_index
                    );
                    break;
                }

                default:
                    single_point_automation.at(index) = new SinglePointAutomationItem(
                        media_track,
                        chunk_name,
                        point_value,
                        spa_type
                    );
            }
        }

        if (single_point_automation.at(index) != nullptr && value > 0) {
            single_point_automation.at(index)->InsertStartPoint(point_value);
            return;
        }

        if (single_point_automation.at(index) != nullptr) {
            single_point_automation.at(index)->InsertEndPoint(point_value, chunk_name);
        }

        single_point_automation.at(index) = nullptr;
    }

public:
    CSurf_FP_8_ChannelManager(
        const std::vector<CSurf_FP_8_Track *> &tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout
    ) : tracks(tracks), navigator(navigator), context(context), m_midiout(m_midiout) {
    }

    virtual ~CSurf_FP_8_ChannelManager() {
        delete navigator;
        delete context;
        delete m_midiout;
    }

    virtual void UpdateTracks(const bool force_update) {
        (void) force_update;
    }

    virtual void HandleEndcoderPush(const int value) {
        (void) value;
    }

    virtual void HandleEndcoderIncrement(const int value) {
        (void) value;
    }

    virtual void HandleEndcoderDecrement(const int value) {
        (void) value;
    }

    virtual void HandleSelectClick(const int index, const int value) {
        (void) index;
        (void) value;
    }

    virtual void HandleMuteClick(const int index, const int value) {
        (void) index;
        (void) value;
    }

    virtual void HandleSoloClick(const int index, const int value) {
        (void) index;
        (void) value;
    }

    virtual void HandleFaderTouch(const int index, const int value) {
        (void) index;
        (void) value;
    }

    virtual void HandleFaderMove(const int index, const int msb, const int lsb) {
        (void) index;
        (void) msb;
        (void) lsb;
    }
};

#endif
