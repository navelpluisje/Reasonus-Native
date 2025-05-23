#include "csurf_fp_v2_track.hpp"

CSurf_FP_V2_Track::CSurf_FP_V2_Track(CSurf_Context *context, midi_Output *m_midiout) : context(context)
{
    soloButton = new CSurf_Button(BTN_SOLO_1, BTN_VALUE_OFF, m_midiout);
    muteButton = new CSurf_Button(BTN_MUTE_1, BTN_VALUE_OFF, m_midiout);
    armButton = new CSurf_Button(BTN_ARM, BTN_VALUE_OFF, m_midiout);
    bypassButton = new CSurf_Button(BTN_BYPASS, BTN_VALUE_OFF, m_midiout);
    fader = new CSurf_Fader(0, 0, m_midiout);
}

CSurf_FP_V2_Track::~CSurf_FP_V2_Track()
{
    delete fader;
};

void CSurf_FP_V2_Track::ClearTrack()
{
    this->SetMuteButtonValue(BTN_VALUE_OFF);
    this->SetSoloButtonValue(BTN_VALUE_OFF);
    this->SetArmButtonValue(BTN_VALUE_OFF);
    this->SetBypassButtonValue(BTN_VALUE_OFF);
    this->SetFaderValue(0);
}

void CSurf_FP_V2_Track::SetSoloButtonValue(Btn_Value value, bool force)
{
    soloButton->SetValue(value, force);
}

void CSurf_FP_V2_Track::SetMuteButtonValue(Btn_Value value, bool force)
{
    muteButton->SetValue(value, force);
}

void CSurf_FP_V2_Track::SetArmButtonValue(Btn_Value value, bool force)
{
    armButton->SetValue(value, force);
}

void CSurf_FP_V2_Track::SetBypassButtonValue(Btn_Value value, bool force)
{
    bypassButton->SetValue(value, force);
}

void CSurf_FP_V2_Track::SetFaderValue(int value, bool force)
{
    fader->SetValue(value, force);
}
