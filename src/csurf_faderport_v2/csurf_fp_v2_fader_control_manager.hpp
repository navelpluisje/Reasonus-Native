#ifndef CSURF_FP_V2_FADER_CONTROL_MANAGER_H_
#define CSURF_FP_V2_FADER_CONTROL_MANAGER_H_

#include "csurf_fp_v2_navigator.hpp"
#include "../shared/csurf_context_resources.hpp"

class CSurf_FP_V2_FaderControlManager {
    FaderModeV2 prevChannelMode = TrackFader;
    midi_Output *m_midiout;
    CSurf_FP_V2_Navigator *navigator;
    CSurf_Context *context;

    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_V2);

    void SetFaderMode(ChannelMode channelMode);

    void SetTracks();

public:
    CSurf_FP_V2_FaderControlManager(
        CSurf_Context *context,
        CSurf_FP_V2_Navigator *navigator,
        midi_Output *m_midiout);

    ~CSurf_FP_V2_FaderControlManager();

    void Refresh(bool force = false);

    void ChangeFaderMode(FaderModeV2 faderMode);

    // ADD ALL THE CHANNELMANAGERS METHODS HERE TO PROXY THEM
    void UpdateTracks(bool force_update = false) const;

    void HandleFaderTouch(int index, int value = 0) const;

    void HandleFaderMove(int index, int msb, int lsb) const;
};

#endif
