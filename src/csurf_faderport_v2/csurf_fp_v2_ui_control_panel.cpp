#include "csurf_fp_v2_ui_control_panel.hpp"
#include "csurf_fp_v2_ui_settings_page.cpp"
#include "../ui/csurf_ui_about_page.cpp"
#include "../ui/csurf_ui_function_keys_page.hpp"
#include "../ui/csurf_ui_colors.hpp"
#include "../ui/csurf_ui_vars.hpp"
#include "../ui/csurf_ui_menu_button.hpp"
#include "../ui/csurf_ui_page_title.hpp"
#include "../ui/csurf_ui_button_bar.hpp"
#include "../ui/csurf_ui_images.h"

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
    InitAssets();

    plugin_register("timer", reinterpret_cast<void *>(&Loop));
}

ReaSonusV2ControlPanel::~ReaSonusV2ControlPanel()
{
    plugin_register("-timer", reinterpret_cast<void *>(&Loop));
}

void ReaSonusV2ControlPanel::InitAssets()
{
    main_font = ImGui::CreateFont("Arial");
    main_font_bold = ImGui::CreateFont("Arial", ImGui::FontFlags_Bold);
    logo = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(reasonus_logo), sizeof(reasonus_logo));
    icon_settings = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_settings), sizeof(img_icon_settings));
    icon_function_actions = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_function_actions), sizeof(img_icon_function_actions));
    icon_custom_filters = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_custom_filters), sizeof(img_icon_custom_filters));
    icon_about = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_about), sizeof(img_icon_about));

    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(logo));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_settings));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_function_actions));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_custom_filters));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_about));
}

void ReaSonusV2ControlPanel::Start(int page)
{
    SetActionState("_REASONUS_SHOW_REASONUS_8_CONTROL_WINDOW", 1);
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

            page_content = new CSurf_UI_FunctionKeysPage(m_ctx, FP_V2);
            break;

        case SETTINGS_PAGE:
            page_content = new CSurf_FP_V2_SettingsPage(m_ctx);
            break;

        case ABOUT_PAGE:
            page_content = new CSurf_UI_AboutPage(m_ctx, FP_V2);
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
    ImGui::PushFont(m_ctx, main_font, 13);
    ImGui::SetNextWindowSize(m_ctx, 1024, 430, ImGui::Cond_Once);
    bool open{true};

    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_NoResize))
    {
        UiElements::PushReaSonusSidebarStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "side_bar", 224.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            ImGui::Image(m_ctx, logo, 200, 52);

            ReaSonusMenuButton(m_ctx, i18n->t("control-panel", menu_items[0]), main_font_bold, icon_function_actions, 0, &current_page);
            ReaSonusMenuButton(m_ctx, i18n->t("control-panel", menu_items[1]), main_font_bold, icon_settings, 1, &current_page);
            ReaSonusMenuButton(m_ctx, i18n->t("control-panel", menu_items[2]), main_font_bold, icon_about, 2, &current_page);

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
                ReaSonusPageTitle(m_ctx, i18n->t("control-panel", menu_items[current_page]), main_font_bold);

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
                    i18n->t("control-panel", "button.save"),
                    main_font_bold,
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