#include "csurf_fp_v2_ui_control.hpp"
#include "csurf_fp_v2_ui_function_keys_page.cpp"
#include "csurf_fp_v2_ui_settings_page.cpp"
#include "csurf_fp_v2_ui_about_page.cpp"
#include "../ui/csurf_ui_colors.hpp"
#include "../ui/csurf_ui_vars.hpp"
#include "../ui/csurf_ui_menu_button.hpp"
#include "../ui/csurf_ui_page_title.hpp"
#include "../ui/csurf_ui_button_bar.hpp"
#include "../ui/csurf_ui_images.h"

bool CSurf_FP_V2_FunctionKeysPage::querying_actions = false;
int CSurf_FP_V2_FunctionKeysPage::selected_function = -1;
int CSurf_FP_V2_FunctionKeysPage::selected_action = -1;

constexpr const char *g_name{"ReaSonus Native Control"};

std::unique_ptr<ReaSonusV2ControlPanel> ReaSonusV2ControlPanel::s_inst;

static void reportError(const ImGui_Error &e)
{
    ShowMessageBox(e.what(), g_name, 0);
}
ReaSonusV2ControlPanel::ReaSonusV2ControlPanel()
    : m_ctx{}
{
    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    initAssets();

    plugin_register("timer", reinterpret_cast<void *>(&loop));
}

ReaSonusV2ControlPanel::~ReaSonusV2ControlPanel()
{
    plugin_register("-timer", reinterpret_cast<void *>(&loop));
}

void ReaSonusV2ControlPanel::initAssets()
{
    main_font = ImGui::CreateFont("sans-serif", 13);
    main_font_bold = ImGui::CreateFont("sans-serif", 13, ImGui::FontFlags_Bold);
    menu_font = ImGui::CreateFont("sans-serif", 15, ImGui::FontFlags_Bold);
    logo = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(reasonus_logo), sizeof(reasonus_logo));
    icon_settings = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_settings), sizeof(img_icon_settings));
    icon_function_actions = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_function_actions), sizeof(img_icon_function_actions));
    icon_custom_filters = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_custom_filters), sizeof(img_icon_custom_filters));

    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(menu_font));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(logo));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_settings));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_function_actions));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_custom_filters));
}

void ReaSonusV2ControlPanel::start()
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

void ReaSonusV2ControlPanel::stop()
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

void ReaSonusV2ControlPanel::loop()
try
{
    s_inst->frame();
}
catch (const ImGui_Error &e)
{
    reportError(e);
    s_inst.reset();
}

void ReaSonusV2ControlPanel::frame()
{
    if (current_page != previous_page)
    {
        previous_page = current_page;

        switch (current_page)
        {
        case 0:
            CSurf_FP_V2_FunctionKeysPage::querying_actions = false;
            CSurf_FP_V2_FunctionKeysPage::selected_function = -1;
            CSurf_FP_V2_FunctionKeysPage::selected_action = -1;

            page_content = new CSurf_FP_V2_FunctionKeysPage(m_ctx);
            break;
        case 1:
            page_content = new CSurf_FP_V2_SettingsPage(m_ctx);
            break;
        case 2:
            page_content = new CSurf_FP_V2_AboutPage(m_ctx);
            break;
        }
    }

    if (save_clicked)
    {
        save_clicked = false;
        page_content->save();
    }

    if (cancel_clicked)
    {
        cancel_clicked = false;
        page_content->reset();
    }

    PushReaSonusColors(m_ctx);
    PushReaSonusStyle(m_ctx);
    ImGui::PushFont(m_ctx, main_font);
    ImGui::SetNextWindowSize(m_ctx, 400, 80, ImGui::Cond_FirstUseEver);
    bool open{true};
    if (ImGui::Begin(m_ctx, g_name, &open))
    {
        UiElements::PushReaSonusSidebarStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "side_bar", 224.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            ImGui::Image(m_ctx, logo, 200, 52);

            ReaSonusMenuButton(m_ctx, menu_items[0], menu_font, icon_function_actions, 0, &current_page);
            ReaSonusMenuButton(m_ctx, menu_items[1], menu_font, icon_settings, 1, &current_page);
            ReaSonusMenuButton(m_ctx, menu_items[2], menu_font, icon_custom_filters, 2, &current_page);

            ImGui::EndChild(m_ctx);
            UiElements::PopReaSonusSidebarStyle(m_ctx);
        }
        ImGui::SameLine(m_ctx);
        UiElements::PushReaSonusContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "main_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            UiElements::PopReaSonusContentStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, -30.0, ImGui::ChildFlags_None))
            {
                ReaSonusPageTitle(m_ctx, menu_items[current_page], menu_font);

                if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, -12.0, ImGui::ChildFlags_None))
                {
                    page_content->render();
                    ImGui::EndChild(m_ctx);
                }
                ImGui::EndChild(m_ctx);
            }
            ReaSonusButtonBar(m_ctx, "Save", main_font_bold, &save_clicked, true, &cancel_clicked, "Cancel");
            ImGui::EndChild(m_ctx);
        }

        ImGui::End(m_ctx);
    }

    ImGui::PopFont(m_ctx);
    PopReaSonusColors(m_ctx);
    PopReaSonusStyle(m_ctx);

    if (!open)
    {
        SetActionState("_REASONUS_SHOW_REASONUS_V2_CONTROL_WINDOW");
        return s_inst.reset();
    }
}