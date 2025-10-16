#include "csurf_ui_page_content.hpp"
#include "csurf_ui_elements.hpp"
#include <array>
#include <string>
#include <reaper_imgui_functions.h>
#include <functional>
#include "csurf_ui_images.h"
#include "../ui/csurf_ui_colors.hpp"
#include "../i18n/i18n.hpp"

class CSurf_UI_FunctionKeysPage : public CSurf_UI_PageContent
{
protected:
    std::vector<std::string> functions;
    ImGui_Font *function_font_bold;
    ImGui_Image *icon_search;
    std::string device;
    I18n *i18n;

public:
    static bool querying_actions;
    static int selected_function;
    static int selected_action;

    CSurf_UI_FunctionKeysPage(ImGui_Context *m_ctx, std::string _device) : CSurf_UI_PageContent(m_ctx, _device)
    {
        i18n = I18n::GetInstance();
        device = _device;
        function_font_bold = ImGui::CreateFont("Arial", ImGui::FontFlags_Bold);
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(function_font_bold));
        icon_search = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_search), sizeof(img_icon_search));
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_search));

        Reset();
    };

    virtual ~CSurf_UI_FunctionKeysPage() {};

    void Reset() override
    {
        functions.push_back(ini["functions"]["1"]);
        functions.push_back(ini["functions"]["2"]);
        functions.push_back(ini["functions"]["3"]);
        functions.push_back(ini["functions"]["4"]);
        if (device == FP_8)
        {
            functions.push_back(ini["functions"]["5"]);
            functions.push_back(ini["functions"]["6"]);
            functions.push_back(ini["functions"]["7"]);
            functions.push_back(ini["functions"]["8"]);
        }
    }

    void Save() override
    {
        mINI::INIFile file(GetReaSonusIniPath(device));
        readAndCreateIni(ini, device);

        ini["functions"]["1"] = functions[0];
        ini["functions"]["2"] = functions[1];
        ini["functions"]["3"] = functions[2];
        ini["functions"]["4"] = functions[3];
        if (device == FP_8)
        {
            ini["functions"]["5"] = functions[4];
            ini["functions"]["6"] = functions[5];
            ini["functions"]["7"] = functions[6];
            ini["functions"]["8"] = functions[7];
        }

        if (file.write(ini, true))
        {
            MB("Changes saved with success", "Woohoo", 0);
        };
    }

    static void ActionListTimer()
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

    static void PromptForFunctionAction(int index)
    {
        selected_function = index;
        PromptForAction(1, 0, 0);

        if (!querying_actions)
        {
            querying_actions = true;
            plugin_register("timer", (void *)&ActionListTimer);
        }
    }

    static void RenderFunction(ImGui_Context *m_ctx, int index, CSurf_UI_FunctionKeysPage &page)
    {
        std::string idx = "function-key-" + std::to_string(index);
        std::string button_idx = "button-key-" + std::to_string(index);
        std::string tooltip_idx = "tooltip-key-" + std::to_string(index);
        int actionId = stoi(page.functions[index]);
        const char *fullName = kbd_getTextFromCmd(actionId, 0);
        std::vector<std::string> actionInfo = split(fullName, ": ");
        std::string action_group = actionInfo.size() > 1 ? actionInfo[0] : page.i18n->t("functions.item.no-group");
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
            ImGui::PushFont(m_ctx, page.function_font_bold, 13);
            ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
            ImGui::Text(m_ctx, (page.i18n->t("functions.item.label") + " " + std::to_string(index + 1) + ":").c_str());
            ImGui::PopStyleColor(m_ctx);
            ImGui::SameLine(m_ctx);
            ImGui::Text(m_ctx, page.functions[index].c_str());
            ImGui::PopFont(m_ctx);
            ImGui::Text(m_ctx, action_group.c_str());
            ImGui::TextWrapped(m_ctx, (action_description_1 + ". " + action_description_2).c_str());

            double x_width, y_width;
            ImGui::GetContentRegionAvail(m_ctx, &x_width, &y_width);
            ImGui::SetCursorPosX(m_ctx, x_width - 16.0);
            ImGui::SetCursorPosY(m_ctx, 0);
            UiElements::PushReaSonusFunctionButtonStyle(m_ctx);

            if (ImGui::ImageButton(m_ctx, button_idx.c_str(), page.icon_search, 24, 24))
            {
                PromptForFunctionAction(index);
            }

            if (ImGui::BeginItemTooltip(m_ctx))
            {
                UiElements::PushReaSonusTooltipStyle(m_ctx);
                if (ImGui::BeginChild(m_ctx, tooltip_idx.c_str(), 0.0, 0.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_AutoResizeX))
                {
                    ImGui::PushTextWrapPos(m_ctx, 260);
                    ImGui::Text(m_ctx, page.i18n->t("functions.item.button.tooltip").c_str());
                    ImGui::PopTextWrapPos(m_ctx);
                    ImGui::EndChild(m_ctx);
                    UiElements::PopReaSonusTooltipStyle(m_ctx);
                }
                ImGui::EndTooltip(m_ctx);
            }
            // ImGui::SetItemTooltip(m_ctx, "Open the action list and select an action");
            UiElements::PopReaSonusFunctionButtonStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void Render() override
    {
        if (selected_action != -1)
        {
            functions[selected_function] = std::to_string(selected_action);
            selected_action = -1;
            selected_function = -1;
        }

        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);
        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_CellPadding, 6, 6);
        if (ImGui::BeginTable(m_ctx, "function_keys_grid", 2))
        {
            for (int i = 0; i < (device == FP_8 ? 8 : 4); i++)
            {
                if (ImGui::TableNextColumn(m_ctx))
                {
                    UiElements::PushReaSonusFunctionActionStyle(m_ctx);
                    RenderFunction(m_ctx, i, *this);
                    UiElements::PopReaSonusFunctionActionStyle(m_ctx);
                }
            }
            ImGui::EndTable(m_ctx);
            ImGui::PopStyleVar(m_ctx);
        }
    }
};
