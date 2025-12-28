#include "csurf_fp_v2_ui_control_panel.hpp"
#include "csurf_fp_v2_ui_settings_page.cpp"
#include "../ui/csurf_ui_about_page.cpp"
#include "../ui/csurf_ui_function_keys_page.hpp"
#include "../ui/csurf_ui_colors.hpp"
#include "../ui/csurf_ui_vars.hpp"
#include "../ui/csurf_ui_menu_button.hpp"
#include "../ui/csurf_ui_page_title.hpp"
#include "../ui/csurf_ui_button_bar.hpp"
#include "../ui/csurf_ui_assets.hpp"

constexpr const char *g_name{"ReaSonus Native V2 Control Panel"};

std::unique_ptr<ReaSonusV2ControlPanel> ReaSonusV2ControlPanel::s_inst;

static void reportError(const ImGui_Error &e)
{
    ShowMessageBox(e.what(), g_name, 0);
}

ReaSonusV2ControlPanel::ReaSonusV2ControlPanel() : m_ctx{}
{
    menu_items.push_back("menu.functions");
    menu_items.push_back("menu.settings");
    menu_items.push_back("menu.about");

    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    assets = new CSurf_UI_Assets(m_ctx);

    plugin_register("timer", reinterpret_cast<void *>(&Loop));
}

ReaSonusV2ControlPanel::~ReaSonusV2ControlPanel()
{
    plugin_register("-timer", reinterpret_cast<void *>(&Loop));
}

void ReaSonusV2ControlPanel::Start(int page)
{
    SetActionState("_REASONUS_SHOW_REASONUS_V2_CONTROL_WINDOW", 1);
    current_page = page;
    try
    {
        control_panel_open = true;
        if (s_inst)
        {
            ImGui::SetNextWindowFocus(s_inst->m_ctx);
        }
        else
        {
            s_inst.reset(new ReaSonusV2ControlPanel);
        }
    }
    catch (const ImGui_Error &e)
    {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusV2ControlPanel::Stop()
{
    try
    {
        control_panel_open = false;
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

void ReaSonusV2ControlPanel::SetCurrentPage(int page)
{
    if (s_inst && current_page != page)
    {
        current_page = page;
        s_inst->SetPageContent();
    }
}

void ReaSonusV2ControlPanel::SetMessage(std::string message)
{
    if (s_inst)
    {
        return s_inst->SetLocalMessage(message);
    }
}

void ReaSonusV2ControlPanel::SetLocalMessage(std::string _message)
{
    message = _message;
}

void ReaSonusV2ControlPanel::Loop()
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

void ReaSonusV2ControlPanel::SetPageContent()
{
    if (current_page != previous_page)
    {
        previous_page = current_page;

        switch (current_page)
        {
        case FUNCTIONS_PAGE:
            CSurf_UI_FunctionKeysPage::querying_actions = false;
            CSurf_UI_FunctionKeysPage::selected_function = -1;
            CSurf_UI_FunctionKeysPage::selected_action = -1;

            page_content = new CSurf_UI_FunctionKeysPage(m_ctx, assets, FP_V2);
            break;

        case SETTINGS_PAGE:
            page_content = new CSurf_FP_V2_SettingsPage(m_ctx, assets);
            break;

        case ABOUT_PAGE:
            page_content = new CSurf_UI_AboutPage(m_ctx, assets, FP_V2);
            break;
        }
    }
}

void ReaSonusV2ControlPanel::Frame()
{
    if (!message.empty())
    {
        int now = (int)GetTickCount();
        if (message_timer == 0)
        {
            message_timer = now;
        }
        else if ((message_timer + 3000) < now)
        {
            message = "";
            message_timer = 0;
        }
    }

    if (ImGui::IsKeyDown(m_ctx, ImGui::Key_Escape))
    {
        control_panel_open = false;
        SetActionState("_REASONUS_SHOW_REASONUS_V2_CONTROL_WINDOW");
        return s_inst.reset();
    }

    SetPageContent();

    if (save_clicked)
    {
        save_clicked = false;
        page_content->Save();
    }

    if (cancel_clicked)
    {
        cancel_clicked = false;
        page_content->Reset();
    }

    PushReaSonusColors(m_ctx);
    PushReaSonusStyle(m_ctx);
    ImGui::PushFont(m_ctx, assets->GetMainFont(), 13);
    ImGui::SetNextWindowSize(m_ctx, 1024, 430, ImGui::Cond_Once);
    bool open{true};

    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_NoResize))
    {
        UiElements::PushReaSonusSidebarStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "side_bar", 224.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            ImGui::Image(m_ctx, assets->GetReaSonusLogo(), 200, 52);

            ReaSonusMenuButton(m_ctx, assets, i18n->t("control-panel", menu_items[0]), IconAction, 0, &current_page);
            ReaSonusMenuButton(m_ctx, assets, i18n->t("control-panel", menu_items[1]), IconSettings, 1, &current_page);
            ReaSonusMenuButton(m_ctx, assets, i18n->t("control-panel", menu_items[2]), IconInformation, 2, &current_page);

            ImGui::EndChild(m_ctx);
            UiElements::PopReaSonusSidebarStyle(m_ctx);
        }
        ImGui::SameLine(m_ctx);
        UiElements::PushReaSonusContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "main_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            UiElements::PopReaSonusContentStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, current_page != 2 ? -34.0 : 0, ImGui::ChildFlags_None))
            {
                ReaSonusPageTitle(m_ctx, assets, i18n->t("control-panel", menu_items[current_page]));

                if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, -12.0, ImGui::ChildFlags_None))
                {
                    page_content->Render();
                    ImGui::EndChild(m_ctx);
                }
                ImGui::EndChild(m_ctx);
            }
            if (current_page != 2)
            {
                ReaSonusButtonBar(
                    m_ctx,
                    assets,
                    i18n->t("control-panel", "button.save"),
                    &save_clicked,
                    true,
                    &cancel_clicked,
                    i18n->t("control-panel", "button.cancel"),
                    &message);
            }
            ImGui::EndChild(m_ctx);
        }

        ImGui::End(m_ctx);
    }

    ImGui::PopFont(m_ctx);
    PopReaSonusColors(m_ctx);
    PopReaSonusStyle(m_ctx);

    if (!open)
    {
        control_panel_open = false;
        SetActionState("_REASONUS_SHOW_REASONUS_V2_CONTROL_WINDOW");
        return s_inst.reset();
    }
}