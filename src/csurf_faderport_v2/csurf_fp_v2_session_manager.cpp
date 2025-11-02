
#ifndef CSURF_SESSION_MANAGER_V2_C_
#define CSURF_SESSION_MANAGER_V2_C_

#include "../controls/csurf_button.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_context.cpp"
#include "../shared/csurf_session_manager_actions.hpp"
#include "csurf_fp_v2_navigator.hpp"
#include <mini/ini.h>
#include "../controls/csurf_color_button.hpp"
#include "../shared/csurf_daw.hpp"
#include "csurf_fp_v2_ui_control_panel.hpp"
#include "../shared/csurf_faderport_ui_imgui_utils.hpp"

enum SessionTypes
{
    Pan,
    Channel,
    Zoom,
    Scroll,
    Master,
    Click,
    Section,
    Marker,
};

class CSurf_FP_V2_SessionManager
{
protected:
    CSurf_Context *context;
    midi_Output *m_midiout;
    CSurf_FP_V2_Navigator *trackNavigator;
    SessionTypes session_type = Channel;
    bool handleFunctionKeys = false;

    CSurf_ColorButton *linkButton;
    CSurf_ColorButton *panButton;
    CSurf_ColorButton *channelButton;
    CSurf_ColorButton *scrollButton;
    CSurf_Button *masterButton;
    CSurf_Button *clickButton;
    CSurf_Button *sectionButton;
    CSurf_Button *markerButton;
    CSurf_Button *prevButton;
    CSurf_Button *nextButton;

    void SetButtonValues()
    {
        // With shift engaged, blink the selected button
        Btn_Value valueOn = context->GetShiftLeft() && !context->GetDistractionFreeMode() ? BTN_VALUE_BLINK : BTN_VALUE_ON;

        linkButton->SetValue(GetToggleCommandStringState("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND") && context->GetShiftLeft()
                                 ? BTN_VALUE_ON
                             : context->GetLastTouchedFxMode()
                                 ? valueOn
                                 : BTN_VALUE_OFF);
        panButton->SetValue(ReaSonusV2ControlPanel::control_panel_open && context->GetShiftLeft()
                                ? BTN_VALUE_ON
                            : session_type == Pan
                                ? valueOn
                                : BTN_VALUE_OFF);
        channelButton->SetValue(session_type == Channel ? valueOn : BTN_VALUE_OFF);
        scrollButton->SetValue(session_type == Scroll
                                   ? valueOn
                               : session_type == Zoom
                                   ? BTN_VALUE_ON
                                   : BTN_VALUE_OFF);
        masterButton->SetValue(session_type == Master ? valueOn : BTN_VALUE_OFF);
        clickButton->SetValue(session_type == Click ? valueOn : BTN_VALUE_OFF);
        sectionButton->SetValue(session_type == Section ? valueOn : BTN_VALUE_OFF);
        markerButton->SetValue(session_type == Marker ? valueOn : BTN_VALUE_OFF);
    }

    void SetButtonColors()
    {
        MediaTrack *media_track = trackNavigator->GetControllerTrack();
        ButtonColor color = DAW::GetTrackColor(media_track);

        channelButton->SetColor(color);
        linkButton->SetColor(context->GetShiftLeft() ? ButtonColorYellow : ButtonColorGreen);
        panButton->SetColor(context->GetShiftLeft() ? ButtonColorYellow : ButtonColorWhite);
        scrollButton->SetColor(session_type == Zoom ? ButtonColorYellow : ButtonColorWhite);
    }

