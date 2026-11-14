#ifndef CSURF_FP_UI_TEXT_ALIGN_H_
#define CSURF_FP_UI_TEXT_ALIGN_H_

#include <reaper_imgui_functions.h>
#include <string>

enum TEXT_ALIGN {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT,
};

static std::string textAlign(
    ImGui_Context *m_ctx,
    std::string label,
    const TEXT_ALIGN alignment
) {
    double available_width;
    double available_height;
    ImGui::GetContentRegionAvail(m_ctx, &available_width, &available_height);

    double text_width;
    double text_height;
    bool full_length = true;

    ImGui::CalcTextSize(m_ctx, label.c_str(), &text_width, &text_height);

    available_width = available_width > 70 ? available_width - 10 : available_width;
    while (text_width > available_width) {
        full_length = false;
        label.pop_back();
        ImGui::CalcTextSize(m_ctx, label.c_str(), &text_width, &text_height);
    }

    if (!full_length) {
        label = label + "...";
    }

    switch (alignment) {
        case ALIGN_CENTER:
            ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + (available_width - text_width) / 2);
            break;

        case ALIGN_RIGHT:
            ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + (available_width - text_width));
            break;

        default: ;
    }

    return label;
}

#endif
