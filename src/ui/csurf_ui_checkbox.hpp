#ifndef CSURF_FP_UI_CHECK_BOX_H_
#define CSURF_FP_UI_CHECK_BOX_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"
#include "csurf_ui_tooltip.hpp"

static void ReaSonusCheckBox(ImGui_Context *m_ctx, std::string label, bool *value)
{
    bool _value = *value;

    UiElements::PushReaSonusCheckboxStyle(m_ctx);
    if (ImGui::Checkbox(m_ctx, label.c_str(), &_value))
    {
        *value = !*value;
    }
    UiElements::PopReaSonusCheckboxStyle(m_ctx);
}

static void RenderInfoCheckbox(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    std::string label,
    bool *value,
    std::string tooltip)
{
    double x_pos = ImGui::GetCursorPosX(m_ctx);
    double y_pos = ImGui::GetCursorPosY(m_ctx);
    std::string id = std::to_string(x_pos) + "-" + std::to_string(y_pos);

    ReaSonusCheckBox(m_ctx, label.c_str(), value);
    ImGui::SameLine(m_ctx);
    ReaSonusTooltip(m_ctx, assets, tooltip.c_str(), id.c_str());
    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
    ImGui::Dummy(m_ctx, 0, 0);
}

#endif