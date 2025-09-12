#ifndef CSURF_FP_UI_TEST_V2_H_
#define CSURF_FP_UI_TEST_V2_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>
#include <memory>
#include <string>
#include <vector>
#include "../ui/csurf_ui_page_content.hpp"

class ReaSonusV2ControlPanel
{
public:
    // The page id's
    static const int FUNCTIONS_PAGE = 0;
    static const int SETTINGS_PAGE = 1;
    static const int ABOUT_PAGE = 2;

    inline static int current_page = 0;
    inline static bool control_panel_open = false;

    static void Start(int page = 0);
    static void Stop();

    static void SetCurrentPage(int page);
    ~ReaSonusV2ControlPanel();

private:
    static void Loop();
    static std::unique_ptr<ReaSonusV2ControlPanel> s_inst;
    void InitAssets();
    void SetPageContent();
    CSurf_UI_PageContent *page_content = NULL;

    ReaSonusV2ControlPanel();
    void Frame();

    ImGui_Context *m_ctx;
    ImGui_Font *main_font;
    ImGui_Font *main_font_bold;
    ImGui_Image *logo;
    ImGui_Image *icon_settings;
    ImGui_Image *icon_function_actions;
    ImGui_Image *icon_custom_filters;

    int previous_page = -1;
    bool save_clicked = false;
    bool cancel_clicked = false;
    std::vector<std::string> menu_items;
};

#endif