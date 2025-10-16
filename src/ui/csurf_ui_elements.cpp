#include "csurf_ui_elements.hpp"
#include "csurf_ui_colors.hpp"

void UiElements::PushReaSonusWindowStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowPadding, 16, 16);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 16, 16);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
}

void UiElements::PopReaSonusWindowStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 3);
}

void UiElements::PushReaSonusSidebarStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 10, 10);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_18);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_18);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_18);
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

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_15);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_15);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_15);
}

void UiElements::PopReaSonusContentStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiElements::PushReaSonusSettingsContentStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 8, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Transparent);
}

void UiElements::PopReaSonusSettingsContentStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiElements::PushReaSonusGroupStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_18);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_18);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_18);
}

void UiElements::PopReaSonusGroupStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiElements::PushReaSonusFieldGroupStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Transparent);
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

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
}

void UiElements::PopReaSonusMenuButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiElements::PushReaSonusButtonStyle(ImGui_Context *m_ctx, ImGui_Font *button_font)
{
    ImGui::PushFont(m_ctx, button_font, 13);

    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ButtonTextAlign, 0.5, 0.5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 16, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Main_15);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Accent);
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
    ImGui::PushFont(m_ctx, button_font, 13);

    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ButtonTextAlign, 0.5, 0.5);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 16, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, UI_COLORS::Accent_07);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, UI_COLORS::Accent_14);
}

void UiElements::PopReaSonusButtonOutlineStyle(ImGui_Context *m_ctx)
{
    ImGui::PopFont(m_ctx);
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 5);
}

void UiElements::PushReaSonusInputStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::FormFieldBorder);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::FormFieldBg);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::FormFieldBgActive);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::FormFieldBgHover);

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiElements::PopReaSonusInputStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 3);
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
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::FormFieldBorder);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::FormFieldBorder);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::FormFieldBg);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::FormFieldBgActive);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::FormFieldBgHover);
}

void UiElements::PopReaSonusComboStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 8);
    ImGui::PopStyleColor(m_ctx, 7);
}

void UiElements::PushReaSonusListBoxStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_28);

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiElements::PopReaSonusListBoxStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiElements::PushReaSonusCheckboxStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 2, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 16, 16);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 8, 4);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_28);
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

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_28);
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

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Transparent);
}

void UiElements::PopReaSonusPageTitleStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiElements::PushReaSonusButtonBarContainerStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 10, 10);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Transparent);
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

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_28);

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

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, UI_COLORS::Main_28);

    ImGui::PushItemWidth(m_ctx, -1);
};

void UiElements::PopReaSonusFunctionButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 4);
};

void UiElements::PushReaSonusIconButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 4, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, UI_COLORS::Main_28);

    ImGui::PushItemWidth(m_ctx, -1);
};

void UiElements::PopReaSonusIconButtonStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 4);
};

void UiElements::PushReaSonusTooltipStyle(ImGui_Context *m_ctx)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 8);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_15);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_15);
}

void UiElements::PopReaSonusTooltipStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiElements::PushReaSonusTreeNodeStyle(ImGui_Context *m_ctx, bool active)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 4, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 4, 4);

    if (active)
    {
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Header, UI_COLORS::Accent_25);
    }
    else
    {
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Header, UI_COLORS::Main_18);
    }

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiElements::PopReaSonusTreeNodeStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 2);
}

void UiElements::PushReaSonusChannelTabStyle(ImGui_Context *m_ctx, bool active)
{
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 4, 4);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, UI_COLORS::Accent_25);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, UI_COLORS::Accent);
    if (active)
    {
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Accent_25);
    }
    else
    {
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Main_18);
    }

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiElements::PopReaSonusChannelTabStyle(ImGui_Context *m_ctx)
{
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 4);
}
