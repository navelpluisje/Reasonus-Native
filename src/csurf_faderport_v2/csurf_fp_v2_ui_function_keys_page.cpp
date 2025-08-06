#include "./csurf_fp_v2_ui_page_content.hpp"
#include "../ui/csurf_ui_elements.hpp"
#include <array>
#include <string>
#include <reaper_imgui_functions.h>
#include <functional>
#include "../ui/csurf_ui_images.h"

class CSurf_FP_V2_FunctionKeysPage : public CSurf_FP_V2_PageContent
{
protected:
    std::array<std::string, 4> functions;
    ImGui_Font *function_font_bold;
    ImGui_Image *icon_search;

public:
    static bool querying_actions;
    static int selected_function;
    static int selected_action;

    CSurf_FP_V2_FunctionKeysPage(ImGui_Context *m_ctx) : CSurf_FP_V2_PageContent(m_ctx)
    {
        function_font_bold = ImGui::CreateFont("sans-serif", 13, ImGui::FontFlags_Bold);
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(function_font_bold));
        icon_search = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_search), sizeof(img_icon_search));
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_search));

        reset();
    };

    virtual ~CSurf_FP_V2_FunctionKeysPage() {};

    void reset() override
    {
        functions[0] = ini["functions"]["1"];
        functions[1] = ini["functions"]["2"];
        functions[2] = ini["functions"]["3"];
        functions[3] = ini["functions"]["4"];
    }

    void save() override
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_V2));
        readAndCreateIni(ini, FP_V2);

        ini["functions"]["1"] = functions[0];
        ini["functions"]["2"] = functions[1];
        ini["functions"]["3"] = functions[2];
        ini["functions"]["4"] = functions[3];

        if (file.write(ini, true))
        {
            MB("Chages saved with success", "Woohoo", 0);
        };
    }

    static void actionListTimer()
    {
        WDL_ASSERT(querying_actions == true);
        const int actionId = PromptForAction(0, 0, 0);
        if (actionId == 0)
        {
            return;
        }
        plugin_register("-timer", (void *)&actionListTimer);
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
            plugin_register("timer", (void *)&actionListTimer);
        }
    }

    static void renderFunction(ImGui_Context *m_ctx, int index, CSurf_FP_V2_FunctionKeysPage &page)
    {
        std::string idx = "function-key-" + std::to_string(index);
        std::string button_idx = "button-key-" + std::to_string(index);
        std::string tooltip_idx = "tooltip-key-" + std::to_string(index);
        int actionId = stoi(page.functions[index]);
        const char *fullName = kbd_getTextFromCmd(actionId, 0);
        std::vector<std::string> actionInfo = split(fullName, ": ");
        std::string action_group = actionInfo.size() > 1 ? actionInfo[0] : "No group";
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
            ImGui::PushFont(m_ctx, page.function_font_bold);
            ImGui::Text(m_ctx, page.functions[index].c_str());
            ImGui::PopFont(m_ctx);
            ImGui::Text(m_ctx, action_group.c_str());
            ImGui::TextWrapped(m_ctx, (action_description_1 + ". " + action_description_2).c_str());

            double x_width, y_width;
            ImGui::GetContentRegionMax(m_ctx, &x_width, &y_width);
            ImGui::SetCursorPosX(m_ctx, x_width - 28.0);
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
                    ImGui::Text(m_ctx, "Open the actionlist to select the\naction for this function key");
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

    void render() override
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
            for (int i = 0; i < (int)functions.size(); i++)
            {
                if (ImGui::TableNextColumn(m_ctx))
                {
                    UiElements::PushReaSonusFunctionActionStyle(m_ctx);
                    renderFunction(m_ctx, i, *this);
                    UiElements::PopReaSonusFunctionActionStyle(m_ctx);
                }
            }
            ImGui::EndTable(m_ctx);
            ImGui::PopStyleVar(m_ctx);
        }
    }
};
