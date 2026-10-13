#include <string>
#include "csurf_ui_plugin_type_mapping.hpp"
#include "csurf_ui_fp_v2_display_widget.hpp"
#include "../csurf_ui_colors.hpp"
#include "../csurf_ui_vars.hpp"
#include "../components/csurf_ui_button_bar.hpp"
#include "../../shared/csurf_utils.hpp"
#include "../../shared/csurf.h"
#include "../../shared/csurf_daw.hpp"
#include "../utils/csurf_ui_text_align.hpp"

#include "../../csurf_faderport_8/csurf_fp_8_channel_manager_resources.hpp"

constexpr auto g_name{"ReaSonus Native Display Widget"};

std::unique_ptr<ReaSonusFpV2DisplayWidget> ReaSonusFpV2DisplayWidget::s_inst;

static void reportError(const ImGui_Error &e) {
    ShowMessageBox(e.what(), g_name, 0);
}

ReaSonusFpV2DisplayWidget::ReaSonusFpV2DisplayWidget()
    : m_ctx{} {
    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    assets = new CSurf_UI_Assets(m_ctx);

    plugin_register("timer", reinterpret_cast<void *>(&Loop));
}

ReaSonusFpV2DisplayWidget::~ReaSonusFpV2DisplayWidget() {
    plugin_register("-timer", reinterpret_cast<void *>(&Loop));
}

