#ifndef CSURF_AUTOMATION_MANAGER_C_
#define CSURF_AUTOMATION_MANAGER_C_

#include "csurf_fp_v2_navigator.hpp"
#include "../controls/csurf_color_button.hpp"
#include "../shared/csurf_reasonus_settings.hpp"

class CSurf_FP_V2_AutomationManager {
protected:
    CSurf_ColorButton *readButton;
    CSurf_ColorButton *writeButton;
    CSurf_ColorButton *touchButton;

    CSurf_Context *context;
    CSurf_FP_V2_Navigator *navigator;
    midi_Output *m_midiout;

    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_V2);

    int channelAutomationMode;

    bool isAutomationMode(const int automationMode, const bool not_selected) const {
        if (not_selected) {
            return false;
        }

        return channelAutomationMode == automationMode;
    }

    void SetButtonValue(const bool not_selected) const {
        if (context->GetShiftLeft()) {
            touchButton->SetValue(isAutomationMode(AUTOMATION_LATCH, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            writeButton->SetValue(isAutomationMode(AUTOMATION_TRIM, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            readButton->SetValue(isAutomationMode(AUTOMATION_PREVIEW, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            return;
        }

        touchButton->SetValue(isAutomationMode(AUTOMATION_TOUCH, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
        writeButton->SetValue(isAutomationMode(AUTOMATION_WRITE, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
        readButton->SetValue(isAutomationMode(AUTOMATION_READ, not_selected) ? BTN_VALUE_ON : BTN_VALUE_OFF);
    }

    void SetButtonColors() const {
        if (context->GetShiftLeft()) {
            touchButton->SetColor(
                settings->GetAutomationColor(
                    AUTOMATION_BUTTON_LATCH,
                    ButtonColorPurple.GetColor()),
                false
            );
            writeButton->SetColor(
                settings->GetAutomationColor(
                    context->IsSinglePointAutomationEnabled()
                        ? AUTOMATION_SINGLE_POINT
                        : AUTOMATION_BUTTON_TRIM,
                    ButtonColorWhite.GetColor()),
                false
            );
            readButton->SetColor(
                settings->GetAutomationColor(
                    AUTOMATION_BUTTON_OFF,
                    ButtonColorBlue.GetColor()),
                false
            );
            return;
        }

        touchButton->SetColor(
            settings->GetAutomationColor(
                AUTOMATION_BUTTON_TOUCH,
                ButtonColorYellow.GetColor()),
            false
        );
        writeButton->SetColor(
            settings->GetAutomationColor(
                AUTOMATION_BUTTON_WRITE,
                ButtonColorRed.GetColor()),
            false
        );
        readButton->SetColor(
            settings->GetAutomationColor(
                AUTOMATION_BUTTON_READ,
                ButtonColorGreen.GetColor()),
            false
        );
    }

public:
    CSurf_FP_V2_AutomationManager(
        CSurf_Context *context,
        CSurf_FP_V2_Navigator *navigator,
        midi_Output *m_midiout
    ) : context(context), navigator(navigator), m_midiout(m_midiout) {
        readButton = new CSurf_ColorButton(ButtonColorGreen, BTN_READ, BTN_VALUE_OFF, m_midiout);
        writeButton = new CSurf_ColorButton(ButtonColorRed, BTN_WRITE, BTN_VALUE_OFF, m_midiout);
        touchButton = new CSurf_ColorButton(ButtonColorYellow, BTN_TOUCH, BTN_VALUE_OFF, m_midiout);
    }

    ~CSurf_FP_V2_AutomationManager() {
    }

    void Update() {
        MediaTrack *media_track = navigator->GetControllerTrack();
        channelAutomationMode = GetTrackAutomationMode(media_track);

        SetButtonColors();
        SetButtonValue(media_track == nullptr);
    }

    void HandleTouchButton(const int value) const {
        if (value == 0) {
            return;
        }

        MediaTrack *media_track = navigator->GetControllerTrack();

        if (context->GetShiftLeft()) {
            SetTrackAutomationMode(media_track, AUTOMATION_LATCH);
            return;
        }
        SetTrackAutomationMode(media_track, AUTOMATION_TOUCH);
    }

    void HandleWriteButton(const int value) const {
        if (value == 0) {
            return;
        }

        MediaTrack *media_track = navigator->GetControllerTrack();

        if (context->GetShiftLeft()) {
            if (
                GetTrackAutomationMode(media_track) == AUTOMATION_TRIM
                && !context->IsSinglePointAutomationEnabled()
            ) {
                context->SetSinglePointAutomation(true);
            } else {
                context->SetSinglePointAutomation(false);
                SetTrackAutomationMode(media_track, AUTOMATION_TRIM);
            }
            return;
        }
        SetTrackAutomationMode(media_track, AUTOMATION_WRITE);
    }

    void HandleReadButton(const int value) const {
        if (value == 0) {
            return;
        }

        MediaTrack *media_track = navigator->GetControllerTrack();

        if (context->GetShiftLeft()) {
            if (GetTrackAutomationMode(media_track) == AUTOMATION_PREVIEW && settings->
                GetLatchPreviewActionEnabled()) {
                Main_OnCommandAsyncEx(settings->GetLatchPreviewActionCode(), 0, nullptr);
            } else {
                SetTrackAutomationMode(media_track, AUTOMATION_PREVIEW);
            }
            return;
        }
        SetTrackAutomationMode(media_track, AUTOMATION_READ);
    }
};

#endif
