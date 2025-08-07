#include "csurf_fp_8_ui_control_panel.hpp"
#include "csurf_fp_8_ui_settings_page.cpp"
#include "../ui/csurf_ui_function_keys_page.cpp"
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
    menu_items.push_back("Function Keys");
    menu_items.push_back("Settings");
    menu_items.push_back("About ReaSonus");

    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    initAssets();

    plugin_register("timer", reinterpret_cast<void *>(&loop));
}

ReaSonus8ControlPanel::~ReaSonus8ControlPanel()
{
    plugin_register("-timer", reinterpret_cast<void *>(&loop));
}

void ReaSonus8ControlPanel::initAssets()
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

void ReaSonus8ControlPanel::start()
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
    }
}
catch (const ImGui_Error &e)
{
    reportError_8(e);
    fp8_inst.reset();
}

void ReaSonus8ControlPanel::stop()
try
{
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

void ReaSonus8ControlPanel::loop()
try
{
    fp8_inst->frame();
}
catch (const ImGui_Error &e)
{
    reportError_8(e);
    fp8_inst.reset();
}

void ReaSonus8ControlPanel::frame()
{
    if (current_page != previous_page)
    {
        previous_page = current_page;

        switch (current_page)
        {
        case 0:
            CSurf_UI_FunctionKeysPage::querying_actions = false;
            CSurf_UI_FunctionKeysPage::selected_function = -1;
            CSurf_UI_FunctionKeysPage::selected_action = -1;

            page_content = new CSurf_UI_FunctionKeysPage(m_ctx, FP_8);
            break;
        case 1:
            page_content = new CSurf_FP_8_SettingsPage(m_ctx);
            break;
        case 2:
            page_content = new CSurf_UI_AboutPage(m_ctx, FP_8);
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
        SetActionState("_REASONUS_SHOW_REASONUS_8_CONTROL_WINDOW");
        return fp8_inst.reset();
    }
}