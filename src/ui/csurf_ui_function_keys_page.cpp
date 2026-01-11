#include "csurf_ui_function_keys_page.hpp"
#include "../csurf_faderport_8/csurf_fp_8_ui_control_panel.hpp"
#include "../csurf_faderport_v2/csurf_fp_v2_ui_control_panel.hpp"
#include "csurf_ui_elements.hpp"
#include "csurf_ui_tooltip.hpp"
#include "csurf_ui_colors.hpp"

CSurf_UI_FunctionKeysPage::CSurf_UI_FunctionKeysPage(ImGui_Context *m_ctx, CSurf_UI_Assets *assets, std::string _device) : CSurf_UI_PageContent(m_ctx, assets)
{
    i18n = I18n::GetInstance();
    device = _device;
    settings = ReaSonusSettings::GetInstance(device);

    Reset();
};

void CSurf_UI_FunctionKeysPage::Reset()
{
    footswitch.push_back(settings->GetFunction("1", true));
    footswitch.push_back(settings->GetFunction("2", true));
    footswitch.push_back(settings->GetFunction("3", true));

    functions.push_back(settings->GetFunction("1"));
    functions.push_back(settings->GetFunction("2"));
    functions.push_back(settings->GetFunction("3"));
    functions.push_back(settings->GetFunction("4"));
    if (device == FP_8)
    {
        functions.push_back(settings->GetFunction("5"));
        functions.push_back(settings->GetFunction("6"));
        functions.push_back(settings->GetFunction("7"));
        functions.push_back(settings->GetFunction("8"));
        functions.push_back(settings->GetFunction("9"));
        functions.push_back(settings->GetFunction("10"));
        functions.push_back(settings->GetFunction("11"));
        functions.push_back(settings->GetFunction("12"));
        functions.push_back(settings->GetFunction("13"));
        functions.push_back(settings->GetFunction("14"));
        functions.push_back(settings->GetFunction("15"));
        functions.push_back(settings->GetFunction("16"));
    }
}

void CSurf_UI_FunctionKeysPage::Save()
{
    settings->SetFunction("1", footswitch[0], true);
    settings->SetFunction("2", footswitch[1], true);
    settings->SetFunction("3", footswitch[2], true);

    settings->SetFunction("1", functions[0]);
    settings->SetFunction("2", functions[1]);
    settings->SetFunction("3", functions[2]);
    settings->SetFunction("4", functions[3]);
    if (device == FP_8)
    {
        settings->SetFunction("5", functions[4]);
        settings->SetFunction("6", functions[5]);
        settings->SetFunction("7", functions[6]);
        settings->SetFunction("8", functions[7]);
        settings->SetFunction("9", functions[8]);
        settings->SetFunction("10", functions[9]);
        settings->SetFunction("11", functions[10]);
        settings->SetFunction("12", functions[11]);
        settings->SetFunction("13", functions[12]);
        settings->SetFunction("14", functions[13]);
        settings->SetFunction("15", functions[14]);
        settings->SetFunction("16", functions[15]);
    }

    if (settings->StoreSettings())
    {
        if (device == FP_8)
        {
            ReaSonus8ControlPanel::SetMessage(i18n->t("functions", "action.save.message"));
        }
        else
        {
            ReaSonusV2ControlPanel::SetMessage(i18n->t("functions", "action.save.message"));
        }
    };
}

void CSurf_UI_FunctionKeysPage::ActionListTimer()
{
    WDL_ASSERT(querying_actions == true);
    const int actionId = PromptForAction(0, 0, 0);

    if (actionId == 0)
    {
        return;
    }

    plugin_register("-timer", (void *)&ActionListTimer);
    querying_actions = false;

    if (actionId > 0)
    {
        selected_action = actionId;
    }

    PromptForAction(-1, 0, 0);
}

void CSurf_UI_FunctionKeysPage::PromptForFunctionAction(int index, FunctionTypes type)
{
    selected_function = index;
    selected_type = type;
    PromptForAction(1, 0, 0);

    if (!querying_actions)
    {
        querying_actions = true;
        plugin_register("timer", (void *)&ActionListTimer);
    }
}

void CSurf_UI_FunctionKeysPage::HandleResetButtonClick(int index, FunctionTypes type)
{
    selected_function = index;
    selected_type = type;
    selected_action = 0;
}

