#include "../ui/csurf_ui_page_content.hpp"
#include "../ui/csurf_ui_checkbox.hpp"
#include "../shared/csurf.h"
#include "../i18n/i18n.hpp"
#include "../ui/csurf_ui_int_input.hpp"
#include "../ui/csurf_ui_tooltip.hpp"
#include "../ui/csurf_ui_combo_input.hpp"
#include "../ui/csurf_ui_button_width.hpp"
#include "../shared/csurf_daw.hpp"
#include "csurf_fp_v2_ui_control_panel.hpp"
#include "../shared/csurf_reasonus_settings.hpp"

class CSurf_FP_V2_SettingsPage : public CSurf_UI_PageContent
{
protected:
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_V2);

    int setting_language;
    bool edit_language = false;
    bool previous_edit_language = false;
    bool momentary_mute_solo;
    bool control_hidden_tracks;
    bool can_disable_fader;
    bool endless_track_scroll;
    bool setting_latch_preview_action_enable;
    int setting_latch_preview_action;
    int *index;

    std::vector<std::string> language_names = {};

    int latch_preview_action_indexes[8] = {42013, 42014, 42015, 42016, 42017, 41160, 41161, 41162};
    std::vector<std::string> latch_preview_action_names = {
        "Write current values for actively-writing envelopes to time selection",
        "Write current values for actively-writing envelopes from cursor to start of project",
        "Write current values for actively-writing envelopes from cursor to end of project",
        "Write current values for actively-writing envelopes from cursor to first touch position",
        "Write current values for actively-writing envelopes to entire envelope",
        "Write current values for all writing envelopes to time selection",
        "Write current values for all writing envelopes from cursor to start of project",
        "Write current values for all writing envelopes from cursor to end of project",
    };

public:
    CSurf_FP_V2_SettingsPage(ImGui_Context *m_ctx, CSurf_UI_Assets *assets) : CSurf_UI_PageContent(m_ctx, assets)
    {
        i18n = I18n::GetInstance();
        settings = ReaSonusSettings::GetInstance(FP_V2);

        GetLanguages(language_names);
        Reset();
    };

    virtual ~CSurf_FP_V2_SettingsPage() {};

    void Render() override
    {
        if (edit_language != previous_edit_language)
        {
            previous_edit_language = edit_language;
            Main_OnCommandStringEx("_REASONUS_TRANSLATIONN_EDITOR");
        }

        int language_button_width = getButtonWidth(m_ctx, i18n->t("settings", "language.button.label"), assets->GetMainFontBold());

        UiElements::PushReaSonusSettingsContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "mapping_lists_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle, ImGui::ChildFlags_AutoResizeY))
        {
            if (ImGui::BeginChild(m_ctx, "language-select", -1 * language_button_width, 0.0, ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY))
            {
                RenderInfoComboInput(
                    m_ctx,
                    assets,
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
                UiElements::PushReaSonusButtonOutlineStyle(m_ctx, assets->GetMainFontBold());
                if (ImGui::Button(m_ctx, i18n->t("settings", "language.button.label").c_str()))
                {
                    edit_language = true;
                }
                UiElements::PopReaSonusButtonOutlineStyle(m_ctx);

                ImGui::EndChild(m_ctx);
            }

            RenderInfoCheckbox(
                m_ctx,
                assets,
                i18n->t("settings", "momentary-mute.label"),
                &momentary_mute_solo,
                i18n->t("settings", "momentary-mute.tooltip"));

            RenderInfoCheckbox(
                m_ctx,
                assets,
                i18n->t("settings", "control_hidden_track.label"),
                &control_hidden_tracks,
                i18n->t("settings", "control_hidden_track.tooltip"));

            RenderInfoCheckbox(
                m_ctx,
                assets,
                i18n->t("settings", "can_disable_fader.label"),
                &can_disable_fader,
                i18n->t("settings", "can_disable_fader.tooltip"));

            RenderInfoCheckbox(
                m_ctx,
                assets,
                i18n->t("settings", "endless_track_scroll.label"),
                &endless_track_scroll,
                i18n->t("settings", "endless_track_scroll.tooltip"));

            RenderInfoCheckbox(
                m_ctx,
                assets,
                i18n->t("settings", "latch-preview-action-enable.label"),
                &setting_latch_preview_action_enable,
                i18n->t("settings", "latch-preview-action-enable.tooltip"));

            if (setting_latch_preview_action_enable)
            {
                ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 26);
                RenderInfoComboInput(
                    m_ctx,
                    assets,
                    i18n->t("settings", "latch-preview-action-list.label"),
                    latch_preview_action_names,
                    &setting_latch_preview_action,
                    i18n->t("settings", "latch-preview-action.tooltip"));
            }

            UiElements::PopReaSonusSettingsContentStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void Save() override
    {
        DAW::SetExtState(EXT_STATE_KEY_UI_LANGUAGE, language_names[setting_language].c_str(), true);
        settings->SetSetting("surface", "mute-solo-momentary", momentary_mute_solo);
        settings->SetSetting("surface", "control-hidden-tracks", control_hidden_tracks);
        settings->SetSetting("surface", "can-disable-fader", can_disable_fader);
        settings->SetSetting("surface", "endless-track-scroll", endless_track_scroll);
        settings->SetSetting("surface", "latch-preview-action", setting_latch_preview_action_enable);
        settings->SetSetting("surface", "latch-preview-action-code", latch_preview_action_indexes[setting_latch_preview_action]);

        if (settings->StoreSettings())
        {
            DAW::SetExtState(EXT_STATE_KEY_SAVED_SETTINGS, EXT_STATE_VALUE_TRUE, false);
            ReaSonusV2ControlPanel::SetMessage(i18n->t("settings", "action.save.message"));
        };
    }

    void Reset() override
    {
        auto language_index = std::find(language_names.begin(), language_names.end(), DAW::GetExtState(EXT_STATE_KEY_UI_LANGUAGE, "en-US"));
        setting_language = language_index - language_names.begin();

        momentary_mute_solo = settings->GetMuteSoloMomentary();
        control_hidden_tracks = settings->GetControlHiddenTracks();
        can_disable_fader = settings->GetCanDisableFader();
        endless_track_scroll = settings->GetEndlessTrackScroll();
        setting_latch_preview_action_enable = settings->GetLatchPreviewActionEnabled();

        index = std::find(latch_preview_action_indexes, latch_preview_action_indexes + 8, settings->GetLatchPreviewActionCode());
        setting_latch_preview_action = index - latch_preview_action_indexes;
    }
};
