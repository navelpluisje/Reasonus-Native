#include <array>
#include "../csurf_ui_page_content.hpp"
#include "../components/csurf_ui_checkbox.hpp"
#include "../components/csurf_ui_int_input.hpp"
#include "../components/csurf_ui_combo_input.hpp"
#include "../../shared/csurf.h"
#include "../../i18n/i18n.hpp"
#include "../utils/csurf_ui_button_width.hpp"
#include "../../shared/csurf_daw.hpp"
#include "../windows/csurf_ui_fp_8_control_panel.hpp"
#include "../../shared/csurf_reasonus_settings.hpp"
#include "../components/csurf_ui_image_combo_input.hpp"


class CSurf_FP_8_SettingsPage : public CSurf_UI_PageContent { // NOLINT(*-use-internal-linkage)
    I18n *i18n = I18n::GetInstance();

    int selected_tab = -1;
    int selected_display_line = -1;

    int setting_language{};
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
    bool setting_plugin_map_param_clear;
    bool setting_instant_multi_select_filter;
    bool setting_mute_master_on_fwd_rwd;
    int setting_plugin_step_size;
    int setting_latch_preview_action;
    int setting_track_color_brightness = 25;
    int setting_time_code;
    int setting_track_display;
    std::array<int, 4> setting_track_value_line_value;
    std::array<int, 4> setting_track_value_line_align;
    std::array<int, 4> setting_track_value_line_invert;
    int setting_track_valuebar_mode;
    int setting_track_valuebar_value;
    int *index;

    std::vector<std::string> language_names;

    std::array<int, 8> latch_preview_action_indexes = {42013, 42014, 42015, 42016, 42017, 41160, 41161, 41162};
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

    std::array<int, 6> time_code_indexes = {0, 2, 3, 4, 5, 8};
    std::vector<std::string> time_code_names = {
        i18n->t("settings", "timecode-list.option.time"), // Time
        i18n->t("settings", "timecode-list.option.beats"), // "Beats",
        i18n->t("settings", "timecode-list.option.seconds"), // "Seconds",
        i18n->t("settings", "timecode-list.option.samples"), // "Samples",
        i18n->t("settings", "timecode-list.option.hr-min-sec-fr"), // "Hr:Min:Sec:Fr",
        i18n->t("settings", "timecode-list.option.frames"), // "Abs. Frames"
    };

    int track_display_lines[9] = {3, 3, 4, 2, 2, 3, 3, 3, 3};
    std::vector<std::string> track_display_names = {
        i18n->t("settings", "display-track.option.display-mode-0"), // "Small, Small, Large (No VU)"
        i18n->t("settings", "display-track.option.display-mode-1"), // "Large, Small, Small (No VU)"
        i18n->t("settings", "display-track.option.display-mode-2"), // "Small, Small, Small, Small (No VU)"
        i18n->t("settings", "display-track.option.display-mode-3"), // "Large, Large  (No VU)"
        i18n->t("settings", "display-track.option.display-mode-4"), // "Large, Large"
        i18n->t("settings", "display-track.option.display-mode-5"), // "Small, Small, Large"
        i18n->t("settings", "display-track.option.display-mode-6"), // "Small, Large, Small (No VU)"
        i18n->t("settings", "display-track.option.display-mode-7"), // "Large, Small, Small"
        i18n->t("settings", "display-track.option.display-mode-8"), // "Small, Large, Small"
    };

    std::vector<std::string> track_display_values = {
        i18n->t("settings", "display-track.option.display-value-name"),
        i18n->t("settings", "display-track.option.display-value-volume"),
        i18n->t("settings", "display-track.option.display-value-pan-1"),
        i18n->t("settings", "display-track.option.display-value-pan-2"),
        i18n->t("settings", "display-track.option.display-value-track-nb"),
        i18n->t("settings", "display-track.option.display-value-phase"),
        i18n->t("settings", "display-track.option.display-value-armed"),
        i18n->t("settings", "display-track.option.display-value-automation"),
        i18n->t("settings", "display-track.option.display-value-fx-state"),
    };

