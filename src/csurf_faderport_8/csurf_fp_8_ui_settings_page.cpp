#include "../ui/csurf_ui_page_content.hpp"
#include <reaper_imgui_functions.h>
#include "../ui/csurf_ui_checkbox.hpp"
#include "../ui/csurf_ui_combo_input.hpp"
#include <algorithm>
#include <vector>

class CSurf_FP_8_SettingsPage : public CSurf_UI_PageContent
{
protected:
    bool setting_disable_plugins;
    bool setting_untouch_after_learn;
    bool setting_master_fader_mode;
    bool setting_swap_shift;
    bool setting_fader_reset;
    bool setting_momentary_mute_solo;
    bool setting_overwrite_time_code;
    int setting_time_code;
    int *index;

    int time_code_indexes[6] = {0, 2, 3, 4, 5, 8};
    std::vector<std::string> time_code_names = {"Time", "Beats", "Seconds", "Samples", "Hr:Min:Sec:Fr", "Abs. Frames"};

public:
    CSurf_FP_8_SettingsPage(ImGui_Context *m_ctx) : CSurf_UI_PageContent(m_ctx, FP_8)
    {
        Reset();
    };

    virtual ~CSurf_FP_8_SettingsPage() {};

    void Render() override
    {
        ReaSonusCheckBox(m_ctx, "Disable Plugin Control", &setting_disable_plugins);
        ReaSonusCheckBox(m_ctx, "Untouch last touched param after learn", &setting_untouch_after_learn);
        ReaSonusCheckBox(m_ctx, "Enable master fader mode", &setting_master_fader_mode);
        ReaSonusCheckBox(m_ctx, "Swap Shift buttons", &setting_swap_shift);
        ReaSonusCheckBox(m_ctx, "Fader Reset: Left shift and fader touch", &setting_fader_reset);
        ReaSonusCheckBox(m_ctx, "Mute/Solo Momentary", &setting_momentary_mute_solo);
        ReaSonusCheckBox(m_ctx, "Overwrite Time Code", &setting_overwrite_time_code);
        if (setting_overwrite_time_code)
        {
            ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 26);
            ReaSonusComboInput(m_ctx, "Time Code", time_code_names, &setting_time_code);
        }
    }

    void Save() override
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));
        readAndCreateIni(ini, FP_8);

        ini["surface"]["disable-plugins"] = setting_disable_plugins ? "1" : "0";
        ini["surface"]["erase-last-param-after-learn"] = setting_untouch_after_learn ? "1" : "0";
        ini["surface"]["master-fader-mode"] = setting_master_fader_mode ? "1" : "0";
        ini["surface"]["swap-shift-buttons"] = setting_swap_shift ? "1" : "0";
        ini["surface"]["fader-reset"] = setting_fader_reset ? "1" : "0";
        ini["surface"]["mute-solo-momentary"] = setting_momentary_mute_solo ? "1" : "0";
        ini["surface"]["overwrite-time-code"] = setting_overwrite_time_code ? "1" : "0";
        ini["surface"]["time-code"] = std::to_string(time_code_indexes[setting_time_code]);

        if (file.write(ini, true))
        {
            MB("Chages saved with success", "Woohoo", 0);
        };
    }

    void Reset() override
    {
        setting_disable_plugins = ini["surface"]["disable-plugins"] == "1";
        setting_untouch_after_learn = ini["surface"]["erase-last-param-after-learn"] == "1";
        setting_master_fader_mode = ini["surface"]["master-fader-mode"] == "1";
        setting_swap_shift = ini["surface"]["swap-shift-buttons"] == "1";
        setting_fader_reset = ini["surface"]["fader-reset"] == "1";
        setting_momentary_mute_solo = ini["surface"]["mute-solo-momentary"] == "1";
        setting_overwrite_time_code = ini["surface"]["overwrite-time-code"] == "1";
        index = std::find(time_code_indexes, time_code_indexes + 6, stoi(ini["surface"]["time-code"]));
        setting_time_code = index - time_code_indexes;
    }
};
