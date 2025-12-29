#ifndef CSURF_UI_ASSETS_HPP_
#define CSURF_UI_ASSETS_HPP_

#include <reaper_imgui_functions.h>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include "../shared/csurf_utils.hpp"

enum IconFont
{
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
    IconSearch = 'k',
    IconUser = 'l',
    IconInstrument = 'm',
    IconAction = 'n',
    IconSettings = 'o',
    IconCog = 'p',
    IconInformation = 'q',
    IconRemove = 'r',
    IconDelete = 's',
};

class CSurf_UI_Assets
{
private:
    ImGui_Context *m_ctx;

    // Fonts
    ImGui_Font *main_font;
    ImGui_Font *main_font_bold;
    ImGui_Font *icon_font;

    // Images
    ImGui_Image *reasonus_logo;

public:
    CSurf_UI_Assets(ImGui_Context *m_ctx);

    ImGui_Font *GetMainFont();
    ImGui_Font *GetMainFontBold();
    ImGui_Font *GetIconFont();

    ImGui_Image *GetReaSonusLogo();
};

#endif