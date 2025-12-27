#ifndef CSURF_FP_UI_TEST_V2_H_
#define CSURF_FP_UI_TEST_V2_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>
#include <memory>
#include <string>
#include <vector>
#include "../ui/csurf_ui_page_content.hpp"
#include "../ui/csurf_ui_assets.hpp"
#include "../i18n/i18n.hpp"

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
    static void SetMessage(std::string message);
    void SetLocalMessage(std::string message);

    ~ReaSonusV2ControlPanel();

private:
    static std::unique_ptr<ReaSonusV2ControlPanel> s_inst;
    CSurf_UI_Assets *assets;

    static void Loop();
    void InitAssets();
    void SetPageContent();
    CSurf_UI_PageContent *page_content = NULL;

    ReaSonusV2ControlPanel();
    void Frame();

    ImGui_Context *m_ctx;

    int previous_page = -1;
    bool save_clicked = false;
    bool cancel_clicked = false;
    std::vector<std::string> menu_items;

    std::string message = "";
    int message_timer = 0;

    I18n *i18n = I18n::GetInstance();
};

#endif