#ifndef CSURF_SHIFT_MANAGER_C_
#define CSURF_SHIFT_MANAGER_C_

#include "csurf_track.hpp"

void CSurf_Track::SetTrackColor(ButtonColor colorActive, ButtonColor colorDim, midi_Output *m_midiout)
{

    selectButton->SetColor(colorActive, colorDim, m_midiout);
}

CSurf_Track::CSurf_Track(ButtonColor colorActive, ButtonColor colorDim, int index, CSurf_Context *context, midi_Output *m_midiout) : context(context)
{
    selectButton = new CSurf_ColorButton(colorActive, colorDim, SelectButtons[index], BTN_VALUE_OFF, m_midiout);
    soloButton = new CSurf_Button(SoloButtons[index], BTN_VALUE_OFF, m_midiout);
    muteButton = new CSurf_Button(MuteButtons[index], BTN_VALUE_OFF, m_midiout);
}

// void SetFaderValue(Btn_Value value, midi_Output *m_midiout)
// {
//     selectButton->SetValue(value, m_midiout);
// }

void CSurf_Track::SetSelectButtonValue(Btn_Value value, midi_Output *m_midiout)
{
    selectButton->SetValue(value, m_midiout);
}

void CSurf_Track::SetSoloButtonValue(Btn_Value value, midi_Output *m_midiout)
{
    soloButton->SetValue(value, m_midiout);
}

void CSurf_Track::SetMuteButtonValue(Btn_Value value, midi_Output *m_midiout)
{
    muteButton->SetValue(value, m_midiout);
}

#endif