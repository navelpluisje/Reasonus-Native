#ifndef CSURF_FP_UI_PAGE_TITLE_H_
#define CSURF_FP_UI_PAGE_TITLE_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include "csurf_ui_elements.hpp"
#include "csurf_ui_assets.hpp"

static void ReaSonusPageTitle(ImGui_Context *m_ctx, CSurf_UI_Assets *assets, std::string label)
{
    std::string id = "##" + label;
    UiElements::PushReaSonusPageTitleStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), 0.0, 32.0, ImGui::ChildFlags_FrameStyle))
    {
        ImGui::PushFont(m_ctx, assets->GetMainFontBold(), 15);
        ImGui::Text(m_ctx, label.c_str());
        ImGui::PopFont(m_ctx);

        ImGui::Separator(m_ctx);

        ImGui::EndChild(m_ctx);
    }
    UiElements::PopReaSonusPageTitleStyle(m_ctx);
}

#endif