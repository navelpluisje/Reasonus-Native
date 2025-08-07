#ifndef CSURF_FP_UI_COLORS_H_
#define CSURF_FP_UI_COLORS_H_

#include <reaper_imgui_functions.h>

/**
 * @brief When using the mac colour picker use Generic RGB
 *
 * @param m_ctx
 */
static void PushReaSonusColors(ImGui_Context *m_ctx)
{
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, 0xFFFFFFFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TextDisabled, 0x808080FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_WindowBg, 0x272727ff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ChildBg, 0x00000000);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PopupBg, 0x141414F0);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Border, 0x6E6E8080);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_BorderShadow, 0x00000000);
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
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ScrollbarGrabActive, 0x45474Aff);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_CheckMark, 0xF7CB15FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_SliderGrab, 0x3D85E0FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_SliderGrabActive, 0x4296FAFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Button, 0x4296FA66);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonHovered, 0xF55D3EFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ButtonActive, 0x0F87FAFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Header, 0x4296FA4F);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_HeaderHovered, 0x4296FACC);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_HeaderActive, 0x4296FAFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Separator, 0x6E6E8080);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_SeparatorHovered, 0x1A66BFC7);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_SeparatorActive, 0x1A66BFFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ResizeGrip, 0x4296FA33);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ResizeGripHovered, 0x4296FAAB);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ResizeGripActive, 0x4296FAF2);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabHovered, 0x4296FACC);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_Tab, 0x2E5994DC);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabSelected, 0x3369ADFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabSelectedOverline, 0x4296FAFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabDimmed, 0x111A26F8);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabDimmedSelected, 0x23436CFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TabDimmedSelectedOverline, 0x808080FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_DockingPreview, 0x4296FAB3);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_DockingEmptyBg, 0x333333FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PlotLines, 0x9C9C9CFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PlotLinesHovered, 0xFF6E59FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PlotHistogram, 0xE6B300FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_PlotHistogramHovered, 0xFF9900FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TableHeaderBg, 0x303033FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TableBorderStrong, 0x4F4F59FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TableBorderLight, 0x3B3B40FF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TableRowBg, 0x00000000);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TableRowBgAlt, 0xFFFFFF0F);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_TextSelectedBg, 0x4296FA59);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_DragDropTarget, 0xFFFF00E6);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_NavHighlight, 0x4296FAFF);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_NavWindowingHighlight, 0xFFFFFFB3);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_NavWindowingDimBg, 0xCCCCCC33);
    ImGui::PushStyleColor(m_ctx, ImGui::Col_ModalWindowDimBg, 0xCCCCCC59);
}

static void PopReaSonusColors(ImGui_Context *m_ctx)
{
    ImGui::PopStyleColor(m_ctx, 57);
}

#endif
