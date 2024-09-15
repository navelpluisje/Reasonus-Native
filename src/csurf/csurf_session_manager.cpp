#ifndef CSURF_SESSION_MANAGER_C_
#define CSURF_SESSION_MANAGER_C_

#include "csurf_button.hpp"
#include "csurf_context.cpp"
#include "csurf_navigator.cpp"

enum SessionTypes
{
    Channel,
    Zoom,
    Scroll,
    Bank,
    Master,
    Click,
    Section,
    Marker,
};

const int TO_FUNCTION = 8;

class CSurf_SessionManager
{
protected:
    CSurf_Context *context;
    midi_Output *m_midiout;
    CSurf_Navigator *trackNavigator;
    SessionTypes session_type = Channel;
    bool handleFunctionKeys = false;

    CSurf_Button *channelButton;
    CSurf_Button *zoomButton;
    CSurf_Button *scrollButton;
    CSurf_Button *bankButton;
    CSurf_Button *masterButton;
    CSurf_Button *clickButton;
    CSurf_Button *sectionButton;
    CSurf_Button *markerButton;
    CSurf_Button *prevButton;
    CSurf_Button *nextButton;

    void SetButtonValues()
    {
        // With shift engaged, blink the selected button
        Btn_Value valueOn = context->GetShiftLeft() ? BTN_VALUE_BLINK : BTN_VALUE_ON;
        // ShowConsoleMsg("SetButtonValues\n");

        channelButton->SetValue(session_type == Channel ? valueOn : BTN_VALUE_OFF);
        zoomButton->SetValue(session_type == Zoom ? valueOn : BTN_VALUE_OFF);
        scrollButton->SetValue(session_type == Scroll ? valueOn : BTN_VALUE_OFF);
        bankButton->SetValue(session_type == Bank ? valueOn : BTN_VALUE_OFF);
        masterButton->SetValue(session_type == Master ? valueOn : BTN_VALUE_OFF);
        clickButton->SetValue(session_type == Click ? valueOn : BTN_VALUE_OFF);
        sectionButton->SetValue(session_type == Section ? valueOn : BTN_VALUE_OFF);
        markerButton->SetValue(session_type == Marker ? valueOn : BTN_VALUE_OFF);
    }

    void handleFunction1() {}
    void handleFunction2() {}
    void handleFunction3() {}
    void handleFunction4() {}
    void handleFunction5() {}
    void handleFunction6() {}
    void handleFunction7() {}
    void handleFunction8() {}

public:
    CSurf_SessionManager(CSurf_Context *context, CSurf_Navigator *trackNavigator, midi_Output *m_midiout) : context(context), m_midiout(m_midiout), trackNavigator(trackNavigator)
    {
        channelButton = new CSurf_Button(BTN_CHANNEL, BTN_VALUE_ON, m_midiout);
        zoomButton = new CSurf_Button(BTN_ZOOM, BTN_VALUE_OFF, m_midiout);
        scrollButton = new CSurf_Button(BTN_SCROLL, BTN_VALUE_OFF, m_midiout);
        bankButton = new CSurf_Button(BTN_BANK, BTN_VALUE_OFF, m_midiout);
        masterButton = new CSurf_Button(BTN_MASTER, BTN_VALUE_OFF, m_midiout);
        clickButton = new CSurf_Button(BTN_CLICK, BTN_VALUE_OFF, m_midiout);
        sectionButton = new CSurf_Button(BTN_SECTION, BTN_VALUE_OFF, m_midiout);
        markerButton = new CSurf_Button(BTN_MARKER, BTN_VALUE_OFF, m_midiout);
        prevButton = new CSurf_Button(BTN_PREV, BTN_VALUE_OFF, m_midiout);
        nextButton = new CSurf_Button(BTN_NEXT, BTN_VALUE_OFF, m_midiout);
    }
    ~CSurf_SessionManager();

    void Refresh() { SetButtonValues(); }

