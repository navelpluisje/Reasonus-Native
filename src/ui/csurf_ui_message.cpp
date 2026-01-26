#include "csurf_ui_message.hpp"
#include "csurf_ui_colors.hpp"
#include "csurf_ui_vars.hpp"
#include "csurf_ui_page_title.hpp"
#include "csurf_ui_button_bar.hpp"
#include "../shared/csurf_utils.hpp"

constexpr const char *g_name{"ReaSonus Native Message"};

std::unique_ptr<ReaSonusMessage> ReaSonusMessage::s_inst;

static void reportError(const ImGui_Error &e)
{
    ShowMessageBox(e.what(), g_name, 0);
}

ReaSonusMessage::ReaSonusMessage()
    : m_ctx{}
{
    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    assets = new CSurf_UI_Assets(m_ctx);

    plugin_register("timer", reinterpret_cast<void *>(&Loop));
}

ReaSonusMessage::~ReaSonusMessage()
{
    plugin_register("-timer", reinterpret_cast<void *>(&Loop));
}

void ReaSonusMessage::Start()
{
    try
    {
        window_open = true;
        if (s_inst)
        {
            ImGui::SetNextWindowFocus(s_inst->m_ctx);
        }
        else
        {
            s_inst.reset(new ReaSonusMessage);
        }
    }
    catch (const ImGui_Error &e)
    {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusMessage::Stop()
{
    try
    {
        window_open = false;
        if (s_inst)
        {
            s_inst.reset();
        }
    }
    catch (const ImGui_Error &e)
    {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusMessage::Loop()
{
    try
    {
        s_inst->Frame();
    }
    catch (const ImGui_Error &e)
    {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusMessage::Frame()
{
    using namespace std::placeholders; // for `_1, _2 etc`

    if (close_clicked)
    {
        close_clicked = false;
        return Stop();
    }

    if (save_clicked)
    {
        save_clicked = false;
    }

    PushReaSonusColors(m_ctx);
    PushReaSonusStyle(m_ctx);
    ImGui::PushFont(m_ctx, assets->GetMainFont(), FontSizeDefault);
    ImGui::SetNextWindowSize(m_ctx, 640, 612, ImGui::Cond_Once);
    bool open{true};

    UiElements::PushReaSonusWindowStyle(m_ctx);
    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse))
    {
        if (ImGui::BeginChild(m_ctx, "logo", 0.0, 52.0, ImGui::ChildFlags_None))
        {
            ImGui::Image(m_ctx, assets->GetReaSonusLogo(), 200, 52);
            ImGui::SameLine(m_ctx);

            ImGui::EndChild(m_ctx); // logo
        }

        UiElements::PushReaSonusGroupStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "actions_container", 0.0, -52.0, ImGui::ChildFlags_None))
        {

            ReaSonusPageTitle(m_ctx, assets, "Languages");

            UiElements::PopReaSonusGroupStyle(m_ctx);
            ImGui::EndChild(m_ctx); // actions_info
        }

        ReaSonusButtonBar(
            m_ctx,
            assets,
            "Open Type Mapping",
            &save_clicked,
            true,
            &close_clicked,
            "Close",
            &save_message);

        UiElements::PopReaSonusWindowStyle(m_ctx);

        ImGui::End(m_ctx); // window
    }

    ImGui::PopFont(m_ctx);
    PopReaSonusColors(m_ctx);
    PopReaSonusStyle(m_ctx);

    if (!open)
    {
        window_open = false;
        SetActionState("_REASONUS_TRANSLATIONN_EDITOR");
        return s_inst.reset();
    }
}