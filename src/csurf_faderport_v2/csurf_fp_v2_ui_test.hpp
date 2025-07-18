#ifndef CSURF_FP_UI_TEST_V2_H_
#define CSURF_FP_UI_TEST_V2_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>
#include <memory>
#include "csurf_fp_v2_ui_settings_page.hpp"

class Example
{
public:
    static void start();
    ~Example();

private:
    static void loop();
    static std::unique_ptr<Example> s_inst;

    Example();
    void frame();

    ImGui_Context *m_ctx;
    char m_text[255];
    CSurf_FP_V2_SettingsPage *settings_page;
};

#endif