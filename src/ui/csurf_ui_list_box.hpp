#ifndef CSURF_FP_UI_EXTENDED_LIST_BOX_H_
#define CSURF_FP_UI_EXTENDED_LIST_BOX_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include <functional>
#include "csurf_ui_elements.hpp"
#include "csurf_ui_text_input.hpp"

/**
 * @brief Create a listbox with
 *
 * @param m_ctx The famous ReaImgui context
 * @param id Id for the leement to create
 * @param items The items of the list
 * @param selected_item The index of the current selected item
 * @param can_deselect Is the selected item deselectale
 * @param delete_icon Icon for the delete button on the selected item
 * @param remove_callback Callback fired when clicking the delete button
 * @param sortable Can de use drag and drop to sort the list
 * @param sort_callback The callback when the order is changed
 */
static void ReaSonusExtendedListBox(
    ImGui_Context *m_ctx,
    std::string id,
    std::vector<std::string> items,
    int *selected_item,
    bool can_deselect,
    ImGui_Image *delete_icon,
    std::function<void(int index)> remove_callback,
    bool sortable = false,
    std::function<void(int current_item, int next_item)> sort_callback = nullptr)
{
    double space_x, space_y;

    UiElements::PushReaSonusListBoxStyle(m_ctx);
    if (ImGui::BeginListBox(m_ctx, ("##" + id).c_str(), 0.0, -0.1))
    {
        for (int i = 0; i < (int)items.size(); i++)
        {
            bool selected = *selected_item == i;
            if (ImGui::Selectable(m_ctx, items[i].c_str(), &selected, ImGui::SelectableFlags_AllowOverlap))
            {
                *selected_item = !can_deselect           ? i
                                 : (*selected_item == i) ? -1
                                                         : i;
            }

            /**
             * Implement dragand drop for the list items
             */
            if (sortable && ImGui::IsItemActive(m_ctx) && !ImGui::IsAnyItemHovered(m_ctx))
            {
                ImGui::GetMouseDragDelta(m_ctx, &space_x, &space_y, ImGui::MouseButton_Left);
                int next_index = i + (space_y < 0 ? -1 : 1);
                if (next_index >= 0 && next_index < (int)items.size())
                {
                    sort_callback(i, next_index);
                }
            }

            if (selected)
            {
                ImGui::SameLine(m_ctx);
                ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + space_x - 18);
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 2);

                ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 4.0, 4.0);
                if (ImGui::ImageButton(m_ctx, std::string("Remove-filter##" + std::to_string(i)).c_str(), delete_icon, 12, 12))
                {
                    remove_callback(i);
                }
                ImGui::PopStyleVar(m_ctx);
            }
        }
        UiElements::PopReaSonusListBoxStyle(m_ctx);
        ImGui::EndListBox(m_ctx);
    }
}

#endif
