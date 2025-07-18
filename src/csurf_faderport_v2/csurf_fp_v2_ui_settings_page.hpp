#ifndef CSURF_FP_UI_FUNCTIONS_PAGE_V2_H_
#define CSURF_FP_UI_FUNCTIONS_PAGE_V2_H_

#include <reaper_imgui_functions.h>

class CSurf_FP_V2_SettingsPage
{
private:
    ImGui_Context *ctx;

public:
    void start();
    void render();
    void save();
    void reset();
    CSurf_FP_V2_SettingsPage(ImGui_Context *m_ctx);
    ~CSurf_FP_V2_SettingsPage();
};

#endif