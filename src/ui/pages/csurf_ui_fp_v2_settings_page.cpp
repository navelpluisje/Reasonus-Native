#include <array>
#include "csurf_ui_fp_v2_control_panel.hpp"
#include "../csurf_ui_page_content.hpp"
#include "../../i18n/i18n.hpp"
#include "../../shared/csurf.h"
#include "../../shared/csurf_daw.hpp"
#include "../../shared/csurf_reasonus_settings.hpp"
#include "../components/csurf_ui_checkbox.hpp"
#include "../components/csurf_ui_color_picker.hpp"
#include "../components/csurf_ui_combo_input.hpp"
#include "../components/csurf_ui_page_title.hpp"
#include "../utils/csurf_ui_button_width.hpp"

class CSurf_FP_V2_SettingsPage : public CSurf_UI_PageContent { // NOLINT(*-use-internal-linkage)
    I18n *i18n = I18n::GetInstance();
    int selected_tab = -1;

    // Global
    int setting_language;
    std::vector<int> plugin_color_palette;

    bool edit_language = false;
    bool previous_edit_language = false;
    bool momentary_mute_solo;
    bool control_hidden_tracks;
    bool can_disable_fader;
    bool endless_track_scroll;
    bool setting_latch_preview_action_enable;
    int setting_latch_preview_action;

    // Automation
    bool setting_use_automation_colors;
    std::array<int, 7> setting_automation_colors;
    std::vector<std::string> setting_automation_trim_labels = {
        "Latch",
        "Trim",
        "Off",
        "Touch",
        "Write",
        "Read",
        "Single",
    };
    int prev_automation_trim_color;

    std::vector<int> settings_plugin_color_palette;
    std::vector<int> settings_initial_plugin_color_palette;

    std::vector<std::string> language_names;
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_V2);
    ReaSonusInfoComboInputRow *language_select_combo;
    ReaSonusInfoComboInputRow *latch_preview_action_combo;

    std::array<int, 8> latch_preview_action_indexes = {42013, 42014, 42015, 42016, 42017, 41160, 41161, 41162};
    std::vector<std::string> latch_preview_action_names = {
        "actively-writing envelopes to time selection",
        "actively-writing envelopes from cursor to start of project",
        "actively-writing envelopes from cursor to end of project",
        "actively-writing envelopes from cursor to first touch position",
        "actively-writing envelopes to entire envelope",
        "all writing envelopes to time selection",
        "all writing envelopes from cursor to start of project",
        "all writing envelopes from cursor to end of project",
    };

