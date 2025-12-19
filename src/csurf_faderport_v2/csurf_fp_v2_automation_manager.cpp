#ifndef CSURF_AUTOMATION_MANAGER_C_
#define CSURF_AUTOMATION_MANAGER_C_

#include "../controls/csurf_color_button.hpp"
#include "csurf_fp_v2_navigator.hpp"
#include "../shared/csurf_reasonus_settings.hpp"

class CSurf_FP_V2_AutomationManager
{
protected:
    CSurf_ColorButton *readButton;
    CSurf_ColorButton *writeButton;
    CSurf_ColorButton *touchButton;

    CSurf_Context *context;
    CSurf_FP_V2_Navigator *navigator;
    midi_Output *m_midiout;

    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_V2);

    int channelAutomationMode;

    bool isAutomationMode(int automationMode, bool not_selected)
    {
        if (not_selected)
        {
            return false;
        }

        return channelAutomationMode == automationMode;
    }

    void SetButtonValue(bool not_selected)
    {
        if (context->GetShiftLeft())
        {
            touchButton->SetValue(isAutomationMode(AUTOMATION_LATCH, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            writeButton->SetValue(isAutomationMode(AUTOMATION_TRIM, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            readButton->SetValue(isAutomationMode(AUTOMATION_PREVIEW, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            return;
        }

        touchButton->SetValue(isAutomationMode(AUTOMATION_TOUCH, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
        writeButton->SetValue(isAutomationMode(AUTOMATION_WRITE, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
        readButton->SetValue(isAutomationMode(AUTOMATION_READ, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
    }

    void SetButtonColors()
    {
        if (context->GetShiftLeft())
        {
            touchButton->SetColor(ButtonColorPurple);
            writeButton->SetColor(ButtonColorWhite);
            readButton->SetColor(ButtonColorBlue);
            return;
        }

        touchButton->SetColor(ButtonColorYellow);
        writeButton->SetColor(ButtonColorRed);
        readButton->SetColor(ButtonColorGreen);
    }

public:
    CSurf_FP_V2_AutomationManager(
        CSurf_Context *context,
        CSurf_FP_V2_Navigator *navigator,
        midi_Output *m_midiout) : context(context), navigator(navigator), m_midiout(m_midiout)
    {
        readButton = new CSurf_ColorButton(ButtonColorGreen, BTN_READ, BTN_VALUE_OFF, m_midiout);
        writeButton = new CSurf_ColorButton(ButtonColorRed, BTN_WRITE, BTN_VALUE_OFF, m_midiout);
        touchButton = new CSurf_ColorButton(ButtonColorYellow, BTN_TOUCH, BTN_VALUE_OFF, m_midiout);
    };
    ~CSurf_FP_V2_AutomationManager() {};

    void Update()
    {
        MediaTrack *media_track = navigator->GetControllerTrack();
        channelAutomationMode = ::GetTrackAutomationMode(media_track);

        SetButtonColors();
        SetButtonValue(media_track == nullptr);
    };

    void HandleTouchButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = navigator->GetControllerTrack();

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

        MediaTrack *media_track = navigator->GetControllerTrack();

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

        MediaTrack *media_track = navigator->GetControllerTrack();

        if (context->GetShiftLeft())
        {
            if (::GetTrackAutomationMode(media_track) == AUTOMATION_PREVIEW && settings->GetLatchPreviewActionEnabled())
            {
                ::Main_OnCommandEx(settings->GetLatchPreviewActionCode(), 0, 0);
            }
            else
            {
                SetTrackAutomationMode(media_track, AUTOMATION_PREVIEW);
            }
            return;
        }
        SetTrackAutomationMode(media_track, AUTOMATION_READ);
    };
};

#endif