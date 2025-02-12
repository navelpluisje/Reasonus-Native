#ifndef CSURF_TRANSPORT_MANAGER_C_
#define CSURF_TRANSPORT_MANAGER_C_

#include "controls/csurf_button.hpp"
#include "csurf_utils.hpp"
#include "csurf_context.cpp"

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

    void SetButtonValues()
    {
        playButton->SetValue(ButtonBlinkOnOff(isPaused, isPlaying));
        stopButton->SetValue(isStopped ? BTN_VALUE_ON : BTN_VALUE_OFF);
        recordButton->SetValue(isRecording ? BTN_VALUE_ON : BTN_VALUE_OFF);
        repeatButton->SetValue(isRepeat ? BTN_VALUE_ON : BTN_VALUE_OFF);
        rewindButton->SetValue(ButtonBlinkOnOff(isRewinding && isFastFwdRwd, isRewinding));
        forwardButton->SetValue(ButtonBlinkOnOff(isForwarding && isFastFwdRwd, isForwarding));
    };

    void StopRewindOrForward()
    {
        isFastFwdRwd = false;
        isForwarding = false;
        isRewinding = false;
    }

    void HandleRewind()
    {
        CSurf_OnRew(0);

        if (isFastFwdRwd)
        {
            CSurf_OnRew(0);
            CSurf_OnRew(0);
            CSurf_OnRew(0);
        }
    }

    void HandleForward()
    {
        CSurf_OnFwd(0);

        if (isFastFwdRwd)
        {
            CSurf_OnFwd(0);
            CSurf_OnFwd(0);
            CSurf_OnFwd(0);
        }
    }

    void SetPause()
    {
        if (isPlaying)
        {
            CSurf_OnPlay();
        }
    }

    void SetRewindingState()
    {
        if (isRewinding)
        {
            isFastFwdRwd = !isFastFwdRwd;
            return;
        }
        isFastFwdRwd = false;
        isForwarding = false;
        isRewinding = true;
    };

    void SetForwardingState()
    {
        if (isForwarding)
        {
            isFastFwdRwd = !isFastFwdRwd;
            return;
        }
        isFastFwdRwd = false;
        isRewinding = false;
        isForwarding = true;
    };

public:
    CSurf_TransportManager(CSurf_Context *context, midi_Output *m_midiout) : context(context)
    {
        playButton = new CSurf_Button(BTN_PLAY, BTN_VALUE_OFF, m_midiout);
        stopButton = new CSurf_Button(BTN_STOP, BTN_VALUE_OFF, m_midiout);
        recordButton = new CSurf_Button(BTN_RECORD, BTN_VALUE_OFF, m_midiout);
        repeatButton = new CSurf_Button(BTN_LOOP, BTN_VALUE_OFF, m_midiout);
        rewindButton = new CSurf_Button(BTN_REWIND, BTN_VALUE_OFF, m_midiout);
        forwardButton = new CSurf_Button(BTN_FORWARD, BTN_VALUE_OFF, m_midiout);
        Update();
    };

    void Update()
    {
        int playState = GetPlayStateEx(0);
        isPlaying = hasBit(playState, 0);
        isPaused = hasBit(playState, 1);
        isRecording = hasBit(playState, 2);
        isStopped = !isPlaying && !isPaused;
        isRepeat = GetSetRepeatEx(0, -1) > 0;

        if (isRewinding)
        {
            HandleRewind();
        }

        if (isForwarding)
        {
            HandleForward();
        }

        SetButtonValues();
    }

    void HandlePlayButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        StopRewindOrForward();
        CSurf_OnPlay();
    };

    void HandleStopButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        StopRewindOrForward();
        CSurf_OnStop();
    };

    void HandleRepeatButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            GetSetRepeatEx(0, 0);
        }
        else
        {
            // SendMessage(g_hwnd, WM_COMMAND, IDC_REPEAT, 0);
            GetSetRepeatEx(0, 2);
        }
    };

    void HandleRecordButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        CSurf_OnRecord();
    };

    void HandleRewindButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            CSurf_GoStart();
            return;
        }
        SetPause();
        SetRewindingState();
    };

    void HandleForwardButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            CSurf_GoEnd();
            return;
        }

        SetPause();
        SetForwardingState();
    };
};

#endif
