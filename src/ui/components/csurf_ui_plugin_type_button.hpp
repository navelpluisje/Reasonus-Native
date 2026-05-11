#ifndef CSURF_FP_UI_PLUGIN_TYPE_BUTTON_H_
#define CSURF_FP_UI_PLUGIN_TYPE_BUTTON_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../csurf_ui_assets.hpp"
#include "../csurf_ui_colors.hpp"

static void ReaSonusPluginTypeButton(
    ImGui_Context *m_ctx,
    const CSurf_UI_Assets *assets,
    const std::string &action_label
) {
    constexpr int size = 16;

    int border_width = 2;
    double x_pos_1;
    double y_pos_1;
    double text_width;
    double text_height;
    double x_mouse_pos;
    double y_mouse_pos;

    if (ImGui::BeginChild(
        m_ctx, ("##" + action_label).c_str(),
        size,
        size
        // ImGui::ChildFlags_FrameStyle
    )) {
        ImGui::GetMousePos(m_ctx, &x_mouse_pos, &y_mouse_pos);
        ImGui::GetItemRectMin(m_ctx, &x_pos_1, &y_pos_1);

        ImGui_DrawList *list = ImGui::GetWindowDrawList(m_ctx);

        ImGui::DrawList_AddRectFilled(
            list,
            x_pos_1,
            y_pos_1,
            x_pos_1 + size,
            y_pos_1 + size,
            UI_COLORS::Main_28,
            4
        );
        ImGui::DrawList_AddRect(
            list,
            x_pos_1,
            y_pos_1,
            x_pos_1 + size,
            y_pos_1 + size,
            UI_COLORS::Accent,
            4,
            ImGui::DrawFlags_None,
            border_width
        );

        // ImGui::PushFont(m_ctx, assets->GetMainFontBold(), 19);
        // ImGui::CalcTextSize(m_ctx, "A", text_width, text_height);
        //
        // ImGui::DrawList_AddText(list, x_pos_1 + 8 - text_width / 2, y_pos_1, UI_COLORS::White, "A");
        // ImGui::PopFont(m_ctx);

        // ImGui::PushFont(m_ctx, assets->GetMainFontBold(), 15);
        // const double font_height = ImGui::GetTextLineHeight(m_ctx);
        // ImGui::DrawList_AddText(list, x_pos_1 + 42, y_pos_1 + (menu_button_height - font_height) / 2, UI_COLORS::White,
        //                         action_label.c_str());
        // ImGui::PopFont(m_ctx);

        ImGui::EndChild(m_ctx);
    }
}

#endif
