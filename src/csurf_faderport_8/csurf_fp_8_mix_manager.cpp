#ifndef CSURF_FP_8_MIX_MANAGER_C_
#define CSURF_FP_8_MIX_MANAGER_C_

#include "csurf_fp_8_fader_manager.hpp"

class CSurf_FP_8_MixManager {
    CSurf_Context *context;
    CSurf_FP_8_Navigator *trackNavigator;
    CSurf_FP_8_FaderManager *faderManager;
    midi_Output *m_midiout;

    NavigatorFilter trackFilter = TrackAllFilter;
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_8);

    CSurf_ColorButton *mixAudioButton;
    CSurf_ColorButton *mixViButton;
    CSurf_ColorButton *mixBusButton;
    CSurf_ColorButton *mixVcaButton;
    CSurf_ColorButton *mixAllButton;
    CSurf_Button *shiftRightButton;

    ShiftState shiftState;

protected:
    void SetButtonValues(const bool force = false) const {
        const NavigatorFilter track_filter = trackNavigator->GetTrackFilter();

        mixAudioButton->SetValue(
            track_filter == TrackWithAudioFilter
            || track_filter == TrackSendsFilter
            || track_filter == TrackReceivesFilter
                ? BTN_VALUE_ON
                : BTN_VALUE_OFF, force
        );

        mixViButton->SetValue(
            track_filter == TrackInstrumentsFilter
            || track_filter == TrackWithMidiFilter
                ? BTN_VALUE_ON
                : BTN_VALUE_OFF,
            force
        );

        mixBusButton->SetValue(
            track_filter == TrackTopFoldersFilter
            || track_filter == TrackAllFoldersFilter
            || track_filter == TrackHarwareOutputFilter
                ? BTN_VALUE_ON
                : BTN_VALUE_OFF,
            force
        );

        mixVcaButton->SetValue(
            track_filter == TrackIsVcaFilter
            || track_filter == TrackEffectsFilter
                ? BTN_VALUE_ON
                : BTN_VALUE_OFF,
            force
        );

        mixAllButton->SetValue(
            track_filter == TrackAllFilter
            || track_filter == TrackCustomFilter
            || track_filter == TrackCustomMultiSelectFilter
                ? BTN_VALUE_ON
                : BTN_VALUE_OFF,
            force
        );

        shiftRightButton->SetValue(
            (!settings->GetSwapShiftButtons() && context->GetShiftRight())
            || (context->GetShiftLeft() && settings->GetSwapShiftButtons())
                ? BTN_VALUE_ON
                : BTN_VALUE_OFF,
            force
        );
    }

    void SetButtonColors(const bool force = false) const {
        const NavigatorFilter track_filter = trackNavigator->GetTrackFilter();

        mixAudioButton->SetColor(
            track_filter == TrackReceivesFilter
                ? ButtonColorGreen
                : track_filter == TrackSendsFilter
                      ? ButtonColorYellow
                      : ButtonColorWhite,
            force
        );
        mixViButton->SetColor(
            track_filter == TrackWithMidiFilter
                ? ButtonColorYellow
                : ButtonColorWhite,
            force
        );
        mixBusButton->SetColor(
            track_filter == TrackAllFoldersFilter
                ? ButtonColorGreen
                : track_filter == TrackHarwareOutputFilter
                      ? ButtonColorYellow
                      : ButtonColorWhite,
            force
        );
        mixVcaButton->SetColor(
            track_filter == TrackEffectsFilter
                ? ButtonColorYellow
                : ButtonColorWhite,
            force
        );
        mixAllButton->SetColor(
            track_filter == TrackCustomMultiSelectFilter
                ? ButtonColorGreen
                : track_filter == TrackCustomFilter
                      ? ButtonColorYellow
                      : ButtonColorWhite,
            force
        );
    }

    void SetTrackFilter(const NavigatorFilter filter) {
        trackFilter = filter;
        trackNavigator->SetMultiSelectFilter(filter == TrackCustomMultiSelectFilter);
    }

