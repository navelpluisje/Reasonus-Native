#ifndef CSURF_FP_8_FILTER_MANAGER_C_
#define CSURF_FP_8_FILTER_MANAGER_C_

#include "../shared/csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_fp_8_track.hpp"
#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_navigator.hpp"
#include <mini/ini.h>
#include <vector>
#include "../shared/csurf_daw.hpp"
#include "../shared/csurf_utils.hpp"
#include "csurf_fp_8_ui_control_panel.hpp"
#include "../shared/csurf_faderport_ui_imgui_utils.hpp"

struct Filter
{
    std::string name;
    std::string id;
};

class CSurf_FP_8_FilterManager : public CSurf_FP_8_ChannelManager
{
protected:
    bool hasLastTouchedFxEnabled = false;
    mINI::INIStructure ini;
    std::vector<Filter> filters;

    void GetFilters()
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));
        file.read(ini);
        int nbFilters = stoi(ini["filters"]["nb-filters"]);

        for (int i = 0; i < nbFilters; i++)
        {
            std::string filterId = ini["filters"][std::to_string(i)];
            Filter filter{ini[filterId]["name"], filterId};
            filters.push_back(filter);
        }
    }

    void GetFaderValue(MediaTrack *media_track, int *faderValue, int *valueBarValue, std::string *_pan1, std::string *_pan2)
    {
        int panMode = 0;
        double volume, pan1, pan2 = 0.0;

        GetTrackUIVolPan(media_track, &volume, &pan1);
        GetTrackUIPan(media_track, &pan1, &pan2, &panMode);
        *_pan1 = GetPan1String(pan1);
        *_pan2 = GetPan2String(pan2, panMode);

        *faderValue = int(volToNormalized(volume) * 16383.0);
        *valueBarValue = int(panToNormalized(pan1) * 127);
    }

public:
    CSurf_FP_8_FilterManager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->ResetChannelManagerItemsCount();
        GetFilters();
        UpdateTracks();
    }
    ~CSurf_FP_8_FilterManager() {};

    void UpdateTracks() override
    {
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        context->SetChannelManagerItemsCount(static_cast<int>(filters.size()) + 1);

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int filter_index = context->GetChannelManagerItemIndex() + i;
            int fader_value = 0, value_bar_value = 0;
            std::string strPan1, strPan2;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track, navigator->HasFilter(filter_index));

            GetFaderValue(media_track, &fader_value, &value_bar_value, &strPan1, &strPan2);

            track->SetTrackColor(color);
            track->SetSelectButtonValue(BTN_VALUE_ON, forceUpdate);
            track->SetMuteButtonValue(DAW::IsTrackMuted(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
            track->SetSoloButtonValue(DAW::IsTrackSoloed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
            track->SetFaderValue(fader_value, forceUpdate);
            track->SetValueBarMode(VALUEBAR_MODE_NORMAL);
            track->SetValueBarValue(0);

            track->SetDisplayMode(DISPLAY_MODE_2, forceUpdate);
            track->SetDisplayLine(0, ALIGN_CENTER, DAW::GetTrackName(media_track).c_str(), NON_INVERT, forceUpdate);

            if (filter_index < (int)filters.size())
            {
                track->SetDisplayLine(1, ALIGN_CENTER, "Filter", INVERT, forceUpdate);
                track->SetDisplayLine(2, ALIGN_CENTER, filters.at(filter_index).name.c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_CENTER, "No Fltr", INVERT, forceUpdate);
                track->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
                track->SetDisplayLine(3, ALIGN_CENTER, "Create", NON_INVERT, forceUpdate);
            }
        }

        forceUpdate = false;
    }

    void HandleSelectClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        int filter_index = context->GetChannelManagerItemIndex() + index;
        if (context->GetShiftChannelLeft())
        {
            if (!ReaSonus8ControlPanel::control_panel_open)
            {
                ToggleFP8ControlPanel(ReaSonus8ControlPanel::FILTERS_PAGE);
            }
            else if (ReaSonus8ControlPanel::current_page != ReaSonus8ControlPanel::FILTERS_PAGE)
            {
                ReaSonus8ControlPanel::SetCurrentPage(ReaSonus8ControlPanel::FILTERS_PAGE);
            }
            else if (ReaSonus8ControlPanel::GetPageProperty(0) == filter_index)
            {
                ToggleFP8ControlPanel(ReaSonus8ControlPanel::FILTERS_PAGE);
            }

            if (ReaSonus8ControlPanel::control_panel_open)
            {
                ReaSonus8ControlPanel::SetPageProperty(0, filter_index);
            }
        }
        else
        {
            if (filter_index < (int)filters.size())
            {
                if (navigator->GetMultiSelectFilter())
                {
                    navigator->ToggleFilter(filter_index);
                }
                else
                {
                    navigator->HandleCustomFilter(filters.at(filter_index).id);
                }
            }
            else
            {
                int result = MB("There is no filter for this button. Do you want to add a new filter?", "No filter", 1);
                if (result == 1)
                {
                    ToggleFP8ControlPanel(ReaSonus8ControlPanel::FILTERS_PAGE);
                }
            }
        }
    }

    void HandleMuteClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, !DAW::IsTrackMuted(media_track)), NULL);
    }

    void HandleSoloClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, !DAW::IsTrackSoloed(media_track)), NULL);
    }

    void HandleFaderTouch(int index, int value) override
    {
        (void)index;
        (void)value;
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetShiftChannelLeft())
        {
            CSurf_SetSurfacePan(media_track, CSurf_OnPanChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false), NULL);
        }
        else if (context->GetShiftChannelRight())
        {
            SetMediaTrackInfo_Value(media_track, "D_WIDTH", CSurf_OnWidthChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false));
        }
        else
        {
            CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), NULL);
        }
    }
};

#endif