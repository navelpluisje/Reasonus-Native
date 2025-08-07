#ifndef CSURF_FP_UI_TEXT_INPUT_H_
#define CSURF_FP_UI_TEXT_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"

static void ReaSonusTextInput(ImGui_Context *m_ctx, std::string label, char *value)
{
    static char test[255];
    strcpy(test, value);

    std::string id = "##" + label;

    UiElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), 0.0, 50.0, ImGui::ChildFlags_FrameStyle, ImGui::ChildFlags_AutoResizeY))
    {
        ImGui::Text(m_ctx, label.c_str());
        UiElements::PushReaSonusInputStyle(m_ctx);
        if (ImGui::InputText(m_ctx, ("##" + label).c_str(), test, sizeof(test)))
        {
            strcpy(value, test);
        }
        UiElements::PopReaSonusInputStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
    UiElements::PopReaSonusFieldGroupStyle(m_ctx);
}

#endif