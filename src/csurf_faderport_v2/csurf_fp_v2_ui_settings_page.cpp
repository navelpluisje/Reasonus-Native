#include "./csurf_fp_v2_ui_settings_page.hpp"

#include <reaper_imgui_functions.h>
#include "../ui/csurf_ui_page_title.hpp"
#include "../ui/csurf_ui_assets.hpp"

/**
 * Private Methods
 */
/**
 * Public Methods
 */
void CSurf_FP_V2_SettingsPage::start() {}

void CSurf_FP_V2_SettingsPage::render()
{
    ReaSonusPageTitle(ctx, "Settings", menu_font);
}

void CSurf_FP_V2_SettingsPage::save() {}

void CSurf_FP_V2_SettingsPage::reset() {}

CSurf_FP_V2_SettingsPage::CSurf_FP_V2_SettingsPage(ImGui_Context *m_ctx)
{
    ctx = m_ctx;
}

CSurf_FP_V2_SettingsPage ::~CSurf_FP_V2_SettingsPage()
{
}
