#ifndef CSURF_FP_UI_PLUGIN_SELECTABLE_H_
#define CSURF_FP_UI_PLUGIN_SELECTABLE_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../utils/csurf_ui_text_overflow.hpp"
#include "../../shared/csurf_utils.hpp"
#include "../csurf_ui_assets.hpp"
#include "csurf_ui_plugin_type_button.hpp"

static void ReaSonusPluginSelectable(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets const *assets,
    std::string plugin_name,
    const std::string &plugin_type,
    const int plugin_index,
    int *selected_plugin,
    const std::function<void(const int index)> &delete_callback
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
    const bool selected = plugin_index == *selected_plugin;
    const bool mouse_over = between(static_cast<int>(pos_screen_x), width, static_cast<int>(mouse_pos_x))
                            && between(static_cast<int>(pos_screen_y), selectable_height,
                                       static_cast<int>(mouse_pos_y));

    /**
     * Definings some colors
     */
    int selectable_bg = mouse_over ? UI_COLORS::Accent_14 : UI_COLORS::Transparent;
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
            ("plugin_selector" + plugin_name + plugin_type).c_str(),
            0.0,
            selectable_height,
            ImGui::ChildFlags_FrameStyle
        )
    ) {
        ReaSonusPluginTypeButton(
            m_ctx,
            assets,
            pos_screen_x + 5,
            pos_screen_y + 4,
            plugin_type
        );

        ImGui::PushFont(m_ctx, assets->GetMainFont(), 13);
        ImGui::DrawList_AddText(
            list,
            pos_screen_x + 28,
            pos_screen_y + 6,
            UI_COLORS::White,
            getTextOverflow(m_ctx, plugin_name, width - (mouse_over ? 63 : 40)).c_str()
        );
        ImGui::PopFont(m_ctx);

        if (mouse_over) {
            ImGui::SetCursorPos(m_ctx, width - 21, 5);

            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 1.0, 1.0);
            ImGui::PushFont(m_ctx, assets->GetIconFont(), 16);
            ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Main_18);

            if (ImGui::Button(m_ctx, std::string(1, IconRemove).c_str())) {
                delete_callback(plugin_index);
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

    if (ImGui::IsItemClicked(m_ctx)) {
        *selected_plugin = plugin_index;
    }
}

#endif