    void SelectSession(SessionTypes _session_type)
    {
        session_type = _session_type;
        SetButtonValues();
    }

    void HandleChannelButton()
    {
        // ShowConsoleMsg("Channel");
        if (context->GetShiftLeft())
        {
            // ShowConsoleMsg("ChannelShift");
            handleFunction1();
        }
        else
        {
            // ShowConsoleMsg("Channel No Shift");
            session_type = Channel;
            SetButtonValues();
        }
    }

    void HandleZoomButton()
    {
        if (context->GetShiftLeft())
        {
            handleFunction2();
        }
        else
        {
            session_type = Zoom;
            SetButtonValues();
        }
    }

    void HandleScrollButton()
    {
        if (context->GetShiftLeft())
        {
            handleFunction3();
        }
        else
        {
            session_type = Scroll;
            SetButtonValues();
        }
    }

    void HandleBankButton()
    {
        if (context->GetShiftLeft())
        {
            handleFunction4();
        }
        else
        {
            session_type = Bank;
            SetButtonValues();
        }
    }

    void HandleMasterButton()
    {
        if (context->GetShiftLeft())
        {
            handleFunction5();
        }
        else
        {
            session_type = Master;
            SetButtonValues();
        }
    }

    void HandleClickButton()
    {
        if (context->GetShiftLeft())
        {
            handleFunction6();
        }
        else
        {
            session_type = Click;
            SetButtonValues();
        }
    }

    void HandleSectionButton()
    {
        if (context->GetShiftLeft())
        {
            handleFunction7();
        }
        else
        {
            session_type = Section;
            SetButtonValues();
        }
    }

    void HandleMarkerButton()
    {
        if (context->GetShiftLeft())
        {
            handleFunction8();
        }
        else
        {
            session_type = Marker;
            SetButtonValues();
        }
    }

    void HandlePrevButton(int val)
    {
        if (!val)
        {
            prevButton->SetValue(BTN_VALUE_OFF);
            return;
        }

        prevButton->SetValue(BTN_VALUE_ON);

        switch (session_type)
        {
        case Channel:
            trackNavigator->DecrementOffset(context->GetShiftLeft() ? 8 : 1);
            TrackList_UpdateAllExternalSurfaces();
            break;
        case Zoom:
            break;
        case Scroll:
            break;
        case Bank:
            break;
        case Master:
            break;
        case Click:
            break;
        case Section:
            break;
        case Marker:
            break;
        }
    }

    void HandleNextButton(int val)
    {
        if (!val)
        {
            nextButton->SetValue(BTN_VALUE_OFF);
            return;
        }

        nextButton->SetValue(BTN_VALUE_ON);

        switch (session_type)
        {
        case Channel:
            trackNavigator->IncrementOffset(context->GetShiftLeft() ? 8 : 1);
            TrackList_UpdateAllExternalSurfaces();
            break;
        case Zoom:
            break;
        case Scroll:
            break;
        case Bank:
            break;
        case Master:
            break;
        case Click:
            break;
        case Section:
            break;
        case Marker:
            break;
        }
    }

    void HandleEncoderIncrement()
    {
        switch (session_type)
        {
        case Channel:
            break;
        case Zoom:
            break;
        case Scroll:
            break;
        case Bank:
            break;
        case Master:
            break;
        case Click:
            break;
        case Section:
            break;
        case Marker:
            break;
        }
    }

    void HandleEncoderDecrement()
    {
        switch (session_type)
        {
        case Channel:
            break;
        case Zoom:
            break;
        case Scroll:
            break;
        case Bank:
            break;
        case Master:
            break;
        case Click:
            break;
        case Section:
            break;
        case Marker:
            break;
        }
    }

    void HandleEncoderClick()
    {
        switch (session_type)
        {
        case Channel:
            break;
        case Zoom:
            break;
        case Scroll:
            break;
        case Bank:
            break;
        case Master:
            break;
        case Click:
            break;
        case Section:
            break;
        case Marker:
            break;
        }
    }
};

#endif