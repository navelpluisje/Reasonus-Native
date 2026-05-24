#ifndef CSURF_FP_UI_FILTER_PREVIEW_H_
#define CSURF_FP_UI_FILTER_PREVIEW_H_

#include <reaper_imgui_functions.h>
#include <vector>
#include "../csurf_ui_colors.hpp"

static void ReaSonusFilterPreview(ImGui_Context *m_ctx, const int value)
{
    double last_item_x;
    double last_item_y;
    double width;
    double height;
    double text_width;
    double text_height;

    const std::vector track_level = {0, 1, 1, 2, 2, 1, 0, 1, 0, 0};
    const std::vector<std::vector<int>> fader_options = {
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 1, 0, 0, 0, 0},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        {1, 0, 1, 1, 1, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0},

        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 1, 1, 1, 1},

        {0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
        {1, 1, 1, 0, 0, 1, 1, 0, 1, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 0, 0},
        {1, 1, 1, 0, 0, 1, 1, 0, 1, 1},
        {0, 0, 1, 1, 1, 0, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
    };

    if (ImGui::BeginChild(m_ctx, "filter-preview", 0, 0, ImGui::ChildFlags_None))
    {
        ImGui::GetItemRectMin(m_ctx, &last_item_x, &last_item_y);
        ImGui::GetContentRegionAvail(m_ctx, &width, &height);
        ImGui_DrawList *list = ImGui::GetWindowDrawList(m_ctx);
        const double track_width = (width - 9 * 6) / 10;

        for (int i = 0; i < static_cast<int>(track_level.size()); i++)
        {
            const int x_pos = static_cast<int>(last_item_x) + i * track_width + i * 6;
            const double track_height = height - (track_level[i] + 1) * 20;
            const bool track_selected = fader_options[value][i] == 1;
            const bool track_matched = i == 2 || i == 6;
            const int border_color = track_selected ? UI_COLORS::Accent : UI_COLORS::Main_38;
            const int bg_color = track_matched ? UI_COLORS::Accent_25 : UI_COLORS::Main_28;

            ImGui::CalcTextSize(m_ctx, std::to_string(i + 1).c_str(), &text_width, &text_height);
            ImGui::DrawList_AddRectFilled(list, x_pos, last_item_y, x_pos + track_width, last_item_y + height, UI_COLORS::Main_23, 4);

            // Track
            ImGui::DrawList_AddRectFilled(list, x_pos, last_item_y, x_pos + track_width, last_item_y + track_height, bg_color, 4);
            ImGui::DrawList_AddRect(list, x_pos, last_item_y, x_pos + track_width, last_item_y + track_height, border_color, 4, ImGui::DrawFlags_None, 2);

            // Fader
            ImGui::DrawList_AddRectFilled(list, x_pos - 2 + track_width / 2, last_item_y + 40, x_pos + 2 + track_width / 2, last_item_y + track_height - 10, UI_COLORS::Main_15, 4);
            ImGui::DrawList_AddRectFilled(list, x_pos - 10 + track_width / 2, last_item_y + track_height - 60, x_pos + 10 + track_width / 2, last_item_y + track_height - 30, UI_COLORS::Main_38, 4);
            ImGui::DrawList_AddRect(list, x_pos - 10 + track_width / 2, last_item_y + track_height - 60, x_pos + 10 + track_width / 2, last_item_y + track_height - 30, UI_COLORS::Main_15, 4, ImGui::DrawFlags_None, 2);

            // Track number
            ImGui::DrawList_AddText(list, x_pos + (track_width - text_width) / 2, last_item_y + 2 + height - 20, UI_COLORS::Accent, std::to_string(i + 1).c_str());
        }

        ImGui::EndChild(m_ctx);
    }
}

#endif
