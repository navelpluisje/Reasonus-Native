#ifndef CSURF_FP_UI_CHECK_BOX_H_
#define CSURF_FP_UI_CHECK_BOX_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../csurf_ui_styles_elements.hpp"
#include "csurf_ui_tooltip.hpp"

static void ReaSonusCheckBox(ImGui_Context *m_ctx, const std::string &label, bool *value)
{
    bool _value = *value;

    UiStyledElements::PushReaSonusCheckboxStyle(m_ctx);
    if (ImGui::Checkbox(m_ctx, label.c_str(), &_value))
    {
        *value = !*value;
    }
    UiStyledElements::PopReaSonusCheckboxStyle(m_ctx);
}

static void RenderInfoCheckbox(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    const std::string& label,
    bool *value,
    const std::string &tooltip)
{
    const double x_pos = ImGui::GetCursorPosX(m_ctx);
    const double y_pos = ImGui::GetCursorPosY(m_ctx);
    const std::string tooltip_id = std::to_string(x_pos) + "-" + std::to_string(y_pos);

    ReaSonusCheckBox(m_ctx, label, value);
    ImGui::SameLine(m_ctx);
    ReaSonusTooltip(m_ctx, assets, tooltip, tooltip_id, 0, -3);
    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
    ImGui::Dummy(m_ctx, 0, 0);
}

#endif