#ifndef CSURF_FP_UI_INT_INPUT_H_
#define CSURF_FP_UI_INT_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"

static void ReaSonusIntInput(ImGui_Context *m_ctx, std::string label, int *value, int v_min, int v_max, double width = 0.0, std::string format = "%d")
{
    int local_value = *value;

    std::string id = "##" + label;

    UiElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), width, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
    {
        ImGui::Text(m_ctx, label.c_str());

        if (ImGui::BeginChild(m_ctx, ("input-container" + label).c_str(), width, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
        {
            UiElements::PushReaSonusInputStyle(m_ctx);
            if (ImGui::SliderInt(m_ctx, ("##" + label).c_str(), &local_value, v_min, v_max, format.c_str()))
            {
                *value = local_value;
            }
            UiElements::PopReaSonusInputStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }

        ImGui::EndChild(m_ctx);
    }
    UiElements::PopReaSonusFieldGroupStyle(m_ctx);
}

#endif