    std::vector<std::string> track_display_alignments = {
        i18n->t("settings", "display-track.option.display-align-center"),
        i18n->t("settings", "display-track.option.display-align-left"),
        i18n->t("settings", "display-track.option.display-align-right"),
    };

    std::vector<std::string> track_display_invert = {
        i18n->t("settings", "display-track.option.display-invert-no"),
        i18n->t("settings", "display-track.option.display-invert-yes"),
    };

    std::vector<std::string> track_valuebar_modees = {
        i18n->t("settings", "display-track.option.value-bar-normal"),
        i18n->t("settings", "display-track.option.value-bar-bipolar"),
        i18n->t("settings", "display-track.option.value-bar-fill"),
        i18n->t("settings", "display-track.option.value-bar-spread"),
        i18n->t("settings", "display-track.option.value-bar-off"),
    };

    std::vector<std::string> track_valuebar_values = {
        i18n->t("settings", "display-track.option.display-value-volume"),
        i18n->t("settings", "display-track.option.display-value-pan-1"),
        i18n->t("settings", "display-track.option.display-value-pan-2"),
    };

public:
    CSurf_FP_8_SettingsPage( // NOLINT(*-pro-type-member-init)
        ImGui_Context *m_ctx,
        CSurf_UI_Assets *assets
    ) : CSurf_UI_PageContent(m_ctx, assets) {
        i18n = I18n::GetInstance();
        settings = ReaSonusSettings::GetInstance(FP_8);

        GetLanguages(language_names);
        CSurf_FP_8_SettingsPage::Reset();
    }

    ~CSurf_FP_8_SettingsPage() override = default;

    void RenderGlobalTab() {
        const int language_button_width = getButtonWidth(
            m_ctx,
            i18n->t("settings", "language.button.label"),
            assets->GetMainFontBold());

        UiStyledElements::PushReaSonusTabStyle(m_ctx, selected_tab == 0);
        if (ImGui::BeginTabItem(m_ctx, i18n->t("settings", "tab.global").c_str())) {
            selected_tab = 0;
            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 16);

            if (ImGui::BeginChild(m_ctx, "language-select", -1 * language_button_width, 0.0,
                                  ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY)) {
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
            if (ImGui::BeginChild(m_ctx, "language-action", 0.0, 0.0,
                                  ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY)) {
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 22);
                UiStyledElements::PushReaSonusButtonOutlineStyle(m_ctx, assets->GetMainFontBold());
                if (ImGui::Button(m_ctx, i18n->t("settings", "language.button.label").c_str())) {
                    edit_language = true;
                }
                UiStyledElements::PopReaSonusButtonOutlineStyle(m_ctx);

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
        UiStyledElements::PopReaSonusTabStyle(m_ctx);
    }

    void RenderControllerTab() {
        UiStyledElements::PushReaSonusTabStyle(m_ctx, selected_tab == 1);
        if (ImGui::BeginTabItem(m_ctx, i18n->t("settings", "tab.functional").c_str())) {
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

            RenderInfoCheckbox(
                m_ctx,
                assets,
                i18n->t("settings", "instant-multi-select-filter.label"),
                &setting_instant_multi_select_filter,
                i18n->t("settings", "instant-multi-select-filter.tooltip"));

            RenderInfoCheckbox(
                m_ctx,
                assets,
                i18n->t("settings", "mute-master-on-fwd-rwd.label"),
                &setting_mute_master_on_fwd_rwd,
                i18n->t("settings", "mute-master-on-fwd-rwd.tooltip"));

            if (setting_latch_preview_action_enable) {
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
        UiStyledElements::PopReaSonusTabStyle(m_ctx);
    }

    void RenderPluginsTab() {
        double width = 0.0;
        double height = 0.0;

        UiStyledElements::PushReaSonusTabStyle(m_ctx, selected_tab == 2);
        if (ImGui::BeginTabItem(m_ctx, i18n->t("settings", "tab.plugins").c_str())) {
            selected_tab = 2;
            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 16);
            ImGui::GetContentRegionAvail(m_ctx, &width, &height);

            if (ImGui::BeginChild(m_ctx, "settings-display", width / 2 - 8, 0.0, ImGui::ChildFlags_None)) {
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
                    i18n->t("settings", "plugin-step-size.tooltip"),
                    "%d");
                ImGui::EndChild(m_ctx);
            }

            ImGui::SameLine(m_ctx);
            ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 16);
            if (ImGui::BeginChild(m_ctx, "settings-colors", width / 2 - 8, 0.0, ImGui::ChildFlags_None)) {
                RenderInfoCheckbox(
                    m_ctx,
                    assets,
                    i18n->t("settings", "plugin-map-param-clear.label"),
                    &setting_plugin_map_param_clear,
                    i18n->t("settings", "plugin-map-param-clear.tooltip"));

                ImGui::EndChild(m_ctx);
            }

            ImGui::EndTabItem(m_ctx);
        }
        UiStyledElements::PopReaSonusTabStyle(m_ctx);
    }