public:
    CSurf_FP_8_MixManager(
        CSurf_Context *context,
        CSurf_FP_8_Navigator *trackNavigator,
        CSurf_FP_8_FaderManager *faderManager,
        midi_Output *m_midiout
    ) : context(context), trackNavigator(trackNavigator), faderManager(faderManager), m_midiout(m_midiout) {
        mixAudioButton = new CSurf_ColorButton(ButtonColorWhite, BTN_AUDIO, BTN_VALUE_OFF, this->m_midiout);
        mixViButton = new CSurf_ColorButton(ButtonColorWhite, BTN_VI, BTN_VALUE_OFF, this->m_midiout);
        mixBusButton = new CSurf_ColorButton(ButtonColorWhite, BTN_BUS, BTN_VALUE_OFF, this->m_midiout);
        mixVcaButton = new CSurf_ColorButton(ButtonColorWhite, BTN_VCA, BTN_VALUE_OFF, this->m_midiout);
        mixAllButton = new CSurf_ColorButton(ButtonColorWhite, BTN_ALL, BTN_VALUE_ON, this->m_midiout);
        shiftRightButton = new CSurf_Button(BTN_SHIFT_RIGHT, BTN_VALUE_OFF, this->m_midiout);

        HandleMixAllButton(BTN_VALUE_ON);
    }

    ~CSurf_FP_8_MixManager() = default;

    void Refresh(const bool force = false) const {
        SetButtonValues(force);
        SetButtonColors(force);
    }

    void Update(const bool force = false) const {
        SetButtonValues(force);
        SetButtonColors(force);
    }

    void HandleMixAudioButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftRight()) {
            trackNavigator->HandleFilter(TrackReceivesFilter);
        } else if (context->GetShiftLeft()) {
            trackNavigator->HandleFilter(TrackSendsFilter);
            faderManager->ResetMixButtonClick();
        } else {
            trackNavigator->HandleFilter(TrackWithAudioFilter);
        }

        faderManager->ResetMixButtonClick();
    }

    void HandleMixViButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            trackNavigator->HandleFilter(TrackWithMidiFilter);
        } else {
            trackNavigator->HandleFilter(TrackInstrumentsFilter);
        }

        faderManager->ResetMixButtonClick();
    }

    void HandleMixBusButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftRight()) {
            trackNavigator->HandleFilter(TrackAllFoldersFilter);
        } else if (context->GetShiftLeft()) {
            trackNavigator->HandleFilter(TrackHarwareOutputFilter);
        } else {
            trackNavigator->HandleFilter(TrackTopFoldersFilter);
        }

        faderManager->ResetMixButtonClick();
    }

    void HandleMixVcaButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            trackNavigator->HandleFilter(TrackEffectsFilter);
        } else {
            trackNavigator->HandleFilter(TrackIsVcaFilter);
        }
        faderManager->ResetMixButtonClick();
    }

    void HandleMixAllButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftRight()) {
            // We do not have multi select enabled yet, so lets go
            if (trackFilter != TrackCustomMultiSelectFilter || context->GetChannelMode() != MixMode) {
                mixAllButton->SetColor(ButtonColorGreen);
                faderManager->HandleMixAllButtonClick();
            } else {
                trackNavigator->HandleFilter(TrackCustomMultiSelectFilter);
                faderManager->ResetMixButtonClick();
            }
            trackNavigator->SetMultiSelectFilter(true);
            trackNavigator->SetTrackFilter(TrackCustomMultiSelectFilter);
        } else if (context->GetShiftLeft()) {
            faderManager->HandleMixAllButtonClick();
            trackNavigator->SetTrackFilter(TrackCustomFilter);
        } else {
            trackNavigator->HandleFilter(TrackAllFilter);
            faderManager->ResetMixButtonClick();
        }
    }

    void HandleShiftButton(const int value) {
        const int time = GetTickCount();
        shiftState.active = value > 0;

        if (shiftState.start == 0) {
            shiftState.start = time;
        } else {
            if (time - shiftState.start < TOGGLE_SPEED) {
                shiftState.ToggleInvert();
            }
            shiftState.start = 0;
        }
        context->SetShiftRight(shiftState.IsActive());
        context->SetShiftRightLocked(shiftState.IsLocked());
    }
};

#endif
