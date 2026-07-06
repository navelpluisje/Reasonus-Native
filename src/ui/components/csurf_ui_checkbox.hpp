#ifndef CSURF_FP_UI_CHECK_BOX_H_
#define CSURF_FP_UI_CHECK_BOX_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../csurf_ui_styles_elements.hpp"
#include "csurf_ui_tooltip.hpp"

static void ReaSonusCheckBox(ImGui_Context *m_ctx, const std::string &label, bool *value) {
    bool _value = *value;

    UiStyledElements::PushReaSonusCheckboxStyle(m_ctx);
    ImGui::PushTextWrapPos(m_ctx, 100.0);
    if (ImGui::Checkbox(m_ctx, label.c_str(), &_value)) {
        *value = !*value;
    }
    ImGui::PopTextWrapPos(m_ctx);
    UiStyledElements::PopReaSonusCheckboxStyle(m_ctx);
}

static void RenderInfoCheckbox(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    const std::string &label,
    bool *value,
    const std::string &tooltip
) {
    double width;
    double height;

    ReaSonusCheckBox(m_ctx, "##" + label, value);
    ImGui::SameLine(m_ctx);

    ImGui::GetContentRegionAvail(m_ctx, &width, &height);
    ImGui::PushTextWrapPos(m_ctx, width - 20);

    ImGui::Text(m_ctx, label.c_str());
    if (ImGui::IsItemClicked(m_ctx)) {
        *value = !*value;
    }
    ImGui::PopTextWrapPos(m_ctx);
    ImGui::SameLine(m_ctx);

    ReaSonusTooltip(m_ctx, assets, tooltip, tooltip + label, 0, -3);
    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
    ImGui::Dummy(m_ctx, 0, 0);
}

#endif
