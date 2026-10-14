#ifndef CSURF_TRACK_V2_H_
#define CSURF_TRACK_V2_H_

#include "../controls/csurf_button.hpp"
#include "../controls/csurf_color_button.hpp"
#include "../controls/csurf_display.hpp"
#include "../controls/csurf_fader.hpp"
#include "../shared/csurf_context.cpp"

class CSurf_FP_V2_Track {
protected:
    CSurf_Context *context;

    CSurf_Button *armButton;
    CSurf_Button *soloButton;
    CSurf_Button *muteButton;
    CSurf_Button *bypassButton;
    CSurf_Fader *fader;

public:
    CSurf_FP_V2_Track(CSurf_Context *context, midi_Output *m_midiout);

    ~CSurf_FP_V2_Track();

    void ClearTrack() const;

    void SetTrackColor(ButtonColor color);

    void SetSoloButtonValue(Btn_Value value, bool force = false) const;

    void SetMuteButtonValue(Btn_Value value, bool force = false) const;

    void SetArmButtonValue(Btn_Value value, bool force = false) const;

    void SetBypassButtonValue(Btn_Value value, bool force = false) const;

    void SetFaderValue(int value, bool force = false) const;
};

#endif
