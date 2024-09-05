#ifndef CSURF_SESSION_MANAGER_C_
#define CSURF_SESSION_MANAGER_C_

#include "csurf_button.hpp"
#include "csurf_context.cpp"

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

void AdjustBankOffset(int amount, bool dosel)
{
    (void)dosel;
    if (!amount)
    {
        return;
    }

    // if (amount < 0)
    // {
    //     if (m_bank_offset > 0)
    //     {
    //         m_bank_offset += amount;
    //         if (m_bank_offset < 0)
    //         {
    //             m_bank_offset = 0;
    //         }

    //         if (dosel)
    //         {
    //             int x;
    //             MediaTrack *t = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);
    //             for (x = 0;; x++)
    //             {
    //                 int f = 0;
    //                 if (!GetTrackInfo(x - 1, &f))
    //                     break;

    //                 MediaTrack *tt = CSurf_TrackFromID(x, false);
    //                 bool sel = tt == t;
    //                 if (tt && !(f & 2) == sel)
    //                 {
    //                     SetTrackSelected(tt, sel);
    //                 }
    //             }
    //         }
    //     }
    // }
    // else
    // {
    //     int msize = CSurf_NumTracks(g_csurf_mcpmode);

    //     if (m_bank_offset < msize)
    //     {
    //         m_bank_offset += amount;
    //         if (m_bank_offset > msize)
    //             m_bank_offset = msize;

    //         if (dosel)
    //         {
    //             int x;
    //             MediaTrack *t = CSurf_TrackFromID(m_bank_offset, g_csurf_mcpmode);
    //             for (x = 0;; x++)
    //             {
    //                 int f = 0;
    //                 if (!GetTrackInfo(x - 1, &f))
    //                     break;

    //                 MediaTrack *tt = CSurf_TrackFromID(x, false);
    //                 bool sel = tt == t;
    //                 if (tt && !(f & 2) == sel)
    //                 {
    //                     SetTrackSelected(tt, sel);
    //                 }
    //             }
    //         }
    //     }
    // }
}

class CSurf_SessionManager
{
protected:
    CSurf_Context *context;
    midi_Output *m_midiout;
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

    void SetButtonValues(midi_Output *m_midiout)
    {
        // With shift engaged, blink the selected button
        Btn_Value valueOn = context->GetSfiftLeft() ? BTN_VALUE_BLINK : BTN_VALUE_ON;
        // ShowConsoleMsg("SetButtonValues\n");

        channelButton->SetValue(session_type == Channel ? valueOn : BTN_VALUE_OFF, m_midiout);
        zoomButton->SetValue(session_type == Zoom ? valueOn : BTN_VALUE_OFF, m_midiout);
        scrollButton->SetValue(session_type == Scroll ? valueOn : BTN_VALUE_OFF, m_midiout);
        bankButton->SetValue(session_type == Bank ? valueOn : BTN_VALUE_OFF, m_midiout);
        masterButton->SetValue(session_type == Master ? valueOn : BTN_VALUE_OFF, m_midiout);
        clickButton->SetValue(session_type == Click ? valueOn : BTN_VALUE_OFF, m_midiout);
        sectionButton->SetValue(session_type == Section ? valueOn : BTN_VALUE_OFF, m_midiout);
        markerButton->SetValue(session_type == Marker ? valueOn : BTN_VALUE_OFF, m_midiout);
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
    CSurf_SessionManager(CSurf_Context *_context, midi_Output *m_midiout)
    {
        context = _context;
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

    void Refresh(midi_Output *m_midiout) { SetButtonValues(m_midiout); }

    void SelectSession(SessionTypes _session_type, midi_Output *m_midiout)
    {
        session_type = _session_type;
        SetButtonValues(m_midiout);
    }

    void HandleChannelButton(midi_Output *m_midiout)
    {
        // ShowConsoleMsg("Channel");
        if (context->GetSfiftLeft())
        {
            // ShowConsoleMsg("ChannelShift");
            handleFunction1();
        }
        else
        {
            // ShowConsoleMsg("Channel No Shift");
            session_type = Channel;
            SetButtonValues(m_midiout);
        }
    }

    void HandleZoomButton(midi_Output *m_midiout)
    {
        if (context->GetSfiftLeft())
        {
            handleFunction2();
        }
        else
        {
            session_type = Zoom;
            SetButtonValues(m_midiout);
        }
    }

    void HandleScrollButton(midi_Output *m_midiout)
    {
        if (context->GetSfiftLeft())
        {
            handleFunction3();
        }
        else
        {
            session_type = Scroll;
            SetButtonValues(m_midiout);
        }
    }

    void HandleBankButton(midi_Output *m_midiout)
    {
        if (context->GetSfiftLeft())
        {
            handleFunction4();
        }
        else
        {
            session_type = Bank;
            SetButtonValues(m_midiout);
        }
    }

    void HandleMasterButton(midi_Output *m_midiout)
    {
        if (context->GetSfiftLeft())
        {
            handleFunction5();
        }
        else
        {
            session_type = Master;
            SetButtonValues(m_midiout);
        }
    }

    void HandleClickButton(midi_Output *m_midiout)
    {
        if (context->GetSfiftLeft())
        {
            handleFunction6();
        }
        else
        {
            session_type = Click;
            SetButtonValues(m_midiout);
        }
    }

    void HandleSectionButton(midi_Output *m_midiout)
    {
        if (context->GetSfiftLeft())
        {
            handleFunction7();
        }
        else
        {
            session_type = Section;
            SetButtonValues(m_midiout);
        }
    }

    void HandleMarkerButton(midi_Output *m_midiout)
    {
        if (context->GetSfiftLeft())
        {
            handleFunction8();
        }
        else
        {
            session_type = Marker;
            SetButtonValues(m_midiout);
        }
    }

    void HandlePrevButton(int val, midi_Output *m_midiout)
    {
        if (!val)
        {
            prevButton->SetValue(BTN_VALUE_OFF, m_midiout);
            return;
        }

        prevButton->SetValue(BTN_VALUE_ON, m_midiout);

        switch (session_type)
        {
        case Channel:
            AdjustBankOffset(context->GetSfiftLeft() ? -8 : -1, true);
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

    void HandleNextButton(int val, midi_Output *m_midiout)
    {
        if (!val)
        {
            nextButton->SetValue(BTN_VALUE_OFF, m_midiout);
            return;
        }

        nextButton->SetValue(BTN_VALUE_ON, m_midiout);

        switch (session_type)
        {
        case Channel:
            AdjustBankOffset(context->GetSfiftLeft() ? -8 : -1, true);
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