
#ifndef CSURF_SESSION_MANAGER_V2_C_
#define CSURF_SESSION_MANAGER_V2_C_

#include "../controls/csurf_button.hpp"
#include "../controls/csurf_color_button.hpp"
#include "../shared/csurf_session_manager_actions.hpp"
#include "csurf_fp_v2_navigator.hpp"
#include "../shared/csurf_daw.hpp"
#include "csurf_fp_v2_ui_control_panel.hpp"
#include "../shared/csurf_faderport_ui_imgui_utils.hpp"

enum SessionTypes // NOLINT(*-use-internal-linkage)
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

class CSurf_FP_V2_SessionManager // NOLINT(*-use-internal-linkage)
{
    CSurf_Context *context;
    CSurf_FP_V2_Navigator *trackNavigator;
    SessionTypes session_type = Channel;
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_V2);

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

protected:
    void SetButtonValues() const {
        // With shift engaged, blink the selected button
        const Btn_Value valueOn = context->GetShiftLeft() && !settings->GetDistractionFreeMode()
                                      ? BTN_VALUE_BLINK
                                      : BTN_VALUE_ON;

        linkButton->SetValue(
            GetToggleCommandStringState("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND") && context->GetShiftLeft()
                ? BTN_VALUE_ON
                : context->GetLastTouchedFxMode() // NOLINT(*-avoid-nested-conditional-operator)
                      ? valueOn
                      : BTN_VALUE_OFF);
        panButton->SetValue(ReaSonusV2ControlPanel::control_panel_open && context->GetShiftLeft()
                                ? BTN_VALUE_ON
                                : session_type == Pan // NOLINT(*-avoid-nested-conditional-operator)
                                      ? valueOn
                                      : BTN_VALUE_OFF);
        channelButton->SetValue(session_type == Channel
                                    ? valueOn
                                    : BTN_VALUE_OFF);
        scrollButton->SetValue(session_type == Scroll
                                   ? valueOn
                                   : session_type == Zoom // NOLINT(*-avoid-nested-conditional-operator)
                                         ? BTN_VALUE_ON
                                         : BTN_VALUE_OFF);
        masterButton->SetValue(session_type == Master ? valueOn : BTN_VALUE_OFF);
        clickButton->SetValue(session_type == Click ? valueOn : BTN_VALUE_OFF);
        sectionButton->SetValue(session_type == Section ? valueOn : BTN_VALUE_OFF);
        markerButton->SetValue(session_type == Marker ? valueOn : BTN_VALUE_OFF);
    }

    void SetButtonColors() const {
        MediaTrack *media_track = trackNavigator->GetControllerTrack();
        const ButtonColor color = DAW::GetTrackColor(media_track);

        channelButton->SetColor(context->GetShiftLeft() && context->GetFaderDisabled() ? ButtonColorYellow : color);
        linkButton->SetColor(context->GetShiftLeft() ? ButtonColorYellow : ButtonColorGreen);
        panButton->SetColor(context->GetShiftLeft() ? ButtonColorYellow : ButtonColorWhite);
        scrollButton->SetColor(session_type == Zoom ? ButtonColorYellow : ButtonColorWhite);
    }

    static void handleFunctionKey(const std::string &key) {
        const mINI::INIFile file(GetReaSonusIniPath(FP_V2));
        mINI::INIStructure ini;
        file.read(ini);

        const std::string actionId = ini["functions"][key];
        if (actionId == "0") {
            const int result = MB("There is no action assigned to this function.\nDo you want to assign an action?",
                                  "No action assigned", 1);
            if (result == 1) {
                if (!ReaSonusV2ControlPanel::control_panel_open) {
                    ToggleFPV2ControlPanel(ReaSonusV2ControlPanel::FUNCTIONS_PAGE);
                }
            }
            return;
        }

        if (isInteger(actionId)) {
            Main_OnCommandAsyncEx(stoi(actionId), 0, nullptr);
        } else {
            Main_OnCommandStringEx(actionId);
        }
    }

