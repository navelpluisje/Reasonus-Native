#ifndef CSURF_FP_8_AUTOMATION_MANAGER_C_
#define CSURF_FP_8_AUTOMATION_MANAGER_C_

#include "csurf_fp_8_fader_manager.hpp"

class CSurf_FP_8_AutomationManager
{
protected:
    CSurf_ColorButton *latchButton;
    CSurf_ColorButton *trimButton;
    CSurf_ColorButton *offButton;
    CSurf_ColorButton *touchButton;
    CSurf_ColorButton *writeButton;
    CSurf_ColorButton *readButton;

    CSurf_Context *context;
    CSurf_FP_8_FaderManager *faderManager;
    midi_Output *m_midiout;

    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_8);

    int channelAutomationMode;
    int globalAutomationMode;
    bool canSafe = false;
    bool canUndo = false;
    bool canRedo = false;

    bool IsAutomationSelected(int automation_mode)
    {
        if (globalAutomationMode == automation_mode)
        {
            return true;
        }
        if (globalAutomationMode == AUTOMATION_OFF && channelAutomationMode == automation_mode)
        {
            return true;
        }
        return false;
    }

    void SetButtonValue(bool force = false)
    {
        if (context->GetShiftRight())
        {
            latchButton->SetValue(globalAutomationMode == AUTOMATION_LATCH ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            trimButton->SetValue(globalAutomationMode == AUTOMATION_TRIM ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            offButton->SetValue(globalAutomationMode == AUTOMATION_PREVIEW ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            touchButton->SetValue(globalAutomationMode == AUTOMATION_TOUCH ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            writeButton->SetValue(globalAutomationMode == AUTOMATION_WRITE ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            readButton->SetValue(globalAutomationMode == AUTOMATION_READ ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        }
        else if (context->GetShiftLeft())
        {
            latchButton->SetValue(canSafe ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            trimButton->SetValue(canRedo ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            offButton->SetValue(canUndo ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            touchButton->SetValue(context->GetChannelMode() == MenuMode ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            writeButton->SetValue(BTN_VALUE_OFF, force);
            readButton->SetValue(BTN_VALUE_OFF, force);
        }
        else
        {
            latchButton->SetValue(IsAutomationSelected(AUTOMATION_LATCH) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            trimButton->SetValue(IsAutomationSelected(AUTOMATION_TRIM) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            offButton->SetValue(IsAutomationSelected(AUTOMATION_PREVIEW) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            touchButton->SetValue(context->GetChannelMode() == MenuMode
                                      ? (settings->GetDistractionFreeMode() ? BTN_VALUE_ON : BTN_VALUE_BLINK)
                                  : IsAutomationSelected(AUTOMATION_TOUCH)
                                      ? BTN_VALUE_ON
                                      : BTN_VALUE_OFF,
                                  force);
            writeButton->SetValue(IsAutomationSelected(AUTOMATION_WRITE) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            readButton->SetValue(IsAutomationSelected(AUTOMATION_READ) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        }
    }

    void SetButtonColors(bool force = false)
    {
        if (context->GetShiftRight())
        {
            latchButton->SetColor(ButtonColorPurple, force);
            trimButton->SetColor(ButtonColorWhite, force);
            offButton->SetColor(ButtonColorBlue, force);
            touchButton->SetColor(ButtonColorYellow, force);
            writeButton->SetColor(ButtonColorRed, force);
            readButton->SetColor(ButtonColorGreen, force);
        }
        else if (context->GetShiftLeft())
        {
            latchButton->SetColor(ButtonColorGreen, force);
            trimButton->SetColor(ButtonColorYellow, force);
            offButton->SetColor(ButtonColorYellow, force);
            touchButton->SetColor(ButtonColorBlack, force);
            writeButton->SetColor(ButtonColorBlack, force);
            readButton->SetColor(ButtonColorBlack, force);
        }
        else
        {
            latchButton->SetColor(ButtonColorPurple, force);
            trimButton->SetColor(ButtonColorWhite, force);
            offButton->SetColor(ButtonColorBlue, force);
            touchButton->SetColor(ButtonColorYellow, force);
            writeButton->SetColor(ButtonColorRed, force);
            readButton->SetColor(ButtonColorGreen, force);
        }
    }

    MediaTrack *GetSelectedAutomationTrack()
    {
        if (context->GetMasterFaderMode() && DAW::IsTrackSelected(::GetMasterTrack(0)))
        {
            return GetMasterTrack(0);
        }
        else
        {
            return GetSelectedTrack(0, 0);
        }
    }

    void SetGlobalAutomationMode(int automation_mode)
    {
        if (::GetGlobalAutomationOverride() == automation_mode)
        {
            ::SetGlobalAutomationOverride(AUTOMATION_OFF);
        }
        else
        {
            ::SetGlobalAutomationOverride(automation_mode);
        }
    }

public:
    CSurf_FP_8_AutomationManager(
        CSurf_Context *context,
        CSurf_FP_8_FaderManager *faderManager,
        midi_Output *m_midiout) : context(context), faderManager(faderManager), m_midiout(m_midiout)
    {
        latchButton = new CSurf_ColorButton(ButtonColorPurple, BTN_LATCH, BTN_VALUE_OFF, m_midiout);
        trimButton = new CSurf_ColorButton(ButtonColorWhite, BTN_TRIM, BTN_VALUE_OFF, m_midiout);
        offButton = new CSurf_ColorButton(ButtonColorBlue, BTN_OFF, BTN_VALUE_OFF, m_midiout);
        touchButton = new CSurf_ColorButton(ButtonColorYellow, BTN_TOUCH, BTN_VALUE_OFF, m_midiout);
        writeButton = new CSurf_ColorButton(ButtonColorRed, BTN_WRITE, BTN_VALUE_OFF, m_midiout);
        readButton = new CSurf_ColorButton(ButtonColorGreen, BTN_READ, BTN_VALUE_OFF, m_midiout);
    };
    ~CSurf_FP_8_AutomationManager() {};

    void Update()
    {
        // Get selected track and get the atomation type
        if (context->GetShiftRight())
        {
            globalAutomationMode = GetGlobalAutomationOverride();
        }
        else if (context->GetShiftLeft())
        {
            canRedo = Undo_CanRedo2(0) != NULL;
            canUndo = Undo_CanUndo2(0) != NULL;
            canSafe = IsProjectDirty(0);
        }
        else
        {
            MediaTrack *media_track = GetSelectedAutomationTrack();
            channelAutomationMode = GetTrackAutomationMode(media_track);
        }

        SetButtonColors();
        SetButtonValue();
    };

    void Refresh(bool force = false)
    {
        SetButtonColors(force);
        SetButtonValue(force);
    }

    void HandleLatchButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight())
        {
            SetGlobalAutomationMode(AUTOMATION_LATCH);
            return;
        }

        if (context->GetShiftLeft())
        {
            Main_SaveProject(0, false);
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        SetTrackAutomationMode(media_track, AUTOMATION_LATCH);
    };

    void HandleTrimButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight())
        {
            SetGlobalAutomationMode(AUTOMATION_TRIM);
            return;
        }

        if (context->GetShiftLeft())
        {
            Undo_DoRedo2(0);
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        SetTrackAutomationMode(media_track, AUTOMATION_TRIM);
    };

    void HandleOffButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight())
        {
            SetGlobalAutomationMode(AUTOMATION_PREVIEW);
            return;
        }

        if (context->GetShiftLeft())
        {
            Undo_DoUndo2(0);
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        if (
            (::GetTrackAutomationMode(media_track) == AUTOMATION_PREVIEW || ::GetGlobalAutomationOverride() == AUTOMATION_PREVIEW) && settings->GetLatchPreviewActionEnabled())
        {
            ::Main_OnCommandEx(settings->GetLatchPreviewActionCode(), 0, 0);
        }
        else
        {
            SetTrackAutomationMode(media_track, AUTOMATION_PREVIEW);
        }
    };

    void HandleTouchButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight() && !context->IsChannelMode(MenuMode))
        {
            SetGlobalAutomationMode(AUTOMATION_TOUCH);
            return;
        }

        if (context->GetShiftLeft() || context->IsChannelMode(MenuMode))
        {
            faderManager->HandleTouchButtonClick();
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        SetTrackAutomationMode(media_track, AUTOMATION_TOUCH);
    };

    void HandleWriteButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight())
        {
            SetGlobalAutomationMode(AUTOMATION_WRITE);
            return;
        }

        if (context->GetShiftLeft())
        {
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        SetTrackAutomationMode(media_track, AUTOMATION_WRITE);
    };

    void HandleReadButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight())
        {
            SetGlobalAutomationMode(AUTOMATION_READ);
            return;
        }

        if (context->GetShiftLeft())
        {
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        SetTrackAutomationMode(media_track, AUTOMATION_READ);
    };
};

#endif