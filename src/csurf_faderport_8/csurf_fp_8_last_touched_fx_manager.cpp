#include "csurf_fp_8_last_touched_fx_manager.hpp"

CSurf_FP_8_LastTouchedFXManager::CSurf_FP_8_LastTouchedFXManager(
    CSurf_FP_8_Track *track,
    CSurf_Context *context,
    midi_Output *m_midiout) : track(track), context(context), m_midiout(m_midiout)
{
    track->SetMuteButtonValue(BTN_VALUE_OFF);
    track->SetSoloButtonValue(BTN_VALUE_OFF);
    track->SetSelectButtonValue(BTN_VALUE_OFF);
};

void CSurf_FP_8_LastTouchedFXManager::UpdateTrack(bool force_update)
{
    int trackNumber = -1;
    int fxNumber = -1;
    int paramNumber = -1;
    const char *trackName;
    char fxName[256];
    char paramName[256];
    char paramValueString[256];
    double paramValue = 0.0;

    if (hasLastTouchedFxEnabled != context->GetLastTouchedFxMode() && context->GetLastTouchedFxMode())
    {
        force_update = true;
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
            TrackFX_GetParamName(media_track, fxNumber, paramNumber, paramName, std::size(paramName));
            TrackFX_GetFormattedParamValue(media_track, fxNumber, paramNumber, paramValueString, std::size(paramValueString));
            paramValue = TrackFX_GetParamNormalized(media_track, fxNumber, paramNumber);
        }
    }
    else
    {
        trackName = "No Track";
    }

    track->SetDisplayMode(DISPLAY_MODE_2, force_update);
    track->SetDisplayLine(0, ALIGN_LEFT, trackName, NON_INVERT, force_update);
    track->SetDisplayLine(1, ALIGN_LEFT, paramNumber < 0 ? "No Param" : StripPluginNamePrefixes(fxName).c_str(), INVERT, force_update);
    track->SetDisplayLine(2, ALIGN_LEFT, paramNumber < 0 ? "  " : std::string(paramName).c_str(), NON_INVERT, force_update);
    track->SetDisplayLine(3, ALIGN_CENTER, paramNumber < 0 ? "  " : paramValueString, NON_INVERT, force_update);
    track->SetFaderValue((int)(paramValue * 16383.0), force_update);

    track->SetMuteButtonValue(BTN_VALUE_OFF, force_update);
    track->SetSoloButtonValue(BTN_VALUE_OFF, force_update);
    track->SetSelectButtonValue(BTN_VALUE_OFF, force_update);

    hasLastTouchedFxEnabled = context->GetLastTouchedFxMode();
    force_update = false;
};

void CSurf_FP_8_LastTouchedFXManager::HandleSelectClick(int index)
{
    (void)index;
};

void CSurf_FP_8_LastTouchedFXManager::HandleMuteClick(int index)
{
    (void)index;
};

void CSurf_FP_8_LastTouchedFXManager::HandleSoloClick(int index)
{
    (void)index;
};

void CSurf_FP_8_LastTouchedFXManager::HandleFaderTouch() {};

void CSurf_FP_8_LastTouchedFXManager::HandleFaderMove(int msb, int lsb)
{
    int trackNumber = -1;
    int fxNumber = -1;
    int paramNumber = -1;

    if (GetLastTouchedFX(&trackNumber, &fxNumber, &paramNumber))
    {
        if (MediaTrack *media_track = GetTrack(0, trackNumber - 1))
        {
            TrackFX_SetParamNormalized(media_track, fxNumber, paramNumber, int14ToNormalized(msb, lsb));
        }
    }
};
