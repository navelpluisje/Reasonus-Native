#ifndef CSURF_SHIFT_MANAGER_C_
#define CSURF_SHIFT_MANAGER_C_

#include "csurf_track.hpp"

CSurf_Track::CSurf_Track(ButtonColor colorActive, ButtonColor colorDim, int index, CSurf_Context *context, midi_Output *m_midiout) : context(context)
{
    selectButton = new CSurf_ColorButton(colorActive, colorDim, SelectButtons[index], BTN_VALUE_OFF, m_midiout);
    soloButton = new CSurf_Button(SoloButtons[index], BTN_VALUE_OFF, m_midiout);
    muteButton = new CSurf_Button(MuteButtons[index], BTN_VALUE_OFF, m_midiout);
    fader = new CSurf_Fader(index, 0, m_midiout);
}

void CSurf_Track::SetTrackColor(ButtonColor colorActive, ButtonColor colorDim)
{

    selectButton->SetColor(colorActive, colorDim);
}

void CSurf_Track::SetSelectButtonValue(Btn_Value value)
{
    selectButton->SetValue(value);
}

void CSurf_Track::SetSoloButtonValue(Btn_Value value)
{
    soloButton->SetValue(value);
}

void CSurf_Track::SetMuteButtonValue(Btn_Value value)
{
    muteButton->SetValue(value);
}

void CSurf_Track::SetFaderValue(double value)
{
    fader->SetValue(value);
}

#endif