void CSurf_UI_FunctionKeysPage::RenderFunction(ImGui_Context *m_ctx, int index, CSurf_UI_FunctionKeysPage &page, CSurf_UI_Assets *assets, FunctionTypes type)
{
    int actionId = type == TypeFunction ? stoi(page.functions[index]) : stoi(page.footswitch[index]);
    double x_width, y_width;
    std::string type_name = type == TypeFunction ? "item" : "footswitch";
    const char *fullName = kbd_getTextFromCmd(actionId, 0);

    std::vector<std::string> actionInfo = split(fullName, ": ");

    std::string idx = "function-key-" + std::to_string(index);
    std::string button_idx = "button-key-" + std::to_string(index);
    std::string tooltip_idx = "tooltip-key-" + std::to_string(index);
    std::string action_group = actionInfo.size() > 1 ? actionInfo[0] : page.i18n->t("functions", type_name + ".no-group");
    std::string action_description_1 = actionInfo.size() > 1
                                           ? actionInfo[1]
                                       : actionInfo.size() > 0
                                           ? actionInfo[0]
                                           : " ";
    std::string action_description_2 = actionInfo.size() > 2
                                           ? actionInfo[2]
                                           : " ";

    if (ImGui::BeginChild(m_ctx, idx.c_str(), 0.0, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_ResizeY))
    {
        ImGui::PushFont(m_ctx, assets->GetMainFontBold(), FontSizeDefault);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
        if (type == TypeFunction)
        {
            ImGui::Text(m_ctx, page.i18n->t("functions", "item.label", std::to_string(index + 1)).c_str());
        }
        else
        {
            ImGui::Text(m_ctx, page.i18n->t("functions", "footswitch.label." + std::to_string(index)).c_str());
        }
        ImGui::PopStyleColor(m_ctx);
        ImGui::SameLine(m_ctx);
        ImGui::Text(m_ctx, std::to_string(actionId).c_str());
        ImGui::PopFont(m_ctx);
        ImGui::Text(m_ctx, action_group.c_str());
        ImGui::TextWrapped(m_ctx, (action_description_1 + ". " + action_description_2).c_str());

        ImGui::GetContentRegionAvail(m_ctx, &x_width, &y_width);
        ImGui::SetCursorPosX(m_ctx, x_width - 55.0);
        ImGui::SetCursorPosY(m_ctx, 0);

        UiElements::PushReaSonusFunctionButtonStyle(m_ctx);
        ImGui::PushFont(m_ctx, assets->GetIconFont(), 24);
        if (ImGui::Button(m_ctx, std::string(1, IconRestore).c_str()))
        {
            HandleResetButtonClick(index, type);
        }
        ImGui::PopFont(m_ctx);
        ReaSonusSimpleTooltip(m_ctx, page.i18n->t("functions", type_name + ".button.clear.tooltip"), "clear-function-tooltip" + std::to_string(index));

        ImGui::SetCursorPosX(m_ctx, x_width - 16.0);
        ImGui::SetCursorPosY(m_ctx, 0);

        ImGui::PushFont(m_ctx, assets->GetIconFont(), 24);
        if (ImGui::Button(m_ctx, std::string(1, IconSearchAction).c_str()))
        {
            PromptForFunctionAction(index, type);
        }
        ImGui::PopFont(m_ctx);

        ReaSonusSimpleTooltip(m_ctx, page.i18n->t("functions", type_name + ".button.tooltip"), "add-function-tooltip" + std::to_string(index));

        UiElements::PopReaSonusFunctionButtonStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
}

void CSurf_UI_FunctionKeysPage::RenderFunctionTab(std::string tab_label, FunctionTypes type, int tab_index, int start_index, int count, CSurf_UI_Assets *assets)
{
    UiElements::PushReaSonusTabStyle(m_ctx, selected_tab == tab_index);
    if (ImGui::BeginTabItem(m_ctx, tab_label.c_str()))
    {
        selected_tab = tab_index;
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 8);
        if (ImGui::BeginChild(m_ctx, "left-shift-group", 0, 0, ImGui::ChildFlags_None))
        {
            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_CellPadding, 6, 6);
            if (ImGui::BeginTable(m_ctx, "function_keys_grid", 2))
            {
                for (int i = start_index; i < start_index + count; i++)
                {
                    if (ImGui::TableNextColumn(m_ctx))
                    {
                        UiElements::PushReaSonusFunctionActionStyle(m_ctx);
                        RenderFunction(m_ctx, i, *this, assets, type);
                        UiElements::PopReaSonusFunctionActionStyle(m_ctx);
                    }
                }
                ImGui::PopStyleVar(m_ctx);
                ImGui::EndTable(m_ctx);
            }
            ImGui::EndChild(m_ctx);
        }
        ImGui::EndTabItem(m_ctx);
    }
    UiElements::PopReaSonusTabStyle(m_ctx);
}

void CSurf_UI_FunctionKeysPage::RenderFPV2FunctionGroup()
{
    if (ImGui::BeginChild(m_ctx, "settings-group", 0, 0, ImGui::ChildFlags_None))
    {
        UiElements::PushReaSonusTabBarStyle(m_ctx);
        if (ImGui::BeginTabBar(m_ctx, "FunctionsTabs", ImGui::TabBarFlags_None))
        {
            RenderFunctionTab(i18n->t("functions", "tab.v2-functions"), TypeFunction, 0, 0, 4, assets);
            RenderFunctionTab(i18n->t("functions", "tab.footswitch"), TypeFootSwitch, 1, 0, 3, assets);

            UiElements::PopReaSonusTabBarStyle(m_ctx);
            ImGui::EndTabBar(m_ctx);
        }
        ImGui::EndChild(m_ctx);
    }
}

void CSurf_UI_FunctionKeysPage::RenderFP8FunctionGroup()
{
    if (ImGui::BeginChild(m_ctx, "settings-group", 0, 0, ImGui::ChildFlags_None))
    {
        UiElements::PushReaSonusTabBarStyle(m_ctx);
        if (ImGui::BeginTabBar(m_ctx, "FunctionsTabs", ImGui::TabBarFlags_None))
        {
            RenderFunctionTab(i18n->t("functions", "tab.left-shift"), TypeFunction, 0, 0, 8, assets);
            RenderFunctionTab(i18n->t("functions", "tab.right-shift"), TypeFunction, 1, 8, 8, assets);
            RenderFunctionTab(i18n->t("functions", "tab.footswitch"), TypeFootSwitch, 2, 0, 3, assets);

            UiElements::PopReaSonusTabBarStyle(m_ctx);
            ImGui::EndTabBar(m_ctx);
        }
        ImGui::EndChild(m_ctx);
    }
}

void CSurf_UI_FunctionKeysPage::Render()
{
    if (selected_action != -1)
    {
        if (selected_type == TypeFunction)
        {
            functions[selected_function] = std::to_string(selected_action);
        }
        else
        {
            footswitch[selected_function] = std::to_string(selected_action);
        }
        selected_action = -1;
        selected_function = -1;
        selected_type = TypeFunction;
    }

    if (device == FP_8)
    {
        RenderFP8FunctionGroup();
    }
    else
    {
        RenderFPV2FunctionGroup();
    }
}
