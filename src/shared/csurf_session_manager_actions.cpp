#include "csurf_session_manager_actions.hpp"
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin.h>
#include <reaper_plugin_functions.h>

void SetMasterPanToCenter() // Xenakios/SWS: Set master volume to 0 dB
{
    MediaTrack *masterTrack = GetMasterTrack(0);
    SetMediaTrackInfo_Value(masterTrack, "D_PAN", 0);
}

void IncrementMetronomeVolume()
{
    int actionId = NamedCommandLookup("_S&M_METRO_VOL_UP");
    Main_OnCommandEx(actionId, 0, 0);
    Main_OnCommandEx(actionId, 0, 0);
    Main_OnCommandEx(actionId, 0, 0);
    Main_OnCommandEx(actionId, 0, 0);
    Main_OnCommandEx(actionId, 0, 0);
    Main_OnCommandEx(actionId, 0, 0);
}

void DecrementMetronomeVolume()
{
    int actionId = NamedCommandLookup("_S&M_METRO_VOL_DOWN");
    Main_OnCommandEx(actionId, 0, 0);
    Main_OnCommandEx(actionId, 0, 0);
    Main_OnCommandEx(actionId, 0, 0);
    Main_OnCommandEx(actionId, 0, 0);
    Main_OnCommandEx(actionId, 0, 0);
    Main_OnCommandEx(actionId, 0, 0);
}
