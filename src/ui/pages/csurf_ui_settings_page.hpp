#ifndef CSURF_UI_SETTINGS_PAGE_H
#define CSURF_UI_SETTINGS_PAGE_H

#include <array>
#include "../csurf_ui_page_content.hpp"
#include "../../i18n/i18n.hpp"

class ImGui_Context;

class CSurf_SettingsPage : public CSurf_UI_PageContent { // NOLINT(*-use-internal-linkage)
protected:
    I18n *i18n = I18n::GetInstance();
    int selected_tab = -1;

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

    std::vector<std::string> automation_mode_shape_labels = {
        i18n->t("settings", "automation-single.point.shape.linear"),
        i18n->t("settings", "automation-single.point.shape.square"),
        i18n->t("settings", "automation-single.point.shape.slow-start-end"),
        i18n->t("settings", "automation-single.point.shape.fast-start"),
        i18n->t("settings", "automation-single.point.shape.fast-end"),
        i18n->t("settings", "automation-single.point.shape.bezier"),
    };

    std::array<int, 6> time_code_indexes = {0, 2, 3, 4, 5, 8};
    std::vector<std::string> time_code_names = {
        i18n->t("settings", "timecode-list.option.time"),          // Time
        i18n->t("settings", "timecode-list.option.beats"),         // "Beats",
        i18n->t("settings", "timecode-list.option.seconds"),       // "Seconds",
        i18n->t("settings", "timecode-list.option.samples"),       // "Samples",
        i18n->t("settings", "timecode-list.option.hr-min-sec-fr"), // "Hr:Min:Sec:Fr",
        i18n->t("settings", "timecode-list.option.frames"),        // "Abs. Frames"
    };

    std::array<int, 9> track_display_lines = {3, 3, 4, 2, 2, 3, 3, 3, 3};
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

    std::vector<std::string> send_display_values = {
        i18n->t("settings", "display-send.option.display-value-volume"),
        i18n->t("settings", "display-send.option.display-value-pan"),
        i18n->t("settings", "display-send.option.display-value-mute"),
        i18n->t("settings", "display-send.option.display-value-phase"),
        i18n->t("settings", "display-send.option.display-value-mono"),
        i18n->t("settings", "display-send.option.display-value-send-mode"),
        i18n->t("settings", "display-send.option.display-value-auto-mode"),
        i18n->t("settings", "display-send.option.display-value-fixed"),
    };

    std::vector<std::string> plugin_map_color_mode_labels = {
        i18n->t("mapping", "edit.color.show.combo.hide"),
        i18n->t("mapping", "edit.color.show.combo.show"),
        i18n->t("mapping", "edit.color.show.combo.bright"),
        i18n->t("mapping", "edit.color.show.combo.dimmed"),
    };

    std::vector<std::string> transport_fwd_rwd_button_tap_options = {
        i18n->t("settings", "transport-fwd-rwd.button-tap.continuous"),
        i18n->t("settings", "transport-fwd-rwd.button-tap.nearest-measure"),
        i18n->t("settings", "transport-fwd-rwd.button-tap.disable"),
    };

    std::vector<std::string> transport_fwd_rwd_button_hold_options = {
        i18n->t("settings", "transport-fwd-rwd.button-hold.momentary"),
        i18n->t("settings", "transport-fwd-rwd.button-hold.nearest-measure"),
        i18n->t("settings", "transport-fwd-rwd.button-hold.disable"),
    };

public:
    CSurf_SettingsPage(ImGui_Context *m_ctx, CSurf_UI_Assets *assets) : CSurf_UI_PageContent(m_ctx, assets) {
    }

    ~CSurf_SettingsPage() override = default;
};

#endif
