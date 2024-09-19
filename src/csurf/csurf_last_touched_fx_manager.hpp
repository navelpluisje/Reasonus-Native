#ifndef CSURF_LAST_TOUCHED_FX_MANAGER_H_
#define CSURF_LAST_TOUCHED_FX_MANAGER_H_

#include "csurf_navigator.hpp"
#include "csurf_channel_manager_resources.hpp"
#include "controls/csurf_button.hpp"
#include "csurf_track.hpp"
#include <vector>

class CSurf_LastTouchedFXManager
{
protected:
    CSurf_Track *track;
    CSurf_Context *context;
    midi_Output *m_midiout;

    ButtonColor colorActive;
    ButtonColor colorDim;

public:
    CSurf_LastTouchedFXManager(CSurf_Track *track, CSurf_Context *context, midi_Output *m_midiout);
    ~CSurf_LastTouchedFXManager() {};

    void UpdateTrack();

    void HandleSelectClick(int index);

    void HandleMuteClick(int index);

    void HandleSoloClick(int index);

    void HandleFaderTouch();

    void HandleFaderMove(int index, int msb, int lsb);
};

#endif