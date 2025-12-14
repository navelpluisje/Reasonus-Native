#ifndef CSURF_FP_8_LAST_TOUCHED_FX_MANAGER_H_
#define CSURF_FP_8_LAST_TOUCHED_FX_MANAGER_H_

#include "csurf_fp_8_track.hpp"

class CSurf_FP_8_LastTouchedFXManager
{
protected:
    CSurf_FP_8_Track *track;
    CSurf_Context *context;
    midi_Output *m_midiout;

    bool forceUpdate = false;
    bool hasLastTouchedFxEnabled = false;

    ButtonColor colorActive;
    ButtonColor colorDim;

public:
    CSurf_FP_8_LastTouchedFXManager(CSurf_FP_8_Track *track, CSurf_Context *context, midi_Output *m_midiout);
    ~CSurf_FP_8_LastTouchedFXManager() {};

    void UpdateTrack();

    void HandleSelectClick(int index);

    void HandleMuteClick(int index);

    void HandleSoloClick(int index);

    void HandleFaderTouch();

    void HandleFaderMove(int msb, int lsb);

    void resetLastTouchedFxEnabled()
    {
        hasLastTouchedFxEnabled = false;
    };
};

#endif