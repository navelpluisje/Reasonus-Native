#ifndef CSURF_FP_UI_BUTTON_BAR_H_
#define CSURF_FP_UI_BUTTON_BAR_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"

static void ReaSonusButtonBar(
    ImGui_Context *m_ctx,
    std::string action_label,
    ImGui_Font *font,
    bool *action_value,
    bool has_cancel = false,
    bool *cancel_value = nullptr,
    std::string cancel_label = "")
{
    double x_pos_1, y_pos_1, x_pos_2, y_pos_2;
    int button_padding_x = 16;

    UiElements::PushReaSonusButtonBarContainerStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, "button-bar", 0.0, 34.0, ImGui::ChildFlags_FrameStyle))
    {
        ImGui::PushFont(m_ctx, font, 13);
        ImGui::CalcTextSize(m_ctx, action_label.c_str(), &x_pos_1, &y_pos_1);
        float buttonWidth1 = (int)x_pos_1 + button_padding_x * 2.f;
        ImGui::CalcTextSize(m_ctx, cancel_label.c_str(), &x_pos_1, &y_pos_1);
        float buttonWidth2 = (int)x_pos_1 + button_padding_x * 2.f;
        ImGui::GetStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, &x_pos_2, &y_pos_2);
        float widthNeeded = (int)buttonWidth1 + x_pos_2 + buttonWidth2;
        ImGui::GetContentRegionAvail(m_ctx, &x_pos_2, &y_pos_2);
        ImGui::PopFont(m_ctx);

        ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + x_pos_2 - widthNeeded - 4);
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 1);
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
    UiElements::PopReaSonusButtonBarContainerStyle(m_ctx);
}

#endif