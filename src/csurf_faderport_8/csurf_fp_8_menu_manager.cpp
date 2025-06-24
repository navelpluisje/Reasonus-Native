#ifndef CSURF_FP_8_MENU_MANAGER_C_
#define CSURF_FP_8_MENU_MANAGER_C_

#include "../shared/csurf_context.cpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"
#include <mini/ini.h>
#include <config.h>
#include <vector>
#include "csurf_fp_8_track.hpp"
#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_navigator.hpp"

class CSurf_FP_8_Menu_Manager : public CSurf_FP_8_ChannelManager
{
protected:
    mINI::INIStructure ini;

    std::vector<std::string> menu_items = {"Plugin Ctr", "Swap Shift", "Moment", "Timecode", "Time type"};
    std::vector<std::string> ini_keys = {"disable-plugins", "swap-shift-buttons", "mute-solo-momentary", "overwrite-time-code", "time-code"};
    std::vector<std::vector<std::vector<std::string>>> menu_options = {
        {{"Disable", "1"}, {"Enable", "0"}},
        {{"Default", "0"}, {"Swap", "1"}},
        {{"Disable", "0"}, {"Enable", "1"}},
        {{"REAPER", "0"}, {"Overwrite", "1"}},
        {{"Time", "0"}, {"Beats", "2"}, {"Seconds", "3"}, {"Samples", "4"}, {"H:M:S:Fr", "5"}, {"Abs. Frames", "8"}},
    };

    int level = 0;
    int option[2] = {0, -1};

public:
    CSurf_FP_8_Menu_Manager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        mINI::INIFile file(GetReaSonusIniPath(FP_8));
        file.read(ini);

        for (int i = 1; i < context->GetNbChannels() - 1; i++)
        {
            CSurf_FP_8_Track *track = tracks.at(i);
            track->SetDisplayMode(DISPLAY_MODE_9);
            track->SetDisplayLine(0, ALIGN_LEFT, "");
            track->SetDisplayLine(1, ALIGN_LEFT, "");
            track->SetDisplayLine(2, ALIGN_LEFT, "");
            track->SetDisplayLine(3, ALIGN_LEFT, "");
            track->SetDisplayLine(4, ALIGN_LEFT, "");
            track->SetDisplayLine(5, ALIGN_LEFT, "");
            track->SetDisplayLine(6, ALIGN_LEFT, "");
        }
        tracks.at(0)->SetValueBarMode(VALUEBAR_MODE_OFF);
        tracks.at(0)->SetDisplayMode(DISPLAY_MODE_0);
        tracks.at(0)->SetDisplayLine(0, ALIGN_CENTER, "ReaSonus", NON_INVERT);
        tracks.at(0)->SetDisplayLine(1, ALIGN_CENTER, "FaderPort", NON_INVERT);
        tracks.at(0)->SetDisplayLine(2, ALIGN_CENTER, "Menu", INVERT);

        int last_display_index = context->GetNbChannels() - 1;
        tracks.at(last_display_index)->SetValueBarMode(VALUEBAR_MODE_OFF);
        tracks.at(last_display_index)->SetDisplayMode(DISPLAY_MODE_2);
        tracks.at(last_display_index)->SetDisplayLine(0, ALIGN_CENTER, "Version", NON_INVERT);
        tracks.at(last_display_index)->SetDisplayLine(1, ALIGN_CENTER, GIT_VERSION, NON_INVERT);
        tracks.at(last_display_index)->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT);
        tracks.at(last_display_index)->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT);

        UpdateTracks();
    }
    ~CSurf_FP_8_Menu_Manager() {};

    void UpdateTracks() override
    {
        int nb_menu_items = static_cast<int>(menu_items.size());
        int nb_options = static_cast<int>(menu_options.at(option[0]).size());
        int sel_option = option[1];

        int option_offset = 0;
        if (nb_options > 6 && sel_option >= (nb_options - 6) + 2)
        {
            option_offset = nb_options - 6;
        }
        else if (nb_options > 6 && sel_option > 2)
        {
            option_offset = sel_option - 2;
        }

        for (int i = 0; i < 7; i++)
        {
            if (i < nb_menu_items)
            {
                tracks.at(1)->SetDisplayLine(i, ALIGN_LEFT, menu_items[i].c_str(), option[0] == i ? INVERT : NON_INVERT);
            }
            else
            {
                tracks.at(1)->SetDisplayLine(i, ALIGN_LEFT, "", NON_INVERT);
            }

            int index = i + option_offset;

            if (index < nb_options)
            {
                std::string option_label = menu_options.at(option[0])[index][0];
                std::string value = menu_options.at(option[0])[index][1];

                if (value == ini["surface"][ini_keys[option[0]]])
                {
                    option_label = ">" + option_label;
                }
                tracks.at(2)->SetDisplayLine(i, ALIGN_LEFT, option_label.c_str(), option[1] == index ? INVERT : NON_INVERT);
            }
            else if (index == nb_options)
            {
                tracks.at(2)->SetDisplayLine(i, ALIGN_LEFT, "<- Back", option[1] == nb_options ? INVERT : NON_INVERT);
            }
            else
            {
                tracks.at(2)->SetDisplayLine(i, ALIGN_LEFT, "", NON_INVERT);
            }
        }
    }

    // Handle the encoder click
    void HandleSelectClick(int index) override
    {
        (void)index;
        int max_items = level == 0 ? 5 : static_cast<int>(menu_options.at(option[0]).size());
        if (level > 0 && option[1] == max_items)
        {
            level = 0;
            option[1] = -1;
        }
        else if (level > 0 && option[1] < max_items)
        {
            ini["surface"][ini_keys[option[0]]] = menu_options[option[0]][option[1]][1];
            mINI::INIFile file(GetReaSonusIniPath(FP_8));
            if (file.write(ini, true))
            {
                // Write all teh values to the context again so they get applied instantly
                context->SetPluginControl(ini["surface"].has("disable-plugins") && ini["surface"]["disable-plugins"] != "1");
                context->SetSwapShiftButtons(ini["surface"].has("swap-shift-buttons") && ini["surface"]["swap-shift-buttons"] == "1");
                context->SetMuteSoloMomentary(ini["surface"].has("mute-solo-momentary") && ini["surface"]["mute-solo-momentary"] == "1");
                context->SetOverwriteTimeCode(ini["surface"].has("overwrite-time-code") && ini["surface"]["overwrite-time-code"] == "1");
                context->SetSurfaceTimeCode(ini["surface"].has("time-code") && std::stoi(ini["surface"]["time-code"]));

                level = 0;
                option[1] = -1;
            }
        }
        else
        {
            level = 1;
            option[1] = 0;
        }
    }

    // Handle the encoder increment
    void HandleMuteClick(int index, int value) override
    {
        (void)index;
        (void)value;
        int max_items = level == 0 ? 5 : static_cast<int>(menu_options.at(option[0]).size());
        option[level] = minmax(0, option[level] + 1, max_items - (level == 0 ? 1 : 0));
    }

    // Handle the encoder decrement
    void HandleSoloClick(int index, int value) override
    {
        (void)index;
        (void)value;
        int max_items = level == 0 ? 5 : static_cast<int>(menu_options.at(option[0]).size());
        option[level] = minmax(0, option[level] - 1, max_items - (level == 0 ? 1 : 0));
    }

    void HandleFaderTouch(int index) override
    {
        (void)index;
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        (void)index;
        (void)msb;
        (void)lsb;
    }
};

#endif