#ifndef CSURF_FP_UI_TEST_8_H_
#define CSURF_FP_UI_TEST_8_H_

#include <string>
#include "../csurf_ui_page_content.hpp"
#include "../csurf_ui_assets.hpp"
#include "../../i18n/i18n.hpp"
#include "../components/csurf_ui_modal.hpp"

class ReaSonus8ControlPanel {
public:
    // The page id's
    static constexpr int FUNCTIONS_PAGE = 0;
    static constexpr int FILTERS_PAGE = 1;
    static constexpr int MAPPING_PAGE = 2;
    static constexpr int SETTINGS_PAGE = 3;
    static constexpr int ABOUT_PAGE = 4;

    inline static int current_page = 0;
    inline static bool control_panel_open;

    static void Start();

    static void Stop();

    static void SetCurrentPage(int page);

    static void SetPageProperty(int type, int value = 0);

    static void SetPageStringProperty(int type, const std::string &value = "");

    static int GetPageProperty(int type);

    // TODO: Add option for message type to make it also work for info messages and error messages
    static void SetMessage(const std::string &_message);

    void SetLocalMessage(const std::string &_message);

    static std::string GetPageStringProperty(int type);

    ~ReaSonus8ControlPanel();

private:
    static std::unique_ptr<ReaSonus8ControlPanel> fp8_inst;
    CSurf_UI_Assets *assets;

    static void Loop();

    void SetPageContent();

    CSurf_UI_PageContent *page_content = nullptr;

    ReaSonus8ControlPanel();

    void Frame();

    ImGui_Context *m_ctx;
    ReaSonusModal *reasonus_modal;
    int previous_page = -1;
    bool save_clicked = false;
    bool cancel_clicked = false;
    std::vector<std::string> menu_items;

    std::string message;
    int message_timer = 0;

    I18n *i18n = I18n::GetInstance();
};

#endif
