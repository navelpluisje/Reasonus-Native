#include "../ui/csurf_ui_page_content.hpp"
#include <reaper_imgui_functions.h>
#include "../ui/csurf_ui_checkbox.hpp"

class CSurf_FP_V2_SettingsPage : public CSurf_UI_PageContent
{
protected:
    bool momentary_mute_solo;
    bool control_hidden_tracks;

public:
    CSurf_FP_V2_SettingsPage(ImGui_Context *m_ctx) : CSurf_UI_PageContent(m_ctx, FP_V2)
    {
        reset();
    };

    virtual ~CSurf_FP_V2_SettingsPage() {};

    void render() override
    {
        ReaSonusCheckBox(m_ctx, "Momentary Mute and Solo button", &momentary_mute_solo);
        ReaSonusCheckBox(m_ctx, "Control hidden tracks", &control_hidden_tracks);
    }

    void save() override
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_V2));
        readAndCreateIni(ini, FP_V2);

        ini["surface"]["mute-solo-momentary"] = momentary_mute_solo ? "1" : "0";
        ini["surface"]["control-hidden-tracks"] = control_hidden_tracks ? "1" : "0";

        if (file.write(ini, true))
        {
            MB("Chages saved with success", "Woohoo", 0);
        };
    }

    void reset() override
    {
        momentary_mute_solo = ini["surface"]["mute-solo-momentary"] == "1";
        control_hidden_tracks = ini["surface"]["control-hidden-tracks"] == "1";
    }
};
