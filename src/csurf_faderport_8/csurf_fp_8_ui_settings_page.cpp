#include "../ui/csurf_ui_page_content.hpp"
#include "../controls/csurf_display_resources.hpp"
#include "../ui/csurf_ui_checkbox.hpp"
#include "../ui/csurf_ui_int_input.hpp"
#include "../ui/csurf_ui_combo_input.hpp"
#include "../ui/csurf_ui_tooltip.hpp"
#include "../ui/csurf_ui_assets.h"
#include "../shared/csurf.h"
#include "../i18n/i18n.hpp"
#include "../ui/csurf_ui_button_width.hpp"
#include "../shared/csurf_daw.hpp"
#include "csurf_fp_8_ui_control_panel.hpp"
#include "../shared/csurf_reasonus_settings.hpp"

class CSurf_FP_8_SettingsPage : public CSurf_UI_PageContent
{
protected:
    I18n *i18n = I18n::GetInstance();

    int selected_tab = -1;

    int setting_language;
    bool edit_language = false;
    bool previous_edit_language = false;
    bool setting_distraction_free_mode = false;
    bool setting_disable_plugins;
    bool setting_untouch_after_learn;
    bool setting_master_fader_mode;
    bool setting_swap_shift;
    bool setting_fader_reset;
    bool setting_momentary_mute_solo;
    bool setting_latch_preview_action_enable;
    bool setting_overwrite_time_code;
    int setting_plugin_step_size;
    int setting_latch_preview_action;
    int setting_track_color_brightness = 25;
    int setting_time_code;
    int setting_track_display;
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

    int time_code_indexes[6] = {0, 2, 3, 4, 5, 8};
    std::vector<std::string> time_code_names = {
        i18n->t("settings", "timecode-list.option.time"),          // Time
        i18n->t("settings", "timecode-list.option.beats"),         // "Beats",
        i18n->t("settings", "timecode-list.option.seconds"),       // "Seconds",
        i18n->t("settings", "timecode-list.option.samples"),       // "Samples",
        i18n->t("settings", "timecode-list.option.hr-min-sec-fr"), // "Hr:Min:Sec:Fr",
        i18n->t("settings", "timecode-list.option.frames"),        // "Abs. Frames"
    };

    int track_display_indexes[4] = {DISPLAY_MODE_4, DISPLAY_MODE_5, DISPLAY_MODE_7, DISPLAY_MODE_8};
    std::vector<std::string> track_display_names = {
        i18n->t("settings", "display-track.option.display-mode-4"), // "Large, Large (No pan info)"
        i18n->t("settings", "display-track.option.display-mode-5"), // "Small, Small, Large"
        i18n->t("settings", "display-track.option.display-mode-7"), // "Large, Small, Small"
        i18n->t("settings", "display-track.option.display-mode-8"), // "Small, Large, Small"
    };

public:
    CSurf_FP_8_SettingsPage(ImGui_Context *m_ctx, CSurf_UI_Assets *assets) : CSurf_UI_PageContent(m_ctx, assets)
    {
        i18n = I18n::GetInstance();
        settings = ReaSonusSettings::GetInstance(FP_8);

        GetLanguages(language_names);
        Reset();
    };

    virtual ~CSurf_FP_8_SettingsPage() {};

