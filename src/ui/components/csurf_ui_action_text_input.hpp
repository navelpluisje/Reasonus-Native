#ifndef CSURF_FP_UI_ACTION_TEXT_INPUT_H_
#define CSURF_FP_UI_ACTION_TEXT_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <functional>
#include "../csurf_ui_styles_elements.hpp"
#include "csurf_ui_text_input.hpp"

/**
 * @brief Create a text input field with an action button
 *
 * @param m_ctx The famous ReaImgui context
 * @param assets Assets helper to get the asset you need
 * @param label The label for the input field. This will also be used to create the id's
 * @param value The value for the input field
 * @param hint The placeholder for the input when the value is empty
 * @param callback Tha callback function triggered when clicking the action button
 */
static void ReaSonusActionTextInput(
    ImGui_Context *m_ctx,
    const CSurf_UI_Assets *assets,
    const std::string &label,
    std::string *value,
    const std::string &hint,
    const std::function<void()> &callback) {
    double space_x;
    double space_y;

    if (ImGui::BeginChild(m_ctx, (label + "-container").c_str(), 0.0, 0.0,
                          ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY)) {
        ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

        ReaSonusTextInput(m_ctx, label, value, hint, space_x - 38, false);
        if (ImGui::IsKeyPressed(m_ctx, ImGui::Key_Enter) && !value->empty()) {
            callback();
        }

        ImGui::SameLine(m_ctx);
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 25);

        UiStyledElements::PushReaSonusIconButtonStyle(m_ctx, assets);
        if (ImGui::Button(m_ctx, std::string(1, IconAddList).c_str())) {
            if (!value->empty()) {
                callback();
            }
        }
        UiStyledElements::PopReaSonusIconButtonStyle(m_ctx);

        ImGui::EndChild(m_ctx);
    }
}

#endif
