#ifndef CSURF_FP_UI_BUTTON_BAR_H_
#define CSURF_FP_UI_BUTTON_BAR_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../csurf_ui_styles_elements.hpp"

static void ReaSonusButtonBar(
    ImGui_Context *m_ctx,
    const CSurf_UI_Assets *assets,
    const std::string &action_label,
    bool *action_value,
    const bool has_cancel,
    bool *cancel_value,
    const std::string &cancel_label,
    const std::string *message) {
    double x_pos_1;
    double y_pos_1;
    double x_pos_2;
    double y_pos_2;
    constexpr float button_padding_x = 16;

    UiStyledElements::PushReaSonusButtonBarContainerStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, "button-bar", 0.0, 36.0, ImGui::ChildFlags_FrameStyle)) {
        if (!message->empty()) {
            UiStyledElements::PushReaSonusButtonBarMessageStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "message-box", 0, 0,
                                  ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeX |
                                  ImGui::WindowFlags_NoScrollbar | ImGui::ChildFlags_AutoResizeY)) {
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 2);
                ImGui::Text(m_ctx, message->c_str());

                UiStyledElements::PopReaSonusButtonBarMessageStyle(m_ctx);
                ImGui::EndChild(m_ctx);
            }
            ImGui::SameLine(m_ctx);
        }

        ImGui::PushFont(m_ctx, assets->GetMainFontBold(), FontSizeDefault);
        ImGui::CalcTextSize(m_ctx, action_label.c_str(), &x_pos_1, &y_pos_1);

        const float cancel_button_width = static_cast<float>(x_pos_1) + button_padding_x * 2.F;
        ImGui::CalcTextSize(m_ctx, cancel_label.c_str(), &x_pos_1, &y_pos_1);

        const float save_button_width = static_cast<float>(x_pos_1) + button_padding_x * 2.F;
        ImGui::GetStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, &x_pos_2, &y_pos_2);

        const float widthNeeded = has_cancel ? cancel_button_width + x_pos_2 + save_button_width : cancel_button_width;
        ImGui::GetContentRegionAvail(m_ctx, &x_pos_2, &y_pos_2);
        ImGui::PopFont(m_ctx);

        ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + x_pos_2 - widthNeeded - 4);
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 1);

        if (has_cancel) {
            UiStyledElements::PushReaSonusButtonOutlineStyle(m_ctx, assets->GetMainFontBold());
            if (ImGui::Button(m_ctx, cancel_label.c_str())) {
                *cancel_value = true;
            }
            UiStyledElements::PopReaSonusButtonOutlineStyle(m_ctx);
            ImGui::SameLine(m_ctx);
        }
        UiStyledElements::PushReaSonusButtonStyle(m_ctx, assets->GetMainFontBold());
        if (ImGui::Button(m_ctx, action_label.c_str())) {
            *action_value = true;
        }
        UiStyledElements::PopReaSonusButtonStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
    UiStyledElements::PopReaSonusButtonBarContainerStyle(m_ctx);
}

#endif
