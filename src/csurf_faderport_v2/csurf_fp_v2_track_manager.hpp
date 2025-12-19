#ifndef CSURF_CHANNEL_MANAGER_V2_H_
#define CSURF_CHANNEL_MANAGER_V2_H_

#include "csurf_fp_v2_navigator.hpp"
#include "csurf_fp_v2_track.hpp"

class CSurf_FP_V2_TrackManager
{
protected:
    CSurf_FP_V2_Navigator *navigator;
    CSurf_Context *context;
    midi_Output *m_midiout;

    CSurf_FP_V2_Track *track;
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_V2);

    int mute_start = 0;
    int solo_start = 0;
    bool hasLastTouchedFxEnabled = false;

    ButtonColor color;

    bool forceUpdate = false;

    void GetFaderValue(MediaTrack *media_track, int *faderValue);

public:
    CSurf_FP_V2_TrackManager(
        CSurf_Context *context,
        CSurf_FP_V2_Navigator *navigator,
        midi_Output *m_midiout);

    ~CSurf_FP_V2_TrackManager()
    {
        delete navigator;
        delete context;
        delete m_midiout;
    };

    void UpdateTrack();

    void ClearTrack();

    void HandleMuteClick(int index, int value);

    void HandleSoloClick(int index, int value);

    void HandleArmClick(int index, int value);

    void HandleBypassClick(int index, int value);

    void HandleFaderTouch(int value)
    {
        (void)value;
    }

    void HandleFaderMove(int msb, int lsb);
};

#endif