    void RenderDisplayLineTab(const int tab_index) {
        UiStyledElements::PushReaSonusTabStyle(m_ctx, selected_display_line == tab_index);
        const std::string tab_id = std::to_string(tab_index + 1);

        if (ImGui::BeginTabItem(m_ctx, tab_id.c_str())) {
            selected_display_line = tab_index;
            ImGui::Image(m_ctx, assets->GetDisplayMode(setting_track_display), 76, 100);

            ImGui::SameLine(m_ctx);


            UiStyledElements::PushReaSonusGroupStyle(m_ctx, true);
            if (ImGui::BeginChild(
                m_ctx,
                "settings-display-line",
                0.0,
                0.0,
                ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_ResizeY
            )) {
                ReaSonusComboInputRow(
                    m_ctx,
                    i18n->t("settings", "display-track.value.label"),
                    track_display_values,
                    &setting_track_value_line_value[tab_index],
                    70.0
                );

                ReaSonusComboInputRow(
                    m_ctx,
                    i18n->t("settings", "display-track.alignment.label"),
                    track_display_alignments,
                    &setting_track_value_line_align[tab_index],
                    70.0
                );

                ReaSonusComboInputRow(
                    m_ctx,
                    i18n->t("settings", "display-track.invert.label"),
                    track_display_invert,
                    &setting_track_value_line_invert[tab_index],
                    70.0
                );

                UiStyledElements::PopReaSonusGroupStyle(m_ctx);
                ImGui::EndChild(m_ctx);
            }

            UiStyledElements::PopReaSonusTabStyle(m_ctx);
            ImGui::EndTabItem(m_ctx);
        } else {
            UiStyledElements::PopReaSonusTabStyle(m_ctx);
        }
    }

