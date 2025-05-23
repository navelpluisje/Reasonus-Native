#include "csurf_fp_v2_ui_test.hpp"
#include "reaper_imgui_functions.h"

constexpr const char *g_name{"ReaImGui C++ example"};
static int g_actionId;

std::unique_ptr<Example> Example::s_inst;

static void reportError(const ImGui_Error &e)
{
    ShowMessageBox(e.what(), g_name, 0);
}

Example::Example()
    : m_ctx{}, m_click_count{},
      m_text{"The quick brown fox jumps over the lazy dog"}
{
    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    plugin_register("timer", reinterpret_cast<void *>(&loop));
}

Example::~Example()
{
    plugin_register("-timer", reinterpret_cast<void *>(&loop));
}

void Example::start()
try
{
    if (s_inst)
        ImGui::SetNextWindowFocus(s_inst->m_ctx);
    else
        s_inst.reset(new Example);
}
catch (const ImGui_Error &e)
{
    reportError(e);
    s_inst.reset();
}

void Example::loop()
try
{
    s_inst->frame();
}
catch (const ImGui_Error &e)
{
    reportError(e);
    s_inst.reset();
}

void Example::frame()
{
    ImGui::SetNextWindowSize(m_ctx, 400, 80, ImGui::Cond_FirstUseEver);

    bool open{true};
    if (ImGui::Begin(m_ctx, g_name, &open))
    {
        if (ImGui::Button(m_ctx, "Click me!"))
            ++m_click_count;

        if (m_click_count & 1)
        {
            ImGui::SameLine(m_ctx);
            ImGui::Text(m_ctx, R"(\o/)");
        }

        ImGui::InputText(m_ctx, "text input", m_text, sizeof(m_text));
        ImGui::End(m_ctx);
    }

    if (!open)
        return s_inst.reset();
}