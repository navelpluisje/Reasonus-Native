#include <reaper_imgui_functions.h>
#include <algorithm>
#include <vector>
#include "../controls/csurf_display_resources.hpp"
#include "../ui/csurf_ui_page_content.hpp"
#include "../ui/csurf_ui_checkbox.hpp"
#include "../ui/csurf_ui_combo_input.hpp"
#include "../ui/csurf_ui_tooltip.hpp"
#include "../ui/csurf_ui_images.h"
#include "../shared/csurf.h"

class CSurf_FP_8_SettingsPage : public CSurf_UI_PageContent
{
protected:
    ImGui_Image *icon_info;

    bool setting_disable_plugins;
    bool setting_untouch_after_learn;
    bool setting_master_fader_mode;
    bool setting_swap_shift;
    bool setting_fader_reset;
    bool setting_momentary_mute_solo;
    bool setting_overwrite_time_code;
    int setting_time_code;
    int setting_track_display;
    int *index;

    int time_code_indexes[6] = {0, 2, 3, 4, 5, 8};
    std::vector<std::string> time_code_names = {"Time", "Beats", "Seconds", "Samples", "Hr:Min:Sec:Fr", "Abs. Frames"};

    int track_display_indexes[4] = {DISPLAY_MODE_4, DISPLAY_MODE_5, DISPLAY_MODE_7, DISPLAY_MODE_8};
    std::vector<std::string> track_display_names = {
        "Large, Large (No pan info)",
        "Small, Small, Large",
        "Large, Small, Small",
        "Small, Large, Small",
    };

public:
    CSurf_FP_8_SettingsPage(ImGui_Context *m_ctx) : CSurf_UI_PageContent(m_ctx, FP_8)
    {
        icon_info = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_info), sizeof(img_icon_info));
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_info));
        Reset();
    };

    virtual ~CSurf_FP_8_SettingsPage() {};

    void RenderSettingsCheckbox(
        ImGui_Context *m_ctx,
        std::string label,
        bool *value,
        std::string tooltip)
    {
        double x_pos = ImGui::GetCursorPosX(m_ctx);
        double y_pos = ImGui::GetCursorPosY(m_ctx);
        std::string id = std::to_string(x_pos) + "-" + std::to_string(y_pos);

        ReaSonusCheckBox(m_ctx, label.c_str(), value);
        ImGui::SameLine(m_ctx);
        ReaSonusTooltip(m_ctx, tooltip.c_str(), id.c_str(), icon_info);
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
        ImGui::Dummy(m_ctx, 0, 0);
    }

    void RenderSettingsComboInput(
        ImGui_Context *m_ctx,
        std::string label,
        std::vector<std::string> list,
        int *value,
        std::string tooltip)
    {
        double x_pos = ImGui::GetCursorPosX(m_ctx);
        double y_pos = ImGui::GetCursorPosY(m_ctx);
        std::string id = std::to_string(x_pos) + "-" + std::to_string(y_pos);

        ReaSonusComboInput(m_ctx, label.c_str(), list, value, -20);
        ImGui::SameLine(m_ctx);
        ReaSonusTooltip(m_ctx, tooltip.c_str(), id.c_str(), icon_info, -20, 26);
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
        ImGui::Dummy(m_ctx, 0, 0);
    }

    void Render() override
    {
        UiElements::PushReaSonusSettingsContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "mapping_lists_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle, ImGui::ChildFlags_AutoResizeY))
        {
            RenderSettingsCheckbox(
                m_ctx,
                "Disable Plugin Control",
                &setting_disable_plugins,
                "If you use another controller for controlling the plugins, you can select this. The plugins mode is still active and you are still able to open the plugin window. From that point, the ReaSonus FaderPort will not do anything anymore for the plugins.");

            RenderSettingsCheckbox(
                m_ctx,
                "Untouch last touched param after learn",
                &setting_untouch_after_learn,
                "When selected the last touched param will be set to untouched after parameter learn.");

            RenderSettingsCheckbox(
                m_ctx,
                "Enable master fader mode",
                &setting_master_fader_mode,
                "When selected en pressing the master button will enable master fader mode. This makes the last track of the FaderPort control the master track.");

            RenderSettingsCheckbox(
                m_ctx,
                "Swap Shift buttons",
                &setting_swap_shift,
                "This swaps the left and right Shift button.");

            RenderSettingsCheckbox(
                m_ctx,
                "Fader Reset: Left shift and fader touch",
                &setting_fader_reset,
                "When eneabled, you can use the [Left Shift] button to reset the fader value while touching teh fader.");

            RenderSettingsCheckbox(
                m_ctx,
                "Mute/Solo Momentary",
                &setting_momentary_mute_solo,
                "When set, it will make the Solo and Mute buttons momentary. The timeout for being momentary is 500 milli seconds. In short: If you press the solo button longer then 500ms and release it, it will instant unsolo again.");

            RenderSettingsCheckbox(
                m_ctx,
                "Overwrite Time Code",
                &setting_overwrite_time_code,
                "Overwrites the time code selected in REAPER.");

            if (setting_overwrite_time_code)
            {
                ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 26);
                RenderSettingsComboInput(
                    m_ctx,
                    "Time Code",
                    time_code_names,
                    &setting_time_code,
                    "Select the time code type you want to overwrite REAPERs time code with.");
            }

            RenderSettingsComboInput(
                m_ctx,
                "Track mode display",
                track_display_names,
                &setting_track_display,
                "Select the display style for the track mode display.");

            UiElements::PopReaSonusSettingsContentStyle(m_ctx);
            ImGui::EndChild(m_ctx);
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
        ini["displays"]["track"] = std::to_string(track_display_indexes[setting_track_display]);

        if (file.write(ini, true))
        {
            ::SetExtState(EXT_STATE_SECTION, EXT_STATE_KEY_SAVED_SETTINGS, EXT_STATE_VALUE_TRUE, false);
            MB("Changes saved with success", "Woohoo", 0);
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

        index = std::find(track_display_indexes, track_display_indexes + 4, stoi(ini["displays"]["track"]));
        setting_track_display = index - track_display_indexes;
    }
};
