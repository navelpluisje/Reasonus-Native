#ifndef CSURF_FP_TRANSLATION_EDITOR_H_
#define CSURF_FP_TRANSLATION_EDITOR_H_

#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <reaper_imgui_functions.h>
#include <string>
#include "./csurf_ui_assets.hpp"

class ReaSonusMessage
{
public:
    inline static bool window_open = false;

    static void Start();
    static void Stop();

    ~ReaSonusMessage();

private:
    static void Loop();
    CSurf_UI_Assets *assets;
    static std::unique_ptr<ReaSonusMessage> s_inst;

    ReaSonusMessage();
    void Frame();

    ImGui_Context *m_ctx;

    bool save_clicked = false;
    bool close_clicked = false;
    std::string save_message = "";
};

#endif