#ifndef CSURF_FP_TRANSLATION_EDITOR_H_
#define CSURF_FP_TRANSLATION_EDITOR_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>
#include <memory>
#include <string>
#include <vector>
#include <mini/ini.h>
#include "csurf_ui_assets.hpp"

class ReaSonusTranslationEditor
{
public:
    inline static bool window_open = false;

    static void Start();
    static void Stop();

    ~ReaSonusTranslationEditor();

private:
    static void Loop();
    CSurf_UI_Assets *assets;
    static std::unique_ptr<ReaSonusTranslationEditor> s_inst;
    void InitAssets();
    void GetBaseLanguage();
    void GetSelectedLanguage();
    void GetLanguageList();
    void HandleAddLanguage();
    void getMultilineString(std::string &value, double width, double &height);
    void RenderTranslation(std::string section, std::string key);
    void HandleRemoveLanguageListItem(int index);
    void SaveChanges();

    ReaSonusTranslationEditor();
    void Frame();

    ImGui_Context *m_ctx;
    ImGui_Image *logo;

    std::vector<std::string> language_list;
    mINI::INIStructure base_file;
    mINI::INIStructure translation_file;

    std::string new_language;
    int selected_language = -1;
    int previous_selected_language = -1;

    bool show_empty_only = false;
    bool show_translation_key = false;
    bool save_clicked = false;
    bool close_clicked = false;
    std::string save_message = "";
    int save_message_timer = 0;
};

#endif