    void Render() override
    {
        double width, height = 0.0;

        if (edit_language != previous_edit_language)
        {
            previous_edit_language = edit_language;
            Main_OnCommandStringEx("_REASONUS_TRANSLATIONN_EDITOR");
        }

        int language_button_width = getButtonWidth(m_ctx, i18n->t("settings", "language.button.label"), assets->GetMainFontBold());

        UiElements::PushReaSonusSettingsContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "settings_lists_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle, ImGui::ChildFlags_AutoResizeY))
        {
            UiElements::PushReaSonusTabBarStyle(m_ctx);
            if (ImGui::BeginTabBar(m_ctx, "SettingsTabs", ImGui::TabBarFlags_None))
            {
                UiElements::PushReaSonusTabStyle(m_ctx, selected_tab == 0);
                if (ImGui::BeginTabItem(m_ctx, i18n->t("settings", "tab.global").c_str()))
                {
                    selected_tab = 0;
                    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 16);

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
                        i18n->t("settings", "distraction-free-mode.label"),
                        &setting_distraction_free_mode,
                        i18n->t("settings", "distraction-free-mode.tooltip"));

                    ImGui::EndTabItem(m_ctx);
                }
                UiElements::PopReaSonusTabStyle(m_ctx);

                UiElements::PushReaSonusTabStyle(m_ctx, selected_tab == 1);
                if (ImGui::BeginTabItem(m_ctx, i18n->t("settings", "tab.functional").c_str()))
                {
                    selected_tab = 1;
                    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 16);

                    RenderInfoCheckbox(
                        m_ctx,
                        assets,
                        i18n->t("settings", "master-fader.label"),
                        &setting_master_fader_mode,
                        i18n->t("settings", "master-fader.tooltip"));

                    RenderInfoCheckbox(
                        m_ctx,
                        assets,
                        i18n->t("settings", "swap-shift.label"),
                        &setting_swap_shift,
                        i18n->t("settings", "swap-shift.tooltip"));

                    RenderInfoCheckbox(
                        m_ctx,
                        assets,
                        i18n->t("settings", "fader-reset.label"),
                        &setting_fader_reset,
                        i18n->t("settings", "fader-reset.tooltip"));

                    RenderInfoCheckbox(
                        m_ctx,
                        assets,
                        i18n->t("settings", "momentary-mute.label"),
                        &setting_momentary_mute_solo,
                        i18n->t("settings", "momentary-mute.tooltip"));

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

                    ImGui::EndTabItem(m_ctx);
                }
                UiElements::PopReaSonusTabStyle(m_ctx);

                UiElements::PushReaSonusTabStyle(m_ctx, selected_tab == 2);
                if (ImGui::BeginTabItem(m_ctx, i18n->t("settings", "tab.plugins").c_str()))
                {
                    selected_tab = 2;
                    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 16);
                    ImGui::GetContentRegionAvail(m_ctx, &width, &height);

