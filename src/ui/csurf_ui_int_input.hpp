#ifndef CSURF_FP_UI_INT_INPUT_H_
#define CSURF_FP_UI_INT_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"

static void ReaSonusIntInput(
    ImGui_Context *m_ctx,
    std::string label,
    int *value,
    int v_min,
    int v_max,
    double width = 0.0,
    std::string format = "%d")
{
    int local_value = *value;

    std::string id = "##" + label;

    UiElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), width, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
    {
        ImGui::Text(m_ctx, label.c_str());

        if (ImGui::BeginChild(m_ctx, ("input-container" + label).c_str(), width, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
        {
            UiElements::PushReaSonusInputStyle(m_ctx);
            if (ImGui::SliderInt(m_ctx, ("##" + label).c_str(), &local_value, v_min, v_max, format.c_str()))
            {
                *value = local_value;
            }
            UiElements::PopReaSonusInputStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }

        ImGui::EndChild(m_ctx);
    }
    UiElements::PopReaSonusFieldGroupStyle(m_ctx);
}

static void RenderinfoIntInput(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    std::string label,
    int *value,
    int min,
    int max,
    std::string tooltip,
    std::string format = "%d")
{
    double x_pos = ImGui::GetCursorPosX(m_ctx);
    double y_pos = ImGui::GetCursorPosY(m_ctx);
    std::string id = std::to_string(x_pos) + "-" + std::to_string(y_pos);

    ReaSonusIntInput(m_ctx, label.c_str(), value, min, max, -20.0, format);
    ImGui::SameLine(m_ctx);
    ReaSonusTooltip(m_ctx, assets, tooltip.c_str(), id.c_str(), -20, 26);
    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
    ImGui::Dummy(m_ctx, 0, 0);
}

#endif