#ifndef CSURF_FP_UI_COLOR_PICKER_H_
#define CSURF_FP_UI_COLOR_PICKER_H_

#include <reaper_imgui_functions.h>
#include <string>

static void ReaSonusColorPicker(
  ImGui_Context *m_ctx,
  const std::string &label,
  int *selected_color,
  int current_color,
  std::vector<int> color_palette
) {
  if (ImGui::ColorButton(
    m_ctx,
    "MyColor##3b",
    *selected_color,
    ImGui::ColorEditFlags_NoOptions | ImGui::ColorEditFlags_NoTooltip | ImGui::ColorEditFlags_NoAlpha
  )) {
    ImGui::OpenPopup(m_ctx, "mypicker");
  }

  if (ImGui::BeginPopup(m_ctx, "mypicker")) {
    ImGui::Text(m_ctx, label.c_str());
    ImGui::Separator(m_ctx);

    ImGui::ColorPicker4(
      m_ctx,
      "##picker",
      selected_color,
      ImGui::ColorEditFlags_NoOptions | ImGui::ColorEditFlags_NoSidePreview |
      ImGui::ColorEditFlags_NoSmallPreview | ImGui::ColorEditFlags_NoAlpha
    );

    ImGui::SameLine(m_ctx);

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
    ImGui::Text(m_ctx, "Previous");

    if (ImGui::ColorButton(
      m_ctx,
      "##previous",
      current_color,
      ImGui::ColorEditFlags_NoPicker | ImGui::ColorEditFlags_NoAlpha,
      60,
      40
    )) {
      *selected_color = current_color;
    }

    ImGui::Separator(m_ctx);
    ImGui::Text(m_ctx, "Palette");

    for (int i = 0; i < static_cast<int>(color_palette.size()); i++) {
      ImGui::PushID(m_ctx, std::to_string(i).c_str());
      if (i % 8 != 0) {
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
      }

      // if (ImGui::BeginDragDropTarget(m_ctx)) {
      //   local drop_color
      //   rv, drop_color = ImGui::AcceptDragDropPayloadRGB(m_ctx);
      //
      //   if (rv) {
      //     widgets.colors.saved_palette[n] = drop_color
      //   }
      //
      //   rv, drop_color = ImGui::AcceptDragDropPayloadRGBA(m_ctx)
      //   if (rv) {
      //     widgets.colors.saved_palette[n] = drop_color >> 8
      //   }
      //   ImGui::EndDragDropTarget(m_ctx);
      // }

      ImGui::PopID(m_ctx);
    }

    ImGui::EndGroup(m_ctx);
    ImGui::EndPopup(m_ctx);
  }
}

#endif
