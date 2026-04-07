#ifndef CSURF_FP_UI_GOTO_INPUT_H_
#define CSURF_FP_UI_GOTO_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include "csurf_ui_styles_elements.hpp"
#include "csurf_ui_icon_button.hpp"
#include "../shared/csurf_context.cpp"

static void ReaSonusGoToInput(ImGui_Context *m_ctx, CSurf_UI_Assets *assets, int *value, int max_value)
{
    double modal_x, modal_y;
    ImGui::GetCursorScreenPos(m_ctx, &modal_x, &modal_y);

    static char str_value[255];
    strcpy(str_value, (std::to_string(*value + 1)).c_str());

    UiStyledElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, "container-goto-input", 30.0, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
    {
        bool is_active = ImGui::IsPopupOpen(m_ctx, "goto-modal");
        bool close_popup = false;

        if (ReaSonusIconButton(
                m_ctx,
                assets,
                is_active ? IconRemove : IconGoTo,
                "mapping-reset-group",
                is_active ? ButtonThemeModal : ButtonThemeAccent))
        {
            if (!is_active)
            {
                ImGui::OpenPopup(m_ctx, "goto-modal");
            }
            else
            {
                close_popup = true;
            }
        }

        /**
         * Create the modal
         */
        ImGui::SetNextWindowPos(m_ctx, modal_x - 63, modal_y - 8);
        ImGui::SetNextWindowSize(m_ctx, 101, 46);

        UiStyledElements::PushReaSonusModalStyle(m_ctx);
        if (ImGui::BeginPopup(m_ctx, "goto-modal", ImGui::WindowFlags_TopMost | ImGui::WindowFlags_NoTitleBar | ImGui::WindowFlags_NoResize | ImGui::WindowFlags_NoMove))
        {
            if ((ImGui::IsPopupOpen(m_ctx, "", ImGui::PopupFlags_AnyPopup) &&
                 (ImGui::IsKeyPressed(m_ctx, ImGui::Key_Enter) ||
                  ImGui::IsKeyReleased(m_ctx, ImGui::Key_Escape))) ||
                close_popup)
            {
                ImGui::CloseCurrentPopup(m_ctx);
            }

            if (ImGui::BeginChild(m_ctx, "goto-modal-content", 98, 46, ImGui::ChildFlags_FrameStyle, ImGui::WindowFlags_NoNavFocus | ImGui::WindowFlags_NoScrollbar))
            {
                UiStyledElements::PushReaSonusInputStyle(m_ctx);
                ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 7, 7);

                ImGui::SetNextItemWidth(m_ctx, 48);
                if (ImGui::IsWindowAppearing(m_ctx))
                {
                    ImGui::SetKeyboardFocusHere(m_ctx);
                }

                int max_group_count = CSurf_Context::GetPluginMaxGroupCount();
                int str_value_buffer_length = std::to_string(max_group_count).length() + 1;

                if (ImGui::InputText(
                        m_ctx,
                        "##goto-input",
                        str_value,
                        str_value_buffer_length,
                        ImGui::InputTextFlags_CallbackCharFilter,
                        assets->GetOnlyDigetsFilter()))
                {
                    if (!std::string(str_value).empty() && isInteger(std::string(str_value)))
                    {
                        int val = stoi(std::string(str_value)) - 1;
                        if (val < max_value)
                        {
                            *value = stoi(std::string(str_value)) - 1;
                        }
                    }
                    else
                    {
                        *value = *value;
                    }
                }

                ImGui::PopStyleVar(m_ctx);
                UiStyledElements::PopReaSonusInputStyle(m_ctx);

                ImGui::EndChild(m_ctx);
            }

            ImGui::EndPopup(m_ctx);
        }
        UiStyledElements::PopReaSonusModalStyle(m_ctx);

        ImGui::EndChild(m_ctx);
    }
    UiStyledElements::PopReaSonusFieldGroupStyle(m_ctx);
}

#endif