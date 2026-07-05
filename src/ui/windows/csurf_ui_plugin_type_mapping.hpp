#ifndef CSURF_FP_UI_TEST_V2_H_
#define CSURF_FP_UI_TEST_V2_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <mini/ini.h>
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>
#include <memory>
#include <string>
#include <vector>
#include <regex>
#include "../csurf_ui_assets.hpp"
#include "../../shared/csurf_utils.hpp"
#include "../../shared/csurf_plugin_utils.hpp"

class ReaSonusPluginTypeMapping {
private:
    static void Loop();

    static std::unique_ptr<ReaSonusPluginTypeMapping> s_inst;
    CSurf_UI_Assets *assets;

    void SetPluginFolders();

    std::vector<std::string> SetDeveloperPlugins(std::string developer);

    bool AddPluginType();

    void RenderMappingsList();

    ReaSonusPluginTypeMapping();

    void Frame();

    ImGui_Context *m_ctx;
    std::string save_message;
    mINI::INIStructure plugin_data;

    std::string plugin_folder_path = createPathName({std::string(GetResourcePath()), "ReaSonus", "Plugins"});
    std::vector<std::string> developers;
    int selected_developer = -1;

    std::vector<std::vector<std::string> > plugins;
    int selected_plugin = -1;

    std::vector<std::string> plugin_types = PluginUtils::GetPluginTypes();
    int selected_plugin_type = -1;

    bool add_type_clicked = false;
    bool close_clicked = false;

public:
    inline static bool window_open = false;

    static void Start();

    static void Stop();

    ~ReaSonusPluginTypeMapping();
};

#endif
