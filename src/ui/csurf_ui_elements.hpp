#ifndef CSURF_FP_UI_ELEMENTS_H_
#define CSURF_FP_UI_ELEMENTS_H_

#include <reaper_imgui_functions.h>

class UiElements
{
public:
    static void PushReaSonusSidebarStyle(ImGui_Context *m_ctx);

    static void PopReaSonusSidebarStyle(ImGui_Context *m_ctx);

    static void PushReaSonusContentStyle(ImGui_Context *m_ctx);

    static void PopReaSonusContentStyle(ImGui_Context *m_ctx);

    static void PushReaSonusFieldGroupStyle(ImGui_Context *m_ctx);

    static void PopReaSonusFieldGroupStyle(ImGui_Context *m_ctx);

    static void PushReaSonusMenuButtonStyle(ImGui_Context *m_ctx);

    static void PopReaSonusMenuButtonStyle(ImGui_Context *m_ctx);

    static void PushReaSonusButtonStyle(ImGui_Context *m_ctx, ImGui_Font *button_font);

    static void PopReaSonusButtonStyle(ImGui_Context *m_ctx);

    static void PushReaSonusButtonOutlineStyle(ImGui_Context *m_ctx, ImGui_Font *button_font);

    static void PopReaSonusButtonOutlineStyle(ImGui_Context *m_ctx);

    static void PushReaSonusInputStyle(ImGui_Context *m_ctx);

    static void PopReaSonusInputStyle(ImGui_Context *m_ctx);

    static void PushReaSonusComboStyle(ImGui_Context *m_ctx);

    static void PopReaSonusComboStyle(ImGui_Context *m_ctx);

    static void PushReaSonusListBoxStyle(ImGui_Context *m_ctx);

    static void PopReaSonusListBoxStyle(ImGui_Context *m_ctx);

    static void PushReaSonusCheckboxStyle(ImGui_Context *m_ctx);

    static void PopReaSonusCheckboxStyle(ImGui_Context *m_ctx);

    static void PushReaSonusRadioButtonStyle(ImGui_Context *m_ctx);

    static void PopReaSonusRadioButtonStyle(ImGui_Context *m_ctx);

    static void PushReaSonusPageTitleStyle(ImGui_Context *m_ctx);

    static void PopReaSonusPageTitleStyle(ImGui_Context *m_ctx);

    static void PushReaSonusButtonBarContainerStyle(ImGui_Context *m_ctx);

    static void PopReaSonusButtonBarContainerStyle(ImGui_Context *m_ctx);

    static void PushReaSonusFunctionActionStyle(ImGui_Context *m_ctx);

    static void PopReaSonusFunctionActionStyle(ImGui_Context *m_ctx);

    static void PushReaSonusFunctionButtonStyle(ImGui_Context *m_ctx);

    static void PopReaSonusFunctionButtonStyle(ImGui_Context *m_ctx);

    static void PushReaSonusTooltipStyle(ImGui_Context *m_ctx);

    static void PopReaSonusTooltipStyle(ImGui_Context *m_ctx);
};

#endif
