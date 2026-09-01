#ifndef CSURF_FP_UI_FILTER_SELECTABLE_H_
#define CSURF_FP_UI_FILTER_SELECTABLE_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../utils/csurf_ui_text_overflow.hpp"
#include "../../shared/csurf_utils.hpp"
#include "../csurf_ui_assets.hpp"
#include "../csurf_ui_colors.hpp"

static void ReaSonusFilterSelectable(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets const *assets,
    std::string filter_name,
    const int filter_color,
    const int filter_index,
    int *selected_filter,
    int *hovered_filter,
    int *active_item,
    const std::function<void(int index)> &delete_callback,
    const std::function<void(int index)> &select_callbask
) {
    /**
     * Handle positioning and sizing
     */
    constexpr double selectable_height = 28.0;
    double width;
    double height;
    double pos_screen_x;
    double pos_screen_y;
    double pos_cursor_x;
    double pos_cursor_y;
    double mouse_pos_x;
    double mouse_pos_y;

    ImGui::GetCursorScreenPos(m_ctx, &pos_screen_x, &pos_screen_y);
    ImGui::GetCursorScreenPos(m_ctx, &pos_cursor_x, &pos_cursor_y);
    ImGui::GetMousePos(m_ctx, &mouse_pos_x, &mouse_pos_y);
    ImGui::GetContentRegionAvail(m_ctx, &width, &height);

    /**
     * Setting some flags here
     */
    const bool popup_open = ImGui::IsPopupOpen(m_ctx, "t",
                                               ImGui::PopupFlags_AnyPopupId | ImGui::PopupFlags_AnyPopupLevel);
    const bool selected = filter_index == *selected_filter;
    const bool mouse_over = (!popup_open
                             && between(pos_screen_x, width, mouse_pos_x)
                             && between(pos_screen_y, selectable_height, mouse_pos_y))
                            || (popup_open && *hovered_filter == filter_index);

    /**
     * Definings some colors§
     */
    int selectable_bg = mouse_over ? UI_COLORS::Accent_14 : UI_COLORS::Transparent;
    if (mouse_over) {
        *hovered_filter = filter_index;
    }

    if (selected) {
        selectable_bg = UI_COLORS::Accent_14;
    } else if (mouse_over && ImGui::IsMouseClicked(m_ctx, ImGui::MouseButton_Left)) {
        selectable_bg = UI_COLORS::Accent_50;
    }

    ImGui_DrawList *list = ImGui::GetWindowDrawList(m_ctx);

    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, selectable_bg);
    if (ImGui::BeginChild(
            m_ctx,
            ("plugin_selector" + filter_name + std::to_string(filter_color)).c_str(),
            0.0,
            selectable_height,
            ImGui::ChildFlags_FrameStyle
        )
    ) {
        // This Invisible Button is needed to generate a proper active state.
        // The child element does not have an active state
        // When hovered and having the remove option, reduce the witdth as the invisible button is on top of the remove button
        ImGui::InvisibleButton(
            m_ctx,
            ("invisible-button" + filter_name).c_str(),
            width - (mouse_over && delete_callback != nullptr ? 40 : 0),
            selectable_height
        );

        if (ImGui::IsItemActive(m_ctx)) {
            if (active_item != nullptr) {
                *active_item = filter_index;
            }
        } else {
            if (active_item != nullptr) {
                *active_item = -1;
            }
        }

        if (ImGui::IsItemClicked(m_ctx, ImGui::MouseButton_Left)) {
            *selected_filter = filter_index;

            if (select_callbask != nullptr) {
                select_callbask(filter_index);
            }
        }

        ImGui::DrawList_AddRectFilled(
            list,
            pos_screen_x + 4,
            pos_screen_y + 6,
            pos_screen_x + 20,
            pos_screen_y + 22,
            filter_color,
            4
        );

        ImGui::PushFont(m_ctx, assets->GetMainFont(), 13);
        ImGui::DrawList_AddText(
            list,
            pos_screen_x + 28,
            pos_screen_y + 6,
            UI_COLORS::White,
            getTextOverflow(m_ctx, filter_name, width - (mouse_over ? 66 : 40)).c_str()
        );
        ImGui::PopFont(m_ctx);

        if (mouse_over) {
            ImGui::SetCursorPos(m_ctx, width - 21, 5);

            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 1.0, 1.0);
            ImGui::PushFont(m_ctx, assets->GetIconFont(), 16);
            ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Main_18);

            if (ImGui::Button(m_ctx, std::string(1, IconRemove).c_str())) {
                delete_callback(filter_index);
            }
            ImGui::PopFont(m_ctx);
            ImGui::PopStyleVar(m_ctx);
            ImGui::PopStyleColor(m_ctx);
        }

        ImGui::EndChild(m_ctx);
    }

    ImGui::PopStyleVar(m_ctx, 2);
    ImGui::PopStyleColor(m_ctx, 1);
}

#endif
