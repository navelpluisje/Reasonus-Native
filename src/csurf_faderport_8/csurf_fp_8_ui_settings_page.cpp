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
#include "../i18n/i18n.hpp"

class CSurf_FP_8_SettingsPage : public CSurf_UI_PageContent
{
protected:
    ImGui_Image *icon_info;
    I18n *i18n = I18n::GetInstance();

    int setting_language;
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

    std::vector<std::string> language_names = {};

    int time_code_indexes[6] = {0, 2, 3, 4, 5, 8};
    std::vector<std::string> time_code_names = {
        i18n->t("settings", "timecode-list.option.1"), // Time
        i18n->t("settings", "timecode-list.option.2"), // "Beats",
        i18n->t("settings", "timecode-list.option.3"), // "Seconds",
        i18n->t("settings", "timecode-list.option.4"), // "Samples",
        i18n->t("settings", "timecode-list.option.5"), // "Hr:Min:Sec:Fr",
        i18n->t("settings", "timecode-list.option.6"), // "Abs. Frames"
    };

    int track_display_indexes[4] = {DISPLAY_MODE_4, DISPLAY_MODE_5, DISPLAY_MODE_7, DISPLAY_MODE_8};
    std::vector<std::string> track_display_names = {
        i18n->t("settings", "display-track.option.1"), // "Large, Large (No pan info)"
        i18n->t("settings", "display-track.option.2"), // "Small, Small, Large"
        i18n->t("settings", "display-track.option.3"), // "Large, Small, Small"
        i18n->t("settings", "display-track.option.4"), // "Small, Large, Small"
    };

public:
    CSurf_FP_8_SettingsPage(ImGui_Context *m_ctx) : CSurf_UI_PageContent(m_ctx, FP_8)
    {
        icon_info = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_info), sizeof(img_icon_info));
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_info));
        GetLanguages();
        Reset();
    };

    virtual ~CSurf_FP_8_SettingsPage() {};

    void GetLanguages()
    {
        language_names.clear();
        bool has_next = true;
        int index = 0;
        std::string path = GetReaSonusLocalesFolderPath();

        while (has_next)
        {
            const char *name = EnumerateFiles(path.c_str(), index);
            if (!name)
            {
                has_next = false;
            }
            else
            {
                index++;
                std::vector<std::string> splitted_name = split(std::string(name), ".");

                if (splitted_name[splitted_name.size() - 1].compare("ini") == 0)
                {
                    language_names.push_back(splitted_name[0]);
                }
            }
        }
    }

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
            RenderSettingsComboInput(
                m_ctx,
                i18n->t("settings", "language.label"),
                language_names,
                &setting_language,
                i18n->t("settings", "language.tooltip"));

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "plugin-control.label"),
                &setting_disable_plugins,
                i18n->t("settings", "plugin-control.tooltip"));

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "untouch-after-learn.label"),
                &setting_untouch_after_learn,
                i18n->t("settings", "untouch-after-learn.tooltip"));

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "master-fader.label"),
                &setting_master_fader_mode,
                i18n->t("settings", "master-fader.tooltip"));

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "swap-shift.label"),
                &setting_swap_shift,
                i18n->t("settings", "swap-shift.tooltip"));

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "fader-reset.label"),
                &setting_fader_reset,
                i18n->t("settings", "fader-reset.tooltip"));

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "momentary-mute.label"),
                &setting_momentary_mute_solo,
                i18n->t("settings", "momentary-mute.tooltip"));

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "overwrite-timecode.label"),
                &setting_overwrite_time_code,
                i18n->t("settings", "overwrite-timecode.tooltip"));

            if (setting_overwrite_time_code)
            {
                ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 26);
                RenderSettingsComboInput(
                    m_ctx,
                    i18n->t("settings", "timecode-list.label"),
                    time_code_names,
                    &setting_time_code,
                    i18n->t("settings", "timecode-list.tooltip"));
            }

            RenderSettingsComboInput(
                m_ctx,
                i18n->t("settings", "display-track.label"),
                track_display_names,
                &setting_track_display,
                i18n->t("settings", "display-track.tooltip"));

            UiElements::PopReaSonusSettingsContentStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void Save() override
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));
        readAndCreateIni(ini, FP_8);

        ini["surface"]["language"] = language_names[setting_language];
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
        auto language_index = std::find(language_names.begin(), language_names.end(), ini["surface"]["language"]);
        setting_language = language_index - language_names.begin();

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
