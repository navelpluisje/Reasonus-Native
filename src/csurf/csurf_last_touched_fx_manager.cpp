
#include "csurf_last_touched_fx_manager.hpp"

CSurf_LastTouchedFXManager::CSurf_LastTouchedFXManager(
    CSurf_Track *track,
    CSurf_Context *context,
    midi_Output *m_midiout) : track(track), context(context), m_midiout(m_midiout) {
                                  // track->SetMuteButtonValue(BTN_VALUE_OFF);
                                  // track->SetSoloButtonValue(BTN_VALUE_OFF);
                                  // track->SetSelectButtonValue(BTN_VALUE_OFF);
                              };

void CSurf_LastTouchedFXManager::UpdateTrack() {
    // int trackNumber = -1;
    // int fxNumber = -1;
    // int paramNumber = -1;
    // const char *trackName;
    // // char fxName[512];
    // // char paramName[512];
    // // double paramValue;
    // // double paramNormalizedValue = 0.0;

    // if (GetLastTouchedFX(&trackNumber, &fxNumber, &paramNumber))
    // {
    //     if (MediaTrack *media_track = GetTrack(0, trackNumber))
    //     {
    //         trackName = (const char *)GetSetMediaTrackInfo(media_track, "P_NAME", NULL);
    //         // TrackFX_GetFXName(media_track, fxNumber, fxName, sizeof(fxName));
    //         // TrackFX_GetParamName(media_track, fxNumber, paramNumber, paramName, size(paramName));
    //         // // paramValue = TrackFX_GetParamNormalized(media_track, fxNumber, paramNumber);
    //         // paramNormalizedValue = TrackFX_GetParamNormalized(media_track, fxNumber, paramNumber);
    //     }
    //     else
    //     {
    //         trackName = "No Track";
    //     }
    // }

    // track->SetDisplayLine(1, ALIGN_LEFT, trackName);
    // // track->SetDisplayLine(1, ALIGN_LEFT, fxName, INVERT);
    // // track->SetDisplayLine(1, ALIGN_LEFT, paramName);
    // // track->SetDisplayLine(1, ALIGN_LEFT, paramValue);
    // // track->SetValueBarValue((int)(paramNormalizedValue * 127.0));
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

void CSurf_LastTouchedFXManager::HandleFaderMove(int index, int msb, int lsb)
{
    (void)index;
    (void)msb;
    (void)lsb;
};
