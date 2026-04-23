// ReSharper disable CppDFAUnreachableFunctionCall
#ifndef CSURF_FP_UI_TOOLTIP_H_
#define CSURF_FP_UI_TOOLTIP_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"
#include "csurf_ui_colors.hpp"

static void ReaSonusSimpleTooltip(
    ImGui_Context *m_ctx,
    const std::string &label,
    const std::string &tooltip_id)
{
    if (ImGui::BeginItemTooltip(m_ctx))
    {
        UiElements::PushReaSonusTooltipStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, tooltip_id.c_str(), 0.0, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_AutoResizeX))
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

static void ReaSonusTooltip(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    const std::string &label,
    const std::string &tooltip_id,
    const double icon_offset_x,
    const double icon_offset_y)
{
    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + icon_offset_y);
    ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + icon_offset_x);

    ImGui::PushFont(m_ctx, assets->GetIconFont(), 22);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
    ImGui::Text(m_ctx, std::string(1, IconInformation).c_str());
    ImGui::PopStyleColor(m_ctx);
    ImGui::PopFont(m_ctx);

    ReaSonusSimpleTooltip(m_ctx, label, tooltip_id);
}

#endif