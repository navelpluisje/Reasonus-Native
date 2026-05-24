#ifndef CSURF_FP_UI_INT_INPUT_H_
#define CSURF_FP_UI_INT_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../csurf_ui_styles_elements.hpp"
#include "csurf_ui_tooltip.hpp"

static void ReaSonusIntInput(
    ImGui_Context *m_ctx,
    const std::string &label,
    int *value,
    int v_min,
    int v_max,
    double width,
    std::string format) {
    if (format.empty()) {
        format = "%d";
    }

    int local_value = *value;

    UiStyledElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), width, 0.0,
                          ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
        ImGui::Text(m_ctx, label.c_str());

        if (ImGui::BeginChild(m_ctx, ("input-container" + label).c_str(), width, 0.0,
                              ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
            UiStyledElements::PushReaSonusInputStyle(m_ctx);
            if (ImGui::SliderInt(m_ctx, ("##" + label).c_str(), &local_value, v_min, v_max, format.c_str())) {
                *value = local_value;
            }
            UiStyledElements::PopReaSonusInputStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }

        ImGui::EndChild(m_ctx);
    }
    UiStyledElements::PopReaSonusFieldGroupStyle(m_ctx);
}

static void RenderinfoIntInput(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    const std::string &label,
    int *value,
    const int min,
    const int max,
    const std::string &tooltip,
    std::string format) {
    if (format.empty()) {
        format = "%d";
    }
    const double x_pos = ImGui::GetCursorPosX(m_ctx);
    const double y_pos = ImGui::GetCursorPosY(m_ctx);
    const std::string tooltip_id = std::to_string(x_pos) + "-" + std::to_string(y_pos);

    ReaSonusIntInput(m_ctx, label, value, min, max, -20.0, format);
    ImGui::SameLine(m_ctx);
    ReaSonusTooltip(m_ctx, assets, tooltip, tooltip_id, -20, 26);
    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
    ImGui::Dummy(m_ctx, 0, 0);
}

#endif
