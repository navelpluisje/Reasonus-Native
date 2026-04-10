#include "csurf_faderport_ui_imgui_utils.hpp"
#include "../ui/windows/csurf_ui_fp_8_control_panel.hpp"
#include "../ui/pages/csurf_ui_fp_v2_control_panel.hpp"

void ToggleFP8ControlPanel(int page_index)
{
    if (ReaSonus8ControlPanel::control_panel_open && ReaSonus8ControlPanel::current_page == page_index)
    {
        Main_OnCommandStringEx("_REASONUS_SHOW_REASONUS_8_CONTROL_WINDOW", 0, 0);
    }
    else
    {
        if (!ReaSonus8ControlPanel::control_panel_open)
        {
            Main_OnCommandStringEx("_REASONUS_SHOW_REASONUS_8_CONTROL_WINDOW", 0, 0);
            ReaSonus8ControlPanel::SetCurrentPage(page_index);
        }
    }
}

void ToggleFPV2ControlPanel(int page_index)
{
    if (ReaSonusV2ControlPanel::control_panel_open && ReaSonusV2ControlPanel::current_page == page_index)
    {
        Main_OnCommandStringEx("_REASONUS_SHOW_REASONUS_V2_CONTROL_WINDOW", 0, 0);
    }
    else
    {
        if (!ReaSonus8ControlPanel::control_panel_open)
        {
            Main_OnCommandStringEx("_REASONUS_SHOW_REASONUS_V2_CONTROL_WINDOW", 0, 0);
            ReaSonusV2ControlPanel::SetCurrentPage(page_index);
        }
    }
}
