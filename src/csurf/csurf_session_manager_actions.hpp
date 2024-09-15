#ifndef CSURF_SESSION_MANAGER_ACTIONS_H_
#define CSURF_SESSION_MANAGER_ACTIONS_H_

void Main_OnCommandStringEx(string action_name, int flag = 0, ReaProject *proj = 0)
{
    int actionId = NamedCommandLookup(action_name.c_str());
    Main_OnCommandEx(actionId, flag, proj);
}

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

#endif // CSURF_SESSION_MANAGER_ACTIONS_H_