public:
    CSurf_FP_V2_SettingsPage( // NOLINT(*-pro-type-member-init)
        ImGui_Context *m_ctx,
        CSurf_UI_Assets *assets
    ) : CSurf_UI_PageContent(m_ctx, assets) {
        i18n = I18n::GetInstance();
        settings = ReaSonusSettings::GetInstance(FP_V2);

        GetLanguages(language_names);
        CSurf_FP_V2_SettingsPage::Reset();

        language_select_combo = new ReaSonusInfoComboInputRow(
            m_ctx,
            assets,
            i18n->t("settings", "language.label"),
            "select-language",
            language_names,
            &setting_language,
            i18n->t("settings", "language.tooltip")
        );

        latch_preview_action_combo = new ReaSonusInfoComboInputRow(
            m_ctx,
            assets,
            i18n->t("settings", "latch-preview-action-list.label"),
            "latch-preview-action-list",
            latch_preview_action_names,
            &setting_latch_preview_action,
            i18n->t("settings", "latch-preview-action-list.tooltip")
        );
    }

    ~CSurf_FP_V2_SettingsPage() override = default;

    void RenderAutomationTab(const int tab_index) {
        double width;
        double height;

        UiStyledElements::PushReaSonusTabStyle(m_ctx, selected_tab == tab_index);
        if (ImGui::BeginTabItem(m_ctx, i18n->t("settings", "tab.automation").c_str())) {
            selected_tab = tab_index;
            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 16);
            ImGui::GetContentRegionAvail(m_ctx, &width, &height);

            if (ImGui::BeginChild(m_ctx, "settings-automation-left", width / 2 - 8, 0.0, ImGui::ChildFlags_None)) {
                UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
                if (ImGui::BeginChild(
                    m_ctx,
                    "automation-color-settings",
                    0.0,
                    0.0,
                    ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
                )) {
                    ReaSonusPageTitle(
                        m_ctx,
                        assets,
                        i18n->t("settings", "automation-color-group.label"),
                        true
                    );

                    RenderInfoCheckbox(
                        m_ctx,
                        assets,
                        i18n->t("settings", "use-automation-color.label"),
                        &setting_use_automation_colors,
                        i18n->t("settings", "use-automation-color.tooltip")
                    );

                    ImGui::BeginDisabled(m_ctx, !setting_use_automation_colors);
                    ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 26);

                    ImGui::BeginGroup(m_ctx);
                    for (int i = 0; i < 6; i++) {
                        ImGui::BeginGroup(m_ctx);
                        ReaSonusColorPicker(
                            m_ctx,
                            assets,
                            setting_automation_trim_labels[i],
                            &setting_automation_colors[i],
                            prev_automation_trim_color,
                            plugin_color_palette,
                            48,
                            32
                        );
                        ImGui::CalcTextSize(m_ctx, setting_automation_trim_labels[i].c_str(), &width, &height);
                        ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + (50 - width) / 2);
                        ImGui::Text(m_ctx, setting_automation_trim_labels[i].c_str());
                        ImGui::EndGroup(m_ctx);

                        if ((i + 1) % 6 != 0) {
                            ImGui::SameLine(m_ctx);
                        }
                    }
                    ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 58);
                    ImGui::BeginGroup(m_ctx);
                    ReaSonusColorPicker(
                        m_ctx,
                        assets,
                        setting_automation_trim_labels[6],
                        &setting_automation_colors[6],
                        prev_automation_trim_color,
                        plugin_color_palette,
                        50,
                        32
                    );
                    ImGui::CalcTextSize(m_ctx, setting_automation_trim_labels[6].c_str(), &width, &height);
                    ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + (50 - width) / 2);
                    ImGui::Text(m_ctx, setting_automation_trim_labels[6].c_str());
                    ImGui::EndGroup(m_ctx);

                    ImGui::EndGroup(m_ctx);

                    ImGui::EndDisabled(m_ctx);

                    ImGui::EndChild(m_ctx);
                }

                if (ImGui::BeginChild(
                    m_ctx,
                    "automation-latch-settings",
                    0.0,
                    0.0,
                    ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
                )) {
                    ReaSonusPageTitle(
                        m_ctx,
                        assets,
                        i18n->t("settings", "automation-latch-group.label"),
                        true
                    );

                    RenderInfoCheckbox(
                        m_ctx,
                        assets,
                        i18n->t("settings", "latch-preview-action-enable.label"),
                        &setting_latch_preview_action_enable,
                        i18n->t("settings", "latch-preview-action-enable.tooltip")
                    );

                    ImGui::BeginDisabled(m_ctx, !setting_latch_preview_action_enable);
                    ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 26);
                    latch_preview_action_combo->Render();
                    ImGui::EndDisabled(m_ctx);

                    ImGui::EndChild(m_ctx);
                }

                UiStyledElements::PopReaSonusGroupStyle(m_ctx);

                ImGui::EndChild(m_ctx);
            }

            ImGui::SameLine(m_ctx);
            ImGui::BeginGroup(m_ctx);
            UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
            if (ImGui::BeginChild(
                m_ctx,
                "automation-single-settings",
                0.0,
                0.0,
                ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
            )) {
                ReaSonusPageTitle(
                    m_ctx,
                    assets,
                    i18n->t("settings", "automation-single-group.label"),
                    true
                );

                // RenderInfoCheckbox(
                //     m_ctx,
                //     assets,
                //     i18n->t("settings", "use-single-point-mode.label"),
                //     &setting_use_single_point_mode,
                //     i18n->t("settings", "use-single-point-mode.tooltip")
                // );
                //
                // RenderInfoCheckbox(
                //     m_ctx,
                //     assets,
                //     i18n->t("settings", "single-point-overwrite-point-shape.label"),
                //     &setting_overwrite_single_point_shape,
                //     i18n->t("settings", "single-point-overwrite-point-shape.tooltip")
                // );
                //
                // ImGui::BeginDisabled(m_ctx, !setting_overwrite_single_point_shape);
                // ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + 26);
                // ImGui::BeginGroup(m_ctx);
                // automation_single_point_shape->Render();
                // ReaSonusAutomationPointShape(
                //     m_ctx,
                //     assets,
                //     i18n->t("settings", "single-point-bezier-tension.label"),
                //     &setting_single_bezier_tension,
                //     setting_single_point_mode_shape,
                //     !setting_overwrite_single_point_shape
                // );
                // ImGui::EndGroup(m_ctx);
                // ImGui::EndDisabled(m_ctx);
                //
                // RenderInfoCheckbox(
                //     m_ctx,
                //     assets,
                //     i18n->t("settings", "single-point-button-blink.label"),
                //     &setting_single_point_button_blink,
                //     i18n->t("settings", "single-point-button-blink.tooltip")
                // );

                ImGui::EndChild(m_ctx);
            }

            UiStyledElements::PopReaSonusGroupStyle(m_ctx);
            ImGui::EndGroup(m_ctx);

            ImGui::EndTabItem(m_ctx);
        }
        UiStyledElements::PopReaSonusTabStyle(m_ctx);
    }

    void RenderGlobalTab(const int tab_index) {
        double width = 0.0;
        double height = 0.0;

        const double language_button_width = getButtonWidth(
            m_ctx,
            i18n->t("settings", "language.button.label"),
            assets->GetMainFontBold()
        );

        UiStyledElements::PushReaSonusTabStyle(m_ctx, selected_tab == tab_index);
        if (ImGui::BeginTabItem(m_ctx, i18n->t("settings", "tab.global").c_str())) {
            selected_tab = tab_index;
            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 16);

            UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
            if (ImGui::BeginChild(
                m_ctx,
                "language-container",
                0.0,
                0.0,
                ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
            )) {
                ImGui::GetContentRegionAvail(m_ctx, &width, &height);

                ReaSonusPageTitle(
                    m_ctx,
                    assets,
                    i18n->t("settings", "language.label"),
                    true
                );

                if (ImGui::BeginChild(
                    m_ctx,
                    "language-select",
                    -1 * language_button_width,
                    0.0,
                    ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY
                )) {
                    language_select_combo->Render();

                    ImGui::EndChild(m_ctx);
                }

                ImGui::SameLine(m_ctx);

                if (ImGui::BeginChild(
                    m_ctx,
                    "language-action",
                    0.0,
                    0.0,
                    ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY
                )) {
                    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 22);
                    UiStyledElements::PushReaSonusButtonOutlineStyle(m_ctx, assets->GetMainFontBold());
                    if (ImGui::Button(m_ctx, i18n->t("settings", "language.button.label").c_str())) {
                        edit_language = true;
                    }
                    UiStyledElements::PopReaSonusButtonOutlineStyle(m_ctx);

                    ImGui::EndChild(m_ctx);
                }

                ImGui::EndChild(m_ctx);
            }
            UiStyledElements::PopReaSonusGroupStyle(m_ctx);

            UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
            if (ImGui::BeginChild(
                m_ctx,
                "global-left-column",
                width / 2 - 8,
                0.0,
                ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
            )) {
                ReaSonusPageTitle(
                    m_ctx,
                    assets,
                    i18n->t("settings", "global-group.label"),
                    true
                );

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

                ImGui::EndChild(m_ctx);
            }
            UiStyledElements::PopReaSonusGroupStyle(m_ctx);

            ImGui::SameLine(m_ctx);

            if (ImGui::BeginChild(m_ctx, "global-right-column", 0.0, 0.0, ImGui::ChildFlags_None)) {
                UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
                if (ImGui::BeginChild(
                    m_ctx,
                    "settings-display",
                    0.0,
                    0.0,
                    ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
                )) {
                    ReaSonusPageTitle(
                        m_ctx,
                        assets,
                        i18n->t("settings", "color-group.label"),
                        true
                    );

                    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 2);
                    ImGui::Text(m_ctx, i18n->t("settings", "color-palette.label").c_str());
                    ImGui::SameLine(m_ctx);
                    ReaSonusTooltip(
                        m_ctx,
                        assets,
                        i18n->t("settings", "color-palette.tooltip"),
                        "color-picker-tooltip",
                        0,
                        -3
                    );
                    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);

                    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4.0);
                    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);
                    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::FormFieldBorder);
                    for (int i = 0; i < static_cast<int>(settings_plugin_color_palette.size()); i++) {
                        if (i % 6 != 0) {
                            ImGui::SameLine(m_ctx);
                        }

                        ReaSonusColorPicker(
                            m_ctx,
                            assets,
                            "Color Picker##picker" + std::to_string(i),
                            &settings_plugin_color_palette[i],
                            settings_initial_plugin_color_palette[i],
                            settings_plugin_color_palette,
                            30,
                            30
                        );
                    }
                    ImGui::PopStyleVar(m_ctx, 2);
                    ImGui::PopStyleColor(m_ctx, 1);
                    ImGui::EndChild(m_ctx);
                }
                UiStyledElements::PopReaSonusGroupStyle(m_ctx);

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
            "mapping_lists_content",
            0.0,
            0.0,
            ImGui::ChildFlags_FrameStyle,
            ImGui::ChildFlags_AutoResizeY
        )) {
            UiStyledElements::PushReaSonusTabBarStyle(m_ctx);
            if (ImGui::BeginTabBar(m_ctx, "SettingsTabs", ImGui::TabBarFlags_None)) {
                RenderGlobalTab(0);
                RenderAutomationTab(1);

                UiStyledElements::PopReaSonusTabBarStyle(m_ctx);
                ImGui::EndTabBar(m_ctx);
            }

            UiStyledElements::PopReaSonusSettingsContentStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void Save() override {
        DAW::SetExtState(EXT_STATE_KEY_UI_LANGUAGE, language_names[setting_language], true);
        settings->SetSetting("surface", "mute-solo-momentary", momentary_mute_solo);
        settings->SetSetting("surface", "control-hidden-tracks", control_hidden_tracks);
        settings->SetSetting("surface", "can-disable-fader", can_disable_fader);
        settings->SetSetting("surface", "endless-track-scroll", endless_track_scroll);
        settings->SetSetting("surface", "latch-preview-action", setting_latch_preview_action_enable);
        settings->SetSetting("surface", "latch-preview-action-code",
                             latch_preview_action_indexes[setting_latch_preview_action]);
        settings->SetSetting("surface", "reasonus-color-palette", join(settings_plugin_color_palette, ","));
        settings->SetSetting("surface", "use-automation-colors", setting_use_automation_colors);
        settings->SetSetting("surface", "automation-colors", join(setting_automation_colors, ","));

        if (settings->StoreSettings()) {
            DAW::SetExtState(EXT_STATE_KEY_SAVED_SETTINGS, EXT_STATE_VALUE_TRUE, false);
            ReaSonusV2ControlPanel::SetMessage(i18n->t("settings", "action.save.message"));
        }
    }

    void Reset() override {
        const auto language_index = std::find(
            language_names.begin(),
            language_names.end(),
            DAW::GetExtState(EXT_STATE_KEY_UI_LANGUAGE, "en-US")
        );
        setting_language = static_cast<int>(language_index - language_names.begin());

        momentary_mute_solo = settings->GetMuteSoloMomentary();
        control_hidden_tracks = settings->GetControlHiddenTracks();
        can_disable_fader = settings->GetCanDisableFader();
        endless_track_scroll = settings->GetEndlessTrackScroll();
        setting_latch_preview_action_enable = settings->GetLatchPreviewActionEnabled();
        settings_initial_plugin_color_palette = settings->GetPluginColorPalette();
        settings_plugin_color_palette = settings->GetPluginColorPalette();
        setting_use_automation_colors = settings->UseAutomationColors();
        setting_automation_colors = settings->GetAutomationColorsArray();

        auto iterator = std::find(
            latch_preview_action_indexes.begin(),
            latch_preview_action_indexes.end(),
            settings->GetLatchPreviewActionCode()
        );

        if (iterator != latch_preview_action_indexes.end()) {
            setting_latch_preview_action = static_cast<int>(std::distance(
                latch_preview_action_indexes.begin(), iterator
            ));
        }
    }
};
