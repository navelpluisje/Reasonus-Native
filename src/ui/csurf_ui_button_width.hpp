#ifndef CSURF_FP_UI_BUTTON_WIDTH_H_
#define CSURF_FP_UI_BUTTON_WIDTH_H_

#include <reaper_imgui_functions.h>
#include <string>

static int getButtonWidth(
    ImGui_Context *m_ctx,
    std::string label,
    ImGui_Font *font)
{
    double x_pos_1, y_pos_1;
    int button_padding_x = 20;

    ImGui::PushFont(m_ctx, font, FontSizeDefault);
    ImGui::CalcTextSize(m_ctx, label.c_str(), &x_pos_1, &y_pos_1);
    int buttonWidth = (x_pos_1 + button_padding_x * 2.f) + 1;
    ImGui::PopFont(m_ctx);

    return buttonWidth;
}

#endif