    void RenderDisplayTab() {
        double width = 0.0;
        double height = 0.0;

        UiStyledElements::PushReaSonusTabStyle(m_ctx, selected_tab == 3);
        if (ImGui::BeginTabItem(m_ctx, i18n->t("settings", "tab.display").c_str())) {
            selected_tab = 3;
            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 16);
            ImGui::GetContentRegionAvail(m_ctx, &width, &height);

            if (ImGui::BeginChild(
                m_ctx,
                "settings-display",
                width / 1.75 - 8,
                0.0,
                ImGui::ChildFlags_None,
                ImGui::ChildFlags_AutoResizeY
            )) {
                UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
                if (ImGui::BeginChild(
                    m_ctx,
                    "settings-display",
                    0.0,
                    0.0,
                    ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
                )) {
                    ReaSonusImageComboInput(
                        m_ctx,
                        assets,
                        i18n->t("settings", "display-track.label"),
                        track_display_names,
                        &setting_track_display,
                        0.0
                    );

                    if (ImGui::BeginChild(
                        m_ctx,
                        "settings-display-line-tabs",
                        0.0,
                        0.0,
                        ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY
                    )) {
                        UiStyledElements::PushReaSonusTabBarStyle(m_ctx);
                        if (ImGui::BeginTabBar(m_ctx, "SettingsDisplayLinesTabs",
                                               ImGui::TabBarFlags_None | ImGui::ChildFlags_AutoResizeY)) {
                            for (int i = 0; i < track_display_lines[setting_track_display]; i++) {
                                RenderDisplayLineTab(i);
                            }

                            UiStyledElements::PopReaSonusTabBarStyle(m_ctx);
                            ImGui::EndTabBar(m_ctx);
                        }
                        ImGui::EndChild(m_ctx);
                    }
                    ImGui::Image(m_ctx, assets->GetValueBarType(setting_track_valuebar_mode), 76, 23);

                    ImGui::SameLine(m_ctx);

                    UiStyledElements::PushReaSonusGroupStyle(m_ctx, true);
                    if (ImGui::BeginChild(
                        m_ctx,
                        "settings-value-bar",
                        0.0,
                        0.0,
                        ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
                    )) {
                        ReaSonusComboInputRow(
                            m_ctx,
                            i18n->t("settings", "display-track.value-bar-type.label"),
                            track_valuebar_modees,
                            &setting_track_valuebar_mode,
                            70.0
                        );
                        ReaSonusComboInputRow(
                            m_ctx,
                            i18n->t("settings", "display-track.value-bar-value.label"),
                            track_valuebar_values,
                            &setting_track_valuebar_value,
                            70.0
                        );

                        UiStyledElements::PopReaSonusGroupStyle(m_ctx);
                        ImGui::EndChild(m_ctx);
                    } else {
                        UiStyledElements::PopReaSonusGroupStyle(m_ctx);
                    }

                    UiStyledElements::PopReaSonusGroupStyle(m_ctx);
                    ImGui::EndChild(m_ctx);
                } else {
                    UiStyledElements::PopReaSonusGroupStyle(m_ctx);
                }

                ImGui::EndChild(m_ctx);
            }

            ImGui::SameLine(m_ctx);

            ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 16);
            if (ImGui::BeginChild(
                m_ctx,
                "settings-colors",
                width - width / 1.75 - 8,
                0.0,
                ImGui::ChildFlags_None
            )) {
                RenderinfoIntInput(
                    m_ctx,
                    assets,
                    i18n->t("settings", "track-color-brightness.label"),
                    &setting_track_color_brightness,
                    5,
                    100,
                    i18n->t("settings", "track-color-brightness.tooltip"),
                    "%d%%");

                RenderInfoCheckbox(
                    m_ctx,
                    assets,
                    i18n->t("settings", "overwrite-timecode.label"),
                    &setting_overwrite_time_code,
                    i18n->t("settings", "overwrite-timecode.tooltip"));

                if (setting_overwrite_time_code) {
                    ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 26);
                    RenderInfoComboInput(
                        m_ctx,
                        assets,
                        i18n->t("settings", "timecode-list.label"),
                        time_code_names,
                        &setting_time_code,
                        i18n->t("settings", "timecode-list.tooltip"));
                }


