#ifndef CSURF_FP_UI_TEXT_OVERFLOW_H_
#define CSURF_FP_UI_TEXT_OVERFLOW_H_

#include <reaper_imgui_functions.h>
#include <string>

static std::string getTextOverflow(
    ImGui_Context *m_ctx,
    std::string &label,
    const double available_width
) {
    double text_width;
    double text_height;
    bool full_length = true;

    ImGui::CalcTextSize(m_ctx, label.c_str(), &text_width, &text_height);

    while (text_width > available_width) {
        full_length = false;
        label.pop_back();
        ImGui::CalcTextSize(m_ctx, label.c_str(), &text_width, &text_height);
    }

    if (!full_length) {
        label = label + "...";
    }
    
    return label;
}

#endif
