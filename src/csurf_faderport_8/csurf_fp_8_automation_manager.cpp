#ifndef CSURF_FP_8_AUTOMATION_MANAGER_C_
#define CSURF_FP_8_AUTOMATION_MANAGER_C_

#include "csurf_fp_8_fader_manager.hpp"

class CSurf_FP_8_AutomationManager {
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

    int channelAutomationMode = -1;
    int globalAutomationMode = -1;
    bool canSafe = false;
    bool canUndo = false;
    bool canRedo = false;

    [[nodiscard]] bool IsAutomationSelected(const int automation_mode) const {
        if (globalAutomationMode == automation_mode) {
            return true;
        }
        if (globalAutomationMode == AUTOMATION_OFF && channelAutomationMode == automation_mode) {
            return true;
        }
        return false;
    }

    void SetButtonValue(const bool force = false) const {
        if (context->GetShiftRight()) {
            latchButton->SetValue(globalAutomationMode == AUTOMATION_LATCH ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            trimButton->SetValue(globalAutomationMode == AUTOMATION_TRIM ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            offButton->SetValue(globalAutomationMode == AUTOMATION_PREVIEW ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            touchButton->SetValue(globalAutomationMode == AUTOMATION_TOUCH ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            writeButton->SetValue(globalAutomationMode == AUTOMATION_WRITE ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            readButton->SetValue(globalAutomationMode == AUTOMATION_READ ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        } else if (context->GetShiftLeft()) {
            latchButton->SetValue(canSafe ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            trimButton->SetValue(canRedo ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            offButton->SetValue(canUndo ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            touchButton->SetValue(context->GetChannelMode() == MenuMode ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            writeButton->SetValue(BTN_VALUE_OFF, force);
            readButton->SetValue(BTN_VALUE_OFF, force);
        } else {
            latchButton->SetValue(IsAutomationSelected(AUTOMATION_LATCH) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            trimButton->SetValue(IsAutomationSelected(AUTOMATION_TRIM) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            offButton->SetValue(IsAutomationSelected(AUTOMATION_PREVIEW) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            touchButton->SetValue(ButtonOnBlinkOff(
                                      IsAutomationSelected(AUTOMATION_TOUCH),
                                      context->GetChannelMode() == MenuMode,
                                      settings->GetDistractionFreeMode()
                                  ),
                                  force
            );
            writeButton->SetValue(IsAutomationSelected(AUTOMATION_WRITE) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
            readButton->SetValue(IsAutomationSelected(AUTOMATION_READ) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        }
    }

    void SetButtonColors(const bool force = false) const {
        if (context->GetShiftRight()) {
            latchButton->SetColor(settings->GetAutomationColor(
                                      AUTOMATION_BUTTON_LATCH,
                                      ButtonColorPurple.GetColor()),
                                  force
            );
            trimButton->SetColor(settings->GetAutomationColor(
                                     AUTOMATION_BUTTON_TRIM,
                                     ButtonColorWhite.GetColor()),
                                 force
            );
            offButton->SetColor(settings->GetAutomationColor(
                                    AUTOMATION_BUTTON_OFF,
                                    ButtonColorBlue.GetColor()),
                                force
            );
            touchButton->SetColor(settings->GetAutomationColor(
                                      AUTOMATION_BUTTON_TOUCH,
                                      ButtonColorYellow.GetColor()),
                                  force
            );
            writeButton->SetColor(settings->GetAutomationColor(
                                      AUTOMATION_BUTTON_WRITE,
                                      ButtonColorRed.GetColor()),
                                  force
            );
            readButton->SetColor(settings->GetAutomationColor(
                                     AUTOMATION_BUTTON_READ,
                                     ButtonColorGreen.GetColor()),
                                 force
            );
        } else if (context->GetShiftLeft()) {
            latchButton->SetColor(ButtonColorGreen, force);
            trimButton->SetColor(ButtonColorYellow, force);
            offButton->SetColor(ButtonColorYellow, force);
            touchButton->SetColor(ButtonColorBlack, force);
            writeButton->SetColor(ButtonColorBlack, force);
            readButton->SetColor(ButtonColorBlack, force);
        } else {
            latchButton->SetColor(settings->GetAutomationColor(
                                      AUTOMATION_BUTTON_LATCH,
                                      ButtonColorPurple.GetColor()),
                                  force
            );
            trimButton->SetColor(settings->GetAutomationColor(
                                     context->IsSinglePointAutomationEnabled()
                                         ? AUTOMATION_SINGLE_POINT
                                         : AUTOMATION_BUTTON_TRIM,
                                     ButtonColorWhite.GetColor()),
                                 force
            );
            offButton->SetColor(settings->GetAutomationColor(
                                    AUTOMATION_BUTTON_OFF,
                                    ButtonColorBlue.GetColor()),
                                force
            );
            touchButton->SetColor(settings->GetAutomationColor(
                                      AUTOMATION_BUTTON_TOUCH,
                                      ButtonColorYellow.GetColor()),
                                  force
            );
            writeButton->SetColor(settings->GetAutomationColor(
                                      AUTOMATION_BUTTON_WRITE,
                                      ButtonColorRed.GetColor()),
                                  force
            );
            readButton->SetColor(settings->GetAutomationColor(
                                     AUTOMATION_BUTTON_READ,
                                     ButtonColorGreen.GetColor()),
                                 force
            );
        }
    }

    [[nodiscard]] MediaTrack *GetSelectedAutomationTrack() const {
        if (context->GetMasterFaderMode() && DAW::IsTrackSelected(GetMasterTrack(nullptr))) {
            return GetMasterTrack(nullptr);
        }
        return GetSelectedTrack(nullptr, 0);
    }

    void SetGlobalAutomationMode(const int automation_mode) const {
        if (GetGlobalAutomationOverride() == automation_mode) {
            SetGlobalAutomationOverride(AUTOMATION_OFF);
        } else {
            SetGlobalAutomationOverride(automation_mode);
        }
    }

public:
    CSurf_FP_8_AutomationManager(
        CSurf_Context *context,
        CSurf_FP_8_FaderManager *faderManager,
        midi_Output *m_midiout
    ) : context(context), faderManager(faderManager), m_midiout(m_midiout) {
        latchButton = new CSurf_ColorButton(ButtonColorPurple, BTN_LATCH, BTN_VALUE_OFF, m_midiout);
        trimButton = new CSurf_ColorButton(ButtonColorWhite, BTN_TRIM, BTN_VALUE_OFF, m_midiout);
        offButton = new CSurf_ColorButton(ButtonColorBlue, BTN_OFF, BTN_VALUE_OFF, m_midiout);
        touchButton = new CSurf_ColorButton(ButtonColorYellow, BTN_TOUCH, BTN_VALUE_OFF, m_midiout);
        writeButton = new CSurf_ColorButton(ButtonColorRed, BTN_WRITE, BTN_VALUE_OFF, m_midiout);
        readButton = new CSurf_ColorButton(ButtonColorGreen, BTN_READ, BTN_VALUE_OFF, m_midiout);
    }

    ~CSurf_FP_8_AutomationManager() = default;

    void Update(const bool force_update = false) {
        // Get selected track and get the atomation type
        globalAutomationMode = GetGlobalAutomationOverride();

        if (context->GetShiftLeft()) {
            canRedo = Undo_CanRedo2(nullptr) != nullptr;
            canUndo = Undo_CanUndo2(nullptr) != nullptr;
            canSafe = IsProjectDirty(nullptr) != 0;
        } else {
            MediaTrack *media_track = GetSelectedAutomationTrack();
            channelAutomationMode = GetTrackAutomationMode(media_track);
        }

        SetButtonColors(force_update);
        SetButtonValue(force_update);
    }

    void Refresh(const bool force = false) const {
        SetButtonColors(force);
        SetButtonValue(force);
    }

    void HandleLatchButton(const int value) const {
        if (value == 0) {
            return;
        }
        context->SetSinglePointAutomation(false);

        if (context->GetShiftRight()) {
            SetGlobalAutomationMode(AUTOMATION_LATCH);
            return;
        }

        if (context->GetShiftLeft()) {
            Main_SaveProject(nullptr, false);
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        SetTrackAutomationMode(media_track, AUTOMATION_LATCH);
    }

    void HandleTrimButton(const int value) const {
        if (value == 0) {
            return;
        }

        if (context->GetShiftRight()) {
            context->SetSinglePointAutomation(false);
            SetGlobalAutomationMode(AUTOMATION_TRIM);
            return;
        }

        if (context->GetShiftLeft()) {
            Undo_DoRedo2(nullptr);
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        if (
            GetTrackAutomationMode(media_track) == AUTOMATION_TRIM
            && !context->IsSinglePointAutomationEnabled()
        ) {
            context->SetSinglePointAutomation(true);
        } else {
            context->SetSinglePointAutomation(false);
            SetTrackAutomationMode(media_track, AUTOMATION_TRIM);
        }
    }

    void HandleOffButton(const int value) const {
        if (value == 0) {
            return;
        }
        context->SetSinglePointAutomation(false);

        if (context->GetShiftRight()) {
            SetGlobalAutomationMode(AUTOMATION_PREVIEW);
            return;
        }

        if (context->GetShiftLeft()) {
            Undo_DoUndo2(nullptr);
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        if ((
                GetTrackAutomationMode(media_track) == AUTOMATION_PREVIEW
                || GetGlobalAutomationOverride() == AUTOMATION_PREVIEW
            )
            && settings->GetLatchPreviewActionEnabled()
        ) {
            Main_OnCommandAsyncEx(settings->GetLatchPreviewActionCode(), 0, nullptr);
        } else {
            SetTrackAutomationMode(media_track, AUTOMATION_PREVIEW);
        }
    }

    void HandleTouchButton(const int value) const {
        if (value == 0) {
            return;
        }
        context->SetSinglePointAutomation(false);

        if (context->GetShiftRight() && !context->IsChannelMode(MenuMode)) {
            SetGlobalAutomationMode(AUTOMATION_TOUCH);
            return;
        }

        if (context->GetShiftLeft() || context->IsChannelMode(MenuMode)) {
            faderManager->HandleTouchButtonClick();
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        SetTrackAutomationMode(media_track, AUTOMATION_TOUCH);
    }

    void HandleWriteButton(const int value) const {
        if (value == 0) {
            return;
        }
        context->SetSinglePointAutomation(false);

        if (context->GetShiftRight()) {
            SetGlobalAutomationMode(AUTOMATION_WRITE);
            return;
        }

        if (context->GetShiftLeft()) {
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        SetTrackAutomationMode(media_track, AUTOMATION_WRITE);
    }

    void HandleReadButton(const int value) const {
        if (value == 0) {
            return;
        }
        context->SetSinglePointAutomation(false);

        if (context->GetShiftRight()) {
            SetGlobalAutomationMode(AUTOMATION_READ);
            return;
        }

        if (context->GetShiftLeft()) {
            return;
        }

        MediaTrack *media_track = GetSelectedAutomationTrack();
        SetTrackAutomationMode(media_track, AUTOMATION_READ);
    }
};

#endif
