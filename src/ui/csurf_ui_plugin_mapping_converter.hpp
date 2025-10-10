#ifndef CSURF_FP_UI_TEST_V2_H_
#define CSURF_FP_UI_TEST_V2_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>
#include <memory>
#include <string>
#include <vector>
#include <regex>
#include "../ui/csurf_ui_page_content.hpp"

class ReaSonusV2PluginMappingConverter
{
public:
    inline static bool converter_open = false;

    static void Start();
    static void Stop();

    ~ReaSonusV2PluginMappingConverter();

    // Regular expressions:
    std::regex select;
    std::regex fader;
    std::regex scribble;
    std::regex zone;
    std::regex namedZone;

private:
    static void Loop();
    static std::unique_ptr<ReaSonusV2PluginMappingConverter> s_inst;
    void InitAssets();
    void SetBaseFileNames(std::string folder);
    void ConvertFile(std::string file_path);
    void ConvertFiles();
    void Convert();
    CSurf_UI_PageContent *page_content = NULL;

    ReaSonusV2PluginMappingConverter();
    void Frame();

    ImGui_Context *m_ctx;
    ImGui_Font *main_font;
    ImGui_Font *main_font_bold;
    ImGui_Image *logo;

    bool convert_clicked = false;
    bool close_clicked = false;
    std::vector<std::string> converted_files;
    std::vector<std::string> unconverted_files;
    std::vector<std::string> file_names;
    mINI::INIStructure ini;
    std::string path_name;
};

#endif