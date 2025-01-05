#ifndef CSURF_SHIFT_MANAGER_C_
#define CSURF_SHIFT_MANAGER_C_

#include "csurf_track.hpp"
#include "csurf_utils.hpp"

CSurf_Track::CSurf_Track(int index, CSurf_Context *context, midi_Output *m_midiout) : context(context)
{
    selectButton = new CSurf_ColorButton(ButtonColorWhite, ButtonColorWhiteDim, SelectButtons[index], BTN_VALUE_OFF, m_midiout);
    soloButton = new CSurf_Button(SoloButtons[index], BTN_VALUE_OFF, m_midiout);
    muteButton = new CSurf_Button(MuteButtons[index], BTN_VALUE_OFF, m_midiout);
    fader = new CSurf_Fader(index, 0, m_midiout);
    valueBar = new CSurf_Valuebar(index, 0, m_midiout);
    display = new CSurf_Display(index, m_midiout);
    vuMeter = new CSurf_VuMeter(index, m_midiout);
}
CSurf_Track::~CSurf_Track()
{
    delete fader;
    delete valueBar;
    delete display;
    delete vuMeter;
};

void CSurf_Track::ClearTrack()
{
    this->SetTrackColor(ButtonColorWhite, ButtonColorWhiteDim);
    this->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT, true);
    this->SetDisplayLine(1, ALIGN_LEFT, "", NON_INVERT, true);
    this->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, true);
    this->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, true);
    this->SetMuteButtonValue(BTN_VALUE_OFF);
    this->SetSoloButtonValue(BTN_VALUE_OFF);
    this->SetSelectButtonValue(BTN_VALUE_OFF);
    this->SetValueBarValue(0);
    this->SetFaderValue(0);
    this->SetVuMeterValue(0);
}

void CSurf_Track::SetTrackColor(ButtonColor colorActive, ButtonColor colorDim)
{

    selectButton->SetColor(colorActive, colorDim);
}

void CSurf_Track::SetSelectButtonValue(Btn_Value value, bool force)
{
    selectButton->SetValue(value, force);
}

void CSurf_Track::SetSoloButtonValue(Btn_Value value, bool force)
{
    soloButton->SetValue(value, force);
}

void CSurf_Track::SetMuteButtonValue(Btn_Value value, bool force)
{
    muteButton->SetValue(value, force);
}

void CSurf_Track::SetFaderValue(int value, bool force)
{
    fader->SetValue(value, force);
}

void CSurf_Track::SetValueBarMode(ValuebarMode mode)
{
    valueBar->SetMode(mode);
}

void CSurf_Track::SetValueBarValue(int value)
{
    valueBar->SetValue(value);
}

void CSurf_Track::SetDisplayMode(DisplayMode mode, bool force)
{
    display->SetMode(mode, force);
};

void CSurf_Track::SetDisplayLine(int line, Alignment alignment, const char *value, Inverted invert, bool force)
{
    try
    {
        display->SetValue(line, alignment, value, invert, force);
    }
    catch (...)
    {
        ShowConsoleMsg("Huh?");
    }
};

void CSurf_Track::SetVuMeterValue(int value, bool force)
{
    vuMeter->SetValue(value, force);
};

#endif