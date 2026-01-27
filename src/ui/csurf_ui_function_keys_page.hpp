#ifndef CSURF_UI_FUNCTION_KEYS_PAGE_H
#define CSURF_UI_FUNCTION_KEYS_PAGE_H

#include <vector>
#include "csurf_ui_page_content.hpp"
#include "../i18n/i18n.hpp"
#include "../shared/csurf_reasonus_settings.hpp"
#include "csurf_ui_assets.hpp"

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
    std::string device;
    int selected_tab = 0;

public:
    static bool querying_actions;
    static int selected_function;
    static int selected_action;
    static FunctionTypes selected_type;

    CSurf_UI_FunctionKeysPage(ImGui_Context *m_ctx, CSurf_UI_Assets *assets, std::string _device);

    virtual ~CSurf_UI_FunctionKeysPage() {};

    void Reset() override;

    void Save() override;

    void Render() override;

    static void ActionListTimer();

    static void PromptForFunctionAction(int index, FunctionTypes type);

    static void HandleResetButtonClick(int index, FunctionTypes type);

    static void RenderFunction(ImGui_Context *m_ctx, int index, CSurf_UI_FunctionKeysPage &page, CSurf_UI_Assets *assets, FunctionTypes type = TypeFunction);

    void RenderFunctionTab(std::string tab_label, FunctionTypes type, int tab_index, int start_index, int count, CSurf_UI_Assets *assets);

    void RenderFPV2FunctionGroup();

    void RenderFP8FunctionGroup();
};

#endif
