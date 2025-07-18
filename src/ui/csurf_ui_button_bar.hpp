#ifndef CSURF_FP_UI_BUTTON_BAR_H_
#define CSURF_FP_UI_BUTTON_BAR_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"

void ReaSonusButtonBar(
    ImGui_Context *m_ctx,
    std::string action_label,
    ImGui_Font *font,
    bool *action_value,
    bool has_cancel,
    bool *cancel_value,
    std::string cancel_label = "")
{
    double x_pos_1, y_pos_1, x_pos_2, y_pos_2;
    int button_padding_x = 16;

    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    if (ImGui::BeginChild(m_ctx, "button-bar", 0.0, 30.0, ImGui::ChildFlags_FrameStyle))
    {
        ImGui::CalcTextSize(m_ctx, action_label.c_str(), &x_pos_1, &y_pos_1);
        float buttonWidth1 = x_pos_1 + button_padding_x * 2.f;
        ImGui::CalcTextSize(m_ctx, cancel_label.c_str(), &x_pos_1, &y_pos_1);
        float buttonWidth2 = x_pos_1 + button_padding_x * 2.f;
        ImGui::GetStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, &x_pos_2, &y_pos_2);
        float widthNeeded = buttonWidth1 + x_pos_2 + buttonWidth2;
        ImGui::GetContentRegionAvail(m_ctx, &x_pos_2, &y_pos_2);

        ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + x_pos_2 - widthNeeded - 4);
        if (has_cancel)
        {
            UiElements::PushReaSonusButtonOutlineStyle(m_ctx, font);
            if (ImGui::Button(m_ctx, cancel_label.c_str()))
            {
                *cancel_value = true;
            }
            UiElements::PopReaSonusButtonOutlineStyle(m_ctx);
            ImGui::SameLine(m_ctx);
        }
        UiElements::PushReaSonusButtonStyle(m_ctx, font);
        if (ImGui::Button(m_ctx, action_label.c_str()))
        {
            *action_value = true;
        }
        UiElements::PopReaSonusButtonStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
    ImGui::PopStyleVar(m_ctx);
}

#endif