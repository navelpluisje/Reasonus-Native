#include <utility>

#include "./csurf_ui_function_keys.hpp"
#include "csurf_ui_fp_v2_control_panel.hpp"
#include "../csurf_ui_colors.hpp"
#include "../csurf_ui_styles_elements.hpp"
#include "../components/csurf_ui_tooltip.hpp"
#include "../windows/csurf_ui_fp_8_control_panel.hpp"

CSurf_UI_FunctionKeysPage::CSurf_UI_FunctionKeysPage(
    ImGui_Context *m_ctx,
    CSurf_UI_Assets *assets,
    std::string _device
) : CSurf_UI_PageContent(m_ctx, assets) {
    i18n = I18n::GetInstance();
    device = std::move(_device);
    settings = ReaSonusSettings::GetInstance(device);

    CSurf_UI_FunctionKeysPage::Reset();
}

void CSurf_UI_FunctionKeysPage::Reset() {
    footswitch.push_back(settings->GetFunction("1", FUNCTION_FOOTSWITCH));
    footswitch.push_back(settings->GetFunction("2", FUNCTION_FOOTSWITCH));
    footswitch.push_back(settings->GetFunction("3", FUNCTION_FOOTSWITCH));

    transport.push_back(settings->GetFunction("1", FUNCTION_TRANSPORT));
    transport.push_back(settings->GetFunction("2", FUNCTION_TRANSPORT));
    transport.push_back(settings->GetFunction("3", FUNCTION_TRANSPORT));
    transport.push_back(settings->GetFunction("4", FUNCTION_TRANSPORT));

    functions.push_back(settings->GetFunction("1", FUNCTION_DEFAULT));
    functions.push_back(settings->GetFunction("2", FUNCTION_DEFAULT));
    functions.push_back(settings->GetFunction("3", FUNCTION_DEFAULT));
    functions.push_back(settings->GetFunction("4", FUNCTION_DEFAULT));
    if (device == FP_8) {
        functions.push_back(settings->GetFunction("5", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("6", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("7", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("8", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("9", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("10", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("11", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("12", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("13", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("14", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("15", FUNCTION_DEFAULT));
        functions.push_back(settings->GetFunction("16", FUNCTION_DEFAULT));
    }
}

void CSurf_UI_FunctionKeysPage::Save() {
    settings->SetFunction("1", footswitch[0], FUNCTION_FOOTSWITCH);
    settings->SetFunction("2", footswitch[1], FUNCTION_FOOTSWITCH);
    settings->SetFunction("3", footswitch[2], FUNCTION_FOOTSWITCH);

    settings->SetFunction("1", transport[0], FUNCTION_TRANSPORT);
    settings->SetFunction("2", transport[1], FUNCTION_TRANSPORT);
    settings->SetFunction("3", transport[2], FUNCTION_TRANSPORT);
    settings->SetFunction("4", transport[3], FUNCTION_TRANSPORT);

    settings->SetFunction("1", functions[0], FUNCTION_DEFAULT);
    settings->SetFunction("2", functions[1], FUNCTION_DEFAULT);
    settings->SetFunction("3", functions[2], FUNCTION_DEFAULT);
    settings->SetFunction("4", functions[3], FUNCTION_DEFAULT);
    if (device == FP_8) {
        settings->SetFunction("5", functions[4], FUNCTION_DEFAULT);
        settings->SetFunction("6", functions[5], FUNCTION_DEFAULT);
        settings->SetFunction("7", functions[6], FUNCTION_DEFAULT);
        settings->SetFunction("8", functions[7], FUNCTION_DEFAULT);
        settings->SetFunction("9", functions[8], FUNCTION_DEFAULT);
        settings->SetFunction("10", functions[9], FUNCTION_DEFAULT);
        settings->SetFunction("11", functions[10], FUNCTION_DEFAULT);
        settings->SetFunction("12", functions[11], FUNCTION_DEFAULT);
        settings->SetFunction("13", functions[12], FUNCTION_DEFAULT);
        settings->SetFunction("14", functions[13], FUNCTION_DEFAULT);
        settings->SetFunction("15", functions[14], FUNCTION_DEFAULT);
        settings->SetFunction("16", functions[15], FUNCTION_DEFAULT);
    }

    if (settings->StoreSettings()) {
        if (device == FP_8) {
            ReaSonus8ControlPanel::SetMessage(i18n->t("functions", "action.save.message"));
        } else {
            ReaSonusV2ControlPanel::SetMessage(i18n->t("functions", "action.save.message"));
        }
    }
}

void CSurf_UI_FunctionKeysPage::ActionListTimer() {
    WDL_ASSERT(querying_actions == true);
    const int actionId = PromptForAction(0, 0, 0);

    if (actionId == 0) {
        return;
    }

    plugin_register("-timer", (void *) &ActionListTimer);
    querying_actions = false;

    if (actionId > 0) {
        selected_action = actionId;
    }

    PromptForAction(-1, 0, 0);
}

void CSurf_UI_FunctionKeysPage::PromptForFunctionAction(int index, FunctionTypes type) {
    selected_function = index;
    selected_type = type;
    PromptForAction(1, 0, 0);

    if (!querying_actions) {
        querying_actions = true;
        plugin_register("timer", (void *) &ActionListTimer);
    }
}

void CSurf_UI_FunctionKeysPage::HandleResetButtonClick(int index, FunctionTypes type) {
    selected_function = index;
    selected_type = type;
    selected_action = 0;
}

void CSurf_UI_FunctionKeysPage::RenderFunction(
    ImGui_Context *m_ctx,
    int index,
    CSurf_UI_FunctionKeysPage &page,
    CSurf_UI_Assets *assets,
    FunctionTypes type
) {
    int actionId;
    std::string type_name;
    std::string type_label;
    double x_width;
    double y_width;

    switch (type) {
        case FUNCTION_DEFAULT:
            actionId = stoi(page.functions[index]);
            type_name = "item";
            type_label = page.i18n->t("functions", "item.label", std::to_string(index + 1));
            break;
        case FUNCTION_FOOTSWITCH:
            actionId = stoi(page.footswitch[index]);
            type_name = "footswitch";
            type_label = page.i18n->t("functions", "footswitch.label." + std::to_string(index));
            break;
        case FUNCTION_TRANSPORT:
            actionId = stoi(page.transport[index]);
            type_name = "transport";
            type_label = page.i18n->t("functions", "transport.label." + std::to_string(index));
            break;
    }

    const char *fullName = kbd_getTextFromCmd(actionId, nullptr);

    std::vector<std::string> actionInfo = split(fullName, PREFIX_SEPARATOR);

    const std::string idx = "function-key-" + std::to_string(index);
    const std::string action_group = actionInfo.size() > 1
                                         ? actionInfo[0]
                                         : page.i18n->t("functions", type_name + ".no-group");
    const std::string action_description_1 = actionInfo.size() > 1
                                                 ? actionInfo[1]
                                                 : actionInfo.size() > 0
                                                       ? actionInfo[0]
                                                       : " ";
    const std::string action_description_2 = actionInfo.size() > 2
                                                 ? actionInfo[2]
                                                 : " ";

    if (ImGui::BeginChild(
        m_ctx,
        idx.c_str(),
        0.0,
        0.0,
        ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_ResizeY
    )) {
        ImGui::PushFont(m_ctx, assets->GetMainFontBold(), FontSizeDefault);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);

        ImGui::Text(m_ctx, type_label.c_str());

        ImGui::PopStyleColor(m_ctx);
        ImGui::SameLine(m_ctx);
        ImGui::Text(m_ctx, std::to_string(actionId).c_str());
        ImGui::PopFont(m_ctx);
        ImGui::Text(m_ctx, action_group.c_str());
        ImGui::TextWrapped(m_ctx, (action_description_1 + ". " + action_description_2).c_str());

        ImGui::GetContentRegionAvail(m_ctx, &x_width, &y_width);
        ImGui::SetCursorPosX(m_ctx, x_width - 55.0);
        ImGui::SetCursorPosY(m_ctx, 0);

        UiStyledElements::PushReaSonusFunctionButtonStyle(m_ctx);

        ImGui::PushFont(m_ctx, assets->GetIconFont(), 24);
        if (ImGui::Button(m_ctx, std::string(1, IconRestore).c_str())) {
            HandleResetButtonClick(index, type);
        }
        ImGui::PopFont(m_ctx);

        ReaSonusSimpleTooltip(
            m_ctx,
            assets,
            page.i18n->t("functions", type_name + ".button.clear.tooltip"),
            "clear-function-tooltip" + std::to_string(index)
        );

        ImGui::SetCursorPosX(m_ctx, x_width - 16.0);
        ImGui::SetCursorPosY(m_ctx, 0);

        ImGui::PushFont(m_ctx, assets->GetIconFont(), 24);
        if (ImGui::Button(m_ctx, std::string(1, IconSearchAction).c_str())) {
            PromptForFunctionAction(index, type);
        }
        ImGui::PopFont(m_ctx);

        ReaSonusSimpleTooltip(
            m_ctx,
            assets,
            page.i18n->t("functions", type_name + ".button.tooltip"),
            "add-function-tooltip" + std::to_string(index)
        );

        UiStyledElements::PopReaSonusFunctionButtonStyle(m_ctx);
        ImGui::EndChild(m_ctx);
    }
}

void CSurf_UI_FunctionKeysPage::RenderFunctionTab(
    std::string tab_label,
    const FunctionTypes type,
    const int tab_index,
    const int start_index,
    const int count,
    CSurf_UI_Assets *assets
) {
    UiStyledElements::PushReaSonusTabStyle(m_ctx, selected_tab == tab_index);
    if (ImGui::BeginTabItem(m_ctx, tab_label.c_str())) {
        selected_tab = tab_index;
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 8);

        if (ImGui::BeginChild(m_ctx, "left-shift-group", 0, 0, ImGui::ChildFlags_None)) {
            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_CellPadding, 6, 6);

            if (ImGui::BeginTable(m_ctx, "function_keys_grid", 2)) {
                for (int i = start_index; i < start_index + count; i++) {
                    if (ImGui::TableNextColumn(m_ctx)) {
                        UiStyledElements::PushReaSonusFunctionActionStyle(m_ctx);
                        RenderFunction(m_ctx, i, *this, assets, type);
                        UiStyledElements::PopReaSonusFunctionActionStyle(m_ctx);
                    }
                }
                ImGui::PopStyleVar(m_ctx);
                ImGui::EndTable(m_ctx);
            }
            ImGui::EndChild(m_ctx);
        }
        ImGui::EndTabItem(m_ctx);
    }
    UiStyledElements::PopReaSonusTabStyle(m_ctx);
}

void CSurf_UI_FunctionKeysPage::RenderFPV2FunctionGroup() {
    if (ImGui::BeginChild(m_ctx, "settings-group", 0, 0, ImGui::ChildFlags_None)) {
        UiStyledElements::PushReaSonusTabBarStyle(m_ctx);
        if (ImGui::BeginTabBar(m_ctx, "FunctionsTabs", ImGui::TabBarFlags_None)) {
            RenderFunctionTab(i18n->t("functions", "tab.v2-functions"), FUNCTION_DEFAULT, 0, 0, 4, assets);
            RenderFunctionTab(i18n->t("functions", "tab.footswitch"), FUNCTION_FOOTSWITCH, 1, 0, 3, assets);
            RenderFunctionTab(i18n->t("functions", "tab.transport"), FUNCTION_TRANSPORT, 2, 0, 4, assets);

            UiStyledElements::PopReaSonusTabBarStyle(m_ctx);
            ImGui::EndTabBar(m_ctx);
        }
        ImGui::EndChild(m_ctx);
    }
}

void CSurf_UI_FunctionKeysPage::RenderFP8FunctionGroup() {
    if (ImGui::BeginChild(m_ctx, "settings-group", 0, 0, ImGui::ChildFlags_None)) {
        UiStyledElements::PushReaSonusTabBarStyle(m_ctx);
        if (ImGui::BeginTabBar(m_ctx, "FunctionsTabs", ImGui::TabBarFlags_None)) {
            RenderFunctionTab(i18n->t("functions", "tab.left-shift"), FUNCTION_DEFAULT, 0, 0, 8, assets);
            RenderFunctionTab(i18n->t("functions", "tab.right-shift"), FUNCTION_DEFAULT, 1, 8, 8, assets);
            RenderFunctionTab(i18n->t("functions", "tab.footswitch"), FUNCTION_FOOTSWITCH, 2, 0, 3, assets);
            RenderFunctionTab(i18n->t("functions", "tab.transport"), FUNCTION_TRANSPORT, 3, 0, 4, assets);

            UiStyledElements::PopReaSonusTabBarStyle(m_ctx);
            ImGui::EndTabBar(m_ctx);
        }
        ImGui::EndChild(m_ctx);
    }
}

void CSurf_UI_FunctionKeysPage::Render() {
    if (selected_action != -1) {
        switch (selected_type) {
            case FUNCTION_DEFAULT:
                functions[selected_function] = std::to_string(selected_action);
                break;
            case FUNCTION_FOOTSWITCH:
                footswitch[selected_function] = std::to_string(selected_action);
                break;
            case FUNCTION_TRANSPORT:
                transport[selected_function] = std::to_string(selected_action);
                break;
        }

        selected_action = -1;
        selected_function = -1;
        selected_type = FUNCTION_DEFAULT;
    }

    if (device == FP_8) {
        RenderFP8FunctionGroup();
    } else {
        RenderFPV2FunctionGroup();
    }
}
