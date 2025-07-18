#ifndef CSURF_FP_UI_RADIO_BUTTON_GROUP_H_
#define CSURF_FP_UI_RADIO_BUTTON_GROUP_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include "csurf_ui_elements.hpp"

void ReaSonusRadioButtonGroup(ImGui_Context *m_ctx, std::string label, std::vector<std::string> list, int *value)
{
    std::string id = "##" + label;

    UiElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), 0.0, 22 + (int)list.size() * 22, ImGui::ChildFlags_FrameStyle))
    {
        ImGui::Text(m_ctx, label.c_str());
        UiElements::PushReaSonusRadioButtonStyle(m_ctx);
        for (int i = 0; i < (int)list.size(); i++)
        {
            int selected = *value;
            if (ImGui::RadioButtonEx(m_ctx, list[i].c_str(), &selected, i))
            {
                *value = selected;
            }
        }
        UiElements::PopReaSonusRadioButtonStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
    UiElements::PopReaSonusFieldGroupStyle(m_ctx);
}

#endif