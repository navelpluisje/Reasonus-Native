#ifndef CSURF_FP_8_SESSION_MANAGER_C_
#define CSURF_FP_8_SESSION_MANAGER_C_

#include "../controls/csurf_button.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_context.cpp"
#include "csurf_fp_8_navigator.hpp"
#include "../shared/csurf_session_manager_actions.hpp"
#include "csurf_fp_8_ui_control_panel.hpp"
#include "../shared/csurf_faderport_ui_imgui_utils.hpp"
#include <mini/ini.h>

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

class CSurf_FP_8_SessionManager
{
protected:
    CSurf_Context *context;
    midi_Output *m_midiout;
    CSurf_FP_8_Navigator *trackNavigator;
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

    void SetButtonValues(bool force = false)
    {
        // With shift engaged, blink the selected button
        Btn_Value valueOn = context->GetShiftLeft() && !context->GetDistractionFreeMode() ? BTN_VALUE_BLINK : BTN_VALUE_ON;

        channelButton->SetValue(session_type == Channel ? valueOn : BTN_VALUE_OFF, force);
        zoomButton->SetValue(session_type == Zoom ? valueOn : BTN_VALUE_OFF, force);
        scrollButton->SetValue(session_type == Scroll ? valueOn : BTN_VALUE_OFF, force);
        bankButton->SetValue(session_type == Bank ? valueOn : BTN_VALUE_OFF, force);
        masterButton->SetValue((context->GetMasterFaderMode() || session_type == Master) ? valueOn : BTN_VALUE_OFF, force);
        clickButton->SetValue(session_type == Click ? valueOn : BTN_VALUE_OFF, force);
        sectionButton->SetValue(session_type == Section ? valueOn : BTN_VALUE_OFF, force);
        markerButton->SetValue(session_type == Marker ? valueOn : BTN_VALUE_OFF, force);
    }

    void handleFunctionKey(std::string key)
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));
        mINI::INIStructure ini;
        file.read(ini);

        std::string actionId = ini["functions"][key];
        if (actionId == "0")
        {
            int result = MB("There is no action assigned to this function.\nDo you want to assign an action?", "No action assigned", 1);
            if (result == 1)
            {
                if (!ReaSonus8ControlPanel::control_panel_open)
                {
                    ToggleFP8ControlPanel(ReaSonus8ControlPanel::FUNCTIONS_PAGE);
                }
            }
            return;
        }

        if (isInteger(actionId))
        {
            Main_OnCommandEx(stoi(actionId), 0, 0);
        }
        else
        {
            Main_OnCommandStringEx(actionId);
        }
    }

