#ifndef CSURF_FP_UI_IMAGE_COMBO_INPUT_H_
#define CSURF_FP_UI_IMAGE_COMBO_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include "../csurf_ui_styles_elements.hpp"

static void ReaSonusImageComboInput( // NOLINT(*-function-cognitive-complexity)
    ImGui_Context *m_ctx,
    const CSurf_UI_Assets *assets,
    const std::string &label,
    const std::vector<std::string> &string_values,
    int *value,
    double width) {
    double modal_x;
    double modal_y;
    ImGui::GetCursorScreenPos(m_ctx, &modal_x, &modal_y);

    double space_x;
    double space_y;

    UiStyledElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), width, 0.0,
                          ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
        static char text[255]; // NOLINT(*-avoid-c-arrays)
        strncpy(text, string_values[*value].c_str(), sizeof(text));

        ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
        if (width == 0.0) {
            width = space_x;
        }

        /**
         * Create the text input
         */
        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_DisabledAlpha, 1);
        ImGui::Text(m_ctx, label.c_str());

        UiStyledElements::PushReaSonusInputStyle(m_ctx);
        ImGui::SetNextItemWidth(m_ctx, width - 39);
        ImGui::InputTextWithHint(m_ctx, ("##" + label).c_str(), "Hint", text, sizeof(text));
        if (ImGui::IsItemClicked(m_ctx)) {
            ImGui::OpenPopup(m_ctx, "image-combo-modal");
        }
        UiStyledElements::PopReaSonusInputStyle(m_ctx);
        ImGui::PopStyleVar(m_ctx);

        /**
         * We place the button in the same line with updated y position
         */
        ImGui::SameLine(m_ctx);
        UiStyledElements::PushReaSonusIconButtonStyle(m_ctx, assets);
        if (ImGui::Button(m_ctx, std::string(1, IconSearch).c_str(), 31, 31)) {
            ImGui::OpenPopup(m_ctx, "image-combo-modal");
        }
        UiStyledElements::PopReaSonusIconButtonStyle(m_ctx);

        /**
         * Create the modal
         */
        ImGui::SetNextWindowPos(m_ctx, modal_x - 8, modal_y - 8);
        ImGui::SetNextWindowSize(m_ctx, width + 16, 400);

        UiStyledElements::PushReaSonusModalStyle(m_ctx);
        if (ImGui::BeginPopup(m_ctx, "image-combo-modal",
                              ImGui::WindowFlags_TopMost | ImGui::WindowFlags_NoTitleBar | ImGui::WindowFlags_NoResize |
                              ImGui::WindowFlags_NoMove)) {
            if (ImGui::IsPopupOpen(m_ctx, "", ImGui::PopupFlags_AnyPopup) &&
                ImGui::IsKeyReleased(m_ctx, ImGui::Key_Escape)) {
                ImGui::CloseCurrentPopup(m_ctx);
            }

            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 16, 16);
            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 36, 8);
            if (ImGui::BeginChild(m_ctx, "image-combo-content", width + 16, 400, ImGui::ChildFlags_FrameStyle)) {
                for (int row = 0; row < 3; row++) {
                    for (int column = 0; column < 3; column++) {
                        const int index = column + row * 3;

                        UiStyledElements::PushReaSonusImageButtonStyle(m_ctx, *value == index);
                        if (ImGui::ImageButton(m_ctx, ("display-button-" + std::to_string(index)).c_str(),
                                               assets->GetDisplayMode(index), 76, 100)) {
                            *value = index;
                            ImGui::CloseCurrentPopup(m_ctx);
                        }
                        UiStyledElements::PopReaSonusImageButtonStyle(m_ctx);
                        if (column < 2) {
                            ImGui::SameLine(m_ctx);
                        }
                    }
                }
                ImGui::PopStyleVar(m_ctx, 2);
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
