#include "csurf_ui_plugin_type_mapping.hpp"
#include "csurf_ui_fp_v2_display_widget.hpp"
#include "../csurf_ui_colors.hpp"
#include "../csurf_ui_vars.hpp"
#include "../components/csurf_ui_button_bar.hpp"
#include "../../shared/csurf_utils.hpp"
#include "../../shared/csurf.h"
#include "../../shared/csurf_daw.hpp"
#include "../utils/csurf_ui_text_overflow.hpp"
#include "../utils/csurf_ui_text_align.hpp"

#include "../../csurf_faderport_8/csurf_fp_8_channel_manager_resources.hpp"

constexpr auto g_name{"ReaSonus Native New Version"};

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
    GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);

    display_values[0] = DAW::GetTrackName(media_track);
    display_values[1] = DAW::GetTrackIndex(media_track);
    display_values[2] = GetVolumeString(DAW::GetTrackVolume(media_track));
    display_values[3] = GetPan1String(pan1, pan_mode);
    display_values[4] = GetPan2String(pan2, pan_mode);
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
    if (ImGui::Begin(
        m_ctx,
        g_name,
        &open,
        ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_AlwaysAutoResize | ImGui::WindowFlags_TopMost |
        ImGui::WindowFlags_NoTitleBar
    )) {
        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        if (ImGui::BeginChild(m_ctx, "wrapper", 0.0, 0.0, ImGui::ChildFlags_FrameStyle)) {
            if (ImGui::BeginChild(m_ctx, "logo", -40.0, 0.0, ImGui::ChildFlags_None)) {
                ImGui::GetContentRegionAvail(m_ctx, &available_width, &available_height);
                ImGui::Text(m_ctx, textAlign(m_ctx, display_values[0], ALIGN_CENTER).c_str());
                ImGui::Text(m_ctx, textAlign(m_ctx, display_values[1], ALIGN_CENTER).c_str());
                ImGui::Text(m_ctx, textAlign(m_ctx, display_values[2], ALIGN_CENTER).c_str());
                ImGui::Text(
                    m_ctx,
                    textAlign(
                        m_ctx,
                        fmt::format("{} / {}", display_values[3], display_values[4]),
                        ALIGN_CENTER
                    ).c_str()
                );

                ImGui::ProgressBar(m_ctx, DAW::GetTrackSurfacePeakInfo(media_track) / 127.0, 10.0, 190, "");
                ImGui::Text(m_ctx, std::to_string(DAW::GetTrackSurfacePeakInfo(media_track)).c_str());

                // Add a value bar style component here

                ImGui::EndChild(m_ctx); // logo
            }

            ImGui::SameLine(m_ctx);

            if (ImGui::BeginChild(m_ctx, "logo_2", 0.0, 0.0, ImGui::ChildFlags_None)) {
                // Create a VU meter here
                ImGui::Text(m_ctx, "VU");
                ImGui::Text(m_ctx, "Me");
                ImGui::Text(m_ctx, "ter");

                ImGui::EndChild(m_ctx); // logo
            }
            ImGui::EndChild(m_ctx); // wrapper
        }
        UiStyledElements::PopReaSonusGroupStyle(m_ctx);

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
