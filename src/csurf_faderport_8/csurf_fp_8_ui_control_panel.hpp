#ifndef CSURF_FP_UI_TEST_8_H_
#define CSURF_FP_UI_TEST_8_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>
#include <memory>
#include <string>
#include "../ui/csurf_ui_page_content.hpp"

class ReaSonus8ControlPanel
{
public:
    inline static int current_page = 0;
    inline static bool control_panel_open;
    static void start();
    static void stop();
    ~ReaSonus8ControlPanel();

private:
    static void loop();
    static std::unique_ptr<ReaSonus8ControlPanel> fp8_inst;
    void initAssets();
    CSurf_UI_PageContent *page_content = NULL;

    ReaSonus8ControlPanel();
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