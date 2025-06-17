#ifndef CSURF_FP_UI_TEST_V2_H_
#define CSURF_FP_UI_TEST_V2_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>
#include <memory>

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
    int m_click_count;
    char m_text[255];
};

#endif