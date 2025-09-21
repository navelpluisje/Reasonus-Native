#ifndef CSURF_FP_UI_ACTION_INPUT_TEXT_H_
#define CSURF_FP_UI_ACTION_INPUT_TEXT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include <functional>
#include "csurf_ui_elements.hpp"
#include "csurf_ui_text_input.hpp"

/**
 * @brief Create a text input field with an action button
 *
 * @param m_ctx The famous ReaImgui context
 * @param label The label for the input field. This will also be used to create the id's
 * @param value The value for the input field
 * @param hint The placeholder for the input when the value is empty
 * @param icon  The icons for the action button
 * @param callback Tha callback function triggered when clicking the action button
 */
static void ReaSonusActionInputText(
    ImGui_Context *m_ctx,
    std::string label,
    std::string *value,
    std::string hint,
    ImGui_Image *icon,
    std::function<void()> callback)
{
    double space_x, space_y;

    if (ImGui::BeginChild(m_ctx, (label + "-container").c_str(), 0.0, 0.0, ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY))
    {

        ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

        ReaSonusTextInput(m_ctx, label, value, hint.c_str(), space_x - 38);
        if (ImGui::IsKeyPressed(m_ctx, ImGui::Key_Enter) && (*value).compare("") != 0)
        {
            callback();
        }

        ImGui::SameLine(m_ctx);
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 25);

        UiElements::PushReaSonusIconButtonStyle(m_ctx);
        if (ImGui::ImageButton(m_ctx, "add-filter", icon, 20, 20))
        {
            if ((*value).compare("") != 0)
            {
                callback();
            }
        }
        UiElements::PopReaSonusIconButtonStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
}

#endif