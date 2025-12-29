#ifndef CSURF_FP_UI_PAGINATION_DIRECTION_BUTTON_H_
#define CSURF_FP_UI_PAGINATION_DIRECTION_BUTTON_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"
#include <reaper_plugin_functions.h>
#include "../shared/csurf_utils.hpp"
#include "csurf_ui_colors.hpp"

static void ReaSonusPaginationIconButton(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    IconFont icon,
    std::string id,
    bool disabled = false,
    std::function<void()> on_click = nullptr)
{
    int menu_button_width = 30;
    int menu_button_height = 30;

    int background_color = UI_COLORS::Main_18;
    int border_color = UI_COLORS::Main_38;
    int border_width = 1;
    double x_pos_1, y_pos_1, x_mouse_pos, y_mouse_pos;

    ImGui::PushFont(m_ctx, assets->GetIconFont(), 20);
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
            if (!disabled)
            {
                on_click();
            }
        }

        ImGui_DrawList *list = ImGui::GetForegroundDrawList(m_ctx);

        ImGui::DrawList_AddRectFilled(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height, background_color, 4);
        ImGui::DrawList_AddRect(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height, border_color, 4, ImGui::DrawFlags_None, border_width);
        ImGui::DrawList_AddText(list, x_pos_1 + 5, y_pos_1 + 5, disabled ? UI_COLORS::White_25 : UI_COLORS::White, std::string(1, icon).c_str());
        ImGui::EndChild(m_ctx);
    }
    ImGui::PopFont(m_ctx);
}

static void ReaSonusPaginationIconButton(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    IconFont icon,
    std::string id,
    std::function<void()> on_click = nullptr)
{
    ReaSonusPaginationIconButton(m_ctx, assets, icon, id, false, on_click);
}

#endif