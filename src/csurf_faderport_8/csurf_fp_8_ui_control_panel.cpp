#include <config.h>
#include "csurf_fp_8_ui_control_panel.hpp"
#include "csurf_fp_8_ui_settings_page.cpp"
#include "csurf_fp_8_ui_plugin_mapping_page.cpp"
#include "csurf_fp_8_ui_custom_filters_page.cpp"
#include "../ui/csurf_ui_function_keys_page.hpp"
#include "../ui/csurf_ui_about_page.cpp"
#include "../ui/csurf_ui_colors.hpp"
#include "../ui/csurf_ui_vars.hpp"
#include "../ui/csurf_ui_menu_button.hpp"
#include "../ui/csurf_ui_page_title.hpp"
#include "../ui/csurf_ui_button_bar.hpp"
#include "../ui/csurf_ui_images.h"

constexpr const char *g_name{"ReaSonus Native 8/16 Control Panel"};

std::unique_ptr<ReaSonus8ControlPanel> ReaSonus8ControlPanel::fp8_inst;

static void reportError_8(const ImGui_Error &e)
{
    ShowMessageBox(e.what(), g_name, 0);
}

ReaSonus8ControlPanel::ReaSonus8ControlPanel()
    : m_ctx{}
{
    menu_items.push_back("menu.functions");
    menu_items.push_back("menu.filters");
    menu_items.push_back("menu.plugin");
    menu_items.push_back("menu.settings");
    menu_items.push_back("menu.about");

    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    InitAssets();

    plugin_register("timer", reinterpret_cast<void *>(&Loop));
}

ReaSonus8ControlPanel::~ReaSonus8ControlPanel()
{
    plugin_register("-timer", reinterpret_cast<void *>(&Loop));
}

void ReaSonus8ControlPanel::InitAssets()
{
    main_font = ImGui::CreateFont("Arial");
    main_font_bold = ImGui::CreateFont("Arial", ImGui::FontFlags_Bold);
    logo = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(reasonus_logo), sizeof(reasonus_logo));
    icon_settings = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_settings), sizeof(img_icon_settings));
    icon_function_actions = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_function_actions), sizeof(img_icon_function_actions));
    icon_custom_filters = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_custom_filters), sizeof(img_icon_custom_filters));
    icon_plugin_mapping = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_param_mapping), sizeof(img_icon_param_mapping));
    icon_about = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_about), sizeof(img_icon_about));

    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(logo));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_settings));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_function_actions));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_custom_filters));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_plugin_mapping));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_about));
}

void ReaSonus8ControlPanel::Start()
{
    try
    {
        control_panel_open = true;
        if (fp8_inst)
        {
            ImGui::SetNextWindowFocus(fp8_inst->m_ctx);
        }
        else
        {
            fp8_inst.reset(new ReaSonus8ControlPanel);
            fp8_inst->Frame();
        }
    }
    catch (const ImGui_Error &e)
    {
        reportError_8(e);
        fp8_inst.reset();
    }
}

void ReaSonus8ControlPanel::Stop()
{
    try
    {
        // Reset the page when closing the ui
        current_page = FUNCTIONS_PAGE;
        control_panel_open = false;
        if (fp8_inst)
        {
            fp8_inst.reset();
        }
    }
    catch (const ImGui_Error &e)
    {
        reportError_8(e);
        fp8_inst.reset();
    }
}

void ReaSonus8ControlPanel::SetCurrentPage(int page)
{
    if (fp8_inst && current_page != page)
    {
        current_page = page;
        fp8_inst->SetPageContent();
    }
}

void ReaSonus8ControlPanel::SetPageProperty(int type, int value)
{
    if (fp8_inst && fp8_inst->page_content != nullptr)
    {
        fp8_inst->page_content->SetPageProperty(type, value);
    }
}

void ReaSonus8ControlPanel::SetPageStringProperty(int type, std::string value)
{
    if (fp8_inst && fp8_inst->page_content != nullptr)
    {
        fp8_inst->page_content->SetPageProperty(type, value);
    }
}

int ReaSonus8ControlPanel::GetPageProperty(int type)
{
    if (fp8_inst && fp8_inst->page_content != nullptr)
    {
        return fp8_inst->page_content->GetPageProperty(type);
    }
    return -1;
}

