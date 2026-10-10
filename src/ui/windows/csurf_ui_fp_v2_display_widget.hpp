#ifndef CSURF_FP_TRANSLATION_EDITOR_H_
#define CSURF_FP_TRANSLATION_EDITOR_H_

#include <array>
#include <reaper_imgui_functions.h>
#include <string>
#include "../csurf_ui_assets.hpp"

class ReaSonusFpV2DisplayWidget {
public:
    inline static bool window_open = false;

    static void Start();

    static void Stop();

    ~ReaSonusFpV2DisplayWidget();

private:
    int track_index = -2;
    MediaTrack *media_track;
    std::array<std::string, 5> display_values;

    void SetDisplayValues();

    static void Loop();

    CSurf_UI_Assets *assets;
    static std::unique_ptr<ReaSonusFpV2DisplayWidget> s_inst;

    ReaSonusFpV2DisplayWidget();

    void Frame();

    ImGui_Context *m_ctx;

    bool save_clicked = false;
    bool close_clicked = false;
    std::string save_message;
};

#endif
