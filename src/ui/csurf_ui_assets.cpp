#include "csurf_ui_assets.hpp"
#include "csurf_ui_assets.h"
#include "../controls/csurf_display_resources.hpp"
#include "../controls/csurf_valuebar_resources.hpp"

CSurf_UI_Assets::CSurf_UI_Assets(ImGui_Context *m_ctx)
    : m_ctx(nullptr) {
    main_font = ImGui::CreateFontFromMem(
        reinterpret_cast<const char *>(reasonus_font_normal_file),
        sizeof(reasonus_font_normal_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font));

    main_font_bold = ImGui::CreateFontFromMem(
        reinterpret_cast<const char *>(
            reasonus_font_bold_file),
        sizeof(reasonus_font_bold_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));

    icon_font = ImGui::CreateFontFromMem(
        reinterpret_cast<const char *>(
            reasonus_icons_file),
        sizeof(reasonus_icons_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_font));

    reasonus_logo = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                  reasonus_logo_file),
                                              sizeof(reasonus_logo_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(reasonus_logo));

    display_mode_0 = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                   reasonus_display_mode_0_file),
                                               sizeof(reasonus_display_mode_0_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(display_mode_0));

    display_mode_1 = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                   reasonus_display_mode_1_file),
                                               sizeof(reasonus_display_mode_1_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(display_mode_1));

    display_mode_2 = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                   reasonus_display_mode_2_file),
                                               sizeof(reasonus_display_mode_2_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(display_mode_2));

    display_mode_3 = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                   reasonus_display_mode_3_file),
                                               sizeof(reasonus_display_mode_3_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(display_mode_3));

    display_mode_4 = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                   reasonus_display_mode_4_file),
                                               sizeof(reasonus_display_mode_4_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(display_mode_4));

    display_mode_5 = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                   reasonus_display_mode_5_file),
                                               sizeof(reasonus_display_mode_5_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(display_mode_5));

    display_mode_6 = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                   reasonus_display_mode_6_file),
                                               sizeof(reasonus_display_mode_6_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(display_mode_6));

    display_mode_7 = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                   reasonus_display_mode_7_file),
                                               sizeof(reasonus_display_mode_7_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(display_mode_7));

    display_mode_8 = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                   reasonus_display_mode_8_file),
                                               sizeof(reasonus_display_mode_8_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(display_mode_8));

    valuebar_type_normal = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                         reasonus_valuebar_normal_file),
                                                     sizeof(reasonus_valuebar_normal_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(valuebar_type_normal));

    valuebar_type_bipolar = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                          reasonus_valuebar_bipolar_file),
                                                      sizeof(reasonus_valuebar_bipolar_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(valuebar_type_bipolar));

    valuebar_type_fill = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                       reasonus_valuebar_fill_file),
                                                   sizeof(reasonus_valuebar_fill_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(valuebar_type_fill));

    valuebar_type_spread = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                         reasonus_valuebar_spread_file),
                                                     sizeof(reasonus_valuebar_spread_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(valuebar_type_spread));

    valuebar_type_off = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(
                                                      reasonus_valuebar_off_file),
                                                  sizeof(reasonus_valuebar_off_file));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(valuebar_type_off));


    combo_filter = ImGui::CreateTextFilter("");
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(combo_filter));

    const char *callback = "EventChar < 48 || EventChar > 57 ? EventChar = 0;";
    onlyDigitsFilter = ImGui::CreateFunctionFromEEL(callback);
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(onlyDigitsFilter));
}

ImGui_Font *CSurf_UI_Assets::GetMainFont() const {
    return main_font;
}

ImGui_Font *CSurf_UI_Assets::GetMainFontBold() const {
    return main_font_bold;
}

ImGui_Font *CSurf_UI_Assets::GetIconFont() const {
    return icon_font;
}

ImGui_Image *CSurf_UI_Assets::GetReaSonusLogo() const {
    return reasonus_logo;
}

ImGui_Image *CSurf_UI_Assets::GetDisplayMode(const int display_mode) const {
    switch (display_mode) {
        case DISPLAY_MODE_0:
            return display_mode_0;

        case DISPLAY_MODE_1:
            return display_mode_1;

        case DISPLAY_MODE_2:
            return display_mode_2;

        case DISPLAY_MODE_3:
            return display_mode_3;

        case DISPLAY_MODE_4:
            return display_mode_4;

        case DISPLAY_MODE_5:
            return display_mode_5;

        case DISPLAY_MODE_6:
            return display_mode_6;

        case DISPLAY_MODE_7:
            return display_mode_7;

        case DISPLAY_MODE_8:
            return display_mode_8;

        default:
            return display_mode_0;
    }
}

ImGui_Image *CSurf_UI_Assets::GetValueBarType(const int value_bar_type) const {
    switch (value_bar_type) {
        case VALUEBAR_MODE_NORMAL:
            return valuebar_type_normal;

        case VALUEBAR_MODE_BIPOLAR:
            return valuebar_type_bipolar;

        case VALUEBAR_MODE_FILL:
            return valuebar_type_fill;

        case VALUEBAR_MODE_SPREAD:
            return valuebar_type_spread;

        case VALUEBAR_MODE_OFF:
            return valuebar_type_off;

        default:
            return valuebar_type_fill;
    }
}


ImGui_TextFilter *CSurf_UI_Assets::GetReaComboFilter() const {
    return combo_filter;
}

ImGui_Function *CSurf_UI_Assets::GetOnlyDigitsFilter() const {
    return onlyDigitsFilter;
}
