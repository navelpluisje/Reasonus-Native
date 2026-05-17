#ifndef CSURF_FP_UI_PLUGIN_TYPE_BUTTON_H_
#define CSURF_FP_UI_PLUGIN_TYPE_BUTTON_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../csurf_ui_assets.hpp"

static void ReaSonusPluginTypeButton(
    ImGui_Context *m_ctx,
    const CSurf_UI_Assets *assets,
    const double x_pos,
    const double y_pos,
    const std::string &label
) {
    double text_width;
    double text_height;
    constexpr double size = 19.0;
    constexpr int border_width = 1;

    const std::string plugin_type = PluginUtils::GetShortPluginType(label);
    const int bg_color = PluginUtils::GetPluginTypeBgColors(label);
    const int fg_color = PluginUtils::GetPluginTypeFgColors(label);

    ImGui_DrawList *list = ImGui::GetWindowDrawList(m_ctx);

    ImGui::DrawList_AddRectFilled(
        list,
        x_pos,
        y_pos,
        x_pos + size,
        y_pos + size + 2,
        bg_color,
        3
    );

    ImGui::PushFont(m_ctx, assets->GetMainFontBold(), 13);
    ImGui::CalcTextSize(m_ctx, plugin_type.c_str(), &text_width, &text_height);
    ImGui::DrawList_AddText(
        list,
        x_pos + size / 2 - text_width / 2,
        y_pos + size / 2 - text_height / 2 + 2,
        fg_color,
        plugin_type.c_str()
    );
    ImGui::PopFont(m_ctx);
}


#endif
