#include "config.h"
#include "csurf_ui_plugin_type_mapping.hpp"
#include "csurf_ui_message.hpp"
#include "csurf_ui_colors.hpp"
#include "csurf_ui_vars.hpp"
#include "csurf_ui_page_title.hpp"
#include "csurf_ui_button_bar.hpp"
#include "../shared/csurf_utils.hpp"

constexpr const char *g_name{"ReaSonus Native New Version"};

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
    double width, height;

    if (close_clicked)
    {
        close_clicked = false;
        return Stop();
    }

    if (save_clicked)
    {
        save_clicked = false;
        ReaSonusPluginTypeMapping::Start();
    }

    PushReaSonusColors(m_ctx);
    PushReaSonusStyle(m_ctx);
    ImGui::PushFont(m_ctx, assets->GetMainFont(), FontSizeDefault);
    bool open{true};

    UiElements::PushReaSonusWindowStyle(m_ctx);
    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_AlwaysAutoResize))
    {
        if (ImGui::BeginChild(m_ctx, "logo", 640.0, 52.0, ImGui::ChildFlags_None))
        {
            ImGui::Image(m_ctx, assets->GetReaSonusLogo(), 200, 52);
            ImGui::SameLine(m_ctx);

            ImGui::EndChild(m_ctx); // logo
        }

        UiElements::PushReaSonusGroupStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "actions_container", 0.0, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
        {

            ReaSonusPageTitle(m_ctx, assets, "ReaSonus Native New Version: " + std::string(GIT_VERSION));
            ImGui::GetContentRegionAvail(m_ctx, &width, &height);
            ImGui::PushTextWrapPos(m_ctx, width - 24);

            ImGui::Text(m_ctx, "This ReaSonus Native version has a lot of changes for the plugin mapping. Some of the changes are:");
            ImGui::BulletText(m_ctx, "'Unlimited' amount of groups.");
            ImGui::BulletText(m_ctx, "Added a search in the parameter selection.");
            ImGui::BulletText(m_ctx, "Added option to not invert the label text.");
            ImGui::BulletText(m_ctx, "Added `next` and `previous` button to scroll through the list of groups. Using them with the left shift of your keyboard will navigate by grops of 13.");
            ImGui::BulletText(m_ctx, "Added setting for the step size when scrolling through the groups on the device.");
            ImGui::BulletText(m_ctx, "Added drag and drop support for groups.");
            ImGui::BulletText(m_ctx, "Added option to add or delete a group at any point.");
            ImGui::BulletText(m_ctx, "Added an indicator for changed groups.");
            ImGui::BulletText(m_ctx, "The version number in ReaSonus Control Panel is now clickable and will open this window.");

            ImGui::Text(m_ctx, "Beside all these nice improvements, some bugs were fixed as well. When creating a plugin mapping, the plugin type was not taken in account. This can potentially cause issues as the mapping between plugin types can differ");
            ImGui::Text(m_ctx, "There has been created a Plugin Type Mapping tool to help you set the type for your mappings. The possitive is you only have to do this once. Click the `Open type Mapping` button below to start the mapping.");
            ImGui::Text(m_ctx, "The tool is also available in the actions list (search for: 'plugin type`)");

            ImGui::PopTextWrapPos(m_ctx);
            UiElements::PopReaSonusGroupStyle(m_ctx);
            ImGui::EndChild(m_ctx);
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