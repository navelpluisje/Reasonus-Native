
#include "csurf_last_touched_fx_manager.hpp"
#include "csurf_utils.hpp"

string StripFxType(char *name)
{
    string s = string(name);
    string delimiter = ": ";
    int pos = s.find(delimiter) + size(delimiter);
    if (pos < 0)
    {
        return s;
    }
    s.erase(0, pos);
    return s;
}

CSurf_LastTouchedFXManager::CSurf_LastTouchedFXManager(
    CSurf_Track *track,
    CSurf_Context *context,
    midi_Output *m_midiout) : track(track), context(context), m_midiout(m_midiout)
{
    track->SetMuteButtonValue(BTN_VALUE_OFF);
    track->SetSoloButtonValue(BTN_VALUE_OFF);
    track->SetSelectButtonValue(BTN_VALUE_OFF);
};

void CSurf_LastTouchedFXManager::UpdateTrack()
{
    int trackNumber = -1;
    int fxNumber = -1;
    int paramNumber = -1;
    const char *trackName;
    char fxName[256];
    char paramName[256];
    char paramValueString[256];
    double paramValue = 0.0, min, max;

    if (hasLastTouchedFxEnabled != context->GetLastTouchedFxMode() && context->GetLastTouchedFxMode())
    {
        forceUpdate = true;
    }

    if (GetLastTouchedFX(&trackNumber, &fxNumber, &paramNumber))
    {
        // Somehow this prevents reaper from crashing while selecting link mode.
        // TODO: finsd a better way for this
        char buffer[250];
        snprintf(buffer, sizeof(buffer), "Zone -- %d\n", paramNumber);

        if (MediaTrack *media_track = GetTrack(0, trackNumber - 1))
        {
            trackName = (const char *)GetSetMediaTrackInfo(media_track, "P_NAME", NULL);
            TrackFX_GetFXName(media_track, fxNumber, fxName, sizeof(fxName));
            TrackFX_GetParamName(media_track, fxNumber, paramNumber, paramName, size(paramName));
            TrackFX_GetFormattedParamValue(media_track, fxNumber, paramNumber, paramValueString, size(paramValueString));
            paramValue = TrackFX_GetParam(media_track, fxNumber, paramNumber, &min, &max);
        }
    }
    else
    {
        trackName = "No Track";
    }

    track->SetDisplayMode(DISPLAY_MODE_2, forceUpdate);
    track->SetDisplayLine(0, ALIGN_LEFT, trackName, NON_INVERT, forceUpdate);
    track->SetDisplayLine(1, ALIGN_LEFT, paramNumber < 0 ? "No Param" : StripFxType(fxName).c_str(), INVERT, forceUpdate);
    track->SetDisplayLine(2, ALIGN_LEFT, paramNumber < 0 ? "  " : string(paramName).c_str(), NON_INVERT, forceUpdate);
    track->SetDisplayLine(3, ALIGN_CENTER, paramNumber < 0 ? "  " : paramValueString, NON_INVERT, forceUpdate);
    track->SetFaderValue((int)(paramValue * 16383.0), forceUpdate);

    track->SetMuteButtonValue(BTN_VALUE_OFF, forceUpdate);
    track->SetSoloButtonValue(BTN_VALUE_OFF, forceUpdate);
    track->SetSelectButtonValue(BTN_VALUE_OFF, forceUpdate);

    hasLastTouchedFxEnabled = context->GetLastTouchedFxMode();
    forceUpdate = false;
};

void CSurf_LastTouchedFXManager::HandleSelectClick(int index)
{
    (void)index;
};

void CSurf_LastTouchedFXManager::HandleMuteClick(int index)
{
    (void)index;
};

void CSurf_LastTouchedFXManager::HandleSoloClick(int index)
{
    (void)index;
};

void CSurf_LastTouchedFXManager::HandleFaderTouch() {};

void CSurf_LastTouchedFXManager::HandleFaderMove(int msb, int lsb)
{
    int trackNumber = -1;
    int fxNumber = -1;
    int paramNumber = -1;

    if (GetLastTouchedFX(&trackNumber, &fxNumber, &paramNumber))
    {
        if (MediaTrack *media_track = GetTrack(0, trackNumber - 1))
        {
            TrackFX_SetParam(media_track, fxNumber, paramNumber, int14ToNormalized(msb, lsb));
        }
    }
};
