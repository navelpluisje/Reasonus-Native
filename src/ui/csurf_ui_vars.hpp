#ifndef CSURF_FP_UI_VARS_H_
#define CSURF_FP_UI_VARS_H_

#include <reaper_imgui_functions.h>

void PushReaSonusStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_Alpha, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_DisabledAlpha, 0.5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowPadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowRounding, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowMinSize, 32, 32);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowTitleAlign, 0.5, 0.5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ChildRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ChildBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_PopupRounding, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_PopupBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 4, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_IndentSpacing, 16);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_CellPadding, 4, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ScrollbarSize, 14);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ScrollbarRounding, 9);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_GrabMinSize, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_GrabRounding, 5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_TabRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_TabBorderSize, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_TabBarBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_TableAngledHeadersAngle, 0.610865);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_TableAngledHeadersTextAlign, 0.5, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ButtonTextAlign, 0.5, 0.5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_SelectableTextAlign, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_SeparatorTextBorderSize, 3);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_SeparatorTextAlign, 0, 0.5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_SeparatorTextPadding, 20, 3);
}

void PopReaSonusStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 32);
}

#endif
