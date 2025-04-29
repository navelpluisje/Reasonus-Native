#ifndef CSURF_AUTOMATION_MANAGER_C_
#define CSURF_AUTOMATION_MANAGER_C_

#include "../controls/csurf_color_button.hpp"
#include "../shared/csurf_context.cpp"
#include "../shared/csurf_utils.hpp"

class CSurf_FP_V2_AutomationManager
{
protected:
    CSurf_ColorButton *readButton;
    CSurf_ColorButton *writeButton;
    CSurf_ColorButton *touchButton;

    CSurf_Context *context;
    midi_Output *m_midiout;

    int channelAutomationMode;
    int globalAutomationMode;

    void SetButtonValue()
    {
        if (context->GetShiftLeft())
        {
            touchButton->SetValue(globalAutomationMode == AUTOMATION_LATCH ? BTN_VALUE_ON : BTN_VALUE_OFF);
            writeButton->SetValue(globalAutomationMode == AUTOMATION_TRIM ? BTN_VALUE_ON : BTN_VALUE_OFF);
            readButton->SetValue(globalAutomationMode == AUTOMATION_PREVIEW ? BTN_VALUE_ON : BTN_VALUE_OFF);
            return;
        }

        touchButton->SetValue(channelAutomationMode == AUTOMATION_TOUCH ? BTN_VALUE_ON : BTN_VALUE_OFF);
        writeButton->SetValue(channelAutomationMode == AUTOMATION_WRITE ? BTN_VALUE_ON : BTN_VALUE_OFF);
        readButton->SetValue(channelAutomationMode == AUTOMATION_READ ? BTN_VALUE_ON : BTN_VALUE_OFF);
    }

    void SetButtonColors()
    {
        if (context->GetShiftLeft())
        {
            touchButton->SetColor(ButtonColorGreen);
            writeButton->SetColor(ButtonColorYellow);
            readButton->SetColor(ButtonColorYellow);
            return;
        }

        touchButton->SetColor(ButtonColorYellow);
        writeButton->SetColor(ButtonColorRed);
        readButton->SetColor(ButtonColorGreen);
    }

public:
    CSurf_FP_V2_AutomationManager(
        CSurf_Context *context,
        midi_Output *m_midiout) : context(context), m_midiout(m_midiout)
    {
        touchButton = new CSurf_ColorButton(ButtonColorYellow, BTN_TOUCH, BTN_VALUE_OFF, m_midiout);
        writeButton = new CSurf_ColorButton(ButtonColorRed, BTN_WRITE, BTN_VALUE_OFF, m_midiout);
        readButton = new CSurf_ColorButton(ButtonColorGreen, BTN_READ, BTN_VALUE_OFF, m_midiout);
    };
    ~CSurf_FP_V2_AutomationManager() {};

    void Update()
    {
        MediaTrack *media_track = GetSelectedTrack(0, 0);
        channelAutomationMode = GetTrackAutomationMode(media_track);

        SetButtonColors();
        SetButtonValue();
    };

    void HandleTouchButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = GetSelectedTrack(0, 0);
        if (context->GetShiftLeft())
        {
            SetTrackAutomationMode(media_track, AUTOMATION_LATCH);
            return;
        }
        SetTrackAutomationMode(media_track, AUTOMATION_TOUCH);
    };

    void HandleWriteButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = GetSelectedTrack(0, 0);
        if (context->GetShiftLeft())
        {
            SetTrackAutomationMode(media_track, AUTOMATION_TRIM);
            return;
        }
        SetTrackAutomationMode(media_track, AUTOMATION_WRITE);
    };

    void HandleReadButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = GetSelectedTrack(0, 0);
        if (context->GetShiftLeft())
        {
            SetTrackAutomationMode(media_track, AUTOMATION_PREVIEW);
            return;
        }
        SetTrackAutomationMode(media_track, AUTOMATION_READ);
    };
};

#endif