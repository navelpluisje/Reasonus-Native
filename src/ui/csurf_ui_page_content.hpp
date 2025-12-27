#ifndef CSURF_FP_UI_ABOUT_PAGE_V2_H_
#define CSURF_FP_UI_ABOUT_PAGE_V2_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>
#include <mini/ini.h>
#include "csurf_ui_assets.hpp"
#include "../i18n/i18n.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_reasonus_settings.hpp"

class CSurf_UI_PageContent
{
protected:
    ImGui_Context *m_ctx;
    mINI::INIStructure ini;
    ReaSonusSettings *settings;
    CSurf_UI_Assets *assets;
    I18n *i18n;

public:
    CSurf_UI_PageContent(ImGui_Context *m_ctx, CSurf_UI_Assets *assets) : m_ctx(m_ctx), assets(assets) {};

    virtual ~CSurf_UI_PageContent() {};

    virtual void Start() {};

    virtual void Render() {};

    virtual void Save() {};

    virtual void Reset() {};

    virtual void SetPageProperty(int type, int value)
    {
        (void)type;
        (void)value;
    };

    virtual void SetPageProperty(int type, std::string value)
    {
        (void)type;
        (void)value;
    };

    virtual int GetPageProperty(int type)
    {
        (void)type;
        return -1;
    };

    virtual std::string GetPageStringProperty(int type)
    {
        (void)type;
        return "";
    };
};

#endif