                ImGui::EndChild(m_ctx);
            }

            ImGui::EndTabItem(m_ctx);
        }
        UiStyledElements::PopReaSonusTabStyle(m_ctx);
    }

    void Render() override {
        if (edit_language != previous_edit_language) {
            previous_edit_language = edit_language;
            Main_OnCommandStringEx("_REASONUS_TRANSLATIONN_EDITOR", 0, nullptr);
        }

        UiStyledElements::PushReaSonusSettingsContentStyle(m_ctx);
        if (ImGui::BeginChild(
            m_ctx,
            "settings_lists_content",
            0.0,
            0.0,
            ImGui::ChildFlags_FrameStyle,
            ImGui::ChildFlags_AutoResizeY
        )) {
            UiStyledElements::PushReaSonusTabBarStyle(m_ctx);
            if (ImGui::BeginTabBar(m_ctx, "SettingsTabs", ImGui::TabBarFlags_None)) {
                RenderGlobalTab();
                RenderControllerTab();
                RenderPluginsTab();
                RenderDisplayTab();

                UiStyledElements::PopReaSonusTabBarStyle(m_ctx);
                ImGui::EndTabBar(m_ctx);
            }

            UiStyledElements::PopReaSonusSettingsContentStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void Save() override {
        DAW::SetExtState(EXT_STATE_KEY_UI_LANGUAGE, language_names[setting_language], true);

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
        settings->SetSetting("surface", "latch-preview-action-code",
                             latch_preview_action_indexes[setting_latch_preview_action]);
        settings->SetSetting("surface", "time-code", time_code_indexes[setting_time_code]);
        settings->SetSetting("surface", "plugin-step-size", setting_plugin_step_size);
        settings->SetSetting("surface", "plugin-map-param-clear", setting_plugin_map_param_clear);
        settings->SetSetting("surface", "instant-multi-select-filter", setting_instant_multi_select_filter);
        settings->SetSetting("surface", "mute-master-on-fwd-rwd", setting_mute_master_on_fwd_rwd);
        settings->SetSetting("displays", "track", setting_track_display);
        settings->SetSetting("displays", "track-lines", joinDisplayValues(setting_track_value_line_value, ","));
        settings->SetSetting("displays", "track-alignment", joinDisplayValues(setting_track_value_line_align, ","));
        settings->SetSetting("displays", "track-invert", joinDisplayValues(setting_track_value_line_invert, ","));
        settings->SetSetting("displays", "track-value-bar-mode", setting_track_valuebar_mode);
        settings->SetSetting("displays", "track-value-bar-value", setting_track_valuebar_value);

        if (settings->StoreSettings()) {
            DAW::SetExtState(EXT_STATE_KEY_SAVED_SETTINGS, EXT_STATE_VALUE_TRUE, false);
            ReaSonus8ControlPanel::SetMessage(i18n->t("settings", "action.save.message"));
        }

        int actionId = NamedCommandLookup("_REASONUS_TOGGLE_DISABLE_PLUGINS_SETTING");
        RefreshToolbar2(0, actionId);
        actionId = NamedCommandLookup("_REASONUS_TOGGLE_DISTRACTION_FREE_MODE");
        RefreshToolbar2(0, actionId);
    }

    void Reset() override {
        setting_language = 0;

        for (int i = 0; i < static_cast<int>(language_names.size()); i++) {
            if (language_names[i] == DAW::GetExtState(EXT_STATE_KEY_UI_LANGUAGE, "en-US")) {
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
        setting_plugin_map_param_clear = settings->ShouldClearParamInput();
        setting_track_display = settings->GetTrackDisplay();
        setting_track_value_line_value = settings->GetTrackDisplayLineValues();
        setting_track_value_line_align = settings->GetTrackDisplayAlignValues();
        setting_track_value_line_invert = settings->GetTrackDisplayInvertValues();
        setting_track_valuebar_mode = settings->GetTrackValueBarMode();
        setting_track_valuebar_value = settings->GetTrackValueBarValue();
        setting_instant_multi_select_filter = settings->ShouldMultiFilterApplyInstant();
        setting_mute_master_on_fwd_rwd = settings->ShouldMuteMasterOnFwdRwd();

        auto iterator = std::find(
            latch_preview_action_indexes.begin(),
            latch_preview_action_indexes.end(),
            settings->GetLatchPreviewActionCode());

        if (iterator != latch_preview_action_indexes.end()) {
            setting_latch_preview_action = static_cast<int>(std::distance(
                latch_preview_action_indexes.begin(), iterator));
        }

        auto time_code_iterator = std::find(
            time_code_indexes.begin(),
            time_code_indexes.end(),
            settings->GetSurfaceTimeCode());

        if (time_code_iterator != time_code_indexes.end()) {
            setting_time_code = static_cast<int>(std::distance(time_code_indexes.begin(), time_code_iterator));
        }
    }

    static std::string joinDisplayValues(const std::array<int, 4> list, const std::string &delimiter) {
        std::string result;

        for (int i = 0; i < static_cast<int>(sizeof(list) / sizeof(list[0])); i++) {
            result += (i > 0 ? delimiter : "") + std::to_string(list[i]);
        }

        return result;
    }
};
