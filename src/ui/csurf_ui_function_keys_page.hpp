#ifndef CSURF_UI_FUNCTION_KEYS_PAGE_H
#define CSURF_UI_FUNCTION_KEYS_PAGE_H

#include <vector>
#include "csurf_ui_page_content.hpp"
#include "../i18n/i18n.hpp"
#include "../shared/csurf_reasonus_settings.hpp"

enum FunctionTypes
{
    TypeFunction,
    TypeFootSwitch,
};

class CSurf_UI_FunctionKeysPage : public CSurf_UI_PageContent
{
protected:
    std::vector<std::string> functions;
    std::vector<std::string> footswitch;
    ImGui_Font *function_font_bold;
    ImGui_Image *icon_search;
    ImGui_Image *icon_reset;
    std::string device;
    I18n *i18n;
    int selected_tab = 0;

public:
    static bool querying_actions;
    static int selected_function;
    static int selected_action;
    static FunctionTypes selected_type;

    CSurf_UI_FunctionKeysPage(ImGui_Context *m_ctx, std::string _device);

    virtual ~CSurf_UI_FunctionKeysPage() {};

    void Reset() override;

    void Save() override;

    void Render() override;

    static void ActionListTimer();

    static void PromptForFunctionAction(int index, FunctionTypes type);

    static void HandleResetButtonClick(int index, FunctionTypes type);

    static void RenderFunction(ImGui_Context *m_ctx, int index, CSurf_UI_FunctionKeysPage &page, FunctionTypes type = TypeFunction);

    void RenderFunctionTab(std::string tab_label, FunctionTypes type, int tab_index, int start_index, int count);

    void RenderFPV2FunctionGroup();

    void RenderFP8FunctionGroup();
};

#endif
