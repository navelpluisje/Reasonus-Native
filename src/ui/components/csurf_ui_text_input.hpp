#ifndef CSURF_FP_UI_TEXT_INPUT_H_
#define CSURF_FP_UI_TEXT_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include "../csurf_ui_styles_elements.hpp"

static void ReaSonusTextInput(
    ImGui_Context *m_ctx,
    const std::string &label,
    std::string *value,
    const std::string &hint,
    double width,
    const bool select
) {
    static char text[255]; // NOLINT(*-avoid-c-arrays)
    strcpy(text, value->c_str()); // NOLINT(*-security.insecureAPI.strcpy)

    UiStyledElements::PushReaSonusFieldGroupStyle(m_ctx);
    if (ImGui::BeginChild(m_ctx, ("container" + label).c_str(), width, 0.0,
                          ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
        ImGui::Text(m_ctx, label.c_str());

        UiStyledElements::PushReaSonusInputStyle(m_ctx);
        if (select) {
            ImGui::SetKeyboardFocusHere(m_ctx);
        }
        if (ImGui::InputTextWithHint(m_ctx, ("##" + label).c_str(), hint.c_str(), text, sizeof(text))) {
            *value = std::string(text);
        }

        UiStyledElements::PopReaSonusInputStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
    UiStyledElements::PopReaSonusFieldGroupStyle(m_ctx);
}

#endif
