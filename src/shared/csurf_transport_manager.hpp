#ifndef CSURF_TRANSPORT_MANAGER_C_
#define CSURF_TRANSPORT_MANAGER_C_

#include "csurf_daw.hpp"
#include "../controls/csurf_button.hpp"
#include "../shared/csurf_context.hpp"
#include "../shared/csurf_reasonus_settings.hpp"

class CSurf_TransportManager {
    bool is_playing = false;
    bool is_paused = false;
    bool is_recording = false;
    bool is_stopped = false;
    bool is_repeat = false;
    bool is_forwarding = false;
    bool is_rewinding = false;
    bool is_fast_fwdRwd = false;

    int rwd_started = 0;
    int fwd_started = 0;
    PLAY_STATES previous_play_state = PLAY_STATE_STOPPED;

    CSurf_Button *play_button;
    CSurf_Button *stop_button;
    CSurf_Button *record_button;
    CSurf_Button *repeat_button;
    CSurf_Button *rewind_button;
    CSurf_Button *forward_button;

    CSurf_Context *context;

    ReaSonusSettings *settings;

    void SetButtonValues(bool force) const;

    void StopRewindOrForward();

    void HandleRewind() const;

    void HandleForward() const;

    void SetPause() const;

    void SetRewindingState();

    void SetForwardingState();

    void handleFunctionKey(const std::string &key, FunctionTypes type) const;

    void SetPreviousPlayState();

    void ResetPlayState() const;

public:
    CSurf_TransportManager(CSurf_Context *context, midi_Output *m_midiout);

    void Update(bool force_update = false);

    void Refresh(bool force) const;

    void HandlePlayButton(int value);

    void HandleStopButton(int value);

    void HandleRepeatButton(int value) const;

    void HandleRecordButton(int value);

    void HandleRewindButton(int value);

    void HandleForwardButton(int value);

    void HandleFootSwitchClick(int value) const;

    void HandlePlayStateChange();
};

#endif
