#ifndef CSURF_FP_UI_COLORS_H_
#define CSURF_FP_UI_COLORS_H_

#include <reaper_imgui_functions.h>

namespace UI_COLORS
{
    const int White = 0xffffffff;
    const int Main_15 = 0x272727ff;
    const int Main_18 = 0x2e2e2eff;
    const int Main_23 = 0x383A3CFF;
    const int Main_28 = 0x45474Aff;
    const int Main_38 = 0x5C6164ff;
    const int Accent = 0xf7cb15ff;
    const int Accent_07 = 0xF7CB1512;
    const int Accent_14 = 0xF7CB1523;
    const int Accent_25 = 0xF7CB153f;
    const int Accent_50 = 0xF7CB157F;
    const int Success = 0x58D23AFF;
    const int Success_50 = 0x58D23A7F;
    const int Transparent = 0x00000000;

    const int FormFieldBorder = UI_COLORS::Main_38;
    const int FormFieldBg = UI_COLORS::Main_23;
    const int FormFieldBgHover = UI_COLORS::Main_28;
    const int FormFieldBgActive = UI_COLORS::Main_28;
}

/**
 * @brief When using the mac colour picker use Generic RGB
 *
 * @param m_ctx
 */
static void PushReaSonusColors(ImGui_Context *m_ctx)
{
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::White);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TextDisabled, 0x808080FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_WindowBg, UI_COLORS::Main_15);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ChildBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PopupBg, 0x141414F0);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0x6E6E8080);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_BorderShadow, 0xff000000);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBg, 0x294A7A8A);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgHovered, 0x4296FA66);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_FrameBgActive, 0x4296FAAB);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TitleBg, 0x252525FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TitleBgActive, 0x292929ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TitleBgCollapsed, 0x292929ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_MenuBarBg, 0x242424FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ScrollbarBg, 0x05050587);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ScrollbarGrab, 0x45474A7f);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ScrollbarGrabHovered, 0x45474A7f);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ScrollbarGrabActive, UI_COLORS::Main_28);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_CheckMark, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_SliderGrab, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_SliderGrabActive, UI_COLORS::Accent_50);

    // Button
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, 0x4296FA66);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, 0xF55D3EFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, 0x0F87FAFF);

    // Header
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Header, UI_COLORS::Accent_50);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_HeaderHovered, UI_COLORS::Accent_25);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_HeaderActive, UI_COLORS::Accent);

    // Separator
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Separator, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_SeparatorHovered, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_SeparatorActive, UI_COLORS::Accent);

    // resize Grip
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ResizeGrip, UI_COLORS::Accent_14);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ResizeGripHovered, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ResizeGripActive, UI_COLORS::Accent_07);

    // Tabs
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabHovered, UI_COLORS::Accent_50);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Tab, UI_COLORS::Accent_25);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabSelected, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabSelectedOverline, UI_COLORS::Accent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabDimmed, 0x111A26F8);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabDimmedSelected, 0x23436CFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabDimmedSelectedOverline, 0x808080FF);

    // Docking
    ImGui::PushStyleColor(m_ctx, ImGui::Col_DockingPreview, UI_COLORS::Accent_50);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_DockingEmptyBg, 0x333333FF);

    // Plots
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PlotLines, 0x9C9C9CFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PlotLinesHovered, 0xFF6E59FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PlotHistogram, 0xE6B300FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PlotHistogramHovered, 0xFF9900FF);

    // Table
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TableHeaderBg, 0x303033FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TableBorderStrong, 0x4F4F59FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TableBorderLight, 0x3B3B40FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TableRowBg, UI_COLORS::Transparent);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TableRowBgAlt, 0xFFFFFF0F);

    ImGui::PushStyleColor(m_ctx, ImGui::Col_TextSelectedBg, 0x4296FA59);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_DragDropTarget, 0xFFFF00E6);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_NavCursor, 0x4296FAFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_NavWindowingHighlight, 0xFFFFFFB3);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_NavWindowingDimBg, 0xCCCCCC33);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ModalWindowDimBg, 0xCCCCCC59);
}

static void PopReaSonusColors(ImGui_Context *m_ctx)
{
    ImGui::PopStyleColor(m_ctx, 57);
}

#endif
