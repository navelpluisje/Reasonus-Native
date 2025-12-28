#ifndef CSURF_FP_UI_ICON_H_
#define CSURF_FP_UI_ICON_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "csurf_ui_elements.hpp"
#include <reaper_plugin_functions.h>
#include "../shared/csurf_utils.hpp"
#include "csurf_ui_colors.hpp"
#include "csurf_ui_assets.hpp"

static void ReaSonusIcon(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    IconFont icon,
    int size,
    int color,
    std::string id)
{
    double x_pos, y_pos;

    if (ImGui::BeginChild(m_ctx, ("##" + id).c_str(), size, size))
    {
        ImGui::GetItemRectMin(m_ctx, &x_pos, &y_pos);

        ImGui_DrawList *icon_draw_list = ImGui::GetForegroundDrawList(m_ctx);

        ImGui::PushFont(m_ctx, assets->GetIconFont(), size);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, color);
        ImGui::DrawList_AddText(icon_draw_list, x_pos, y_pos, color, std::string(1, icon).c_str());
        ImGui::PopStyleColor(m_ctx);
        ImGui::PopFont(m_ctx);

        ImGui::EndChild(m_ctx);
    }
}

#endif