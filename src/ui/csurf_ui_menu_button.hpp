#ifndef CSURF_FP_UI_MENU_BUTTON_H_
#define CSURF_FP_UI_MENU_BUTTON_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"
#include <reaper_plugin_functions.h>
#include "../shared/csurf_utils.hpp"

double between(int min, int val, int max)
{
    double diff = max - min;
    return diff > 0 && diff < val;
}

void ReaSonusMenuButton(
    ImGui_Context *m_ctx,
    std::string action_label,
    ImGui_Font *font,
    ImGui_Image *icon,
    int value,
    int *active_item)
{
    int menu_button_width = 200;
    int menu_button_height = 42;

    int background_color = 0x4f4f4fff;
    int border_width = value == *active_item ? 2 : 1;
    double x_pos_1, y_pos_1, x_mouse_pos, y_mouse_pos;

    if (ImGui::BeginChild(m_ctx, ("##" + action_label).c_str(), menu_button_width, menu_button_height, ImGui::ChildFlags_FrameStyle))
    {
        ImGui::GetMousePos(m_ctx, &x_mouse_pos, &y_mouse_pos);
        ImGui::GetItemRectMin(m_ctx, &x_pos_1, &y_pos_1);

        bool hovered = between(x_pos_1, menu_button_width, x_mouse_pos) && between(y_pos_1, menu_button_height, y_mouse_pos);

        if (hovered)
        {
            ImGui::SetMouseCursor(m_ctx, ImGui::MouseCursor_Hand);
            background_color = 0x5a5a5aff;
        }

        /**
         * Extra chck is needed for 'soft-click' of touch pads. They do not trigger IsItemActive
         */
        if (ImGui::IsItemActive(m_ctx) || (hovered && ImGui::IsMouseClicked(m_ctx, 0)))
        {
            background_color = 0x5a5a5aff;
            border_width = 3;
            *active_item = value;
        }

        ImGui_DrawList *list = ImGui::GetForegroundDrawList(m_ctx);

        ImGui::DrawList_AddRectFilled(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height, background_color, 4);
        ImGui::DrawList_AddRect(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height, 0xF7CB15FF, 4, ImGui::DrawFlags_None, border_width);
        ImGui::DrawList_AddImage(list, icon, x_pos_1 + 9, y_pos_1 + 9, x_pos_1 + 33, y_pos_1 + 33);

        ImGui::PushFont(m_ctx, font);
        double font_height = ImGui::GetTextLineHeight(m_ctx);
        ImGui::DrawList_AddText(list, x_pos_1 + 42, y_pos_1 + (menu_button_height - font_height) / 2, 0xffffffff, action_label.c_str());
        ImGui::PopFont(m_ctx);

        ImGui::EndChild(m_ctx);
    }
}

#endif