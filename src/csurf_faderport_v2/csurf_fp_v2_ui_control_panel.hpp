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
    inline static bool control_panel_open = false;
    inline static int current_page = 0;
    static void start(int page = 0);
    static void stop();
    ~ReaSonusV2ControlPanel();

private:
    static void loop();
    static std::unique_ptr<ReaSonusV2ControlPanel> s_inst;
    void initAssets();
    CSurf_UI_PageContent *page_content = NULL;

    ReaSonusV2ControlPanel();
    void frame();

    ImGui_Context *m_ctx;
    ImGui_Font *main_font;
    ImGui_Font *main_font_bold;
    ImGui_Font *menu_font;
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