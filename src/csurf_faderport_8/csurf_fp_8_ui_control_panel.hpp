#ifndef CSURF_FP_UI_TEST_8_H_
#define CSURF_FP_UI_TEST_8_H_

#include <string>
#include "../ui/csurf_ui_page_content.hpp"
#include "../ui/csurf_ui_assets.hpp"
#include "../i18n/i18n.hpp"

class ReaSonus8ControlPanel
{
public:
    // The page id's
    static const int FUNCTIONS_PAGE = 0;
    static const int FILTERS_PAGE = 1;
    static const int MAPPING_PAGE = 2;
    static const int SETTINGS_PAGE = 3;
    static const int ABOUT_PAGE = 4;

    inline static int current_page = 0;
    inline static bool control_panel_open;

    static void Start();
    static void Stop();

    static void SetCurrentPage(int page);
    static void SetPageProperty(int type, int value = 0);
    static void SetPageStringProperty(int type, std::string value = "");
    static int GetPageProperty(int type);
    static void SetMessage(std::string message);
    void SetLocalMessage(std::string message);
    static std::string GetPageStringProperty(int type);
    ~ReaSonus8ControlPanel();

private:
    static std::unique_ptr<ReaSonus8ControlPanel> fp8_inst;
    CSurf_UI_Assets *assets;

    static void Loop();
    void SetPageContent();
    CSurf_UI_PageContent *page_content = NULL;

    ReaSonus8ControlPanel();
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