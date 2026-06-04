#ifndef CSURF_FP_UI_PLUGIN_SELECT_MODAL_H_
#define CSURF_FP_UI_PLUGIN_SELECT_MODAL_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <vector>
#include "../csurf_ui_styles_elements.hpp"
#include "../../../lib/reaImgui/reaper_imgui_functions.h"

static void ReaSonusPluginSelectModal( // NOLINT(*-function-cognitive-complexity)
    ImGui_Context *m_ctx,
    const CSurf_UI_Assets *assets,
    const std::string &label,
    std::vector<PluginMeta> &installed_plugins
    // const std::vector<std::string> &list,
    // int *value,
    // std::string *search_query,
    // const bool clear_param,
    // double width
) {
    constexpr int label_offset = 24;
    double modal_x;
    double modal_y;
    ImGui::GetCursorScreenPos(m_ctx, &modal_x, &modal_y);

    double space_x;
    double space_y;

    if (ImGui::BeginChild(
        m_ctx, ("container" + label).c_str(),
        20.0,
        20.0,
        ImGui::ChildFlags_None
    )) {
        // std::string current_value = list[*value];
        // static char text[255]; // NOLINT(*-avoid-c-arrays)
        // strncpy(text, list[*value].c_str(), sizeof(text));

        ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

        UiStyledElements::PushReaSonusIconButtonStyle(m_ctx, assets);
        if (ImGui::Button(m_ctx, std::string(1, IconAdd).c_str(), 20, 20)) {
            ImGui::OpenPopup(m_ctx, "plugin-select-modal");
        }
        UiStyledElements::PopReaSonusIconButtonStyle(m_ctx);

        ImGui::SetNextWindowSize(m_ctx, 770, 370);

        UiStyledElements::PushReaSonusModalStyle(m_ctx);
        if (ImGui::BeginPopupModal(
            m_ctx,
            "plugin-select-modal",
            nullptr,
            ImGui::WindowFlags_TopMost | ImGui::WindowFlags_NoResize
        )) {
            if (ImGui::BeginTable(m_ctx, "installed-plugins-table", 5,
                                  ImGui::TableFlags_BordersV | ImGui::TableFlags_RowBg |
                                  ImGui::TableFlags_SizingStretchProp | ImGui::TableFlags_ScrollY, 0.0, -30.0)) {
                ImGui::TableSetupColumn(m_ctx, "Plugin");
                ImGui::TableSetupColumn(m_ctx, "Developer");
                ImGui::TableSetupColumn(m_ctx, "Category");
                ImGui::TableSetupColumn(m_ctx, "Type");
                ImGui::TableSetupColumn(m_ctx, " ");
                ImGui::TableSetupScrollFreeze(m_ctx, 0, 1);
                ImGui::TableHeadersRow(m_ctx);

                for (const auto &installed_plugin: installed_plugins) {
                    ImGui::TableNextRow(m_ctx);
                    ImGui::TableNextColumn(m_ctx);
                    ImGui::Text(m_ctx, installed_plugin.GetShortName().c_str());
                    ImGui::TableNextColumn(m_ctx);
                    ImGui::Text(m_ctx, installed_plugin.GetDeveloper().c_str());
                    ImGui::TableNextColumn(m_ctx);
                    ImGui::Text(m_ctx, installed_plugin.GetCategory().c_str());
                    ImGui::TableNextColumn(m_ctx);
                    ImGui::Text(m_ctx, installed_plugin.GetPluginType().c_str());
                    ImGui::TableNextColumn(m_ctx);
                    ImGui::PushFont(m_ctx, assets->GetIconFont(), 14);
                    ImGui::SmallButton(m_ctx, std::string(1, IconAdd).c_str());
                    ImGui::PopFont(m_ctx);
                }

                ImGui::EndTable(m_ctx);
            }

            if (ImGui::BeginChild(m_ctx, "popup-buttons")) {
                ImGui::Text(m_ctx, "hgfjghfjhg");

                ImGui::EndChild(m_ctx);
            }

            ImGui::EndPopup(m_ctx);
        }
        UiStyledElements::PopReaSonusModalStyle(m_ctx);

        ImGui::EndChild(m_ctx);
    }
}

#endif
