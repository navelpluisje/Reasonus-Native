#ifndef CSURF_FP_UI_SEARCH_COMBO_INPUT_H_
#define CSURF_FP_UI_SEARCH_COMBO_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include "csurf_ui_elements.hpp"
// #include "csurf_ui_combo_input.hpp"
#include "csurf_ui_icon_button.hpp"

static void ReaSonusSearchComboInput(ImGui_Context *m_ctx, CSurf_UI_Assets *assets, std::string label, std::vector<std::string> list, int *value, std::string *search_query, double width = 0.0)
{
    int label_offset = 24;
    double modal_x, modal_y;
    ImGui::GetCursorScreenPos(m_ctx, &modal_x, &modal_y);

    std::string id = "##" + label;
    double space_x, space_y;

    UiElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), width, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
    {
        std::string current_value = list[*value];
        static char text[255];
        strcpy(text, list[*value].c_str());

        ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
        if (width == 0.0)
        {
            width = space_x;
        }

        /**
         * Create the text input
         */
        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_DisabledAlpha, 1);
        ImGui::Text(m_ctx, label.c_str());

        UiElements::PushReaSonusInputStyle(m_ctx);
        ImGui::SetNextItemWidth(m_ctx, width - 39);
        ImGui::InputTextWithHint(m_ctx, ("##" + label).c_str(), "Hint", text, sizeof(text));
        if (ImGui::IsItemClicked(m_ctx))
        {
            *search_query = "";
            ImGui::OpenPopup(m_ctx, "search-combo-modal");
        }
        UiElements::PopReaSonusInputStyle(m_ctx);
        ImGui::PopStyleVar(m_ctx);

        /**
         * We place the button in the same line with updated y position
         *
         */
        ImGui::SameLine(m_ctx);
        UiElements::PushReaSonusIconButtonStyle(m_ctx, assets);
        if (ImGui::Button(m_ctx, std::string(1, IconSearch).c_str(), 31, 31))
        {
            *search_query = "";
            ImGui::OpenPopup(m_ctx, "search-combo-modal");
        }
        UiElements::PopReaSonusIconButtonStyle(m_ctx);

        /**
         * Create the modal
         */
        ImGui::SetNextWindowPos(m_ctx, modal_x - 8, modal_y - 8);
        ImGui::SetNextWindowSize(m_ctx, width + 16, 370);

        UiElements::PushReaSonusModalStyle(m_ctx);
        if (ImGui::BeginPopup(m_ctx, "search-combo-modal", ImGui::WindowFlags_TopMost | ImGui::WindowFlags_NoTitleBar | ImGui::WindowFlags_NoResize | ImGui::WindowFlags_NoMove))
        {
            if (ImGui::IsPopupOpen(m_ctx, "", ImGui::PopupFlags_AnyPopup) &&
                ImGui::IsKeyReleased(m_ctx, ImGui::Key_Escape))
            {
                ImGui::CloseCurrentPopup(m_ctx);
            }

            ImGui::TextFilter_Set(assets->GetReaComboFilter(), search_query->c_str());

            if (ImGui::BeginChild(m_ctx, "search-combo-content", width + 16, 370, ImGui::ChildFlags_FrameStyle))
            {
                ReaSonusTextInput(m_ctx, label, search_query, "Search", width - 39, ImGui::IsWindowAppearing(m_ctx));
                if (ImGui::IsWindowAppearing(m_ctx))
                {
                    ImGui::TextFilter_Clear(assets->GetReaComboFilter());
                }

                ImGui::SameLine(m_ctx);
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + label_offset);

                UiElements::PushReaSonusIconButtonStyle(m_ctx, assets);
                if (ImGui::Button(m_ctx, std::string(1, IconRemove).c_str(), 31, 31))
                {
                    ImGui::CloseCurrentPopup(m_ctx);
                }
                UiElements::PopReaSonusIconButtonStyle(m_ctx);

                /**
                 * Start the list
                 */
                UiElements::PushReaSonusListBoxStyle(m_ctx);
                if (ImGui::BeginListBox(m_ctx, "##listbox", 0.0, -0.1))
                {
                    for (int i = 0; i < (int)list.size(); i++)
                    {
                        if (ImGui::TextFilter_PassFilter(assets->GetReaComboFilter(), list[i].c_str()))
                        {
                            bool selected = *value == i;
                            if (ImGui::Selectable(m_ctx, list[i].c_str(), &selected, ImGui::SelectableFlags_AllowOverlap))
                            {
                                *value = i;
                                ImGui::CloseCurrentPopup(m_ctx);
                            }
                        }
                    }
                    UiElements::PopReaSonusListBoxStyle(m_ctx);
                    ImGui::EndListBox(m_ctx);
                }
                ImGui::EndChild(m_ctx);
            }

            ImGui::EndPopup(m_ctx);
        }
        UiElements::PopReaSonusModalStyle(m_ctx);

        ImGui::EndChild(m_ctx);
    }
    UiElements::PopReaSonusFieldGroupStyle(m_ctx);
}

#endif