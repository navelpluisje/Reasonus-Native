#ifndef CSURF_FP_UI_EXTENDED_LIST_BOX_H_
#define CSURF_FP_UI_EXTENDED_LIST_BOX_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include <functional>

#include "csurf_ui_list_selectable.hpp"
#include "../csurf_ui_styles_elements.hpp"

/**
 * @brief Create a listbox with
 *
 * @param m_ctx The famous ReaImgui context
 * @param assets
 * @param element_id Id for the leement to create
 * @param items The items of the list
 * @param selected_item The index of the current selected item
 * @param hovered_item The index of the current hovered item
 * @param active_item The currently active Item
 * @param remove_callback Callback fired when clicking the delete button
 * @param sortable Can de use drag and drop to sort the list
 * @param sort_callback The callback when the order is changed
 */
class ReaSonusExtendedListBox {
    ImGui_Context *m_ctx;
    const CSurf_UI_Assets *assets;
    const std::string element_id;
    const std::vector<std::string> &items;
    int *selected_item;
    const bool sortable;
    const std::function<void(int index)> remove_callback;
    const std::function<void(int current_item, int next_item)> sort_callback;

    int hovered_item;
    int active_item;

public:
    ReaSonusExtendedListBox(
        ImGui_Context *m_ctx,
        const CSurf_UI_Assets *assets,
        const std::string &element_id,
        const std::vector<std::string> &items,
        int *selected_item,
        const std::function<void(int index)> &remove_callback,
        const bool sortable,
        const std::function<void(int current_item, int next_item)> &sort_callback
    ) : m_ctx(m_ctx), assets(assets), element_id(element_id), items(items), selected_item(selected_item),
        sortable(sortable),
        remove_callback(remove_callback), sort_callback(sort_callback) {
        hovered_item = -1;
        active_item = -1;
    }

    ~ReaSonusExtendedListBox() = default;

    void HandleRemoveClick(const int index) const {
        if (remove_callback != nullptr) {
            remove_callback(index);
        }
    }

    void HandleItemSelect(const int index) const {
        if (remove_callback != nullptr) {
            remove_callback(index);
        }
    }

    void Render() {
        UiStyledElements::PushReaSonusListBoxStyle(m_ctx);
        if (ImGui::BeginListBox(m_ctx, ("##" + element_id).c_str(), 0.0, -0.1)) {
            if (items.size() > 0) {
                for (int i = 0; i < static_cast<int>(items.size()); i++) {
                    ReaSonusListSelectable(
                        m_ctx,
                        assets,
                        items[i],
                        i,
                        selected_item,
                        &hovered_item,
                        &active_item,
                        nullptr,
                        nullptr,
                        nullptr,
                        remove_callback
                    );

                    // When the active item is not the hovered item, we are dragging. Call the sort_callback in that case
                    if (sortable && active_item > -1 && active_item != hovered_item
                    ) {
                        sort_callback(active_item, hovered_item);
                    }
                }
            } else {
                ImGui::Text(m_ctx, "No items");
            }
            ImGui::EndListBox(m_ctx);
        }
        UiStyledElements::PopReaSonusListBoxStyle(m_ctx);
    }
};

#endif
