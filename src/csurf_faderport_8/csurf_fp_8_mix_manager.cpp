#ifndef CSURF_FP_8_MIX_MANAGER_C_
#define CSURF_FP_8_MIX_MANAGER_C_

#include "../controls/csurf_color_button.hpp"
#include "csurf_fp_8_navigator.hpp"
#include "../shared/csurf_context.cpp"
#include "csurf_fp_8_fader_manager.hpp"

class CSurf_FP_8_MixManager
{
protected:
    CSurf_Context *context;
    CSurf_FP_8_Navigator *trackNavigator;
    CSurf_FP_8_FaderManager *faderManager;
    midi_Output *m_midiout;

    NavigatorFilter trackFilter = TrackAllFilter;

    CSurf_ColorButton *mixAudioButton;
    CSurf_ColorButton *mixViButton;
    CSurf_ColorButton *mixBusButton;
    CSurf_ColorButton *mixVcaButton;
    CSurf_ColorButton *mixAllButton;
    CSurf_Button *shiftRightButton;

    ShiftState shiftState;

    void SetButtonValue(bool force = false)
    {
        mixAudioButton->SetValue(trackFilter == TrackWithAudioFilter ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        mixViButton->SetValue((trackFilter == TrackInstrumentsFilter || trackFilter == TrackWithMidiFilter) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        mixBusButton->SetValue((trackFilter == TrackTopFoldersFilter || trackFilter == TrackAllFoldersFilter || trackFilter == TrackHarwareOutputFilter) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        mixVcaButton->SetValue((trackFilter == TrackIsVcaFilter || trackFilter == TrackReceivesFilter || trackFilter == TrackEffectsFilter) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        mixAllButton->SetValue((trackFilter == TrackAllFilter || trackFilter == TrackCustomFilter || trackFilter == TrackSendsFilter) ? BTN_VALUE_ON : BTN_VALUE_OFF, force);
        shiftRightButton->SetValue((context->GetShiftRight() || (context->GetShiftLeft() && context->GetSwapShiftButtons()))
                                       ? BTN_VALUE_ON
                                       : BTN_VALUE_OFF,
                                   force);
    }

    void SetButtonColor(bool force = false)
    {
        mixAudioButton->SetColor(ButtonColorWhite, force);
        mixViButton->SetColor(ButtonColorWhite, force);
        mixBusButton->SetColor(ButtonColorWhite, force);
        mixVcaButton->SetColor(ButtonColorWhite, force);
        mixAllButton->SetColor(ButtonColorWhite, force);
    }

public:
    CSurf_FP_8_MixManager(
        CSurf_Context *context,
        CSurf_FP_8_Navigator *trackNavigator,
        CSurf_FP_8_FaderManager *faderManager,
        midi_Output *m_midiout) : context(context), trackNavigator(trackNavigator), faderManager(faderManager), m_midiout(m_midiout)
    {
        mixAudioButton = new CSurf_ColorButton(ButtonColorWhite, BTN_AUDIO, BTN_VALUE_OFF, m_midiout);
        mixViButton = new CSurf_ColorButton(ButtonColorWhite, BTN_VI, BTN_VALUE_OFF, m_midiout);
        mixBusButton = new CSurf_ColorButton(ButtonColorWhite, BTN_BUS, BTN_VALUE_OFF, m_midiout);
        mixVcaButton = new CSurf_ColorButton(ButtonColorWhite, BTN_VCA, BTN_VALUE_OFF, m_midiout);
        mixAllButton = new CSurf_ColorButton(ButtonColorWhite, BTN_ALL, BTN_VALUE_ON, m_midiout);
        shiftRightButton = new CSurf_Button(BTN_SHIFT_RIGHT, BTN_VALUE_OFF, m_midiout);

        HandleMixAllButton(BTN_VALUE_ON);
    }
    ~CSurf_FP_8_MixManager() {};

    void Refresh(bool force = false)
    {
        SetButtonValue(force);
        SetButtonColor(force);
    }

    void HandleMixAudioButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        mixAudioButton->SetColor(ButtonColorWhite);
        trackFilter = TrackWithAudioFilter;
        trackNavigator->HandleFilter(TrackWithAudioFilter);
        faderManager->ResetMixButtonClick();
        SetButtonValue();
    }

    void HandleMixViButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            mixViButton->SetColor(ButtonColorYellow);
            trackFilter = TrackWithMidiFilter;
            trackNavigator->HandleFilter(TrackWithMidiFilter);
        }
        else
        {
            mixViButton->SetColor(ButtonColorWhite);
            trackFilter = TrackInstrumentsFilter;
            trackNavigator->HandleFilter(TrackInstrumentsFilter);
        }
        faderManager->ResetMixButtonClick();
        SetButtonValue();
    }

    void HandleMixBusButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight())
        {
            mixBusButton->SetColor(ButtonColorGreen);
            trackFilter = TrackAllFoldersFilter;
            trackNavigator->HandleFilter(TrackAllFoldersFilter);
        }
        else if (context->GetShiftLeft())
        {
            mixBusButton->SetColor(ButtonColorYellow);
            trackFilter = TrackHarwareOutputFilter;
            trackNavigator->HandleFilter(TrackHarwareOutputFilter);
        }
        else
        {
            mixBusButton->SetColor(ButtonColorWhite);
            trackFilter = TrackTopFoldersFilter;
            trackNavigator->HandleFilter(TrackTopFoldersFilter);
        }
        faderManager->ResetMixButtonClick();
        SetButtonValue();
    }

    void HandleMixVcaButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight())
        {
            mixVcaButton->SetColor(ButtonColorGreen);
            trackFilter = TrackReceivesFilter;
            trackNavigator->HandleFilter(TrackReceivesFilter);
        }
        else if (context->GetShiftLeft())
        {
            mixVcaButton->SetColor(ButtonColorYellow);
            trackFilter = TrackEffectsFilter;
            trackNavigator->HandleFilter(TrackEffectsFilter);
        }
        else
        {
            mixVcaButton->SetColor(ButtonColorWhite);
            trackFilter = TrackIsVcaFilter;
            trackNavigator->HandleFilter(TrackIsVcaFilter);
        }
        faderManager->ResetMixButtonClick();
        SetButtonValue();
    }

    void HandleMixAllButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftRight())
        {
            mixAllButton->SetColor(ButtonColorGreen);
            trackFilter = TrackSendsFilter;
            trackNavigator->HandleFilter(TrackSendsFilter);
            faderManager->ResetMixButtonClick();
        }
        else if (context->GetShiftLeft())
        {
            mixAllButton->SetColor(ButtonColorYellow);
            trackFilter = TrackCustomFilter;
            faderManager->HandleMixAllButtonClick();
        }
        else
        {
            mixAllButton->SetColor(ButtonColorWhite);
            trackFilter = TrackAllFilter;
            trackNavigator->HandleFilter(TrackAllFilter);
            faderManager->ResetMixButtonClick();
        }
        SetButtonValue();
    }

    void HandleShiftButton(int value)
    {
        int time = GetTickCount();
        shiftState.active = value > 0;

        if (shiftState.start == 0)
        {
            shiftState.start = time;
        }
        else
        {
            if (time - shiftState.start < TOGGLE_SPEED)
            {
                shiftState.ToggleInvert();
            }
            shiftState.start = 0;
        }
        context->SetShiftRight(shiftState.IsActive());

        SetButtonValue();
    }
};

#endif