public:
    CSurf_FP_8_SessionManager(
        CSurf_Context *context,
        CSurf_FP_8_Navigator *trackNavigator,
        midi_Output *m_midiout) : context(context), m_midiout(m_midiout), trackNavigator(trackNavigator)
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
    ~CSurf_FP_8_SessionManager();

    void Refresh(bool force = false)
    {
        SetButtonValues(force);
    }

    void SelectSession(SessionTypes _session_type)
    {
        session_type = _session_type;
        SetButtonValues();
    }

    void HandleChannelButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            handleFunctionKey("1");
            return;
        }
        session_type = Channel;
        SetButtonValues();
    }

    void HandleZoomButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            handleFunctionKey("2");
            return;
        }
        session_type = Zoom;
        SetButtonValues();
    }

    void HandleScrollButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            handleFunctionKey("3");
            return;
        }
        session_type = Scroll;
        SetButtonValues();
    }

    void HandleBankButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            handleFunctionKey("4");
            return;
        }
        session_type = Bank;
        SetButtonValues();
    }

    void HandleMasterButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            handleFunctionKey("5");
            return;
        }

        if (context->GetMasterFaderModeEnabled())
        {
            context->ToggleMasterFaderMode();
        }
        else
        {
            session_type = Master;
        }

        SetButtonValues();
    }

    void HandleClickButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            handleFunctionKey("6");
            return;
        }
        session_type = Click;
        SetButtonValues();
    }

    void HandleSectionButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            handleFunctionKey("7");
            return;
        }
        // Once selected and repressing it again, it will create a region from the time selection
        if (session_type == Section)
        {
            Main_OnCommandEx(40306, 0, 0); // Markers: Insert region from time selection and edit...
        }
        session_type = Section;
        SetButtonValues();
    }

    void HandleMarkerButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            handleFunctionKey("8");
            return;
        }
        session_type = Marker;
        SetButtonValues();
    }

    void HandlePrevButton(int value)
    {
        if (!value)
        {
            prevButton->SetValue(BTN_VALUE_OFF);
            return;
        }

        prevButton->SetValue(BTN_VALUE_ON);

        switch (session_type)
        {
        case Channel:
            trackNavigator->DecrementOffset(context->GetShiftLeft() ? context->GetNbBankChannels() : 1);
            TrackList_UpdateAllExternalSurfaces();
            break;
        case Zoom:
            context->GetShiftLeft() ? Main_OnCommandEx(40112, 0, 0) // View: Zoom out vertical
                                    : Main_OnCommandEx(1011, 0, 0); // View: Zoom out horizontal
            break;
        case Scroll:
            // TODO: Change to up/down, left/right scrolling
            Main_OnCommandEx(40286, 0, 0); // Track: Go to previous track
            break;
        case Bank:
            trackNavigator->DecrementOffset(context->GetShiftLeft() ? 1 : context->GetNbBankChannels());
            TrackList_UpdateAllExternalSurfaces();
            break;
        case Master:
            trackNavigator->DecrementOffset(context->GetShiftLeft() ? context->GetNbBankChannels() : 1);
            TrackList_UpdateAllExternalSurfaces();
            break;
        case Click:
            Main_OnCommandEx(42456, 0, 0); // Options: Set metronome speed to 1x
            break;
        case Section:
            context->GetShiftLeft() ? Main_OnCommandEx(40625, 0, 0)               // Time selection: Set start point
                                    : Main_OnCommandStringEx("_SWS_SELPREVMORR"); // SWS: Goto/select previous marker/region
            break;
        case Marker:
            Main_OnCommandStringEx("_SWS_SELPREVMORR"); // SWS: Goto/select previous marker/region
            break;
        }
    }

    void HandleNextButton(int value)
    {
        if (!value)
        {
            nextButton->SetValue(BTN_VALUE_OFF);
            return;
        }

        nextButton->SetValue(BTN_VALUE_ON);

        switch (session_type)
        {
        case Channel:
            trackNavigator->IncrementOffset(context->GetShiftLeft() ? context->GetNbBankChannels() : 1);
            TrackList_UpdateAllExternalSurfaces();
            break;
        case Zoom:
            context->GetShiftLeft() ? Main_OnCommandEx(40111, 0, 0) // View: Zoom in vertical
                                    : Main_OnCommandEx(1012, 0, 0); // View: Zoom in horizontal
            break;
        case Scroll:
            Main_OnCommandEx(40285, 0, 0); // Track: Go to next track
            break;
        case Bank:
            trackNavigator->IncrementOffset(context->GetShiftLeft() ? 1 : context->GetNbBankChannels());
            TrackList_UpdateAllExternalSurfaces();
            break;
        case Master:
            trackNavigator->IncrementOffset(context->GetShiftLeft() ? context->GetNbBankChannels() : 1);
            TrackList_UpdateAllExternalSurfaces();
            break;
        case Click:
            Main_OnCommandEx(42457, 0, 0); // Options: Set metronome speed to 2x
            break;
        case Section:
            context->GetShiftLeft() ? Main_OnCommandEx(40626, 0, 0)               // Time selection: Set end point
                                    : Main_OnCommandStringEx("_SWS_SELNEXTMORR"); // SWS: Goto/select next marker/region

            break;
        case Marker:
            Main_OnCommandStringEx("_SWS_SELNEXTMORR"); // SWS: Goto/select next marker/region
            break;
        }
    }

    void HandleEncoderIncrement(int value)
    {
        switch (session_type)
        {
        case Channel:
            trackNavigator->IncrementOffset(context->GetShiftLeft() ? context->GetNbBankChannels() : 1);
            break;
        case Zoom:
            context->GetShiftLeft() ? Main_OnCommandEx(40111, 0, 0) // View: Zoom in vertical
                                    : Main_OnCommandEx(1012, 0, 0); // View: Zoom in horizontal
            break;
        case Scroll:
            context->GetShiftLeft() ? Main_OnCommandEx(40141, 0, 0)  // View: Scroll view left
                                    : Main_OnCommandEx(40139, 0, 0); // View: Scroll view up
            break;
        case Bank:
            trackNavigator->IncrementOffset(context->GetShiftLeft() ? 1 : context->GetNbBankChannels());
            break;
        case Master:
            context->GetShiftLeft()    ? IncrementMasterPan(value, true)
            : context->GetShiftRight() ? IncrementMasterPan(value, false)
                                       : Main_OnCommandStringEx("_XENAKIOS_NUDMASVOL1DBU"); // Xenakios/SWS: Nudge master volume 1 dB up;
            break;
        case Click:
            IncrementMetronomeVolume();
            break;
        case Section:
            context->GetShiftLeft()    ? Main_OnCommandEx(40105, 0, 0)  // View: Move cursor right one pixel
            : context->GetShiftRight() ? Main_OnCommandEx(40103, 0, 0)  // Time selection: Move cursor right, creating time selection
                                       : Main_OnCommandEx(41044, 0, 0); // Move edit cursor forward one beat
            break;
        case Marker:
            context->GetShiftLeft() ? Main_OnCommandEx(40105, 0, 0)  // View: Move cursor right one pixel
                                    : Main_OnCommandEx(41044, 0, 0); // Move edit cursor forward one beat
            break;
        }
    }

    void HandleEncoderDecrement(int value)
    {
        switch (session_type)
        {
        case Channel:
            trackNavigator->DecrementOffset(context->GetShiftLeft() ? context->GetNbBankChannels() : 1);
            break;
        case Zoom:
            context->GetShiftLeft() ? Main_OnCommandEx(40112, 0, 0) // View: Zoom out vertical
                                    : Main_OnCommandEx(1011, 0, 0); // View: Zoom out horizontal
            break;
        case Scroll:
            context->GetShiftLeft() ? Main_OnCommandEx(40140, 0, 0)  // View: Scroll view right
                                    : Main_OnCommandEx(40138, 0, 0); // View: Scroll view down
            break;
        case Bank:
            trackNavigator->DecrementOffset(context->GetShiftLeft() ? 1 : context->GetNbBankChannels());
            break;
        case Master:
            context->GetShiftLeft()    ? DecrementMasterPan(value, true)
            : context->GetShiftRight() ? DecrementMasterPan(value, false)
                                       : Main_OnCommandStringEx("_XENAKIOS_NUDMASVOL1DBD"); // Xenakios/SWS: Nudge master volume 1 dB down
            break;
        case Click:
            DecrementMetronomeVolume();
            break;
        case Section:
            context->GetShiftLeft()    ? Main_OnCommandEx(40104, 0, 0)  // View: Move cursor left one pixel, View: Move cursor right one pixel
            : context->GetShiftRight() ? Main_OnCommandEx(40102, 0, 0)  // Time selection: Move cursor left, creating time selection, Time selection: Move cursor left, creating time selection
                                       : Main_OnCommandEx(41045, 0, 0); // Move edit cursor back one beat, Move edit cursor forward one beat
            break;
        case Marker:
            context->GetShiftLeft() ? Main_OnCommandEx(40104, 0, 0)  // View: Move cursor left one pixel, View: Move cursor right one pixel
                                    : Main_OnCommandEx(41045, 0, 0); // Move edit cursor back one beat, Move edit cursor forward one beat
            break;
        }
    }

    void HandleEncoderClick(int value)
    {
        if (value == 0)
        {
            return;
        }

        switch (session_type)
        {
        case Channel:
            break;
        case Zoom:
            context->GetShiftLeft() ? Main_OnCommandEx(40113, 0, 0)  // View: Toggle track zoom to maximum height
                                    : Main_OnCommandEx(40110, 0, 0); // View: Toggle track zoom to minimum height
            break;
        case Scroll:
            context->GetShiftLeft() ? Main_OnCommandStringEx("_SWS_HSCROLLPLAY50") // SWS: Horizontal scroll to put play cursor at 50%
                                    : Main_OnCommandEx(40913, 0, 0);               // Track: Vertical scroll selected tracks into view
            break;
        case Bank:
            break;
        case Master:
            context->GetShiftLeft() ? SetMasterPanToCenter()
                                    : Main_OnCommandStringEx("_XENAKIOS_SETMASTVOLTO0"); // Xenakios/SWS: Set master volume to 0 dB
            break;
        case Click:
            context->GetShiftLeft() ? Main_OnCommandEx(40363, 0, 0)  // Options: Show metronome/pre-roll settings
                                    : Main_OnCommandEx(40364, 0, 0); // Options: Toggle metronome
            break;
        case Section:
            context->GetShiftLeft()    ? Main_OnCommandEx(41041, 0, 0)  // Move edit cursor to start of current measure
            : context->GetShiftRight() ? Main_OnCommandEx(40306, 0, 0)  // Markers: Insert region from time selection and edit...
                                       : Main_OnCommandEx(40616, 0, 0); // Markers: Edit region near cursor
            break;
        case Marker:
            context->GetShiftLeft()    ? Main_OnCommandEx(40171, 0, 0)  // Markers: Insert and/or edit marker at current position
            : context->GetShiftRight() ? Main_OnCommandEx(40613, 0, 0)  // Markers: Delete marker near cursor
                                       : Main_OnCommandEx(40157, 0, 0); // Markers: Insert marker at current position
            break;
        }
    }

    void HandleSessionNavEncoderChange(int value)
    {
        if (hasBit(value, 6))
        {
            HandleEncoderDecrement(value - 64);
        }
        else
        {
            HandleEncoderIncrement(value);
        }
    }
};

#endif