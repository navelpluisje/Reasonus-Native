#ifndef CSURF_FP_UI_CHECK_BOX_H_
#define CSURF_FP_UI_CHECK_BOX_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"

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

#endif