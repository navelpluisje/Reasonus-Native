#ifndef CSURF_FP_UI_TOOLTIP_H_
#define CSURF_FP_UI_TOOLTIP_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"

static void ReaSonusTooltip(
    ImGui_Context *m_ctx,
    std::string label,
    std::string id,
    ImGui_Image *icon = nullptr,
    double icon_offset_x = 0,
    double icon_offset_y = -3)
{
    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + icon_offset_y);
    ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + icon_offset_x);
    if (icon != nullptr)
    {
        ImGui::Image(m_ctx, icon, 24, 24);
    }

    if (ImGui::BeginItemTooltip(m_ctx))
    {
        UiElements::PushReaSonusTooltipStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, id.c_str(), 0.0, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_AutoResizeX))
        {
            ImGui::PushTextWrapPos(m_ctx, 350);
            ImGui::Text(m_ctx, label.c_str());
            ImGui::PopTextWrapPos(m_ctx);

            UiElements::PopReaSonusTooltipStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
        ImGui::EndTooltip(m_ctx);
    }
}

static void ReaSonusSimpleTooltip(
    ImGui_Context *m_ctx,
    std::string label,
    std::string id)
{
    if (ImGui::BeginItemTooltip(m_ctx))
    {
        UiElements::PushReaSonusTooltipStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, id.c_str(), 0.0, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_AutoResizeX))
        {
            ImGui::PushTextWrapPos(m_ctx, 350);
            ImGui::Text(m_ctx, label.c_str());
            ImGui::PopTextWrapPos(m_ctx);

            UiElements::PopReaSonusTooltipStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
        ImGui::EndTooltip(m_ctx);
    }
}
#endif