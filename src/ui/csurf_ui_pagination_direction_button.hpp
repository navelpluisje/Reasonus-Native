#ifndef CSURF_FP_UI_PAGINATION_DIRECTION_BUTTON_H_
#define CSURF_FP_UI_PAGINATION_DIRECTION_BUTTON_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"
#include <reaper_plugin_functions.h>
#include "../shared/csurf_utils.hpp"
#include "csurf_ui_colors.hpp"

enum PaginationDirection
{
    PaginationPrevious,
    PaginationNext,
};

static void
ReaSonusPaginationDirectionButton(
    ImGui_Context *m_ctx,
    PaginationDirection direction,
    std::string id,
    std::function<void()> on_click = nullptr)
{
    int menu_button_width = 30;
    int menu_button_height = 30;

    int background_color = UI_COLORS::Main_18;
    int border_color = UI_COLORS::Main_38;
    int border_width = 1;
    double x_pos_1, y_pos_1, x_mouse_pos, y_mouse_pos;

    if (ImGui::BeginChild(m_ctx, ("##" + id).c_str(), menu_button_width, menu_button_height, ImGui::ChildFlags_FrameStyle))
    {
        ImGui::GetMousePos(m_ctx, &x_mouse_pos, &y_mouse_pos);
        ImGui::GetItemRectMin(m_ctx, &x_pos_1, &y_pos_1);

        bool hovered = between((int)x_pos_1, menu_button_width, (int)x_mouse_pos) && between((int)y_pos_1, menu_button_height, (int)y_mouse_pos);

        if (hovered)
        {
            ImGui::SetMouseCursor(m_ctx, ImGui::MouseCursor_Hand);
            background_color = UI_COLORS::Main_38;
        }

        /**
         * Extra check is needed for 'soft-click' of touch pads. They do not trigger IsItemActive
         */
        if ((hovered && ImGui::IsMouseReleased(m_ctx, 0)))
        {
            background_color = UI_COLORS::Main_38;
            border_width = 2;
            on_click();
        }

        ImGui_DrawList *list = ImGui::GetForegroundDrawList(m_ctx);

        ImGui::DrawList_AddRectFilled(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height, background_color, 4);
        ImGui::DrawList_AddRect(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height, border_color, 4, ImGui::DrawFlags_None, border_width);

        if (direction == PaginationPrevious)
        {
            ImGui::DrawList_AddTriangleFilled(list, x_pos_1 + 10, y_pos_1 + menu_button_height / 2, x_pos_1 + menu_button_width - 10, y_pos_1 + 10, x_pos_1 + menu_button_width - 10, y_pos_1 + menu_button_height - 10, UI_COLORS::White);
        }
        if (direction == PaginationNext)
        {
            ImGui::DrawList_AddTriangleFilled(list, x_pos_1 + menu_button_width - 10, y_pos_1 + menu_button_height / 2, x_pos_1 + 10, y_pos_1 + 10, x_pos_1 + 10, y_pos_1 + menu_button_height - 10, UI_COLORS::White);
        }
        ImGui::EndChild(m_ctx);
    }
}

#endif