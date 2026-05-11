#ifndef CSURF_SHIFT_MANAGER_C_
#define CSURF_SHIFT_MANAGER_C_

#include "csurf_fp_8_track.hpp"

CSurf_FP_8_Track::CSurf_FP_8_Track(
    const int index,
    CSurf_Context *_context,
    midi_Output *m_midiout
) : context(_context) {
    const int device_id = _context->GetNbChannels() == 8 ? SYSEX_DEVICE_FP8 : SYSEX_DEVICE_FP16;

    selectButton = new CSurf_ColorButton(ButtonColorWhite, SelectButtons[index], BTN_VALUE_OFF, m_midiout);
    soloButton = new CSurf_Button(SoloButtons[index], BTN_VALUE_OFF, m_midiout);
    muteButton = new CSurf_Button(MuteButtons[index], BTN_VALUE_OFF, m_midiout);
    fader = new CSurf_Fader(index, 0, m_midiout);
    valueBar = new CSurf_Valuebar(index, 0, m_midiout);
    display = new CSurf_Display(index, m_midiout, device_id);
    vuMeter = new CSurf_VuMeter(index, m_midiout);
}

CSurf_FP_8_Track::~CSurf_FP_8_Track() {
    delete fader;
    delete valueBar;
    delete display;
    delete vuMeter;
}

void CSurf_FP_8_Track::ClearTrack(const bool display, const bool forceUpdate) const {
    this->SetTrackColor(ButtonColorWhite);

    if (display) {
        this->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT, forceUpdate);
        this->SetDisplayLine(1, ALIGN_LEFT, "", NON_INVERT, forceUpdate);
        this->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
        this->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
    }

    this->SetMuteButtonValue(BTN_VALUE_OFF);
    this->SetSoloButtonValue(BTN_VALUE_OFF);
    this->SetSelectButtonValue(BTN_VALUE_OFF);
    this->SetValueBarValue(0);
    this->SetFaderValue(0, true);
    this->SetVuMeterValue(0);
    this->SetValueBarMode(VALUEBAR_MODE_OFF);
}

void CSurf_FP_8_Track::SetTrackColor(const ButtonColor color, const bool force) const {
    selectButton->SetColor(color, force);
}

void CSurf_FP_8_Track::SetSelectButtonValue(const Btn_Value value, const bool force) const {
    selectButton->SetValue(value, force);
}

void CSurf_FP_8_Track::SetSoloButtonValue(const Btn_Value value, const bool force) const {
    soloButton->SetValue(value, force);
}

void CSurf_FP_8_Track::SetMuteButtonValue(const Btn_Value value, const bool force) const {
    muteButton->SetValue(value, force);
}

void CSurf_FP_8_Track::SetFaderValue(const int value, const bool force) const {
    fader->SetValue(value, force);
}

void CSurf_FP_8_Track::SetValueBarMode(const ValuebarMode mode) const {
    valueBar->SetMode(mode);
}

void CSurf_FP_8_Track::SetValueBarValue(const int value) const {
    valueBar->SetValue(value);
}

void CSurf_FP_8_Track::SetDisplayMode(const DisplayMode mode, const bool force) const {
    display->SetMode(mode, force);
}

void CSurf_FP_8_Track::SetDisplayLine(
    const int line,
    const Alignment alignment,
    const char *value,
    const Inverted invert,
    const bool force
) const {
    try {
        display->SetValue(line, alignment, value, invert, force);
    } catch (...) {
        ShowConsoleMsg("Huh?");
    }
}

void CSurf_FP_8_Track::SetVuMeterValue(const int value, const bool force) const {
    vuMeter->SetValue(value, force);
}

#endif
