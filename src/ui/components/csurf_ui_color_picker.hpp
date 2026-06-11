#ifndef CSURF_FP_UI_COLOR_PICKER_H_
#define CSURF_FP_UI_COLOR_PICKER_H_

#include <reaper_imgui_functions.h>
#include <string>

static void ReaSonusColorPicker(
  ImGui_Context *m_ctx,
  const std::string &label,
  int *selected_color,
  int current_color,
  std::vector<int> color_palette,
  double width = 24,
  double height = 24

) {
  ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::FormFieldBorder);
  /**
   * Add the button to trigger the actual color picker
   */
  if (ImGui::ColorButton(
    m_ctx,
    ("color-picker" + label).c_str(),
    *selected_color,
    ImGui::ColorEditFlags_NoOptions | ImGui::ColorEditFlags_NoTooltip | ImGui::ColorEditFlags_NoAlpha,
    width,
    height
  )) {
    ImGui::OpenPopup(m_ctx, label.c_str());
  }
  ImGui::PopStyleColor(m_ctx);

  if (ImGui::IsItemHovered(m_ctx)) {
    ImGui::SetMouseCursor(m_ctx, ImGui::MouseCursor_Hand);
  }

  UiStyledElements::PushReaSonusContextMenuStyle(m_ctx);
  ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 0, 0);
  ImGui::PushStyleColor(m_ctx, ImGui::Col_Separator, UI_COLORS::Main_38);
  if (ImGui::BeginPopup(m_ctx, label.c_str())) {
    /**
     * Create the color picker header
     */
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    if (ImGui::BeginChild(
      m_ctx,
      "color-picker-label",
      0,
      0,
      ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_AutoResizeX | ImGui::ChildFlags_FrameStyle
    )) {
      ImGui::Text(m_ctx, label.c_str());
      ImGui::EndChild(m_ctx);
    }
    ImGui::PopStyleVar(m_ctx);

    ImGui::Separator(m_ctx);

    /**
     * Create the color picker content
     */
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12, 12);
    if (ImGui::BeginChild(
      m_ctx,
      "color-picker-content",
      0,
      0,
      ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_AutoResizeX | ImGui::ChildFlags_FrameStyle
    )) {
      ImGui::SetNextItemWidth(m_ctx, 200);
      ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 2, 2);
      ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
      ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::FormFieldBorder);
      ImGui::ColorPicker4(
        m_ctx,
        ("##picker" + label).c_str(),
        selected_color,
        ImGui::ColorEditFlags_NoOptions | ImGui::ColorEditFlags_NoSidePreview |
        ImGui::ColorEditFlags_NoSmallPreview | ImGui::ColorEditFlags_NoAlpha
      );
      ImGui::PopStyleColor(m_ctx);
      ImGui::PopStyleVar(m_ctx, 2);

      ImGui::SameLine(m_ctx);

      if (ImGui::BeginChild(m_ctx, "colo-picker-sidebar", 160.0, 0.0)) {
        ImGui::BeginGroup(m_ctx);
        ImGui::Text(m_ctx, "Current");
        ImGui::ColorButton(
          m_ctx,
          "##current",
          *selected_color,
          ImGui::ColorEditFlags_NoPicker | ImGui::ColorEditFlags_NoAlpha,
          60,
          40
        );
        ImGui::EndGroup(m_ctx);

        ImGui::SameLine(m_ctx);

        ImGui::BeginGroup(m_ctx);
        ImGui::Text(m_ctx, "Previous");
        if (ImGui::ColorButton(
          m_ctx,
          "##previous",
          current_color,
          ImGui::ColorEditFlags_NoPicker | ImGui::ColorEditFlags_NoAlpha,
          60,
          40
        )) {
          double r, g, b, a;
          ImGui::ColorConvertU32ToDouble4(current_color, &r, &g, &b, &a);
          logDouble("r", r);
          logDouble("g", g);
          logDouble("b", b);
          logDouble("a", a);
          *selected_color = current_color;
        }
        ImGui::EndGroup(m_ctx);

        ImGui::Separator(m_ctx);

        ImGui::Text(m_ctx, "Palette");
        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 8, 8);
        for (int i = 0; i < static_cast<int>(color_palette.size()); i++) {
          ImGui::PushID(m_ctx, std::to_string(i).c_str());
          if (i % 6 != 0) {
            ImGui::SameLine(m_ctx);
          }

          if (ImGui::ColorButton(
            m_ctx,
            "##palette",
            color_palette[i],
            ImGui::ColorEditFlags_NoAlpha | ImGui::ColorEditFlags_NoPicker | ImGui::ColorEditFlags_NoTooltip,
            20,
            20
          )) {
            *selected_color = color_palette[i];
            ImGui::CloseCurrentPopup(m_ctx);
          }
          ImGui::PopID(m_ctx);
        }
        ImGui::PopStyleVar(m_ctx);
        ImGui::EndChild(m_ctx);
      }
      ImGui::EndChild(m_ctx);
    }
    ImGui::PopStyleVar(m_ctx, 2);

    ImGui::EndPopup(m_ctx);
  }
  ImGui::PopStyleVar(m_ctx);
  ImGui::PopStyleColor(m_ctx);
  UiStyledElements::PopReaSonusContextMenuStyle(m_ctx);
}

#endif
