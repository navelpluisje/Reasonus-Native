#ifndef CSURF_FP_UI_PAGINATION_BUTTON_H_
#define CSURF_FP_UI_PAGINATION_BUTTON_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"
#include <reaper_plugin_functions.h>
#include "../shared/csurf_utils.hpp"
#include "csurf_ui_colors.hpp"

static void ReaSonusPaginationButton(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    std::string action_label,
    int value,
    bool alert,
    bool selected,
    std::function<void(int index)> on_click = nullptr)
{
    int menu_button_width = 30;
    int menu_button_height = 30;

    int background_color = selected ? UI_COLORS::Accent_25 : UI_COLORS::Main_18;
    int border_color = selected ? UI_COLORS::Accent : UI_COLORS::Main_38;
    int border_width = 1;
    double x_pos_1, y_pos_1, x_mouse_pos, y_mouse_pos;

    if (ImGui::BeginChild(m_ctx, ("##" + action_label).c_str(), menu_button_width, menu_button_height, ImGui::ChildFlags_FrameStyle))
    {
        ImGui::GetMousePos(m_ctx, &x_mouse_pos, &y_mouse_pos);
        ImGui::GetItemRectMin(m_ctx, &x_pos_1, &y_pos_1);

        bool hovered = between((int)x_pos_1, menu_button_width, (int)x_mouse_pos) && between((int)y_pos_1, menu_button_height, (int)y_mouse_pos);

        if (hovered)
        {
            ImGui::SetMouseCursor(m_ctx, ImGui::MouseCursor_Hand);
            background_color = selected ? UI_COLORS::Accent_50 : UI_COLORS::Main_38;
        }

        /**
         * Extra check is needed for 'soft-click' of touch pads. They do not trigger IsItemActive
         */
        if (ImGui::IsItemActive(m_ctx) || (hovered && ImGui::IsMouseClicked(m_ctx, 0)))
        {
            background_color = selected ? UI_COLORS::Accent_50 : UI_COLORS::Main_38;
            border_width = 3;
            on_click(value);
        }

        ImGui_DrawList *list = ImGui::GetForegroundDrawList(m_ctx);

        ImGui::DrawList_AddRectFilled(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height, background_color, 4);
        ImGui::DrawList_AddRect(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height, border_color, 4, ImGui::DrawFlags_None, border_width);
        if (alert)
        {
            ImGui::DrawList_AddCircleFilled(list, x_pos_1 + 27, y_pos_1 + 2, 5, border_color);
            ImGui::DrawList_AddCircleFilled(list, x_pos_1 + 27, y_pos_1 + 2, 4, UI_COLORS::Main_18);
            ImGui::DrawList_AddCircleFilled(list, x_pos_1 + 27, y_pos_1 + 2, 4, UI_COLORS::Success_50);
        }
        ImGui::PushFont(m_ctx, assets->GetMainFontBold(), 13);
        double text_height, text_width;
        ImGui::CalcTextSize(m_ctx, action_label.c_str(), &text_width, &text_height);
        ImGui::DrawList_AddText(list, x_pos_1 + (menu_button_width - text_width) / 2, y_pos_1 + (menu_button_height - text_height) / 2, UI_COLORS::White, action_label.c_str());
        ImGui::PopFont(m_ctx);

        ImGui::EndChild(m_ctx);
    }
}

#endif