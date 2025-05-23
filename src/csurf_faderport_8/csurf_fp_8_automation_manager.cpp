#ifndef CSURF_FP_8_AUTOMATION_MANAGER_C_
#define CSURF_FP_8_AUTOMATION_MANAGER_C_

#include "../controls/csurf_color_button.hpp"
#include "../shared/csurf_context.cpp"
#include "../shared/csurf_utils.hpp"

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
    midi_Output *m_midiout;

    int channelAutomationMode;
    int globalAutomationMode;
    bool canSafe = false;
    bool canUndo = false;
    bool canRedo = false;

    void SetButtonValue()
    {
        if (context->GetShiftRight())
        {
            latchButton->SetValue(globalAutomationMode == AUTOMATION_LATCH ? BTN_VALUE_ON : BTN_VALUE_OFF);
            trimButton->SetValue(globalAutomationMode == AUTOMATION_TRIM ? BTN_VALUE_ON : BTN_VALUE_OFF);
            offButton->SetValue(globalAutomationMode == AUTOMATION_PREVIEW ? BTN_VALUE_ON : BTN_VALUE_OFF);
            touchButton->SetValue(globalAutomationMode == AUTOMATION_TOUCH ? BTN_VALUE_ON : BTN_VALUE_OFF);
            writeButton->SetValue(globalAutomationMode == AUTOMATION_WRITE ? BTN_VALUE_ON : BTN_VALUE_OFF);
            readButton->SetValue(globalAutomationMode == AUTOMATION_READ ? BTN_VALUE_ON : BTN_VALUE_OFF);
            return;
        }

        if (context->GetShiftLeft())
        {
            latchButton->SetValue(canSafe ? BTN_VALUE_ON : BTN_VALUE_OFF);
            trimButton->SetValue(canRedo ? BTN_VALUE_ON : BTN_VALUE_OFF);
            offButton->SetValue(canUndo ? BTN_VALUE_ON : BTN_VALUE_OFF);
            touchButton->SetValue(BTN_VALUE_OFF);
            writeButton->SetValue(BTN_VALUE_OFF);
            readButton->SetValue(BTN_VALUE_OFF);
            return;
        }

        latchButton->SetValue(channelAutomationMode == AUTOMATION_LATCH ? BTN_VALUE_ON : BTN_VALUE_OFF);
        trimButton->SetValue(channelAutomationMode == AUTOMATION_TRIM ? BTN_VALUE_ON : BTN_VALUE_OFF);
        offButton->SetValue(channelAutomationMode == AUTOMATION_PREVIEW ? BTN_VALUE_ON : BTN_VALUE_OFF);
        touchButton->SetValue(channelAutomationMode == AUTOMATION_TOUCH ? BTN_VALUE_ON : BTN_VALUE_OFF);
        writeButton->SetValue(channelAutomationMode == AUTOMATION_WRITE ? BTN_VALUE_ON : BTN_VALUE_OFF);
        readButton->SetValue(channelAutomationMode == AUTOMATION_READ ? BTN_VALUE_ON : BTN_VALUE_OFF);
    }

    void SetButtonColors()
    {
        if (context->GetShiftLeft())
        {
            latchButton->SetColor(ButtonColorGreen);
            trimButton->SetColor(ButtonColorYellow);
            offButton->SetColor(ButtonColorYellow);
            touchButton->SetColor(ButtonColorBlack);
            writeButton->SetColor(ButtonColorBlack);
            readButton->SetColor(ButtonColorBlack);
            return;
        }

        latchButton->SetColor(ButtonColorPurple);
        trimButton->SetColor(ButtonColorWhite);
        offButton->SetColor(ButtonColorBlue);
        touchButton->SetColor(ButtonColorYellow);
        writeButton->SetColor(ButtonColorRed);
        readButton->SetColor(ButtonColorGreen);
    }

public:
    CSurf_FP_8_AutomationManager(
        CSurf_Context *context,
        midi_Output *m_midiout) : context(context), m_midiout(m_midiout)
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
            MediaTrack *media_track = GetSelectedTrack(0, 0);
            channelAutomationMode = GetTrackAutomationMode(media_track);
        }

        SetButtonColors();
        SetButtonValue();
    };

    void HandleLatchButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight())
        {
            SetGlobalAutomationOverride(AUTOMATION_LATCH);
            return;
        }
        if (context->GetShiftLeft())
        {
            Main_SaveProject(0, false);
            return;
        }
        MediaTrack *media_track = GetSelectedTrack(0, 0);
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
            SetGlobalAutomationOverride(AUTOMATION_TRIM);
            return;
        }
        if (context->GetShiftLeft())
        {
            Undo_DoRedo2(0);
            return;
        }
        MediaTrack *media_track = GetSelectedTrack(0, 0);
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
            SetGlobalAutomationOverride(AUTOMATION_PREVIEW);
            return;
        }
        if (context->GetShiftLeft())
        {
            Undo_DoUndo2(0);
            return;
        }
        MediaTrack *media_track = GetSelectedTrack(0, 0);
        SetTrackAutomationMode(media_track, AUTOMATION_PREVIEW);
    };

    void HandleTouchButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight())
        {
            SetGlobalAutomationOverride(AUTOMATION_TOUCH);
            return;
        }
        if (context->GetShiftLeft())
        {
            return;
        }
        MediaTrack *media_track = GetSelectedTrack(0, 0);
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
            SetGlobalAutomationOverride(AUTOMATION_WRITE);
            return;
        }
        if (context->GetShiftLeft())
        {
            return;
        }
        MediaTrack *media_track = GetSelectedTrack(0, 0);
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
            SetGlobalAutomationOverride(AUTOMATION_READ);
            return;
        }
        if (context->GetShiftLeft())
        {
            return;
        }
        MediaTrack *media_track = GetSelectedTrack(0, 0);
        SetTrackAutomationMode(media_track, AUTOMATION_READ);
    };
};

#endif