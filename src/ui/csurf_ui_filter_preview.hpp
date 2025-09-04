#ifndef CSURF_FP_UI_FILTER_PREVIEW_H_
#define CSURF_FP_UI_FILTER_PREVIEW_H_

#include <reaper_imgui_functions.h>
#include <vector>
#include "csurf_ui_elements.hpp"
#include "csurf_ui_colors.hpp"

static void ReaSonusFilterPreview(ImGui_Context *m_ctx, int value)
{
    double x, y, width, height, text_width, text_height;
    std::vector<int> track_level = {0, 1, 1, 2, 2, 1, 0, 1, 0, 0};
    std::vector<std::vector<int>> fader_options = {
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

    if (ImGui::BeginChild(m_ctx, "lfilter-preview", 0, 0, ImGui::ChildFlags_None))
    {
        ImGui::GetItemRectMin(m_ctx, &x, &y);
        ImGui::GetContentRegionAvail(m_ctx, &width, &height);
        ImGui_DrawList *list = ImGui::GetForegroundDrawList(m_ctx);
        double track_width = (width - (9 * 6)) / 10;

        for (int i = 0; i < (int)track_level.size(); i++)
        {
            int x_pos = x + i * track_width + i * 6;
            double track_height = height - ((track_level[i] + 1) * 20);
            bool track_selected = fader_options[value][i] == 1;
            bool track_matched = (i == 2 || i == 6);
            int border_color = track_selected ? UI_COLORS::Accent : UI_COLORS::Main_38;
            int bg_color = track_matched ? UI_COLORS::Accent_25 : UI_COLORS::Main_28;

            ImGui::CalcTextSize(m_ctx, std::to_string(i + 1).c_str(), &text_width, &text_height);
            ImGui::DrawList_AddRectFilled(list, x_pos, y, x_pos + track_width, y + height, UI_COLORS::Main_23, 4);

            // Track
            ImGui::DrawList_AddRectFilled(list, x_pos, y, x_pos + track_width, y + track_height, bg_color, 4);
            ImGui::DrawList_AddRect(list, x_pos, y, x_pos + track_width, y + track_height, border_color, 4, ImGui::DrawFlags_None, 2);

            // Fader
            ImGui::DrawList_AddRectFilled(list, x_pos - 2 + track_width / 2, y + 40, x_pos + 2 + track_width / 2, y + track_height - 10, UI_COLORS::Main_15, 4);
            ImGui::DrawList_AddRectFilled(list, x_pos - 10 + track_width / 2, y + track_height - 60, x_pos + 10 + track_width / 2, y + track_height - 30, UI_COLORS::Main_38, 4);
            ImGui::DrawList_AddRect(list, x_pos - 10 + track_width / 2, y + track_height - 60, x_pos + 10 + track_width / 2, y + track_height - 30, UI_COLORS::Main_15, 4, ImGui::DrawFlags_None, 2);

            // Track number
            ImGui::DrawList_AddText(list, x_pos + (track_width - text_width) / 2, y + 2 + height - 20, UI_COLORS::Accent, std::to_string(i + 1).c_str());
        }

        ImGui::EndChild(m_ctx);
    }
}

#endif
