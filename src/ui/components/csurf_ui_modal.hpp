#ifndef CSURF_FP_UI_MODAL_H_
#define CSURF_FP_UI_MODAL_H_

#include <functional>
#include <string>

#include <reaper_imgui_functions.h>
#include "csurf_ui_filter_selectable.hpp"
#include "csurf_ui_page_title.hpp"

enum ModalTypes {
    INFO_MODAL,
    WARNING_MODAL,
    ERROR_MODAL,
};

/**
 * @brief Create a modal with
 *
 * @param m_ctx The famous ReaImgui context
 * @param assets
 * @param title Id for the leement to create
 * @param message The message to tell the viewer
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
    std::string modal_title;
    std::string title;
    std::string message;
    bool has_action{false};
    std::string action_button;
    std::string cancel_button;
    ModalTypes type = INFO_MODAL;
    std::function<void()> action_callback;

    bool show_modal{false};

public:
    /**
     *
     * @param m_ctx The famous ReaImgui context
     * @param assets Tha available assets
     */
    ReaSonusModal(
        ImGui_Context *m_ctx,
        const CSurf_UI_Assets *assets
    ) : m_ctx(m_ctx), assets(assets) {
    }

    ~ReaSonusModal() = default;

    /**
     * Set the values for the modal and set show_modal to true
     * @param _title The title for the modal
     * @param _message The message to communicate to the user
     * @param _action_button Label for the action button
     * @param _cancel_button Label for the Cancel button
     * @param _type The type of modal
     * @param _action_callback The callback for the action button
     */
    void ShowModal(
        const std::string &_title,
        const std::string &_message,
        const std::string &_action_button,
        const std::string &_cancel_button,
        const ModalTypes _type,
        const std::function<void()> &_action_callback

    ) {
        title = _title;
        message = _message;
        has_action = _action_button.empty() && _action_callback != nullptr;
        action_button = _action_button;
        cancel_button = _cancel_button.empty() ? "Cancel" : _cancel_button;
        type = _type;
        action_callback = _action_callback;

        switch (type) {
            case ERROR_MODAL:
                modal_title = "ReaSonus Error";
                break;

            case WARNING_MODAL:
                modal_title = "ReaSonus Warning";
                break;

            default:
                modal_title = "ReaSonus Message";
        }

        show_modal = true;
    }

    void Render() {
        double left;
        double top;

        /**
         * Calculate the center position of ReaSonus Control Panel
         * and set the modal position
         */
        ImGui::Viewport_GetCenter(ImGui::GetWindowViewport(m_ctx), &left, &top);
        ImGui::SetNextWindowPos(m_ctx, left, top, ImGui::Cond_Appearing, 0.5, 0.5);
        ImGui::SetNextWindowSize(m_ctx, 320, 0.0);
        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 0, 0);

        if (ImGui::BeginPopupModal(
            m_ctx,
            modal_title.c_str(),
            nullptr,
            ImGui::WindowFlags_AlwaysAutoResize | ImGui::WindowFlags_TopMost
        )) {
            UiStyledElements::PushReaSonusContentStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "modal-icon", 0, 0,
                                  ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_FrameStyle)) {
                ImGui::Text(m_ctx, "Icon here");
                ImGui::EndChild(m_ctx);
            }

            if (ImGui::BeginChild(m_ctx, "modal-title", 0, 0,
                                  ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_FrameStyle)) {
                ReaSonusPageTitle(m_ctx, assets, title, true);
                ImGui::EndChild(m_ctx);
            }

            if (ImGui::BeginChild(m_ctx, "modal-message", 0, 0,
                                  ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_FrameStyle)) {
                UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
                if (ImGui::BeginChild(
                    m_ctx,
                    "modal-message",
                    0.0,
                    0.0,
                    ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
                )) {
                    ImGui::Text(m_ctx, message.c_str());
                    ImGui::EndChild(m_ctx);
                }
                UiStyledElements::PopReaSonusGroupStyle(m_ctx);
                ImGui::EndChild(m_ctx);
            }

            ImGui::Separator(m_ctx);

            if (has_action) {
                if (ImGui::Button(m_ctx, "OK", 120, 0)) {
                    if (action_callback != nullptr) {
                        action_callback();
                    }
                    ImGui::CloseCurrentPopup(m_ctx);
                }
            }

            ImGui::SetItemDefaultFocus(m_ctx);
            ImGui::SameLine(m_ctx);

            if (ImGui::Button(m_ctx, "Cancel", 120, 0)) {
                ImGui::CloseCurrentPopup(m_ctx);
            }
            UiStyledElements::PopReaSonusContentStyle(m_ctx);
            ImGui::EndPopup(m_ctx);
        }

        ImGui::PopStyleVar(m_ctx);

        if (show_modal) {
            ImGui::OpenPopup(m_ctx, modal_title.c_str());
            show_modal = false;
        }
    }
};

#endif
