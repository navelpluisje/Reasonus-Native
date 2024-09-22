#ifndef CSURF_MIX_MANAGER_C_
#define CSURF_MIX_MANAGER_C_

#include "controls/csurf_color_button.hpp"
#include "csurf_navigator.hpp"
#include "csurf_context.cpp"

class CSurf_MixManager
{
protected:
    CSurf_Context *context;
    CSurf_Navigator *trackNavigator;
    midi_Output *m_midiout;

    NavigatorFilter trackFilter = TrackAllFilter;

    CSurf_ColorButton *mixAudioButton;
    CSurf_ColorButton *mixViButton;
    CSurf_ColorButton *mixBusButton;
    CSurf_ColorButton *mixVcaButton;
    CSurf_ColorButton *mixAllButton;
    CSurf_Button *shiftRightButton;

    ShiftState shiftState;

    void SetButtonValue()
    {
        mixAudioButton->SetValue(BTN_VALUE_OFF);
        mixViButton->SetValue(trackFilter == TrackInstrumentsFilter ? BTN_VALUE_ON : BTN_VALUE_OFF);
        mixBusButton->SetValue((trackFilter == TrackAllFoldersFilter || trackFilter == TrackTopFoldersFilter) ? BTN_VALUE_ON : BTN_VALUE_OFF);
        mixVcaButton->SetValue((trackFilter == TrackSendsFilter || trackFilter == TrackReceivesFilter) ? BTN_VALUE_ON : BTN_VALUE_OFF);
        mixAllButton->SetValue(trackFilter == TrackAllFilter ? BTN_VALUE_ON : BTN_VALUE_OFF);
        shiftRightButton->SetValue(context->GetShiftRight() ? BTN_VALUE_ON : BTN_VALUE_OFF);
    }

public:
    CSurf_MixManager(
        CSurf_Context *context,
        CSurf_Navigator *trackNavigator,
        midi_Output *m_midiout) : context(context), trackNavigator(trackNavigator), m_midiout(m_midiout)
    {
        mixAudioButton = new CSurf_ColorButton(ButtonColorWhite, ButtonColorWhiteDim, BTN_AUDIO, BTN_VALUE_OFF, m_midiout);
        mixViButton = new CSurf_ColorButton(ButtonColorWhite, ButtonColorWhiteDim, BTN_VI, BTN_VALUE_OFF, m_midiout);
        mixBusButton = new CSurf_ColorButton(ButtonColorWhite, ButtonColorWhiteDim, BTN_BUS, BTN_VALUE_OFF, m_midiout);
        mixVcaButton = new CSurf_ColorButton(ButtonColorWhite, ButtonColorWhiteDim, BTN_VCA, BTN_VALUE_OFF, m_midiout);
        mixAllButton = new CSurf_ColorButton(ButtonColorWhite, ButtonColorWhiteDim, BTN_ALL, BTN_VALUE_ON, m_midiout);
        shiftRightButton = new CSurf_Button(BTN_SHIFT_RIGHT, BTN_VALUE_OFF, m_midiout);

        HandleMixAllButton();
    }
    ~CSurf_MixManager() {};

    void HandleMixAudioButton()
    {
        SetButtonValue();
    }

    void HandleMixViButton()
    {
        trackFilter = TrackInstrumentsFilter;
        trackNavigator->handleFilter(TrackInstrumentsFilter);
        SetButtonValue();
    }

    void HandleMixBusButton()
    {
        if (context->GetShiftLeft())
        {
            mixBusButton->SetColor(ButtonColorYellow, ButtonColorRed);
            trackFilter = TrackAllFoldersFilter;
            trackNavigator->handleFilter(TrackAllFoldersFilter);
        }
        else
        {
            mixBusButton->SetColor(ButtonColorWhite, ButtonColorWhiteDim);
            trackFilter = TrackTopFoldersFilter;
            trackNavigator->handleFilter(TrackTopFoldersFilter);
        }
        SetButtonValue();
    }

    void HandleMixVcaButton()
    {
        if (context->GetShiftLeft())
        {
            mixVcaButton->SetColor(ButtonColorYellow, ButtonColorRed);
            trackFilter = TrackReceivesFilter;
            trackNavigator->handleFilter(TrackReceivesFilter);
        }
        else
        {
            mixVcaButton->SetColor(ButtonColorWhite, ButtonColorWhiteDim);
            trackFilter = TrackSendsFilter;
            trackNavigator->handleFilter(TrackSendsFilter);
        }
        SetButtonValue();
    }

    void HandleMixAllButton()
    {
        trackFilter = TrackAllFilter;
        trackNavigator->handleFilter(TrackAllFilter);
        SetButtonValue();
    }

    void HandleShiftButton(int val)
    {
        int time = timeGetTime();
        shiftState.active = val > 0;

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
        context->SetShiftRight(shiftState.invert ? !shiftState.active : shiftState.active);

        SetButtonValue();
    }
};

#endif