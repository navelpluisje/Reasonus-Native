#ifndef CSURF_TRANSPORT_MANAGER_C_
#define CSURF_TRANSPORT_MANAGER_C_

#include "../controls/csurf_button.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_context.cpp"

class CSurf_TransportManager
{
    bool isPlaying = false;
    bool isPaused = false;
    bool isRecording = false;
    bool isStopped = false;
    bool isRepeat = false;
    bool isForwarding = false;
    bool isRewinding = false;
    bool isFastFwdRwd = false;

    CSurf_Button *playButton;
    CSurf_Button *stopButton;
    CSurf_Button *recordButton;
    CSurf_Button *repeatButton;
    CSurf_Button *rewindButton;
    CSurf_Button *forwardButton;

    CSurf_Context *context;

    void SetButtonValues(bool force);

    void StopRewindOrForward();

    void HandleRewind();

    void HandleForward();

    void SetPause();

    void SetRewindingState();

    void SetForwardingState();

    void handleFootSwitchKey(std::string key);

public:
    CSurf_TransportManager(CSurf_Context *context, midi_Output *m_midiout);

    void Update();

    void Refresh(bool force);

    void HandlePlayButton(int value);

    void HandleStopButton(int value);

    void HandleRepeatButton(int value);

    void HandleRecordButton(int value);

    void HandleRewindButton(int value);

    void HandleForwardButton(int value);

    void HandleFootSwitchClick(int value);
};

#endif
