// ReSharper disable CppDFAUnreachableFunctionCall
#ifndef CSURF_FP_UI_PLUGIN_TYPE_SELECT_H_
#define CSURF_FP_UI_PLUGIN_TYPE_SELECT_H_

#include <string>
#include <reaper_imgui_functions.h>
#include "../csurf_ui_styles_elements.hpp"

static void ReaSonusPluginTypeSelect(
    ImGui_Context *m_ctx,
    const CSurf_UI_Assets *assets,
    const std::string &title,
    const std::string &plugin_name,
    int value,
) {
    UiStyledElements::PushReaSonusListBoxStyle(m_ctx);
    if (ImGui::BeginListBox(m_ctx, "##converted_files", 0.0, 250)) {
        for (int i = 0; i < (int) plugin_types.size(); i++) {
            bool selected = selected_plugin_type == i;
            if (ImGui::Selectable(m_ctx, plugin_types[i].c_str(), &selected,
                                  ImGui::SelectableFlags_AllowOverlap)) {
                selected_plugin_type = i;
            }
        }
        UiStyledElements::PopReaSonusListBoxStyle(m_ctx);
        ImGui::EndListBox(m_ctx); // converted_files
    }
}


#endif
