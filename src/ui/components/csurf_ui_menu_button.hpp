#ifndef CSURF_FP_UI_MENU_BUTTON_H_
#define CSURF_FP_UI_MENU_BUTTON_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../../shared/csurf_utils.hpp"
#include "../csurf_ui_assets.hpp"

static void ReaSonusMenuButton(
    ImGui_Context *m_ctx,
    const CSurf_UI_Assets *assets,
    const std::string &action_label,
    const IconFont icon,
    const int value,
    int *active_item) {
    int menu_button_width = 200;
    int menu_button_height = 42;

    int background_color = value == *active_item ? UI_COLORS::Accent_25 : UI_COLORS::Main_28;
    int border_width = value == *active_item ? 2 : 1;
    double x_pos_1;
    double y_pos_1;
    double x_mouse_pos;
    double y_mouse_pos;

    if (ImGui::BeginChild(m_ctx, ("##" + action_label).c_str(), menu_button_width, menu_button_height,
                          ImGui::ChildFlags_FrameStyle)) {
        ImGui::GetMousePos(m_ctx, &x_mouse_pos, &y_mouse_pos);
        ImGui::GetItemRectMin(m_ctx, &x_pos_1, &y_pos_1);

        const bool hovered = between(static_cast<int>(x_pos_1), menu_button_width, static_cast<int>(x_mouse_pos))
                             && between(static_cast<int>(y_pos_1), menu_button_height, static_cast<int>(y_mouse_pos));

        if (hovered) {
            ImGui::SetMouseCursor(m_ctx, ImGui::MouseCursor_Hand);
            background_color = value == *active_item ? UI_COLORS::Accent_50 : UI_COLORS::Main_38;
        }

        /**
         * Extra check is needed for 'soft-click' of touch pads. They do not trigger IsItemActive
         */
        if (ImGui::IsItemActive(m_ctx) || (hovered && ImGui::IsMouseClicked(m_ctx, 0))) {
            background_color = value == *active_item ? UI_COLORS::Accent_50 : UI_COLORS::Main_38;
            border_width = 3;
            *active_item = value;
        }

        ImGui_DrawList *list = ImGui::GetWindowDrawList(m_ctx);

        ImGui::DrawList_AddRectFilled(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height,
                                      background_color, 4);
        ImGui::DrawList_AddRect(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height,
                                UI_COLORS::Accent, 4, ImGui::DrawFlags_None, border_width);
        ImGui::PushFont(m_ctx, assets->GetIconFont(), 24);
        ImGui::DrawList_AddText(list, x_pos_1 + 9, y_pos_1 + 9, UI_COLORS::White, std::string(1, icon).c_str());
        ImGui::PopFont(m_ctx);

        ImGui::PushFont(m_ctx, assets->GetMainFontBold(), FontSizeLarge);
        const double font_height = ImGui::GetTextLineHeight(m_ctx);
        ImGui::DrawList_AddText(list, x_pos_1 + 42, y_pos_1 + (menu_button_height - font_height) / 2, UI_COLORS::White,
                                action_label.c_str());
        ImGui::PopFont(m_ctx);

        ImGui::EndChild(m_ctx);
    }
}

#endif
