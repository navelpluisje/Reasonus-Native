#ifndef CSURF_FP_UI_PAGINATION_BUTTON_H_
#define CSURF_FP_UI_PAGINATION_BUTTON_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <functional>
#include "../../shared/csurf_utils.hpp"
#include "../csurf_ui_colors.hpp"

static void ReaSonusPaginationButton( // NOLINT(*-function-cognitive-complexity)
    ImGui_Context *m_ctx,
    const CSurf_UI_Assets *assets,
    const std::string &action_label,
    const int value,
    const bool alert,
    const bool selected,
    const std::function<void(int index)> &on_click,
    const std::function<void(int from_index, int to_index)> &drop_callback) {
    char data[256]; // NOLINT(*-avoid-c-arrays)

    constexpr int menu_button_width = 30;
    constexpr int menu_button_height = 30;

    int background_color = selected ? UI_COLORS::Accent_25 : UI_COLORS::Main_18;
    const int border_color = selected ? UI_COLORS::Accent : UI_COLORS::Main_38;
    int border_width = 1;
    double x_pos_1;
    double y_pos_1;
    double y_pos_2;
    double x_mouse_pos;
    double y_mouse_pos;
    double text_height;
    double text_width;

    if (ImGui::BeginChild(m_ctx, ("##" + action_label).c_str(), menu_button_width, menu_button_height,
                          ImGui::ChildFlags_FrameStyle)) {
        ImGui::GetMousePos(m_ctx, &x_mouse_pos, &y_mouse_pos);
        ImGui::GetItemRectMin(m_ctx, &x_pos_1, &y_pos_1);

        const bool hovered = between(static_cast<int>(x_pos_1), menu_button_width, static_cast<int>(x_mouse_pos))
                             && between(static_cast<int>(y_pos_1), menu_button_height, static_cast<int>(y_mouse_pos));

        if (hovered) {
            ImGui::SetMouseCursor(m_ctx, ImGui::MouseCursor_Hand);
            background_color = selected ? UI_COLORS::Accent_50 : UI_COLORS::Main_38;
        }

        /**
         * Extra check is needed for 'soft-click' of touch pads. They do not trigger IsItemActive
         */
        if (ImGui::IsItemActive(m_ctx) || (hovered && ImGui::IsMouseClicked(m_ctx, 0))) {
            background_color = selected ? UI_COLORS::Accent_50 : UI_COLORS::Main_38;
            border_width = 3;
            on_click(value);
        }

        ImGui_DrawList *list = ImGui::GetForegroundDrawList(m_ctx);

        ImGui::DrawList_AddRectFilled(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height,
                                      background_color, 4);
        ImGui::DrawList_AddRect(list, x_pos_1, y_pos_1, x_pos_1 + menu_button_width, y_pos_1 + menu_button_height,
                                border_color, 4, ImGui::DrawFlags_None, border_width);
        if (alert) {
            ImGui::DrawList_AddCircleFilled(list, x_pos_1 + 27, y_pos_1 + 2, 5, border_color);
            ImGui::DrawList_AddCircleFilled(list, x_pos_1 + 27, y_pos_1 + 2, 4, UI_COLORS::Main_18);
            ImGui::DrawList_AddCircleFilled(list, x_pos_1 + 27, y_pos_1 + 2, 4, UI_COLORS::Success_50);
        }
        ImGui::PushFont(m_ctx, assets->GetMainFontBold(), FontSizeDefault);
        ImGui::CalcTextSize(m_ctx, action_label.c_str(), &text_width, &text_height);
        ImGui::DrawList_AddText(list, x_pos_1 + (menu_button_width - text_width) / 2,
                                y_pos_1 + (menu_button_height - text_height) / 2, UI_COLORS::White,
                                action_label.c_str());
        ImGui::PopFont(m_ctx);

        /**
         * Instantiate the drag and drop source
         */
        if (ImGui::BeginDragDropSource(m_ctx)) {
            /**
             * Setting the data for the drag and drop
             */
            ImGui::SetDragDropPayload(m_ctx, "GROUP_DRAG", std::to_string(value).c_str());

            /**
             * Create the drag visuals
             */
            if (ImGui::BeginChild(m_ctx, ("##group-drag-" + action_label).c_str(), 0.0, 0.0,
                                  ImGui::ChildFlags_AutoResizeX | ImGui::ChildFlags_AutoResizeY)) {
                ImGui::GetItemRectMin(m_ctx, &x_pos_1, &y_pos_2);

                ImGui::DrawList_AddRectFilled(list, x_pos_1 - 20, y_pos_1 + 40, x_pos_1 + menu_button_width - 20,
                                              y_pos_1 + menu_button_height + 40, background_color, 4);
                ImGui::DrawList_AddRect(list, x_pos_1 - 20, y_pos_1 + 40, x_pos_1 + menu_button_width - 20,
                                        y_pos_1 + menu_button_height + 40, border_color, 4, ImGui::DrawFlags_None, 1);

                ImGui::PushFont(m_ctx, assets->GetMainFontBold(), FontSizeDefault);
                ImGui::CalcTextSize(m_ctx, action_label.c_str(), &text_width, &text_height);
                ImGui::DrawList_AddText(list, x_pos_1 - 20 + (menu_button_width - text_width) / 2,
                                        y_pos_1 + 40 + (menu_button_height - text_height) / 2, UI_COLORS::White,
                                        action_label.c_str());
                ImGui::PopFont(m_ctx);
                ImGui::EndChild(m_ctx);
            }

            ImGui::EndDragDropSource(m_ctx);
        }
        ImGui::EndChild(m_ctx);
    }
    /**
     * Instantiate the drag and drop target
     */
    if (ImGui::BeginDragDropTarget(m_ctx)) {
        /**
         * Read the drop data and trigger the call_back
         */
        if (ImGui::AcceptDragDropPayload(m_ctx, "GROUP_DRAG", data, sizeof(data))) {
            drop_callback(strtol(data, nullptr, 10), value);
        }
        ImGui::EndDragDropTarget(m_ctx);
    }
}

#endif
