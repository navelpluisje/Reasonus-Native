#ifndef CSURF_SHIFT_MANAGER_C_
#define CSURF_SHIFT_MANAGER_C_

#include "csurf_fp_8_track.hpp"
#include "../shared/csurf_utils.hpp"

CSurf_FP_8_Track::CSurf_FP_8_Track(int index, CSurf_Context *context, midi_Output *m_midiout) : context(context)
{
    selectButton = new CSurf_ColorButton(ButtonColorWhite, SelectButtons[index], BTN_VALUE_OFF, m_midiout);
    soloButton = new CSurf_Button(SoloButtons[index], BTN_VALUE_OFF, m_midiout);
    muteButton = new CSurf_Button(MuteButtons[index], BTN_VALUE_OFF, m_midiout);
    fader = new CSurf_Fader(index, 0, m_midiout);
    valueBar = new CSurf_Valuebar(index, 0, m_midiout);
    display = new CSurf_Display(index, m_midiout);
    vuMeter = new CSurf_VuMeter(index, m_midiout);
}
CSurf_FP_8_Track::~CSurf_FP_8_Track()
{
    delete fader;
    delete valueBar;
    delete display;
    delete vuMeter;
};

void CSurf_FP_8_Track::ClearTrack()
{
    this->SetTrackColor(ButtonColorWhite);
    this->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT, true);
    this->SetDisplayLine(1, ALIGN_LEFT, "", NON_INVERT, true);
    this->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, true);
    this->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, true);
    this->SetMuteButtonValue(BTN_VALUE_OFF);
    this->SetSoloButtonValue(BTN_VALUE_OFF);
    this->SetSelectButtonValue(BTN_VALUE_OFF);
    this->SetValueBarValue(0);
    this->SetValueBarMode(VALUEBAR_MODE_OFF);
    this->SetFaderValue(0);
    this->SetVuMeterValue(0);
}

void CSurf_FP_8_Track::SetTrackColor(ButtonColor color)
{
    selectButton->SetColor(color);
}

void CSurf_FP_8_Track::SetSelectButtonValue(Btn_Value value, bool force)
{
    selectButton->SetValue(value, force);
}

void CSurf_FP_8_Track::SetSoloButtonValue(Btn_Value value, bool force)
{
    soloButton->SetValue(value, force);
}

void CSurf_FP_8_Track::SetMuteButtonValue(Btn_Value value, bool force)
{
    muteButton->SetValue(value, force);
}

void CSurf_FP_8_Track::SetFaderValue(int value, bool force)
{
    fader->SetValue(value, force);
}

void CSurf_FP_8_Track::SetValueBarMode(ValuebarMode mode)
{
    valueBar->SetMode(mode);
}

void CSurf_FP_8_Track::SetValueBarValue(int value)
{
    valueBar->SetValue(value);
}

void CSurf_FP_8_Track::SetDisplayMode(DisplayMode mode, bool force)
{
    display->SetMode(mode, force);
};

void CSurf_FP_8_Track::SetDisplayLine(int line, Alignment alignment, const char *value, Inverted invert, bool force)
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

void CSurf_FP_8_Track::SetVuMeterValue(int value, bool force)
{
    vuMeter->SetValue(value, force);
};

#endif