std::string ReaSonus8ControlPanel::GetPageStringProperty(int type)
{
    if (fp8_inst && fp8_inst->page_content != nullptr)
    {
        return fp8_inst->page_content->GetPageStringProperty(type);
    }
    return "";
}

void ReaSonus8ControlPanel::SetMessage(std::string message)
{
    if (fp8_inst)
    {
        return fp8_inst->SetLocalMessage(message);
    }
}

void ReaSonus8ControlPanel::SetLocalMessage(std::string _message)
{
    message = _message;
}

void ReaSonus8ControlPanel::Loop()
{
    try
    {
        fp8_inst->Frame();
    }
    catch (const ImGui_Error &e)
    {
        reportError_8(e);
        fp8_inst.reset();
    }
}

void ReaSonus8ControlPanel::SetPageContent()
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

            page_content = new CSurf_UI_FunctionKeysPage(m_ctx, FP_8);
            break;

        case FILTERS_PAGE:
            page_content = new CSurf_FP_8_CustomFilterstPage(m_ctx);
            break;

        case MAPPING_PAGE:
            page_content = new CSurf_FP_8_PluginMappingPage(m_ctx);
            break;

        case SETTINGS_PAGE:
            page_content = new CSurf_FP_8_SettingsPage(m_ctx);
            break;

        case ABOUT_PAGE:
            page_content = new CSurf_UI_AboutPage(m_ctx, FP_8);
            break;
        }
    }
}

void ReaSonus8ControlPanel::Frame()
{
    double space_x, space_y, width, height;

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
        SetActionState("_REASONUS_SHOW_REASONUS_8_CONTROL_WINDOW");
        return fp8_inst.reset();
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
    ImGui::SetNextWindowSize(m_ctx, 1024, 640, ImGui::Cond_Once);
    bool open{true};
    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_NoResize))
    {
        UiElements::PushReaSonusSidebarStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "side_bar", 224.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
            ImGui::CalcTextSize(m_ctx, GIT_VERSION, &width, &height);

            if (ImGui::BeginChild(m_ctx, "side_bar_content", 0.0, space_y - height - 10))
                ;
            {
                ImGui::Image(m_ctx, logo, 200, 52);

                ReaSonusMenuButton(m_ctx, i18n->t("control-panel", menu_items[0]), main_font_bold, icon_function_actions, FUNCTIONS_PAGE, &current_page);
                ReaSonusMenuButton(m_ctx, i18n->t("control-panel", menu_items[1]), main_font_bold, icon_custom_filters, FILTERS_PAGE, &current_page);
                ReaSonusMenuButton(m_ctx, i18n->t("control-panel", menu_items[2]), main_font_bold, icon_plugin_mapping, MAPPING_PAGE, &current_page);
                ReaSonusMenuButton(m_ctx, i18n->t("control-panel", menu_items[3]), main_font_bold, icon_settings, SETTINGS_PAGE, &current_page);
                ReaSonusMenuButton(m_ctx, i18n->t("control-panel", menu_items[4]), main_font_bold, icon_about, ABOUT_PAGE, &current_page);

                ImGui::EndChild(m_ctx);
            }

            ImGui::Text(m_ctx, GIT_VERSION);

            ImGui::EndChild(m_ctx);
            UiElements::PopReaSonusSidebarStyle(m_ctx);
        }
        ImGui::SameLine(m_ctx);
        UiElements::PushReaSonusContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "main_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            UiElements::PopReaSonusContentStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, current_page != 4 ? -34.0 : 0, ImGui::ChildFlags_None))
            {
                ReaSonusPageTitle(m_ctx, i18n->t("control-panel", menu_items[current_page]), main_font_bold);

                if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, -12.0, ImGui::ChildFlags_None))
                {
                    page_content->Render();
                    ImGui::EndChild(m_ctx);
                }
                ImGui::EndChild(m_ctx);
            }
            if (current_page != 4)
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
        SetActionState("_REASONUS_SHOW_REASONUS_8_CONTROL_WINDOW");
        return fp8_inst.reset();
    }
}