    void handleFunctionKey(std::string key)
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_V2));
        mINI::INIStructure ini;
        file.read(ini);

        std::string actionId = ini["functions"][key];
        if (actionId == "0")
        {
            int result = MB("There is no action assigned to this function.\nDo you want to assign an action?", "No action assigned", 1);
            if (result == 1)
            {
                if (!ReaSonusV2ControlPanel::control_panel_open)
                {
                    ToggleFPV2ControlPanel(ReaSonusV2ControlPanel::FUNCTIONS_PAGE);
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
    CSurf_FP_V2_SessionManager(
        CSurf_Context *context,
        CSurf_FP_V2_Navigator *trackNavigator,
        midi_Output *m_midiout) : context(context), m_midiout(m_midiout), trackNavigator(trackNavigator)
    {
        linkButton = new CSurf_ColorButton(ButtonColorGreen, BTN_LINK, BTN_VALUE_OFF, m_midiout);
        panButton = new CSurf_ColorButton(ButtonColorWhite, BTN_PAN, BTN_VALUE_OFF, m_midiout);
        channelButton = new CSurf_ColorButton(ButtonColorWhite, BTN_CHANNEL, BTN_VALUE_ON, m_midiout);
        scrollButton = new CSurf_ColorButton(ButtonColorWhite, BTN_SCROLL, BTN_VALUE_OFF, m_midiout);
        masterButton = new CSurf_Button(BTN_MASTER, BTN_VALUE_OFF, m_midiout);
        clickButton = new CSurf_Button(BTN_CLICK, BTN_VALUE_OFF, m_midiout);
        sectionButton = new CSurf_Button(BTN_SECTION, BTN_VALUE_OFF, m_midiout);
        markerButton = new CSurf_Button(BTN_MARKER, BTN_VALUE_OFF, m_midiout);

        prevButton = new CSurf_Button(BTN_PREV, BTN_VALUE_OFF, m_midiout);
        nextButton = new CSurf_Button(BTN_NEXT, BTN_VALUE_OFF, m_midiout);
    }
    ~CSurf_FP_V2_SessionManager() {};

    void Refresh()
    {
        SetButtonValues();
        SetButtonColors();
    }

    void Update()
    {
        SetButtonValues();
        SetButtonColors();
    }

    void SetSession(SessionTypes _session_type)
    {
        session_type = _session_type;
        context->SetMasterFaderMode(_session_type == Master);
    }

    void resetPan()
    {
        MediaTrack *media_track = trackNavigator->GetControllerTrack();

        int pan_mode = DAW::GetTrackPanMode(media_track);
        if (pan_mode < 4)
        {
            SetMediaTrackInfo_Value(media_track, "D_PAN", 0);
        }
        if (pan_mode == 6)
        {
            SetMediaTrackInfo_Value(media_track, "D_DUALPANL", -1);
            SetMediaTrackInfo_Value(media_track, "D_DUALPANR", 1);
        }
        if (pan_mode == 5)
        {
            SetMediaTrackInfo_Value(media_track, "D_PAN", 0);
            SetMediaTrackInfo_Value(media_track, "D_WIDTH", 1);
        }
    }

    void UpdatePanValue(int val)
    {
        double pan1, pan2 = 0.0;
        int pan_mode;
        MediaTrack *media_track = trackNavigator->GetControllerTrack();
        GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);

        if (context->GetShiftLeft())
        {
            double newValue = int(panToNormalized(pan2) * 127.0) + val;
            newValue = minmax(0.0, newValue, 127.0);
            SetMediaTrackInfo_Value(media_track, pan_mode < 6 ? "D_WIDTH" : "D_DUALPANR", normalizedToPan(newValue / 127));
        }
        else
        {
            double newValue = int(panToNormalized(pan1) * 127.0) + val;
            newValue = minmax(0.0, newValue, 127.0);
            SetMediaTrackInfo_Value(media_track, pan_mode < 6 ? "D_PAN" : "D_DUALPANL", normalizedToPan(newValue / 127));
        }
    }

    void IncrementPan(int val)
    {
        UpdatePanValue(val);
    }

    void DecrementPan(int val)
    {
        UpdatePanValue(val * -1);
    }

    void HandleLinkButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            Main_OnCommandStringEx("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND");
        }
        else
        {
            context->ToggleLastTouchedFxMode();
        }
    };

    void HandlePanButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            ToggleFPV2ControlPanel(ReaSonusV2ControlPanel::FUNCTIONS_PAGE);
            return;
        }
        SetSession(Pan);
    }

    void HandleChannelButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft() && context->GetCanDisableFader())
        {
            context->SetFaderDisabled(!context->GetFaderDisabled());
            return;
        }

        SetSession(Channel);
    }

    void HandleScrollButton(int value)
    {
        if (value == 0)
        {
            return;
        }

        if (context->GetShiftLeft())
        {
            SetSession(Zoom);
            return;
        }
        SetSession(Scroll);
    }

    void HandleMasterButton(int value)
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
        SetSession(Master);
    }

    void HandleClickButton(int value)
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
        SetSession(Click);
    }

    void HandleSectionButton(int value)
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

        // Once selected and repressing it again, it will edit the region near the cursor
        if (session_type == Section)
        {
            Main_OnCommandEx(40616, 0, 0); // Markers: Edit region near cursor
        }
        SetSession(Section);
    }

    void HandleMarkerButton(int value)
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

        // Once selected and repressing it again, it will create a marker
        if (session_type == Marker)
        {
            Main_OnCommandEx(40157, 0, 0); // Markers: Insert marker at current position
        }
        SetSession(Marker);
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
        case Pan:
            context->GetShiftLeft() ? DAW::EditUndo()
                                    : Main_OnCommandEx(40286, 0, 0); // Track: Go to previous track
            break;
        case Channel:
            context->GetShiftLeft() ? DAW::EditUndo()
                                    : DAW::SetUniqueSelectedTrack(trackNavigator->GetPreviousTrack());
            break;
        case Zoom:
            context->GetShiftLeft() ? Main_OnCommandEx(40112, 0, 0) // View: Zoom out vertical
                                    : Main_OnCommandEx(1011, 0, 0); // View: Zoom out horizontal
            break;
        case Scroll:
            context->GetShiftLeft() ? DAW::EditUndo()
                                    : Main_OnCommandEx(40286, 0, 0); // Track: Go to previous track
            break;
        case Master:
            context->GetShiftLeft() ? DAW::EditUndo()
                                    : Main_OnCommandEx(40286, 0, 0); // Track: Go to previous track
            break;
        case Click:
            context->GetShiftLeft() ? DAW::EditUndo()
                                    : Main_OnCommandEx(42456, 0, 0); // Options: Set metronome speed to 1x
            break;
        case Section:
            context->GetShiftLeft() ? Main_OnCommandEx(40625, 0, 0)               // Time selection: Set start point
                                    : Main_OnCommandStringEx("_SWS_SELPREVMORR"); // SWS: Goto/select previous marker/region
            break;
        case Marker:
            context->GetShiftLeft() ? Main_OnCommandEx(40029, 0, 0)               // Edit: Undo
                                    : Main_OnCommandStringEx("_SWS_SELPREVMORR"); // SWS: Goto/select previous marker/region
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
        case Pan:
            context->GetShiftLeft() ? DAW::EditRedo()
                                    : Main_OnCommandEx(40285, 0, 0); // Track: Go to next track
            break;
        case Channel:
            context->GetShiftLeft() ? DAW::EditRedo()
                                    : DAW::SetUniqueSelectedTrack(trackNavigator->GetNextTrack());
            break;
        case Zoom:
            context->GetShiftLeft() ? Main_OnCommandEx(40111, 0, 0) // View: Zoom in vertical
                                    : Main_OnCommandEx(1012, 0, 0); // View: Zoom in horizontal
            break;
        case Scroll:
            context->GetShiftLeft() ? DAW::EditRedo()
                                    : Main_OnCommandEx(40285, 0, 0); // Track: Go to next track
            break;
        case Master:
            context->GetShiftLeft() ? DAW::EditRedo()
                                    : Main_OnCommandEx(40285, 0, 0); // Track: Go to next track
            break;
        case Click:
            context->GetShiftLeft() ? DAW::EditRedo()
                                    : Main_OnCommandEx(42457, 0, 0); // Options: Set metronome speed to 2x
            break;
        case Section:
            context->GetShiftLeft() ? Main_OnCommandEx(40626, 0, 0)               // Time selection: Set end point
                                    : Main_OnCommandStringEx("_SWS_SELNEXTMORR"); // SWS: Goto/select next marker/region

            break;
        case Marker:
            context->GetShiftLeft() ? DAW::EditRedo()
                                    : Main_OnCommandStringEx("_SWS_SELNEXTMORR"); // SWS: Goto/select next marker/region
            break;
        }
    }

    void HandleEncoderIncrement(int value)
    {
        switch (session_type)
        {
        case Pan:
            IncrementPan(value);
            break;
        case Channel:
            Main_OnCommandEx(40285, 0, 0); // Track: Go to next track
            break;
        case Zoom:
            context->GetShiftLeft() ? Main_OnCommandEx(40111, 0, 0) // View: Zoom in vertical
                                    : Main_OnCommandEx(1012, 0, 0); // View: Zoom in horizontal
            break;
        case Scroll:
            context->GetShiftLeft() ? Main_OnCommandEx(40141, 0, 0)  // View: Scroll view left
                                    : Main_OnCommandEx(40139, 0, 0); // View: Scroll view up
            break;
        case Master:
            IncrementPan(value);
            break;
        case Click:
            IncrementMetronomeVolume();
            break;
        case Section:
            context->GetShiftLeft() ? Main_OnCommandEx(40105, 0, 0)  // View: Move cursor right one pixel
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
        case Pan:
            DecrementPan(value);
            break;
        case Channel:
            Main_OnCommandEx(40286, 0, 0); // Track: Go to previous track
            break;
        case Zoom:
            context->GetShiftLeft() ? Main_OnCommandEx(40112, 0, 0) // View: Zoom out vertical
                                    : Main_OnCommandEx(1011, 0, 0); // View: Zoom out horizontal
            break;
        case Scroll:
            context->GetShiftLeft() ? Main_OnCommandEx(40140, 0, 0)  // View: Scroll view right
                                    : Main_OnCommandEx(40138, 0, 0); // View: Scroll view down
            break;
        case Master:
            DecrementPan(value);
            break;
        case Click:
            DecrementMetronomeVolume();
            break;
        case Section:
            context->GetShiftLeft() ? Main_OnCommandEx(40104, 0, 0)  // View: Move cursor left one pixel, View: Move cursor right one pixel
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
        case Pan:
            context->GetShiftLeft() ? DAW::EditSave()
                                    : resetPan();
            break;
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
        case Master:
            context->GetShiftLeft() ? DAW::EditSave()
                                    : resetPan();
            break;
        case Click:
            context->GetShiftLeft() ? Main_OnCommandEx(40363, 0, 0)  // Options: Show metronome/pre-roll settings
                                    : Main_OnCommandEx(40364, 0, 0); // Options: Toggle metronome
            break;
        case Section:
            context->GetShiftLeft() ? Main_OnCommandEx(40615, 0, 0)  // Markers: Delete region near cursor
                                    : Main_OnCommandEx(40306, 0, 0); // Markers: Insert region from time selection and edit..
            break;
        case Marker:
            context->GetShiftLeft() ? Main_OnCommandEx(40613, 0, 0)  // Markers: Delete marker near cursor
                                    : Main_OnCommandEx(40171, 0, 0); // Markers: Insert and/or edit marker at current position
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