#include "./csurf_fp_v2_ui_page_content.hpp"

#include <reaper_imgui_functions.h>

class CSurf_FP_V2_AboutPage : public CSurf_FP_V2_PageContent
{
protected:
public:
    CSurf_FP_V2_AboutPage(ImGui_Context *m_ctx) : CSurf_FP_V2_PageContent(m_ctx) {};

    virtual ~CSurf_FP_V2_AboutPage() {};

    void render() override
    {
        ImGui::Text(m_ctx, "AboutPage");
    }
};
