#ifndef CSURF_FP_UI_LIST_SELECTABLE_H_
#define CSURF_FP_UI_LIST_SELECTABLE_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../utils/csurf_ui_text_overflow.hpp"
#include "../csurf_ui_assets.hpp"
#include "../csurf_ui_colors.hpp"
#include "../csurf_ui_styles_elements.hpp"
#include "../../shared/csurf_utils.hpp"

static void ReaSonusListSelectable(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets const *assets,
    std::string label,
    const int item_index,
    int *selected_item,
    int *hovered_item,
    int *active_item,
    std::string *open_context_id,
    const std::function<void(ImGui_Context *m_ctx)> &select_callbask,
    const std::function<void(int index)> &render_context_menu,
    const std::function<void(int index)> &delete_callback
) {
    /**
     * Handle positioning and sizing
     */
    constexpr double selectable_height = 28.0;
    bool popup_open = false;
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

    const std::string context_menu_id = "list-selectable-context" + std::to_string(item_index);

    /**
     * Setting some flags here
     */
    const bool has_delete = delete_callback != nullptr;
    if (open_context_id != nullptr && ImGui::IsPopupOpen(m_ctx, std::move(open_context_id)->c_str())) {
        popup_open = true;
    } else if (open_context_id != nullptr) {
        popup_open = false;
        *open_context_id = "";
    }

    const bool selected = item_index == *selected_item;
    const bool mouse_over = (!popup_open
                             && between(static_cast<int>(pos_screen_x), width, static_cast<int>(mouse_pos_x))
                             && between(static_cast<int>(pos_screen_y), selectable_height,
                                        static_cast<int>(mouse_pos_y))) || (
                                popup_open && *hovered_item == item_index);
    const bool context_over = between(static_cast<int>(pos_screen_x), width, static_cast<int>(mouse_pos_x))
                              && between(static_cast<int>(pos_screen_y), selectable_height,
                                         static_cast<int>(mouse_pos_y));

    /**
     * Definings some colors§
     */
    int selectable_bg = mouse_over ? UI_COLORS::Accent_25 : UI_COLORS::Transparent;
    if (mouse_over) {
        *hovered_item = item_index;
    }

    if (context_over && ImGui::IsMouseClicked(m_ctx, ImGui::MouseButton_Right)) {
        if (render_context_menu != nullptr) {
            ImGui::OpenPopup(m_ctx, context_menu_id.c_str());
            *open_context_id = context_menu_id;
        }
    }

    if (selected) {
        selectable_bg = UI_COLORS::Accent_25;
    } else if (mouse_over && ImGui::IsMouseClicked(m_ctx, ImGui::MouseButton_Left)) {
        selectable_bg = UI_COLORS::Accent_50;
    }

    UiStyledElements::PushReaSonusContextMenuStyle(m_ctx);
    if (ImGui::BeginPopupContextWindow(m_ctx, context_menu_id.c_str())) {
        render_context_menu(item_index);
        ImGui::EndPopup(m_ctx);
    }
    UiStyledElements::PopReaSonusContextMenuStyle(m_ctx);

    ImGui_DrawList *list = ImGui::GetWindowDrawList(m_ctx);

    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, selectable_bg);
    if (ImGui::BeginChild(
            m_ctx,
            ("item" + std::to_string(item_index)).c_str(),
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
            "invisible-button",
            width - (mouse_over && delete_callback != nullptr ? 40 : 0),
            selectable_height
        );
        if (ImGui::IsItemActive(m_ctx)) {
            if (active_item != nullptr) {
                *active_item = item_index;
            }
        } else {
            if (active_item != nullptr) {
                *active_item = -1;
            }
        }
        if (ImGui::IsItemClicked(m_ctx, ImGui::MouseButton_Left)) {
            *selected_item = item_index;

            if (select_callbask != nullptr) {
                select_callbask(m_ctx);
            }
        }

        ImGui::PushFont(m_ctx, assets->GetMainFont(), 13);
        ImGui::DrawList_AddText(
            list,
            pos_screen_x + 4,
            pos_screen_y + 6,
            UI_COLORS::White,
            getTextOverflow(m_ctx, label, width - (mouse_over && has_delete ? 40 : 0)).c_str()
        );
        ImGui::PopFont(m_ctx);

        if (mouse_over && has_delete) {
            ImGui::SetCursorPos(m_ctx, width - 21, 5);

            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 1.0, 1.0);
            ImGui::PushFont(m_ctx, assets->GetIconFont(), 16);
            ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Main_18);

            if (ImGui::Button(m_ctx, std::string(1, IconRemove).c_str())) {
                delete_callback(item_index);
            }
            ImGui::PopFont(m_ctx);
            ImGui::PopStyleVar(m_ctx);
            ImGui::PopStyleColor(m_ctx);
        }

        ImGui::SetMouseCursor(m_ctx, ImGui::MouseCursor_Hand);

        ImGui::EndChild(m_ctx);
    }

    ImGui::PopStyleVar(m_ctx, 2);
    ImGui::PopStyleColor(m_ctx, 1);
}

#endif