                    if (ImGui::BeginChild(m_ctx, "settings-display", width / 2 - 8, 0.0, ImGui::ChildFlags_None))
                    {

                        RenderInfoCheckbox(
                            m_ctx,
                            assets,
                            i18n->t("settings", "plugin-control.label"),
                            &setting_disable_plugins,
                            i18n->t("settings", "plugin-control.tooltip"));

                        RenderInfoCheckbox(
                            m_ctx,
                            assets,
                            i18n->t("settings", "untouch-after-learn.label"),
                            &setting_untouch_after_learn,
                            i18n->t("settings", "untouch-after-learn.tooltip"));

                        RenderinfoIntInput(
                            m_ctx,
                            assets,
                            i18n->t("settings", "plugin-step-size.label"),
                            &setting_plugin_step_size,
                            1,
                            settings->GetSurface(),
                            i18n->t("settings", "plugin-step-size.tooltip"));
                        ImGui::EndChild(m_ctx);
                    }
                    ImGui::EndTabItem(m_ctx);
                }
                UiElements::PopReaSonusTabStyle(m_ctx);

                UiElements::PushReaSonusTabStyle(m_ctx, selected_tab == 3);
                if (ImGui::BeginTabItem(m_ctx, i18n->t("settings", "tab.display").c_str()))
                {
                    selected_tab = 3;
                    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 16);
                    ImGui::GetContentRegionAvail(m_ctx, &width, &height);

                    if (ImGui::BeginChild(m_ctx, "settings-display", width / 2 - 8, 0.0, ImGui::ChildFlags_None))
                    {

                        RenderInfoCheckbox(
                            m_ctx,
                            assets,
                            i18n->t("settings", "overwrite-timecode.label"),
                            &setting_overwrite_time_code,
                            i18n->t("settings", "overwrite-timecode.tooltip"));

                        if (setting_overwrite_time_code)
                        {
                            ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 26);
                            RenderInfoComboInput(
                                m_ctx,
                                assets,
                                i18n->t("settings", "timecode-list.label"),
                                time_code_names,
                                &setting_time_code,
                                i18n->t("settings", "timecode-list.tooltip"));
                        }

                        RenderInfoComboInput(
                            m_ctx,
                            assets,
                            i18n->t("settings", "display-track.label"),
                            track_display_names,
                            &setting_track_display,
                            i18n->t("settings", "display-track.tooltip"));

                        ImGui::EndChild(m_ctx);
                    }
                    ImGui::SameLine(m_ctx);
                    ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 16);
                    if (ImGui::BeginChild(m_ctx, "settings-colors", width / 2 - 8, 0.0, ImGui::ChildFlags_None))
                    {
                        RenderinfoIntInput(
                            m_ctx,
                            assets,
                            i18n->t("settings", "track-color-brightness.label"),
                            &setting_track_color_brightness,
                            5,
                            100,
                            i18n->t("settings", "track-color-brightness.tooltip"),
                            "%d%%");

                        ImGui::EndChild(m_ctx);
                    }

                    ImGui::EndTabItem(m_ctx);
                }
                UiElements::PopReaSonusTabStyle(m_ctx);

                ImGui::EndTabBar(m_ctx);
            }

            UiElements::PopReaSonusSettingsContentStyle(m_ctx);
            UiElements::PopReaSonusTabBarStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void Save() override
    {
        DAW::SetExtState(EXT_STATE_KEY_UI_LANGUAGE, language_names[setting_language].c_str(), true);

        settings->SetSetting("surface", "disable-plugins", setting_disable_plugins);
        settings->SetSetting("surface", "distraction-free", setting_distraction_free_mode);

        settings->SetSetting("surface", "erase-last-param-after-learn", setting_untouch_after_learn);
        settings->SetSetting("surface", "master-fader-mode", setting_master_fader_mode);
        settings->SetSetting("surface", "swap-shift-buttons", setting_swap_shift);
        settings->SetSetting("surface", "fader-reset", setting_fader_reset);
        settings->SetSetting("surface", "mute-solo-momentary", setting_momentary_mute_solo);
        settings->SetSetting("surface", "overwrite-time-code", setting_overwrite_time_code);
        settings->SetSetting("surface", "latch-preview-action", setting_latch_preview_action_enable);
        settings->SetSetting("surface", "track-color-brightness", setting_track_color_brightness);
        settings->SetSetting("surface", "latch-preview-action-code", latch_preview_action_indexes[setting_latch_preview_action]);
        settings->SetSetting("surface", "time-code", time_code_indexes[setting_time_code]);
        settings->SetSetting("surface", "plugin-step-size", setting_plugin_step_size);
        settings->SetSetting("displays", "track", track_display_indexes[setting_track_display]);

        if (settings->StoreSettings())
        {
            DAW::SetExtState(EXT_STATE_KEY_SAVED_SETTINGS, EXT_STATE_VALUE_TRUE, false);
            ReaSonus8ControlPanel::SetMessage(i18n->t("settings", "action.save.message"));
        };

        int actionId = NamedCommandLookup("_REASONUS_TOGGLE_DISABLE_PLUGINS_SETTING");
        RefreshToolbar2(0, actionId);
        actionId = NamedCommandLookup("_REASONUS_TOGGLE_DISTRACTION_FREE_MODE");
        RefreshToolbar2(0, actionId);
    }

    void Reset() override
    {
        setting_language = 0;

        for (int i = 0; i < (int)language_names.size(); i++)
        {
            if (language_names[i].compare(DAW::GetExtState(EXT_STATE_KEY_UI_LANGUAGE, "en-US")) == 0)
            {
                setting_language = i;
            }
        }

        setting_disable_plugins = settings->GetDisablePluginControl();
        setting_distraction_free_mode = settings->GetDistractionFreeMode();
        setting_untouch_after_learn = settings->GetUntouchAfterLearn();
        setting_master_fader_mode = settings->GetMasterFaderModeEnabled();
        setting_swap_shift = settings->GetSwapShiftButtons();
        setting_fader_reset = settings->GetFaderReset();
        setting_momentary_mute_solo = settings->GetMuteSoloMomentary();
        setting_overwrite_time_code = settings->GetOverwriteTimeCode();
        setting_latch_preview_action_enable = settings->GetLatchPreviewActionEnabled();
        setting_plugin_step_size = settings->GetpluginStepSize();
        setting_track_color_brightness = settings->GetTrackColorBrightness();

        index = std::find(latch_preview_action_indexes, latch_preview_action_indexes + 8, settings->GetLatchPreviewActionCode());
        setting_latch_preview_action = index - latch_preview_action_indexes;

        index = std::find(time_code_indexes, time_code_indexes + 6, settings->GetSurfaceTimeCode());
        setting_time_code = index - time_code_indexes;

        index = std::find(track_display_indexes, track_display_indexes + 4, settings->GetTrackDisplay());
        setting_track_display = index - track_display_indexes;
    }
};
