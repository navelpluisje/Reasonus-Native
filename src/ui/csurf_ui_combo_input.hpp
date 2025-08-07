#ifndef CSURF_FP_UI_COMBO_INPUT_H_
#define CSURF_FP_UI_COMBO_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include "csurf_ui_elements.hpp"

static void ReaSonusComboInput(ImGui_Context *m_ctx, std::string label, std::vector<std::string> list, int *value)
{
    std::string id = "##" + label;

    UiElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), 0.0, 50.0, ImGui::ChildFlags_FrameStyle, ImGui::ChildFlags_AutoResizeY))
    {
        ImGui::Text(m_ctx, label.c_str());
        UiElements::PushReaSonusComboStyle(m_ctx);

        if (ImGui::BeginCombo(m_ctx, id.c_str(), list[*value].c_str()))
        {
            UiElements::PushReaSonusListBoxStyle(m_ctx);
            for (int i = 0; i < (int)list.size(); i++)
            {
                bool selected = *value == i;
                if (ImGui::Selectable(m_ctx, list[i].c_str(), &selected))
                {
                    *value = i;
                }
            }
            UiElements::PopReaSonusListBoxStyle(m_ctx);
            ImGui::EndCombo(m_ctx);
        }
        UiElements::PopReaSonusComboStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
    UiElements::PopReaSonusFieldGroupStyle(m_ctx);
}

#endif