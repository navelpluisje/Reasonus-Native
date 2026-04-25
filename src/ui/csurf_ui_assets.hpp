#ifndef CSURF_UI_ASSETS_HPP_
#define CSURF_UI_ASSETS_HPP_

#include <reaper_imgui_functions.h>

enum IconFont {
    IconArrowLeft = 'a',
    IconArrowRight = 'b',
    IconRestore = 'c',
    IconUndo = 'd',
    IconFilter = 'e',
    IconCustomFilter = 'f',
    IconFilterOff = 'g',
    IconAdd = 'h',
    IconAddList = 'i',
    IconAddInPlace = 'j',
    IconSearchAction = 'k',
    IconUser = 'l',
    IconInstrument = 'm',
    IconAction = 'n',
    IconSettings = 'o',
    IconCog = 'p',
    IconInformation = 'q',
    IconRemove = 'r',
    IconDelete = 's',
    IconSearch = 't',
    IconGoTo = 'u',
};

enum FonSize {
    FontSizeDefault = 14,
    FontSizeLarge = 16,
    FontSizePageTitle = 18,
};

class CSurf_UI_Assets {
    ImGui_Context *m_ctx;

    // Fonts
    ImGui_Font *main_font;
    ImGui_Font *main_font_bold;
    ImGui_Font *icon_font;

    // Images
    ImGui_Image *reasonus_logo;
    ImGui_Image *display_mode_0;
    ImGui_Image *display_mode_1;
    ImGui_Image *display_mode_2;
    ImGui_Image *display_mode_3;
    ImGui_Image *display_mode_4;
    ImGui_Image *display_mode_5;
    ImGui_Image *display_mode_6;
    ImGui_Image *display_mode_7;
    ImGui_Image *display_mode_8;
    ImGui_Image *valuebar_mode_normal;
    ImGui_Image *valuebar_mode_bipolar;
    ImGui_Image *valuebar_mode_fill;
    ImGui_Image *valuebar_mode_spread;
    ImGui_Image *valuebar_mode_off;

    // Filters
    ImGui_TextFilter *combo_filter;

    // EEL Functions
    ImGui_Function *onlyDigitsFilter;

public:
    explicit CSurf_UI_Assets(ImGui_Context *m_ctx);

    [[nodiscard]] ImGui_Font *GetMainFont() const;

    [[nodiscard]] ImGui_Font *GetMainFontBold() const;

    [[nodiscard]] ImGui_Font *GetIconFont() const;

    [[nodiscard]] ImGui_Image *GetReaSonusLogo() const;

    [[nodiscard]] ImGui_Image *GetDisplayMode(int display_mode) const;

    [[nodiscard]] ImGui_Image *GetValueBarType(int valuebar_mode) const;

    [[nodiscard]] ImGui_TextFilter *GetReaComboFilter() const;

    [[nodiscard]] ImGui_Function *GetOnlyDigitsFilter() const;
};

#endif
