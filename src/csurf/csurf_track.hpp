#ifndef CSURF_TRACK_H_
#define CSURF_TRACK_H_

#include "controls/csurf_button.hpp"
#include "controls/csurf_color_button.hpp"
#include "controls/csurf_display.hpp"
#include "controls/csurf_fader.hpp"
#include "controls/csurf_valuebar.hpp"
#include "csurf_context.cpp"
#include "controls/csurf_vu_meter.hpp"

inline Btn_Types SelectButtons[8] = {
    BTN_SELECT_1,
    BTN_SELECT_2,
    BTN_SELECT_3,
    BTN_SELECT_4,
    BTN_SELECT_5,
    BTN_SELECT_6,
    BTN_SELECT_7,
    BTN_SELECT_8,
};

inline Btn_Types SoloButtons[8] = {
    BTN_SOLO_1,
    BTN_SOLO_2,
    BTN_SOLO_3,
    BTN_SOLO_4,
    BTN_SOLO_5,
    BTN_SOLO_6,
    BTN_SOLO_7,
    BTN_SOLO_8,
};

inline Btn_Types MuteButtons[8] = {
    BTN_MUTE_1,
    BTN_MUTE_2,
    BTN_MUTE_3,
    BTN_MUTE_4,
    BTN_MUTE_5,
    BTN_MUTE_6,
    BTN_MUTE_7,
    BTN_MUTE_8,
};

class CSurf_Track
{
protected:
    CSurf_Context *context;

    CSurf_ColorButton *selectButton;
    CSurf_Button *soloButton;
    CSurf_Button *muteButton;
    CSurf_Fader *fader;
    CSurf_Valuebar *valueBar;
    CSurf_Display *display;
    CSurf_VuMeter *vuMeter;

public:
    CSurf_Track(int index, CSurf_Context *_context, midi_Output *m_midiout);
    ~CSurf_Track() {};

    void SetTrackColor(ButtonColor colorActive, ButtonColor colorDim);

    void SetSelectButtonValue(Btn_Value value, bool force = false);

    void SetSoloButtonValue(Btn_Value value, bool force = false);

    void SetMuteButtonValue(Btn_Value value, bool force = false);

    void SetFaderValue(int value, bool force = false);

    void SetValueBarMode(ValuebarMode mode);
    void SetValueBarValue(int value);

    void SetDisplayMode(DisplayMode mode, bool force = false);
    void SetDisplayLine(int line, Alignment alignment, const char *value, Inverted invert = NON_INVERT, bool force = false);

    void SetVuMeterValue(int value, bool force = false);
};

#endif