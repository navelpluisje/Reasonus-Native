#include "csurf_ui_elements.hpp"

void UiElements::PushReaSonusSidebarStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 10, 10);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x2e2e2eff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x2e2e2eff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x2e2e2eff);
}

void UiElements::PopReaSonusSidebarStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiElements::PushReaSonusContentStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x272727ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x272727ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x272727ff);
}

void UiElements::PopReaSonusContentStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiElements::PushReaSonusFieldGroupStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x00000000);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x00000000);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x00000000);
}

void UiElements::PopReaSonusFieldGroupStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiElements::PushReaSonusMenuButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ButtonTextAlign, 0.0, 0.5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, 0x4f4f4fff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, 0x5a5a5aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, 0x5a5a5aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0xF7CB15FF);
}

void UiElements::PopReaSonusMenuButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiElements::PushReaSonusButtonStyle(ImGui_Context *m_ctx, ImGui_Font *button_font)
{
    ImGui::PushFont(m_ctx, button_font);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ButtonTextAlign, 0.5, 0.5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 16, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, 0x272727ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0xF7CB15FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, 0xF7CB15FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, 0xF7CB15bF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, 0xF7CB15dF);
}

void UiElements::PopReaSonusButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PopFont(m_ctx);
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 5);
}

void UiElements::PushReaSonusButtonOutlineStyle(ImGui_Context *m_ctx, ImGui_Font *button_font)
{
    ImGui::PushFont(m_ctx, button_font);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ButtonTextAlign, 0.5, 0.5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 16, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, 0xF7CB15FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0xF7CB15FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, 0xF7CB1500);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, 0xF7CB1512);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, 0xF7CB1523);
}

void UiElements::PopReaSonusButtonOutlineStyle(ImGui_Context *m_ctx)
{
    ImGui::PopFont(m_ctx);
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 5);
}

void UiElements::PushReaSonusInputStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ButtonTextAlign, 0.5, 0.5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0x5C6164ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x383A3Cff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x45474Aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x45474Aff);

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiElements::PopReaSonusInputStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiElements::PushReaSonusComboStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    // The select list
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 10, 10);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowPadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_PopupRounding, 4);
    // Combo colors
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, 0xFFFFFFFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0x5C6164ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, 0x5C6164ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, 0xF7CB15FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, 0xF7CB15FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x383A3Cff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x45474Aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x45474Aff);
    // The select list colors
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PopupBg, 0x272727ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Header, 0xF7CB157F);
    // ImGui::PushStyleColor(m_ctx, ImGui::Col_HeaderHovered, 0xF7CB153F);
    // ImGui::PushStyleColor(m_ctx, ImGui::Col_HeaderActive, 0xF7CB15FF);

    // ImGui::PushItemWidth(m_ctx, -1);
}

void UiElements::PopReaSonusComboStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 8);
    ImGui::PopStyleColor(m_ctx, 10);
}

void UiElements::PushReaSonusListBoxStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 10, 10);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, 0xFFFFFFFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0x5C6164ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x383A3Cff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x45474Aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x45474Aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Header, 0xF7CB157F);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_HeaderHovered, 0xF7CB153F);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_HeaderActive, 0xF7CB15FF);

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiElements::PopReaSonusListBoxStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 8);
}

void UiElements::PushReaSonusCheckboxStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 2, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 16, 16);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 8, 4);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0x5C6164ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x383A3Cff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x45474Aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x45474Aff);
}

void UiElements::PopReaSonusCheckboxStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 5);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiElements::PushReaSonusRadioButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 2, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 0, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 8, 0);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0x5C6164ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x383A3Cff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x45474Aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x45474Aff);
}

void UiElements::PopReaSonusRadioButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiElements::PushReaSonusPageTitleStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 0, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 8, 8);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Separator, 0xF7CB15FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, 0xF7CB15FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x00000000);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x00000000);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x00000000);
}

void UiElements::PopReaSonusPageTitleStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 5);
}

void UiElements::PushReaSonusButtonBarContainerStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 10, 10);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x00000000);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x00000000);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x00000000);
}

void UiElements::PopReaSonusButtonBarContainerStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiElements::PushReaSonusFunctionActionStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12, 12);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0x5C6164ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x383A3Cff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x45474Aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x45474Aff);

    ImGui::PushItemWidth(m_ctx, -1);
};

void UiElements::PopReaSonusFunctionActionStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
};

void UiElements::PushReaSonusFunctionButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0x5C6164ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, 0x383A3Cff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, 0x45474Aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, 0x45474Aff);

    ImGui::PushItemWidth(m_ctx, -1);
};

void UiElements::PopReaSonusFunctionButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 4);
};

void UiElements::PushReaSonusTooltipStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0x5C6164ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_BorderShadow, 0xff0000ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x272727ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x272727ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x272727ff);
}

void UiElements::PopReaSonusTooltipStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 5);
}
