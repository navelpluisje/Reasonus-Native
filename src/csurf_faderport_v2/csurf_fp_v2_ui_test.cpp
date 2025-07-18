#include "csurf_fp_v2_ui_test.hpp"
#include "../ui/csurf_ui_vars.hpp"
#include "../ui/csurf_ui_colors.hpp"
#include "../ui/csurf_ui_elements.hpp"
#include "../ui/csurf_ui_text_input.hpp"
#include "../ui/csurf_ui_combo_input.hpp"
#include "../ui/csurf_ui_listbox.hpp"
#include "../ui/csurf_ui_radio_button_group.hpp"
#include "../ui/csurf_ui_checkbox.hpp"
#include "../ui/csurf_ui_button_bar.hpp"
#include "../ui/csurf_ui_menu_button.hpp"
#include "../ui/csurf_ui_page_title.hpp"
#include "../ui/csurf_ui_assets.hpp"

constexpr const char *g_name{"ReaSonus Native Control"};

std::unique_ptr<Example> Example::s_inst;

static void reportError(const ImGui_Error &e)
{
    ShowMessageBox(e.what(), g_name, 0);
}
static bool checkbox_value = false;
static int combo_value = 0;
static int list_value = -1;
static int radio_value = -1;
static bool button_clicked = false;
static int selected_menu_2 = 0;

Example::Example()
    : m_ctx{},
      m_text{"The quick brown fox jumps over the lazy dog"}
{
    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(menu_font));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(logo));
    settings_page = new CSurf_FP_V2_SettingsPage(m_ctx);

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
    if (button_clicked)
    {
        button_clicked = false;
        ShowConsoleMsg("Button Clicked\n");
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

            ReaSonusMenuButton(m_ctx, "Functions Keys", menu_font, icon_function_actions, 0, &selected_menu_2);
            ReaSonusMenuButton(m_ctx, "Custom Filters", menu_font, icon_custom_filters, 1, &selected_menu_2);
            ReaSonusMenuButton(m_ctx, "Settings", menu_font, icon_settings, 2, &selected_menu_2);

            ImGui::EndChild(m_ctx);
            UiElements::PopReaSonusSidebarStyle(m_ctx);
        }
        ImGui::SameLine(m_ctx);
        UiElements::PushReaSonusContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "main_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            ReaSonusPageTitle(m_ctx, "Function Keys", menu_font);
            settings_page->render();
            ReaSonusTextInput(m_ctx, "My Label", m_text);

            ReaSonusComboInput(m_ctx, "Combo component", {"Select an item", "Item 1", "item 2", "item 3"}, &combo_value);

            ReaSonusListBox(m_ctx, "List component", {"Select an item", "Item 1", "item 2", "item 3", "Item 4", "item 5", "item 6"}, &list_value);

            ReaSonusRadioButtonGroup(m_ctx, "Radio Group List component", {"Check box 1", "Check box 2", "Check box 3"}, &radio_value);

            ReaSonusCheckBox(m_ctx, "Check box Component", &checkbox_value);

            ReaSonusButtonBar(m_ctx, "Save", main_font_bold, &button_clicked, true, &button_clicked, "Cancel");

            ImGui::EndChild(m_ctx);
        }

        UiElements::PopReaSonusContentStyle(m_ctx);
        ImGui::End(m_ctx);

        // ImGui::ShowDebugLogWindow(m_ctx);
        // ImGui::ShowIDStackToolWindow(m_ctx);
        // ImGui::ShowMetricsWindow(m_ctx);
    }

    ImGui::PopFont(m_ctx);
    PopReaSonusColors(m_ctx);
    PopReaSonusStyle(m_ctx);

    if (!open)
        return s_inst.reset();
}