#include "csurf_session_manager_actions.hpp"
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin.h>
#include <reaper_plugin_functions.h>
#include "csurf_daw.hpp"
#include "csurf_utils.hpp"

void SetMasterPanToCenter() // Xenakios/SWS: Set master volume to 0 dB
{
    MediaTrack *master_track = GetMasterTrack(0);
    int pan_mode = DAW::GetTrackPanMode(master_track);
    if (pan_mode < 4)
    {
        SetMediaTrackInfo_Value(master_track, "D_PAN", 0);
    }
    if (pan_mode == 6)
    {
        SetMediaTrackInfo_Value(master_track, "D_DUALPANL", -1);
        SetMediaTrackInfo_Value(master_track, "D_DUALPANR", 1);
    }
    if (pan_mode == 5)
    {
        SetMediaTrackInfo_Value(master_track, "D_PAN", 0);
        SetMediaTrackInfo_Value(master_track, "D_WIDTH", 1);
    }
}

void UpdateMasterPanValue(int val, bool left_shift)
{
    double pan1, pan2 = 0.0;
    int pan_mode;
    MediaTrack *media_track = GetMasterTrack(0);
    GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);

    if (left_shift || pan_mode < 4)
    {
        double newValue = int(panToNormalized(pan1) * 127.0) + val;
        newValue = minmax(0.0, newValue, 127.0);
        SetMediaTrackInfo_Value(media_track, pan_mode < 6 ? "D_PAN" : "D_DUALPANL", normalizedToPan(newValue / 127));
    }
    else
    {
        double newValue = int(panToNormalized(pan2) * 127.0) + val;
        newValue = minmax(0.0, newValue, 127.0);
        SetMediaTrackInfo_Value(media_track, pan_mode < 6 ? "D_WIDTH" : "D_DUALPANR", normalizedToPan(newValue / 127));
    }
}

void IncrementMasterPan(int val, bool left_shift)
{
    UpdateMasterPanValue(val, left_shift);
}

void DecrementMasterPan(int val, bool left_shift)
{
    UpdateMasterPanValue(val * -1, left_shift);
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
