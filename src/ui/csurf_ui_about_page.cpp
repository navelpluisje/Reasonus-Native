#include "csurf_ui_page_content.hpp"
#include <string.h>
#include <reaper_imgui_functions.h>

class CSurf_UI_AboutPage : public CSurf_UI_PageContent
{
protected:
    std::string device;

public:
    CSurf_UI_AboutPage(ImGui_Context *m_ctx, std::string _device) : CSurf_UI_PageContent(m_ctx, _device)
    {
        device = _device;
    };

    virtual ~CSurf_UI_AboutPage() {};

    void render() override
    {
        ImGui::Text(m_ctx, "AboutPage");
    }
};
