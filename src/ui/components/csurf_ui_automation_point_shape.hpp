#ifndef CSURF_FP_UI_AUTOMATION_POINT_SHAPE_H_
#define CSURF_FP_UI_AUTOMATION_POINT_SHAPE_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../csurf_ui_colors.hpp"
#include "../csurf_ui_styles_elements.hpp"

/**
 * @brief Create a combo input for automation point shape, with a bezier tension option
 *
 * @param m_ctx The famous ReaImgui context
 * @param assets Assets helper to get the asset you need
 * @param label The label for the input field. This will also be used to create the id's
 * @param value The value for the input field
 * @param shape The automationpont shape to display
 * @param disabled Wether the block is disabled
 */
static void ReaSonusAutomationPointShape(
    ImGui_Context *m_ctx,
    const CSurf_UI_Assets *assets,
    const std::string &label,
    double *value,
    const int shape,
    const bool disabled
) {
    double width;
    double height;
    double pos_x;
    double pos_y;
    const double tension = (*value + 1) / 2;
    double shape_box_width;

    ImGui_DrawList *list = ImGui::GetWindowDrawList(m_ctx);
    const int background_color = disabled ? UI_COLORS::Accent_07 : UI_COLORS::Accent_25;
    const int border_color = disabled ? UI_COLORS::Accent_14 : UI_COLORS::Accent_50;
    const int shape_color = disabled ? UI_COLORS::Accent_25 : UI_COLORS::Accent;

    if (ImGui::BeginChild(
        m_ctx,
        (label + "-container").c_str(),
        0.0,
        72.0,
        ImGui::ChildFlags_None
    )) {
        ImGui::GetContentRegionAvail(m_ctx, &width, &height);
        ImGui::GetItemRectMin(m_ctx, &pos_x, &pos_y);

        shape_box_width = (width - 16) / 2;
        constexpr double shape_box_height = 68;
        const double shape_width = shape_box_width - 16;
        constexpr double shape_height = shape_box_height - 16;
        const double shape_1_start_x = pos_x + 8;
        const double shape_1_start_y = pos_y + 8;
        const double shape_2_start_x = pos_x + shape_box_width + 24;
        const double shape_2_start_y = pos_y + 8;

        ImGui::DrawList_AddRectFilled(
            list,
            pos_x, pos_y,
            pos_x + shape_box_width, pos_y + shape_box_height,
            background_color,
            4
        );

        ImGui::DrawList_AddRect(
            list,
            pos_x, pos_y,
            pos_x + shape_box_width, pos_y + shape_box_height,
            border_color,
            4,
            0,
            1
        );

        ImGui::DrawList_AddRectFilled(
            list,
            pos_x + shape_box_width + 16, pos_y,
            pos_x + shape_box_width * 2 + 16, pos_y + shape_box_height,
            background_color,
            4
        );

        ImGui::DrawList_AddRect(
            list,
            pos_x + shape_box_width + 16, pos_y,
            pos_x + shape_box_width * 2 + 16, pos_y + shape_box_height,
            border_color,
            4,
            0,
            1
        );

        if (shape == 0) {
            ImGui::DrawList_AddLine(
                list,
                shape_1_start_x, shape_1_start_y,
                shape_1_start_x + shape_width, shape_1_start_y + shape_height,
                shape_color,
                2
            );

            ImGui::DrawList_AddLine(
                list,
                shape_2_start_x, shape_2_start_y + shape_height,
                shape_2_start_x + shape_width, shape_2_start_y,
                shape_color,
                2
            );
        } else if (shape == 1) {
            ImGui::DrawList_AddLine(
                list,
                shape_1_start_x, shape_1_start_y,
                shape_1_start_x + shape_width - 30, shape_1_start_y,
                shape_color,
                2
            );
            ImGui::DrawList_AddLine(
                list,
                shape_1_start_x + shape_width - 30, shape_1_start_y,
                shape_1_start_x + shape_width - 30, shape_1_start_y + shape_height,
                shape_color,
                2
            );
            ImGui::DrawList_AddLine(
                list,
                shape_1_start_x + shape_width - 30, shape_1_start_y + shape_height,
                shape_1_start_x + shape_width, shape_1_start_y + shape_height,
                shape_color,
                2
            );

            ImGui::DrawList_AddLine(
                list,
                shape_2_start_x, shape_2_start_y + shape_height,
                shape_2_start_x + shape_width - 30, shape_2_start_y + shape_height,
                shape_color,
                2
            );
            ImGui::DrawList_AddLine(
                list,
                shape_2_start_x + shape_width - 30, shape_2_start_y + shape_height,
                shape_2_start_x + shape_width - 30, shape_2_start_y,
                shape_color,
                2
            );
            ImGui::DrawList_AddLine(
                list,
                shape_2_start_x + shape_width - 30, shape_2_start_y,
                shape_2_start_x + shape_width, shape_2_start_y,
                shape_color,
                2
            );
        } else if (shape == 2) {
            ImGui::DrawList_AddBezierCubic(
                list,
                shape_1_start_x, shape_1_start_y,
                shape_1_start_x + shape_width / 2, shape_1_start_y,
                shape_1_start_x + shape_width / 2, shape_1_start_y + shape_height,
                shape_1_start_x + shape_width, shape_1_start_y + shape_height,
                shape_color,
                2
            );

            ImGui::DrawList_AddBezierCubic(
                list,
                shape_2_start_x, shape_2_start_y + shape_height,
                shape_2_start_x + shape_width / 2, shape_2_start_y + shape_height,
                shape_2_start_x + shape_width / 2, shape_2_start_y,
                shape_2_start_x + shape_width, shape_2_start_y,
                shape_color,
                2
            );
        } else if (shape == 3) {
            ImGui::DrawList_AddBezierQuadratic(
                list,
                shape_1_start_x, shape_1_start_y,
                shape_1_start_x, shape_1_start_y + shape_height,
                shape_1_start_x + shape_width, shape_1_start_y + shape_height,
                shape_color,
                2
            );

            ImGui::DrawList_AddBezierQuadratic(
                list,
                shape_2_start_x, shape_2_start_y + shape_height,
                shape_2_start_x, shape_2_start_y,
                shape_2_start_x + shape_width, shape_2_start_y,
                shape_color,
                2
            );
        } else if (shape == 4) {
            ImGui::DrawList_AddBezierQuadratic(
                list,
                shape_1_start_x, shape_1_start_y,
                shape_1_start_x + shape_width, shape_1_start_y,
                shape_1_start_x + shape_width, shape_1_start_y + shape_height,
                shape_color,
                2
            );

            ImGui::DrawList_AddBezierQuadratic(
                list,
                shape_2_start_x, shape_2_start_y + shape_height,
                shape_2_start_x + shape_width, shape_2_start_y + shape_height,
                shape_2_start_x + shape_width, shape_2_start_y,
                shape_color,
                2
            );
        } else if (shape == 5) {
            ImGui::DrawList_AddBezierCubic(
                list,
                shape_1_start_x, shape_1_start_y,
                shape_1_start_x + shape_width * tension, shape_1_start_y + shape_height * (1 - tension),
                shape_1_start_x + shape_width * tension, shape_1_start_y + shape_height * (1 - tension),
                shape_1_start_x + shape_width, shape_1_start_y + shape_height,
                shape_color,
                2
            );

            ImGui::DrawList_AddBezierCubic(
                list,
                shape_2_start_x, shape_2_start_y + shape_height,
                shape_2_start_x + shape_width * tension, shape_2_start_y + shape_height * tension,
                shape_2_start_x + shape_width * tension, shape_2_start_y + shape_height * tension,
                shape_2_start_x + shape_width, shape_2_start_y,
                shape_color,
                2
            );
        }
        ImGui::EndChild(m_ctx);
    }

    // The previews are there. Lets add the bezier tension slider. Onluy enabled when the shape is bezier
    ImGui::BeginDisabled(m_ctx, shape != 5);
    ImGui::SetNextItemWidth(m_ctx, shape_box_width);
    ImGui::Text(m_ctx, label.c_str());
    ImGui::SameLine(m_ctx, shape_box_width + 16);

    UiStyledElements::PushReaSonusInputStyle(m_ctx);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushFont(m_ctx, assets->GetMainFont(), 0);
    ImGui::SetNextItemWidth(m_ctx, shape_box_width);
    ImGui::SliderDouble(m_ctx, "##Bezier tension", value, -1, 1, "");
    ImGui::PopFont(m_ctx);
    ImGui::PopStyleVar(m_ctx);
    UiStyledElements::PopReaSonusInputStyle(m_ctx);
    ImGui::EndDisabled(m_ctx);
}

#endif
