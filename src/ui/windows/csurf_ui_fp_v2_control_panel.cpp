#include "../pages/csurf_ui_fp_v2_control_panel.hpp"
#include "../pages/csurf_ui_fp_v2_settings_page.cpp"
#include "csurf_ui_about.cpp"
#include "../pages/csurf_ui_function_keys.hpp"
#include "../csurf_ui_colors.hpp"
#include "../csurf_ui_vars.hpp"
#include "../components/csurf_ui_menu_button.hpp"
#include "../components/csurf_ui_page_title.hpp"
#include "../components/csurf_ui_button_bar.hpp"
#include "../csurf_ui_assets.hpp"

constexpr auto g_name{"ReaSonus Native V2 Control Panel"};

std::unique_ptr<ReaSonusV2ControlPanel> ReaSonusV2ControlPanel::s_inst; // NOLINT(*-statically-constructed-objects)

static void reportError(const ImGui_Error &error) // NOLINT(*-use-anonymous-namespace)
{
    ShowMessageBox(error.what(), g_name, 0);
}

void ReaSonusV2ControlPanel::Start(const int page)
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

void ReaSonusV2ControlPanel::SetCurrentPage(const int page)
{
    if (s_inst && current_page != page)
    {
        current_page = page;
        s_inst->SetPageContent();
    }
}

void ReaSonusV2ControlPanel::SetMessage(const std::string &_message)
{
    if (s_inst)
    {
        s_inst->SetLocalMessage(_message);
    }
}

void ReaSonusV2ControlPanel::SetLocalMessage(const std::string &_message)
{
    message = _message;
}

ReaSonusV2ControlPanel::~ReaSonusV2ControlPanel()
{
    plugin_register("-timer", reinterpret_cast<void *>(&Loop));
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
            default: ;
        }
    }
}

ReaSonusV2ControlPanel::ReaSonusV2ControlPanel() : m_ctx{}
{
    menu_items.emplace_back("menu.functions");
    menu_items.emplace_back("menu.settings");
    menu_items.emplace_back("menu.about");

    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    assets = new CSurf_UI_Assets(m_ctx);

    plugin_register("timer", reinterpret_cast<void *>(&Loop));
}

void ReaSonusV2ControlPanel::Frame() // NOLINT(*-function-cognitive-complexity)
{
    if (!message.empty())
    {
        const int now = static_cast<int>(GetTickCount());
        if (message_timer == 0)
        {
            message_timer = now;
        }
        else if (message_timer + 3000 < now)
        {
            message = "";
            message_timer = 0;
        }
    }

    if (ImGui::IsKeyDown(m_ctx, ImGui::Key_Escape))
    {
        control_panel_open = false;
        SetActionState("_REASONUS_SHOW_REASONUS_V2_CONTROL_WINDOW");
        s_inst.reset();
        return;
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
    ImGui::PushFont(m_ctx, assets->GetMainFont(), FontSizeDefault);
    ImGui::SetNextWindowSize(m_ctx, 1024, 430, ImGui::Cond_Once);
    bool open{true};

    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_NoResize))
    {
        UiStyledElements::PushReaSonusSidebarStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "side_bar", 224.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            ImGui::Image(m_ctx, assets->GetReaSonusLogo(), 200, 52);

            ReaSonusMenuButton(m_ctx, assets, i18n->t("control-panel", menu_items[0]), IconAction, 0, &current_page);
            ReaSonusMenuButton(m_ctx, assets, i18n->t("control-panel", menu_items[1]), IconSettings, 1, &current_page);
            ReaSonusMenuButton(m_ctx, assets, i18n->t("control-panel", menu_items[2]), IconInformation, 2, &current_page);

            ImGui::EndChild(m_ctx);
            UiStyledElements::PopReaSonusSidebarStyle(m_ctx);
        }
        ImGui::SameLine(m_ctx);
        UiStyledElements::PushReaSonusContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "main_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            UiStyledElements::PopReaSonusContentStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, current_page != 2 ? -34.0 : 0, ImGui::ChildFlags_None))
            {
                ReaSonusPageTitle(m_ctx, assets, i18n->t("control-panel", menu_items[current_page]), false);

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
        s_inst.reset();
    }
}
