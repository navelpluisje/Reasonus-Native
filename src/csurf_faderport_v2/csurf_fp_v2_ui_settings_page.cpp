#include "../ui/csurf_ui_page_content.hpp"
#include <reaper_imgui_functions.h>
#include "../ui/csurf_ui_checkbox.hpp"
#include "../shared/csurf.h"
#include "../i18n/i18n.hpp"
#include "../ui/csurf_ui_images.h"
#include "../ui/csurf_ui_tooltip.hpp"
#include "../ui/csurf_ui_combo_input.hpp"
#include "../ui/csurf_ui_button_width.hpp"
#include "../shared/csurf_daw.hpp"
#include "csurf_fp_v2_ui_control_panel.hpp"

class CSurf_FP_V2_SettingsPage : public CSurf_UI_PageContent
{
protected:
    ImGui_Image *icon_info;
    I18n *i18n = I18n::GetInstance();
    ImGui_Font *main_font_bold;

    int setting_language;
    bool edit_language = false;
    bool previous_edit_language = false;
    bool momentary_mute_solo;
    bool control_hidden_tracks;
    bool can_disable_fader;
    bool endless_track_scroll;

    std::vector<std::string> language_names = {};

public:
    CSurf_FP_V2_SettingsPage(ImGui_Context *m_ctx) : CSurf_UI_PageContent(m_ctx, FP_V2)
    {
        main_font_bold = ImGui::CreateFont("Arial", ImGui::FontFlags_Bold);
        icon_info = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_info), sizeof(img_icon_info));

        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_info));
        GetLanguages(language_names);
        Reset();
    };

    virtual ~CSurf_FP_V2_SettingsPage() {};

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
        if (edit_language != previous_edit_language)
        {
            previous_edit_language = edit_language;
            Main_OnCommandStringEx("_REASONUS_TRANSLATIONN_EDITOR");
        }

        int language_button_width = getButtonWidth(m_ctx, i18n->t("settings", "language.button.label"), main_font_bold);

        UiElements::PushReaSonusSettingsContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "mapping_lists_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle, ImGui::ChildFlags_AutoResizeY))
        {
            if (ImGui::BeginChild(m_ctx, "language-select", -1 * language_button_width, 0.0, ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY))
            {
                RenderSettingsComboInput(
                    m_ctx,
                    i18n->t("settings", "language.label"),
                    language_names,
                    &setting_language,
                    i18n->t("settings", "language.tooltip"));

                ImGui::EndChild(m_ctx);
            }
            ImGui::SameLine(m_ctx);
            if (ImGui::BeginChild(m_ctx, "language-action", 0.0, 0.0, ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY))
            {
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 22);
                UiElements::PushReaSonusButtonOutlineStyle(m_ctx, main_font_bold);
                if (ImGui::Button(m_ctx, i18n->t("settings", "language.button.label").c_str()))
                {
                    edit_language = true;
                }
                UiElements::PopReaSonusButtonOutlineStyle(m_ctx);

                ImGui::EndChild(m_ctx);
            }

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "momentary-mute.label"),
                &momentary_mute_solo,
                i18n->t("settings", "momentary-mute.tooltip"));

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "control_hidden_track.label"),
                &control_hidden_tracks,
                i18n->t("settings", "control_hidden_track.tooltip"));

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "can_disable_fader.label"),
                &can_disable_fader,
                i18n->t("settings", "can_disable_fader.tooltip"));

            RenderSettingsCheckbox(
                m_ctx,
                i18n->t("settings", "endless_track_scroll.label"),
                &endless_track_scroll,
                i18n->t("settings", "endless_track_scroll.tooltip"));

            UiElements::PopReaSonusSettingsContentStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void Save() override
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_V2));
        readAndCreateIni(ini, FP_V2);

        DAW::SetExtState(EXT_STATE_KEY_UI_LANGUAGE, language_names[setting_language].c_str(), true);
        ini["surface"]["mute-solo-momentary"] = momentary_mute_solo ? "1" : "0";
        ini["surface"]["control-hidden-tracks"] = control_hidden_tracks ? "1" : "0";
        ini["surface"]["can-disable-fader"] = can_disable_fader ? "1" : "0";
        ini["surface"]["endless-track-scroll"] = endless_track_scroll ? "1" : "0";

        if (file.write(ini, true))
        {
            DAW::SetExtState(EXT_STATE_KEY_SAVED_SETTINGS, EXT_STATE_VALUE_TRUE, false);
            ReaSonusV2ControlPanel::SetMessage(i18n->t("settings", "action.save.message"));
        };
    }

    void Reset() override
    {
        auto language_index = std::find(language_names.begin(), language_names.end(), DAW::GetExtState(EXT_STATE_KEY_UI_LANGUAGE, "en-US"));
        setting_language = language_index - language_names.begin();

        momentary_mute_solo = ini["surface"]["mute-solo-momentary"] == "1";
        control_hidden_tracks = ini["surface"]["control-hidden-tracks"] == "1";
        can_disable_fader = ini["surface"]["can-disable-fader"] == "1";
        endless_track_scroll = ini["surface"]["endless-track-scrollr"] == "1";
    }
};
