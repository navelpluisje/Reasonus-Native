#ifndef CSURF_SHIFT_MANAGER_C_
#define CSURF_SHIFT_MANAGER_C_

#include "csurf_track.hpp"

CSurf_Track::CSurf_Track(int index, CSurf_Context *context, midi_Output *m_midiout) : context(context)
{
    selectButton = new CSurf_ColorButton(ButtonColorWhite, ButtonColorWhiteDim, SelectButtons[index], BTN_VALUE_OFF, m_midiout);
    soloButton = new CSurf_Button(SoloButtons[index], BTN_VALUE_OFF, m_midiout);
    muteButton = new CSurf_Button(MuteButtons[index], BTN_VALUE_OFF, m_midiout);
    fader = new CSurf_Fader(index, 0, m_midiout);
    valueBar = new CSurf_Valuebar(index, 0, m_midiout);
    display = new CSurf_Display(index, m_midiout);
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

void CSurf_Track::SetFaderValue(int value)
{
    fader->SetValue(value);
}

void CSurf_Track::SetValueBarMode(ValuebarMode mode)
{
    valueBar->SetMode(mode);
}

void CSurf_Track::SetValueBarValue(int value)
{
    valueBar->SetValue(value);
}

void CSurf_Track::SetDisplayMode(DisplayMode mode)
{
    display->SetMode(mode);
};

void CSurf_Track::SetDisplayLine(int line, Alignment alignment, const char *value, Inverted invert)
{
    display->SetValue(line, alignment, value, invert);
};

#endif