public:
    CSurf_FP_V2_SessionManager(
        CSurf_Context *context,
        CSurf_FP_V2_Navigator *trackNavigator,
        midi_Output *m_midiout) : context(context), trackNavigator(trackNavigator) {
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

    ~CSurf_FP_V2_SessionManager() = default;

    void Refresh() const {
        SetButtonValues();
        SetButtonColors();
    }

    void Update() const {
        SetButtonValues();
        SetButtonColors();
    }

    void SetSession(const SessionTypes _session_type) {
        session_type = _session_type;
        context->SetMasterFaderMode(_session_type == Master);
    }

    void resetPan() const {
        MediaTrack *media_track = trackNavigator->GetControllerTrack();

        switch (DAW::GetTrackPanMode(media_track)) {
            case PAN_MODE_STEREO_PAN: {
                SetMediaTrackInfo_Value(media_track, "D_PAN", 0);
                SetMediaTrackInfo_Value(media_track, "D_WIDTH", 1);
                break;
            }
            case PAN_MODE_DUAL_PAN: {
                SetMediaTrackInfo_Value(media_track, "D_DUALPANL", -1);
                SetMediaTrackInfo_Value(media_track, "D_DUALPANR", 1);
                break;
            }
            default: {
                SetMediaTrackInfo_Value(media_track, "D_PAN", 0);
            }
        }
    }

    void UpdatePanValue(const int val) const {
        double pan1 = 0.0;
        double pan2 = 0.0;
        int pan_mode;
        MediaTrack *media_track = trackNavigator->GetControllerTrack();
        GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);

        if (context->GetShiftLeft()) {
            double newValue = static_cast<int>(panToNormalized(pan2) * 127.0) + val;
            newValue = minmax(0.0, newValue, 127.0);
            SetMediaTrackInfo_Value(media_track, pan_mode != PAN_MODE_DUAL_PAN ? "D_WIDTH" : "D_DUALPANR",
                                    normalizedToPan(newValue / 127));
        } else {
            double newValue = static_cast<int>(panToNormalized(pan1) * 127.0) + val;
            newValue = minmax(0.0, newValue, 127.0);
            SetMediaTrackInfo_Value(media_track, pan_mode != PAN_MODE_DUAL_PAN ? "D_PAN" : "D_DUALPANL",
                                    normalizedToPan(newValue / 127));
        }
    }

    void IncrementPan(const int val) const {
        UpdatePanValue(val);
    }

    void DecrementPan(const int val) const {
        UpdatePanValue(val * -1);
    }

    void HandleLinkButton(const int value) const {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            Main_OnCommandStringEx("_REASONUS_TOGGLE_PLAY_CURSOR_COMMAND");
        } else {
            context->ToggleLastTouchedFxMode();
        }
    }

    void HandlePanButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            ToggleFPV2ControlPanel(ReaSonusV2ControlPanel::FUNCTIONS_PAGE);
            return;
        }
        SetSession(Pan);
    }

    void HandleChannelButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft() && settings->GetCanDisableFader()) {
            context->SetFaderDisabled(!context->GetFaderDisabled());
            return;
        }

        SetSession(Channel);
    }

    void HandleScrollButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            SetSession(Zoom);
            return;
        }
        SetSession(Scroll);
    }

    void HandleMasterButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            handleFunctionKey("1");
            return;
        }
        SetSession(Master);
    }

    void HandleClickButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            handleFunctionKey("2");
            return;
        }
        SetSession(Click);
    }

    void HandleSectionButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            handleFunctionKey("3");
            return;
        }

        // Once selected and repressing it again, it will edit the region near the cursor
        if (session_type == Section) {
            Main_OnCommandAsyncEx(40616, 0, nullptr); // Markers: Edit region near cursor
        }
        SetSession(Section);
    }

    void HandleMarkerButton(const int value) {
        if (value == 0) {
            return;
        }

        if (context->GetShiftLeft()) {
            handleFunctionKey("4");
            return;
        }

        // Once selected and repressing it again, it will create a marker
        if (session_type == Marker) {
            Main_OnCommandAsyncEx(40157, 0, nullptr); // Markers: Insert marker at current position
        }
        SetSession(Marker);
    }

    void HandlePrevButton(const int value) const {
        if (value == 0) {
            prevButton->SetValue(BTN_VALUE_OFF);
            return;
        }

        prevButton->SetValue(BTN_VALUE_ON);

        switch (session_type) {
            case Pan:
            case Channel:
            case Master:
            case Scroll:
                context->GetShiftLeft()
                    ? DAW::EditUndo()
                    : DAW::SetUniqueSelectedTrack(trackNavigator->GetPreviousTrack());
                break;
            case Zoom:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40112, 0, nullptr) // View: Zoom out vertical
                    : Main_OnCommandAsyncEx(1011, 0, nullptr); // View: Zoom out horizontal
                break;
            case Click:
                context->GetShiftLeft()
                    ? DAW::EditUndo()
                    : Main_OnCommandAsyncEx(42456, 0, nullptr); // Options: Set metronome speed to 1x
                break;
            case Section:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40625, 0, nullptr) // Time selection: Set start point
                    : Main_OnCommandStringEx("_SWS_SELPREVMORR"); // SWS: Goto/select previous marker/region
                break;
            case Marker:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40029, 0, nullptr) // Edit: Undo
                    : Main_OnCommandStringEx("_SWS_SELPREVMORR"); // SWS: Goto/select previous marker/region
                break;
        }
    }

    void HandleNextButton(const int value) const {
        if (value == 0) {
            nextButton->SetValue(BTN_VALUE_OFF);
            return;
        }

        nextButton->SetValue(BTN_VALUE_ON);

        switch (session_type) {
            case Pan:
            case Channel:
            case Scroll:
            case Master:
                context->GetShiftLeft()
                    ? DAW::EditRedo()
                    : DAW::SetUniqueSelectedTrack(trackNavigator->GetNextTrack());
                break;
            case Zoom:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40111, 0, nullptr) // View: Zoom in vertical
                    : Main_OnCommandAsyncEx(1012, 0, nullptr); // View: Zoom in horizontal
                break;
            case Click:
                context->GetShiftLeft()
                    ? DAW::EditRedo()
                    : Main_OnCommandAsyncEx(42457, 0, nullptr); // Options: Set metronome speed to 2x
                break;
            case Section:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40626, 0, nullptr) // Time selection: Set end point
                    : Main_OnCommandStringEx("_SWS_SELNEXTMORR"); // SWS: Goto/select next marker/region

                break;
            case Marker:
                context->GetShiftLeft()
                    ? DAW::EditRedo()
                    : Main_OnCommandStringEx("_SWS_SELNEXTMORR"); // SWS: Goto/select next marker/region
                break;
        }
    }

    void HandleEncoderIncrement(const int value) const {
        switch (session_type) {
            case Pan:
                IncrementPan(value);
                break;
            case Channel:
                Main_OnCommandAsyncEx(40285, 0, nullptr); // Track: Go to next track
                break;
            case Zoom:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40111, 0, nullptr) // View: Zoom in vertical
                    : Main_OnCommandAsyncEx(1012, 0, nullptr); // View: Zoom in horizontal
                break;
            case Scroll:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40141, 0, nullptr) // View: Scroll view left
                    : Main_OnCommandAsyncEx(40139, 0, nullptr); // View: Scroll view up
                break;
            case Master:
                IncrementPan(value);
                break;
            case Click:
                IncrementMetronomeVolume();
                break;
            case Section:
            case Marker:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40105, 0, nullptr) // View: Move cursor right one pixel
                    : Main_OnCommandAsyncEx(41044, 0, nullptr); // Move edit cursor forward one beat
                break;
        }
    }

    void HandleEncoderDecrement(const int value) const {
        switch (session_type) {
            case Pan:
                DecrementPan(value);
                break;
            case Channel:
                Main_OnCommandAsyncEx(40286, 0, nullptr); // Track: Go to previous track
                break;
            case Zoom:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40112, 0, nullptr) // View: Zoom out vertical
                    : Main_OnCommandAsyncEx(1011, 0, nullptr); // View: Zoom out horizontal
                break;
            case Scroll:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40140, 0, nullptr) // View: Scroll view right
                    : Main_OnCommandAsyncEx(40138, 0, nullptr); // View: Scroll view down
                break;
            case Master:
                DecrementPan(value);
                break;
            case Click:
                DecrementMetronomeVolume();
                break;
            case Section:
            case Marker:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40104, 0, nullptr)
                    // View: Move cursor left one pixel, View: Move cursor right one pixel
                    : Main_OnCommandAsyncEx(41045, 0, nullptr);
                // Move edit cursor back one beat, Move edit cursor forward one beat
                break;
        }
    }

    void HandleEncoderClick(const int value) const {
        if (value == 0) {
            return;
        }

        switch (session_type) {
            case Pan:
                context->GetShiftLeft()
                    ? DAW::EditSave()
                    : resetPan();
                break;
            case Channel:
                break;
            case Zoom:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40113, 0, nullptr) // View: Toggle track zoom to maximum height
                    : Main_OnCommandAsyncEx(40110, 0, nullptr); // View: Toggle track zoom to minimum height
                break;
            case Scroll:
                context->GetShiftLeft()
                    ? Main_OnCommandStringEx("_SWS_HSCROLLPLAY50") // SWS: Horizontal scroll to put play cursor at 50%
                    : Main_OnCommandAsyncEx(40913, 0, nullptr); // Track: Vertical scroll selected tracks into view
                break;
            case Master:
                context->GetShiftLeft()
                    ? DAW::EditSave()
                    : resetPan();
                break;
            case Click:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40363, 0, nullptr) // Options: Show metronome/pre-roll settings
                    : Main_OnCommandAsyncEx(40364, 0, nullptr); // Options: Toggle metronome
                break;
            case Section:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40615, 0, nullptr) // Markers: Delete region near cursor
                    : Main_OnCommandAsyncEx(40306, 0, nullptr); // Markers: Insert region from time selection and edit
                break;
            case Marker:
                context->GetShiftLeft()
                    ? Main_OnCommandAsyncEx(40613, 0, nullptr) // Markers: Delete marker near cursor
                    : Main_OnCommandAsyncEx(40171, 0, nullptr);
                // Markers: Insert and/or edit marker at current position
                break;
        }
    }

    void HandleSessionNavEncoderChange(const int value) const {
        if (hasBit(value, 6)) {
            HandleEncoderDecrement(value - 64);
        } else {
            HandleEncoderIncrement(value);
        }
    }
};

#endif
