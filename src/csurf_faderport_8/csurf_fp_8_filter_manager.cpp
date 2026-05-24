#ifndef CSURF_FP_8_FILTER_MANAGER_C_
#define CSURF_FP_8_FILTER_MANAGER_C_

#include <utility>

#include "csurf_fp_8_channel_manager.hpp"
#include "../ui/windows/csurf_ui_fp_8_control_panel.hpp"
#include "../shared/csurf_faderport_ui_imgui_utils.hpp"

struct Filter {
    std::string name;
    std::string id;
};

class CSurf_FP_8_FilterManager : public CSurf_FP_8_ChannelManager {
    bool hasLastTouchedFxEnabled;
    mINI::INIStructure ini;
    std::vector<Filter> filters;

protected:
    void GetFilters() {
        const mINI::INIFile file(GetReaSonusIniPath(FP_8));
        file.read(ini);
        const int nbFilters = stoi(ini["filters"]["nb-filters"]);

        for (int i = 0; i < nbFilters; i++) {
            const std::string filterId = ini["filters"][std::to_string(i)];
            const Filter filter{ini[filterId]["name"], filterId};
            filters.push_back(filter);
        }
    }

    static void GetFaderValue(
        MediaTrack *media_track,
        int *faderValue,
        int *valueBarValue,
        std::string *_pan1,
        std::string *_pan2
    ) {
        int panMode = 0;
        double volume = 0.0;
        double pan1 = 0.0;
        double pan2 = 0.0;

        GetTrackUIVolPan(media_track, &volume, &pan1);
        GetTrackUIPan(media_track, &pan1, &pan2, &panMode);
        *_pan1 = GetPan1String(pan1);
        *_pan2 = GetPan2String(pan2, panMode);

        *faderValue = static_cast<int>(volToNormalized(volume) * 16383.0);
        *valueBarValue = static_cast<int>(panToNormalized(pan1) * 127);
    }

public:
    CSurf_FP_8_FilterManager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout
    ) : CSurf_FP_8_ChannelManager(std::move(tracks), navigator, context, m_midiout) {
        hasLastTouchedFxEnabled = false;
        context->ResetChannelManagerItemIndex();
        context->ResetChannelManagerItemsCount();
        GetFilters();
        CSurf_FP_8_FilterManager::UpdateTracks(true);
    }

    ~CSurf_FP_8_FilterManager() override = default;

    void UpdateTracks(const bool force_update) override {
        const WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        context->SetChannelManagerItemsCount(static_cast<int>(filters.size()) + 1);

        for (int i = 0; i < context->GetNbChannels(); i++) {
            const int filter_index = context->GetChannelManagerItemIndex() + i;
            int fader_value = 0;
            int valuebar_value = 0;
            std::string strPan1;
            std::string strPan2;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track, navigator->HasFilter(filter_index));

            GetFaderValue(media_track, &fader_value, &valuebar_value, &strPan1, &strPan2);

            track->SetTrackColor(color);
            track->SetSelectButtonValue(BTN_VALUE_ON, force_update);
            track->SetMuteButtonValue(DAW::IsTrackMuted(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, force_update);
            track->SetSoloButtonValue(DAW::IsTrackSoloed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, force_update);
            track->SetFaderValue(fader_value, force_update);
            track->SetValueBarMode(VALUEBAR_MODE_NORMAL);
            track->SetValueBarValue(0);

            track->SetDisplayMode(DISPLAY_MODE_2, force_update);
            track->SetDisplayLine(0, ALIGN_CENTER, DAW::GetTrackName(media_track).c_str(), NON_INVERT, force_update);

            if (filter_index < static_cast<int>(filters.size())) {
                track->SetDisplayLine(1, ALIGN_CENTER, "Filter", INVERT, force_update);
                track->SetDisplayLine(2, ALIGN_CENTER, filters.at(filter_index).name.c_str(), NON_INVERT, force_update);
                track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, force_update);
            } else {
                track->SetDisplayLine(1, ALIGN_CENTER, "No Fltr", INVERT, force_update);
                track->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, force_update);
                track->SetDisplayLine(3, ALIGN_CENTER, "Create", NON_INVERT, force_update);
            }
        }
    }

    void HandleSelectClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        const int filter_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft()) {
            if (!ReaSonus8ControlPanel::control_panel_open
                || ReaSonus8ControlPanel::GetPageProperty(0) == filter_index
            ) {
                ToggleFP8ControlPanel(ReaSonus8ControlPanel::FILTERS_PAGE);
            } else if (ReaSonus8ControlPanel::current_page != ReaSonus8ControlPanel::FILTERS_PAGE) {
                ReaSonus8ControlPanel::SetCurrentPage(ReaSonus8ControlPanel::FILTERS_PAGE);
            }

            if (ReaSonus8ControlPanel::control_panel_open) {
                ReaSonus8ControlPanel::SetPageProperty(0, filter_index);
            }
        } else {
            if (filter_index < static_cast<int>(filters.size())) {
                if (navigator->GetMultiSelectFilter()) {
                    navigator->ToggleFilter(filter_index);
                    if (stoi(context->GetSetting("surface", "instant-multi-select-filter")) > 0) {
                        navigator->HandleFilter(TrackCustomMultiSelectFilter);
                    }
                } else {
                    navigator->HandleCustomFilter(filters.at(filter_index).id);
                }
            } else {
                const int result = MB(
                    "There is no filter for this button. Do you want to add a new filter?",
                    "No filter",
                    1
                );

                if (result == 1) {
                    ToggleFP8ControlPanel(ReaSonus8ControlPanel::FILTERS_PAGE);
                }
            }
        }
    }

    void HandleMuteClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        CSurf_SetSurfaceMute(
            media_track,
            CSurf_OnMuteChange(media_track, static_cast<int>(!DAW::IsTrackMuted(media_track))),
            nullptr
        );
    }

    void HandleSoloClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        CSurf_SetSurfaceSolo(
            media_track,
            CSurf_OnSoloChange(media_track, static_cast<int>(!DAW::IsTrackSoloed(media_track))),
            nullptr
        );
    }

    void HandleFaderTouch(const int index, const int value) override {
        (void) index;
        (void) value;
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetShiftChannelLeft()) {
            CSurf_SetSurfacePan(
                media_track,
                CSurf_OnPanChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false),
                nullptr
            );
        } else if (context->GetShiftChannelRight()) {
            SetMediaTrackInfo_Value(
                media_track,
                "D_WIDTH",
                CSurf_OnWidthChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false)
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
