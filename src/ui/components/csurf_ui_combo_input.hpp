#ifndef CSURF_FP_UI_COMBO_INPUT_H_
#define CSURF_FP_UI_COMBO_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include "../csurf_ui_styles_elements.hpp"

static void ReaSonusComboInput(ImGui_Context *m_ctx, const std::string &label, const std::vector<std::string> &list, int *value, double width = 0.0) // NOLINT(*-default-arguments-declarations)
{
    double space_x;
    double space_y;

    UiStyledElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), width, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
    {
        ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
        if (width == 0.0)
        {
            width = space_x;
        }

        ImGui::Text(m_ctx, label.c_str());
        UiStyledElements::PushReaSonusComboStyle(m_ctx);

        ImGui::SetNextItemWidth(m_ctx, width);
        if (ImGui::BeginCombo(m_ctx, ("##" + label).c_str(), list[*value].c_str()))
        {
            UiStyledElements::PushReaSonusListBoxStyle(m_ctx);
            for (int i = 0; i < static_cast<int>(list.size()); i++)
            {
                bool selected = *value == i;
                if (ImGui::Selectable(m_ctx, list[i].c_str(), &selected))
                {
                    *value = i;
                }
            }
            UiStyledElements::PopReaSonusListBoxStyle(m_ctx);
            ImGui::EndCombo(m_ctx);
        }
        UiStyledElements::PopReaSonusComboStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
    UiStyledElements::PopReaSonusFieldGroupStyle(m_ctx);
}

static void ReaSonusComboInputRow(ImGui_Context *m_ctx, const std::string &label, const std::vector<std::string> &list, int *value, const double label_width)
{
    double space_x;
    double space_y;
    const double start_pos = ImGui::GetCursorPosX(m_ctx);

    UiStyledElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container-" + label).c_str(), 0.0, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
    {
        ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
        const double combo_width = space_x - label_width;

        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 8);
        ImGui::Text(m_ctx, label.c_str());

        ImGui::SameLine(m_ctx);

        ImGui::SetCursorPosX(m_ctx, start_pos + label_width);
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 8);
        ImGui::SetNextItemWidth(m_ctx, combo_width);

        UiStyledElements::PushReaSonusComboStyle(m_ctx);
        if (ImGui::BeginCombo(m_ctx, ("##" + label).c_str(), list[*value].c_str()))
        {

            UiStyledElements::PushReaSonusListBoxStyle(m_ctx);
            for (int i = 0; i < static_cast<int>(list.size()); i++)
            {
                bool selected = *value == i;
                if (ImGui::Selectable(m_ctx, list[i].c_str(), &selected))
                {
                    *value = i;
                }
            }
            UiStyledElements::PopReaSonusListBoxStyle(m_ctx);
            ImGui::EndCombo(m_ctx);
        }
        UiStyledElements::PopReaSonusComboStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
    UiStyledElements::PopReaSonusFieldGroupStyle(m_ctx);
}

static void RenderInfoComboInput(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    const std::string &label,
    const std::vector<std::string> &list,
    int *value,
    const std::string &tooltip)
{
    const double x_pos = ImGui::GetCursorPosX(m_ctx);
    const double y_pos = ImGui::GetCursorPosY(m_ctx);
    const std::string combo_id = std::to_string(x_pos) + "-" + std::to_string(y_pos);

    ReaSonusComboInput(m_ctx, label, list, value, -20);
    ImGui::SameLine(m_ctx);
    ReaSonusTooltip(m_ctx, assets, tooltip, combo_id, -20, 26);
    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
    ImGui::Dummy(m_ctx, 0, 0);
}

#endif