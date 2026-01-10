#include "csurf_ui_assets.hpp"
#include "csurf_ui_assets.h"

CSurf_UI_Assets::CSurf_UI_Assets(ImGui_Context *m_ctx)
{
    main_font = ImGui::CreateFontFromMem(reinterpret_cast<const char *>(reasonus_font_normal_file), sizeof(reasonus_font_normal_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font));

    main_font_bold = ImGui::CreateFontFromMem(reinterpret_cast<const char *>(reasonus_font_bold_file), sizeof(reasonus_font_bold_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));

    icon_font = ImGui::CreateFontFromMem(reinterpret_cast<const char *>(reasonus_icons_file), sizeof(reasonus_icons_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_font));

    reasonus_logo = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(reasonus_logo_file), sizeof(reasonus_logo_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(reasonus_logo));

    combo_filter = ImGui::CreateTextFilter("");
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(combo_filter));
}

ImGui_Font *CSurf_UI_Assets::GetMainFont()
{
    return main_font;
}

ImGui_Font *CSurf_UI_Assets::GetMainFontBold()
{

    return main_font_bold;
}

ImGui_Font *CSurf_UI_Assets::GetIconFont()
{
    return icon_font;
}

ImGui_Image *CSurf_UI_Assets::GetReaSonusLogo()
{
    return reasonus_logo;
}

ImGui_TextFilter *CSurf_UI_Assets::GetReaComboFilter()
{
    return combo_filter;
}
