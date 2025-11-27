#include "csurf_transport_manager.hpp"
#include "../controls/csurf_button.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_context.cpp"

void CSurf_TransportManager::SetButtonValues(bool force)
{
    playButton->SetValue(ButtonBlinkOnOff(isPaused, isPlaying), force);
    stopButton->SetValue(isStopped ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
    recordButton->SetValue(isRecording ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
    repeatButton->SetValue(isRepeat ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
    rewindButton->SetValue(ButtonBlinkOnOff(isRewinding && isFastFwdRwd, isRewinding), force);
    forwardButton->SetValue(ButtonBlinkOnOff(isForwarding && isFastFwdRwd, isForwarding), force);
};

void CSurf_TransportManager::StopRewindOrForward()
{
    isFastFwdRwd = false;
    isForwarding = false;
    isRewinding = false;
}

void CSurf_TransportManager::HandleRewind()
{
    CSurf_OnRew(0);

    if (isFastFwdRwd)
    {
        CSurf_OnRew(0);
        CSurf_OnRew(0);
        CSurf_OnRew(0);
    }
}

void CSurf_TransportManager::HandleForward()
{
    CSurf_OnFwd(0);

    if (isFastFwdRwd)
    {
        CSurf_OnFwd(0);
        CSurf_OnFwd(0);
        CSurf_OnFwd(0);
    }
}

void CSurf_TransportManager::SetPause()
{
    if (isPlaying)
    {
        CSurf_OnPlay();
    }
}

void CSurf_TransportManager::SetRewindingState()
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

void CSurf_TransportManager::SetForwardingState()
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

CSurf_TransportManager::CSurf_TransportManager(CSurf_Context *context, midi_Output *m_midiout) : context(context)
{
    playButton = new CSurf_Button(BTN_PLAY, BTN_VALUE_OFF, m_midiout);
    stopButton = new CSurf_Button(BTN_STOP, BTN_VALUE_OFF, m_midiout);
    recordButton = new CSurf_Button(BTN_RECORD, BTN_VALUE_OFF, m_midiout);
    repeatButton = new CSurf_Button(BTN_LOOP, BTN_VALUE_OFF, m_midiout);
    rewindButton = new CSurf_Button(BTN_REWIND, BTN_VALUE_OFF, m_midiout);
    forwardButton = new CSurf_Button(BTN_FORWARD, BTN_VALUE_OFF, m_midiout);
    Update();
};

void CSurf_TransportManager::Update()
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

    SetButtonValues(false);
}

void CSurf_TransportManager::Refresh(bool force)
{
    SetButtonValues(force);
}

void CSurf_TransportManager::HandlePlayButton(int value)
{
    if (value == 0)
    {
        return;
    }

    StopRewindOrForward();
    CSurf_OnPlay();
};

void CSurf_TransportManager::HandleStopButton(int value)
{
    if (value == 0)
    {
        return;
    }

    StopRewindOrForward();
    CSurf_OnStop();
};

void CSurf_TransportManager::HandleRepeatButton(int value)
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

void CSurf_TransportManager::HandleRecordButton(int value)
{
    if (value == 0)
    {
        return;
    }

    CSurf_OnRecord();
};

void CSurf_TransportManager::HandleRewindButton(int value)
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

void CSurf_TransportManager::HandleForwardButton(int value)
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

void CSurf_TransportManager::HandleFootSwitchClick(int value)
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
