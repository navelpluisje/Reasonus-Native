#ifndef CSURF_FP_UI_ABOUT_PAGE_V2_H_
#define CSURF_FP_UI_ABOUT_PAGE_V2_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>

class CSurf_FP_V2_PageContent
{
protected:
    ImGui_Context *m_ctx;

public:
    CSurf_FP_V2_PageContent(ImGui_Context *m_ctx) : m_ctx(m_ctx) {};

    virtual ~CSurf_FP_V2_PageContent() {};

    virtual void start() {};

    virtual void render() {};

    virtual void save()
    {
        ShowConsoleMsg("Save Pressed");
    };

    virtual void reset()
    {
        ShowConsoleMsg("Reset Pressed");
    };
};

#endif