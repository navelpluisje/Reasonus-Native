#ifndef CSURF_FP_UI_LIST_BOX_H_
#define CSURF_FP_UI_LIST_BOX_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include "csurf_ui_elements.hpp"

static void ReaSonusListBox(ImGui_Context *m_ctx, std::string label, std::vector<std::string> list, int *value, int height = 100, bool hide_label = false)
{
    std::string id = "##" + label;

    UiElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), 0.0, height + 22, ImGui::ChildFlags_FrameStyle, ImGui::ChildFlags_AutoResizeY))
    {
        if (!hide_label)
        {
            ImGui::Text(m_ctx, label.c_str());
        }

        UiElements::PushReaSonusListBoxStyle(m_ctx);
        if (ImGui::BeginListBox(m_ctx, id.c_str(), 0.0, height))
        {
            for (int i = 0; i < (int)list.size(); i++)
            {
                bool selected = *value == i;
                if (ImGui::Selectable(m_ctx, list[i].c_str(), &selected))
                {
                    *value = i;
                }
            }
            ImGui::EndListBox(m_ctx);
        }
        UiElements::PopReaSonusListBoxStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
    UiElements::PopReaSonusFieldGroupStyle(m_ctx);
}

#endif