void ReaSonusFpV2DisplayWidget::Start() {
    try {
        window_open = true;
        if (s_inst) {
            ImGui::SetNextWindowFocus(s_inst->m_ctx);
        } else {
            s_inst.reset(new ReaSonusFpV2DisplayWidget);
        }
    } catch (const ImGui_Error &e) {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusFpV2DisplayWidget::Stop() {
    try {
        window_open = false;
        if (s_inst) {
            s_inst.reset();
        }
    } catch (const ImGui_Error &e) {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusFpV2DisplayWidget::SetDisplayValues() {
    double pan1 = 0.0;
    double pan2 = 0.0;
    int pan_mode;

    const auto widget_mode = static_cast<FP_V2_WIDGET_MODE>(stoi(DAW::GetExtState(
        EXT_STATE_FP_V2_WIDGET_MODE,
        std::to_string(FP_V2_WIDGET_TRACK_MODE)
    )));

    switch (widget_mode) {
        case FP_V2_WIDGET_TRACK_MODE: {
            GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);

            if (media_track != nullptr) {
                display_values[0] = DAW::GetTrackName(media_track);
                display_values[1] = DAW::GetTrackIndex(media_track);
                display_values[2] = GetVolumeString(DAW::GetTrackVolume(media_track));
                display_values[3] = GetPan2String(pan2, pan_mode).empty()
                                        ? GetPan1String(pan1, pan_mode)
                                        : fmt::format(
                                            "{} / {}",
                                            GetPan1String(pan1, pan_mode),
                                            GetPan2String(pan2, pan_mode)
                                        );
            } else {
                display_values[0] = "No track";
                display_values[1] = "selected";
                display_values[2] = "";
                display_values[3] = "";
            }
            break;
        }
        case FP_V2_WIDGET_MASTER_MODE: {
            GetTrackUIPan(GetMasterTrack(nullptr), &pan1, &pan2, &pan_mode);
            display_values[0] = "Master";
            display_values[1] = "";
            display_values[2] = GetVolumeString(DAW::GetTrackVolume(media_track));
            display_values[3] = GetPan2String(pan2, pan_mode).empty()
                                    ? GetPan1String(pan1, pan_mode)
                                    : fmt::format(
                                        "{} / {}",
                                        GetPan1String(pan1, pan_mode),
                                        GetPan2String(pan2, pan_mode)
                                    );
            break;
        }

        case FP_V2_WIDGET_PARAM_MODE: {
            int plugin_index = -1;
            int param_index = -1;

            GetLastTouchedFX(&track_index, &plugin_index, &param_index);
            MediaTrack *plugin_track = GetTrack(nullptr, track_index - 1);

            if (plugin_track != nullptr) {
                display_values[0] = DAW::GetTrackName(plugin_track);
                display_values[1] = DAW::GetTrackFxName(plugin_track, plugin_index, false);
                display_values[2] = DAW::GetTrackFxParamName(plugin_track, plugin_index, param_index);
                display_values[3] = DAW::GetTrackFxFormattedParamValue(plugin_track, plugin_index, param_index);
            } else {
                display_values[0] = "No plugin";
                display_values[1] = "parameter";
                display_values[2] = "selected";
                display_values[3] = "";
            }
            break;
        }

        default:


    }
}

int ReaSonusFpV2DisplayWidget::GetHighlightColor() {
    const auto widget_mode = static_cast<FP_V2_WIDGET_MODE>(stoi(DAW::GetExtState(
        EXT_STATE_FP_V2_WIDGET_MODE,
        std::to_string(FP_V2_WIDGET_TRACK_MODE)
    )));

    switch (widget_mode) {
        case FP_V2_WIDGET_TRACK_MODE:
            return UI_COLORS::Accent_50;

        case FP_V2_WIDGET_MASTER_MODE:
            return UI_COLORS::Blue;

        case FP_V2_WIDGET_PARAM_MODE:
            return UI_COLORS::Success_50;

        default:
            return UI_COLORS::Accent_50;
    }
}

void ReaSonusFpV2DisplayWidget::Loop() {
    try {
        s_inst->Frame();
    } catch (const ImGui_Error &e) {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusFpV2DisplayWidget::Frame() {
    double available_width;
    double available_height;

    if (close_clicked) {
        close_clicked = false;
        Stop();
        return;
    }

    if (save_clicked) {
        save_clicked = false;
        ReaSonusPluginTypeMapping::Start();
    }

    MediaTrack *track = GetSelectedTrack(nullptr, 0);
    if (track != media_track) {
        media_track = track;
    }
    SetDisplayValues();

    PushReaSonusColors(m_ctx);
    PushReaSonusStyle(m_ctx);
    ImGui::PushFont(m_ctx, assets->GetMainFont(), FontSizeDefault);
    bool open{true};

    UiStyledElements::PushReaSonusWindowStyle(m_ctx);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowPadding, 8, 8);
    if (ImGui::IsWindowDocked(m_ctx)) {
        ShowConsoleMsg("");
        ShowConsoleMsg("Docked");
    }
    if (ImGui::Begin(
        m_ctx,
        g_name,
        &open,
        ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_TopMost
    )) {
        ImGui::GetContentRegionAvail(m_ctx, &available_width, &available_height);
        double font_size = available_width / 176 * 18;
        if (!ImGui::IsWindowDocked(m_ctx)) {
            if (available_width < 120) {
                ImGui::SetWindowSizeEx(m_ctx, g_name, 176, 190);
            }
        }

        ImGui::PushFont(m_ctx, assets->GetMainFont(), font_size);
        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, GetHighlightColor());
        if (ImGui::BeginChild(m_ctx, "wrapper", 0.0, 0.0, ImGui::ChildFlags_FrameStyle)) {
            if (ImGui::BeginChild(m_ctx, "logo", 0.0, 0.0, ImGui::ChildFlags_None)) {
                ImGui::GetContentRegionAvail(m_ctx, &available_width, &available_height);
                ImGui::Text(m_ctx, textAlign(m_ctx, display_values[0], ALIGN_CENTER).c_str());
                ImGui::Text(m_ctx, textAlign(m_ctx, display_values[1], ALIGN_CENTER).c_str());
                ImGui::Text(m_ctx, textAlign(m_ctx, display_values[2], ALIGN_CENTER).c_str());
                ImGui::Text(m_ctx, textAlign(m_ctx, display_values[3], ALIGN_CENTER).c_str());

                ImGui::EndChild(m_ctx); // logo
            }

            ImGui::EndChild(m_ctx); // wrapper
        }
        ImGui::PopStyleColor(m_ctx);
        UiStyledElements::PopReaSonusGroupStyle(m_ctx);
        ImGui::PopFont(m_ctx);
        ImGui::End(m_ctx); // window
    }
    ImGui::PopStyleVar(m_ctx);
    UiStyledElements::PopReaSonusWindowStyle(m_ctx);

    ImGui::PopFont(m_ctx);
    PopReaSonusColors(m_ctx);
    PopReaSonusStyle(m_ctx);

    if (!open) {
        window_open = false;
        SetActionState("_REASONUS_TRANSLATIONN_EDITOR");
        s_inst.reset();
    }
}
