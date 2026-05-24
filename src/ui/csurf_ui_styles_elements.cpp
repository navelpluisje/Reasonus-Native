#include "csurf_ui_styles_elements.hpp"
#include "csurf_ui_colors.hpp"
#include "csurf_ui_assets.hpp"

void UiStyledElements::PushReaSonusWindowStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_WindowPadding, 16, 16);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 16, 16);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
}

void UiStyledElements::PopReaSonusWindowStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 3);
}

void UiStyledElements::PushReaSonusSidebarStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 10, 10);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_18);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_18);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_18);
}

void UiStyledElements::PopReaSonusSidebarStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiStyledElements::PushReaSonusContentStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_15);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_15);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_15);
}

void UiStyledElements::PopReaSonusContentStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiStyledElements::PushReaSonusSettingsContentStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 8, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Transparent);
}

void UiStyledElements::PopReaSonusSettingsContentStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiStyledElements::PushReaSonusGroupStyle(ImGui_Context *m_ctx, bool in_group) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, !in_group ? UI_COLORS::Main_18 : UI_COLORS::Main_15);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_18);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_18);
}

void UiStyledElements::PopReaSonusGroupStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusFieldGroupStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Transparent);
}

void UiStyledElements::PopReaSonusFieldGroupStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiStyledElements::PushReaSonusButtonStyle(ImGui_Context *m_ctx, ImGui_Font *button_font) {
    ImGui::PushFont(m_ctx, button_font, FontSizeDefault);

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

void UiStyledElements::PopReaSonusButtonStyle(ImGui_Context *m_ctx) {
    ImGui::PopFont(m_ctx);
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 5);
}

void UiStyledElements::PushReaSonusButtonOutlineStyle(ImGui_Context *m_ctx, ImGui_Font *button_font) {
    ImGui::PushFont(m_ctx, button_font, FontSizeDefault);

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

void UiStyledElements::PopReaSonusButtonOutlineStyle(ImGui_Context *m_ctx) {
    ImGui::PopFont(m_ctx);
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 5);
}

void UiStyledElements::PushReaSonusInputStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::FormFieldBorder);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::FormFieldBg);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::FormFieldBgActive);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::FormFieldBgHover);

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiStyledElements::PopReaSonusInputStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusButtonBarMessageStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 16, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Success);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Success_50);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Success_50);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Success_50);

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiStyledElements::PopReaSonusButtonBarMessageStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusComboStyle(ImGui_Context *m_ctx) {
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

void UiStyledElements::PopReaSonusComboStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 8);
    ImGui::PopStyleColor(m_ctx, 7);
}

void UiStyledElements::PushReaSonusListBoxStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_28);

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiStyledElements::PopReaSonusListBoxStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusCheckboxStyle(ImGui_Context *m_ctx) {
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

void UiStyledElements::PopReaSonusCheckboxStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 5);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusRadioButtonStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 2, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 0, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 8, 0);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_28);
}

void UiStyledElements::PopReaSonusRadioButtonStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusPageTitleStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 0, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 8, 8);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Transparent);
}

void UiStyledElements::PopReaSonusPageTitleStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusButtonBarContainerStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 0);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 10, 10);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Transparent);
}

void UiStyledElements::PopReaSonusButtonBarContainerStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 3);
}

void UiStyledElements::PushReaSonusFunctionActionStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12, 12);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_28);

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiStyledElements::PopReaSonusFunctionActionStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusFunctionButtonStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, UI_COLORS::Main_28);

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiStyledElements::PopReaSonusFunctionButtonStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 5);
}

void UiStyledElements::PushReaSonusIconButtonStyle(ImGui_Context *m_ctx, const CSurf_UI_Assets *assets) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 4, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, UI_COLORS::Main_28);

    ImGui::PushItemWidth(m_ctx, -1);

    ImGui::PushFont(m_ctx, assets->GetIconFont(), 20);
}

void UiStyledElements::PopReaSonusIconButtonStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 5);
    ImGui::PopFont(m_ctx);
}

void UiStyledElements::PushReaSonusTooltipStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 12, 12);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 2);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 8);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::White);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_23);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, UI_COLORS::Main_15);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, UI_COLORS::Main_15);
}

void UiStyledElements::PopReaSonusTooltipStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 5);
}

void UiStyledElements::PushReaSonusTreeNodeStyle(ImGui_Context *m_ctx, const bool active) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 4, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 4, 4);

    if (active) {
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Header, UI_COLORS::Accent_25);
    } else {
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Header, UI_COLORS::Main_18);
    }

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiStyledElements::PopReaSonusTreeNodeStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 4);
    ImGui::PopStyleColor(m_ctx, 2);
}

void UiStyledElements::PushReaSonusChannelTabStyle(ImGui_Context *m_ctx, const bool active) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemInnerSpacing, 4, 4);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, UI_COLORS::Accent_25);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, UI_COLORS::Accent);
    if (active) {
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Accent_25);
    } else {
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Main_38);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, UI_COLORS::Main_18);
    }

    ImGui::PushItemWidth(m_ctx, -1);
}

void UiStyledElements::PopReaSonusChannelTabStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusTabBarStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_TabBarBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_TabRounding, 4);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 16, 8);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Separator, UI_COLORS::Accent);
}

void UiStyledElements::PopReaSonusTabBarStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 3);
    ImGui::PopStyleColor(m_ctx, 1);
}

void UiStyledElements::PushReaSonusTabStyle(ImGui_Context *m_ctx, const bool selected) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_TabBorderSize, selected ? 2 : 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
    if (selected) {
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Tab, UI_COLORS::Accent_25);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_TabSelected, UI_COLORS::Accent_25);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_TabHovered, UI_COLORS::Accent_50);
    } else {
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Tab, UI_COLORS::Main_28);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_TabSelected, UI_COLORS::Main_38);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_TabHovered, UI_COLORS::Main_38);
    }
}

void UiStyledElements::PopReaSonusTabStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 1);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusTranslationItemStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Main_18);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::FormFieldBorder);
}

void UiStyledElements::PopReaSonusTranslationItemStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 1);
    ImGui::PopStyleColor(m_ctx, 2);
}

void UiStyledElements::PushReaSonusModalStyle(ImGui_Context *m_ctx) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameRounding, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ChildRounding, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_PopupRounding, 8);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_PopupBorderSize, 1);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PopupBg, UI_COLORS::Main_15);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ModalWindowDimBg, UI_COLORS::Transparent);
}

void UiStyledElements::PopReaSonusModalStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 5);
    ImGui::PopStyleColor(m_ctx, 4);
}

void UiStyledElements::PushReaSonusImageButtonStyle(ImGui_Context *m_ctx, const bool selected) {
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 8, 8);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, selected ? UI_COLORS::Accent : UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, selected ? UI_COLORS::Accent_25 : UI_COLORS::Main_38);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, selected ? UI_COLORS::Accent_50 : UI_COLORS::Main_48);
}

void UiStyledElements::PopReaSonusImageButtonStyle(ImGui_Context *m_ctx) {
    ImGui::PopStyleVar(m_ctx, 2);
    ImGui::PopStyleColor(m_ctx, 4);
}

