#ifndef CSURF_FP_UI_MODAL_H_
#define CSURF_FP_UI_MODAL_H_

#include <functional>
#include <reaper_imgui_functions.h>
#include <string>

#include "csurf_ui_filter_selectable.hpp"

enum ModalTypes {
    INFO_MODAL,
    WARNING_MODAL,
    ERROR_MODAL,
};

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
class ReaSonusModal {
    ImGui_Context *m_ctx;
    const CSurf_UI_Assets *assets;
    std::string title;
    std::string message;
    bool has_action;
    std::string action_button;
    std::string cancel_button;
    ModalTypes type;
    std::function<void(int index)> action_callback;

    bool show_modal;

public:
    ReaSonusModal(
        ImGui_Context *m_ctx,
        const CSurf_UI_Assets *assets
    ) : m_ctx(m_ctx), assets(assets) {
    }

    ~ReaSonusModal() = default;

    void ShowModal(
        std::string _title,
        std::string _message,
        std::string _action_button,
        std::string _cancel_button,
        ModalTypes _type,
        const std::function<void(int index)> &_action_callback

    ) {
        title = _title;
        message = _message;
        has_action = _action_button.empty() && _action_callback != nullptr;
        action_button = _action_button;
        cancel_button = _cancel_button.empty() ? "Cancel" : _cancel_button;
        type = _type;
        action_callback = _action_callback;

        show_modal = true;
    }

    void Render() {
        if (ImGui::BeginPopupModal(m_ctx, "ReaSomus Message", nullptr,
                                   ImGui::WindowFlags_AlwaysAutoResize | ImGui::WindowFlags_TopMost)) {
            ImGui::Text(m_ctx, message.c_str());
            ImGui::Separator(m_ctx);

            if (has_action) {
                if (ImGui::Button(m_ctx, "OK", 120, 0)) {
                    if (action_callback != nullptr) {
                        action_callback(0);
                    }
                    ImGui::CloseCurrentPopup(m_ctx);
                }
            }

            ImGui::SetItemDefaultFocus(m_ctx);
            ImGui::SameLine(m_ctx);

            if (ImGui::Button(m_ctx, "Cancel", 120, 0)) {
                ImGui::CloseCurrentPopup(m_ctx);
            }
            ImGui::EndPopup(m_ctx);
        }

        if (show_modal) {
            ImGui::OpenPopup(m_ctx, "ReaSomus Message");
            show_modal = false;
        }
    }
};

#endif
