#ifndef CSURF_FP_8_TRACK_SENDS_MANAGER_C_
#define CSURF_FP_8_TRACK_SENDS_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"
#include <WDL/db2val.h>

class CSurf_FP_8_TrackSendsManager : public CSurf_FP_8_ChannelManager {
protected:
    void GetFaderValue(
        MediaTrack *media_track,
        const int send_index,
        int *fader_value,
        int *value_bar_value,
        double *_pan,
        std::string *pan_str,
        const bool is_hardware_out
    ) const {
        const double volume = GetTrackSendInfo_Value(
            media_track,
            is_hardware_out ? SEND_MODE_HARDWARE : SEND_MODE_SEND,
            send_index,
            "D_VOL"
        );
        const double pan = GetTrackSendInfo_Value(
            media_track,
            is_hardware_out ? SEND_MODE_HARDWARE : SEND_MODE_SEND,
            send_index,
            "D_PAN"
        );

        *pan_str = GetPan1String(pan);
        *_pan = pan;

        if (context->GetShiftChannelLeft()) {
            *fader_value = static_cast<int>(panToNormalized(pan) * 16383.0);
            *value_bar_value = static_cast<int>(volToNormalized(volume) * 127);
        } else {
            *fader_value = static_cast<int>(volToNormalized(volume) * 16383.0);
            *value_bar_value = static_cast<int>(panToNormalized(pan) * 127);
        }
    }

    int GetSendIndex(MediaTrack *media_track, const int slot_index, bool *is_hardware_out) const {
        const int hardware_count = GetTrackNumSends(media_track, SEND_MODE_HARDWARE);
        const int sends_count = GetTrackNumSends(media_track, SEND_MODE_SEND);

        if (hardware_count + sends_count <= slot_index) {
            return -1;
        }

        if (slot_index < hardware_count) {
            *is_hardware_out = true;
            return slot_index;
        }

        *is_hardware_out = false;
        return slot_index - hardware_count;
    }

    std::string GetLine3Content(
        MediaTrack *media_track,
        const int send_index,
        const bool is_hardware_out
    ) const {
        const auto send_value = is_hardware_out
                                    ? static_cast<DisplaySendValue>(settings->GetSendDisplayValueHardware())
                                    : static_cast<DisplaySendValue>(settings->GetSendDisplayValueSend());

        switch (send_value) {
            case DISPLAY_SEND_VALUE_VOLUME:
                return GetVolumeString(GetTrackSendInfo_Value(
                    media_track,
                    is_hardware_out ? SEND_MODE_HARDWARE : SEND_MODE_SEND,
                    send_index,
                    "D_VOL"
                ));

            case DISPLAY_SEND_VALUE_PAN:
                return GetPan1String(GetTrackSendInfo_Value(
                    media_track,
                    is_hardware_out ? SEND_MODE_HARDWARE : SEND_MODE_SEND,
                    send_index,
                    "D_PAN"
                ));

            case DISPLAY_SEND_VALUE_MUTE:
                return DAW::GetTrackSendMute(media_track, send_index, is_hardware_out)
                           ? "Muted"
                           : "";

            case DISPLAY_SEND_VALUE_PHASE:
                return DAW::GetTrackSendPhase(media_track, send_index, is_hardware_out)
                           ? "Phase Rev"
                           : "";

            case DISPLAY_SEND_VALUE_MONO:
                return DAW::GetTrackSendMono(media_track, send_index, is_hardware_out)
                           ? "Mono"
                           : "Stereo";

            case DISPLAY_SEND_VALUE_SEND_MODE:
                return DAW::GetTrackSurfaceSendMode(media_track, send_index, is_hardware_out);

            case DISPLAY_SEND_VALUE_AUTO_MODE:
                return DAW::GetTrackSurfaceSendAutoMode(media_track, send_index, is_hardware_out);

            case DISPLAY_SEND_VALUE_FIXED:
                return is_hardware_out ? "Hw out" : "";

            default:
                return "";
        }
    }

    std::string GetLine4Content(MediaTrack *media_track, const int index) const {
        if (index < context->GetNbChannels() - 2) {
            return "";
        }
        if (index == context->GetNbChannels() - 2) {
            return fmt::format("Sends: {}", GetTrackNumSends(media_track, SEND_MODE_SEND));
        }

        return fmt::format("Hardw: {}", GetTrackNumSends(media_track, SEND_MODE_HARDWARE));
    }

public:
    CSurf_FP_8_TrackSendsManager(
        const std::vector<CSurf_FP_8_Track *> &tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout) {
        context->ResetChannelManagerItemIndex();
        context->ResetChannelManagerItemsCount();
        context->SetChannelManagerType(Track);
        context->SetAddSendReceiveMode(-1);

        CSurf_FP_8_TrackSendsManager::UpdateTracks(true);
    }

    ~CSurf_FP_8_TrackSendsManager() override = default;

    void UpdateTracks(const bool force_update) override {
        const WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *sends_track = GetSelectedTrack(nullptr, 0);
        MediaTrack *add_send_track;
        context->SetChannelManagerItemsCount(GetTrackNumSends(sends_track, 0));

        for (int i = 0; i < context->GetNbChannels(); i++) {
            bool is_hardware_out;
            const CSurf_FP_8_Track *faderport_channel = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);

            const int send_index = GetSendIndex(sends_track, i, &is_hardware_out);
            const bool add_send_enabled = context->GetAddSendReceiveMode() == i;

            if (add_send_enabled) {
                add_send_track = GetTrack(nullptr, context->GetCurrentSelectedSendReceive());
            }

            int fader_value = 0;
            int value_bar_value = 0;
            double pan = 0.0;
            std::string pan_str;

            SetTrackColors(media_track, DAW::IsTrackSelected(media_track), false);

            GetFaderValue(sends_track, send_index, &fader_value, &value_bar_value, &pan, &pan_str, is_hardware_out);

            if (!media_track) {
                faderport_channel->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT, force_update);
            } else {
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_1,
                    ALIGN_LEFT,
                    DAW::GetTrackName(media_track).c_str(),
                    sends_track == media_track ? INVERT : NON_INVERT,
                    force_update
                );
            }

            // Handle the displays
            if (add_send_enabled) {
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_2,
                    ALIGN_LEFT,
                    ("Trk: " + DAW::GetTrackIndex(add_send_track)).c_str(),
                    INVERT,
                    force_update
                );
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_3,
                    ALIGN_CENTER,
                    DAW::GetTrackName(add_send_track).c_str(),
                    INVERT,
                    force_update
                );
            } else if (send_index > -1) {
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_2,
                    ALIGN_LEFT,
                    DAW::GetTrackSendName(sends_track, send_index, is_hardware_out).c_str(),
                    INVERT,
                    force_update
                );
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_3,
                    ALIGN_CENTER,
                    GetLine3Content(sends_track, send_index, is_hardware_out).c_str(),
                    NON_INVERT,
                    force_update
                );
            } else {
                faderport_channel->SetDisplayLine(DISPLAY_LINE_2, ALIGN_LEFT, "", NON_INVERT, force_update);
                faderport_channel->SetDisplayLine(DISPLAY_LINE_3, ALIGN_CENTER, "", NON_INVERT, force_update);
            }

            faderport_channel->SetDisplayLine(
                DISPLAY_LINE_4,
                ALIGN_CENTER,
                GetLine4Content(sends_track, i).c_str(),
                NON_INVERT,
                force_update
            );

            // Set the fader and valuebar values
            if (send_index > -1) {
                faderport_channel->SetFaderValue(fader_value, force_update);
                faderport_channel->SetValueBarMode(
                    context->GetShiftChannelLeft()
                        ? VALUEBAR_MODE_FILL
                        : VALUEBAR_MODE_BIPOLAR
                );
                faderport_channel->SetValueBarValue(value_bar_value);
            } else {
                faderport_channel->SetFaderValue(0, force_update);
                faderport_channel->SetValueBarMode(VALUEBAR_MODE_FILL);
                faderport_channel->SetValueBarValue(0);
            }

            faderport_channel->SetTrackColor(color, force_update);
            faderport_channel->SetSelectButtonValue(
                media_track == nullptr
                    ? BTN_VALUE_OFF
                    : BTN_VALUE_ON,
                force_update
            );
            faderport_channel->SetMuteButtonValue(
                ButtonBlinkOnOff(
                    context->GetShiftChannelLeft() && DAW::GetTrackSendMute(sends_track, send_index, is_hardware_out),
                    DAW::GetTrackSendMute(sends_track, send_index, is_hardware_out),
                    settings->GetDistractionFreeMode()),
                force_update
            );
            faderport_channel->SetSoloButtonValue(
                (context->GetShiftChannelLeft() && DAW::GetTrackSendMono(sends_track, send_index, is_hardware_out))
                || (!context->GetShiftChannelLeft() && DAW::GetTrackSendPhase(sends_track, send_index, is_hardware_out))
                    ? BTN_VALUE_ON
                    : BTN_VALUE_OFF,
                force_update
            );

            faderport_channel->SetDisplayMode(DISPLAY_MODE_2, force_update);
        }
    }

    void HandleSelectClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        /**
         * Set add_send_receive_mode when left shift and select are engaged.
         * If add_send_receive_mode is set, regardless the shift keys, we will reset the add_send_receive_mode
         */
        if (context->GetShiftChannelLeft()) {
            if (context->GetAddSendReceiveMode() == -1) {
                DAW::SetUniqueSelectedTrack(media_track);
                context->SetCurrentSelectedSendReceive(0);
                context->SetAddSendReceiveMode(index);
            } else {
                context->SetAddSendReceiveMode(-1);
            }
            return;
        }

        if (context->GetAddSendReceiveMode() == index) {
            context->SetAddSendReceiveMode(-1);
        }

        if (context->GetShiftChannelRight()) {
            bool is_hardware_out;
            MediaTrack *selected_track = GetSelectedTrack(nullptr, 0);
            const int send_index = GetSendIndex(selected_track, index, &is_hardware_out);

            RemoveTrackSend(selected_track, is_hardware_out ? SEND_MODE_HARDWARE : SEND_MODE_SEND, send_index);
            return;
        }

        DAW::SetUniqueSelectedTrack(media_track);
    }

    void HandleMuteClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        bool is_hardware_out;
        MediaTrack *send_track = GetSelectedTrack(nullptr, 0);
        const int send_index = GetSendIndex(send_track, index, &is_hardware_out);

        if (context->GetShiftChannelLeft()) {
            DAW::SetNextTrackSendMode(send_track, send_index, is_hardware_out);
        } else {
            DAW::ToggleTrackSendMute(send_track, send_index, is_hardware_out);
        }
    }

    void HandleSoloClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        bool is_hardware_out;
        MediaTrack *send_track = GetSelectedTrack(nullptr, 0);
        const int send_index = GetSendIndex(send_track, index, &is_hardware_out);

        if (context->GetShiftChannelLeft()) {
            DAW::ToggleTrackSendMono(send_track, send_index, is_hardware_out);
        } else {
            DAW::ToggleTrackSendPhase(send_track, send_index, is_hardware_out);
        }
    }

    void HandleFaderTouch(const int index, const int value) override {
        MediaTrack *media_track = GetSelectedTrack(nullptr, 0);

        /**
         * Check if we have the proper conditions to continue with Single point automation
         * All pan related values (except for width) are displayed reversed.
         * Therefore some fo the values are inverted as well, to make it mork as it should
         */
        if (HasSinglePointAutomation(media_track, index, value)) {
            const int send_index = context->GetChannelManagerItemIndex() + index;
            const std::string chunk_name = context->GetShiftLeft() ? "<PANENV" : "<VOLENV";
            double volume = 0.0;
            double pan = 0.0;

            // Get the value to write for the automation
            if (value > 0) {
                // We need to set it to read first to get the actual volume of the envelope
                SetTrackAutomationMode(media_track, AUTOMATION_READ);
                GetTrackSendUIVolPan(media_track, send_index, &volume, &pan);
                SetTrackAutomationMode(media_track, AUTOMATION_TRIM);
            } else {
                GetTrackSendUIVolPan(media_track, send_index, &volume, &pan);
            }

            HandleSinglePointAutomation(
                media_track,
                index,
                value,
                chunk_name,
                context->GetShiftLeft() ? pan : DB2SLIDER(VAL2DB(volume)),
                SPA_Send,
                send_index
            );
        }
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        bool is_hardware_out;
        MediaTrack *send_track = GetSelectedTrack(nullptr, 0);
        const int send_index = GetSendIndex(send_track, index, &is_hardware_out);

        if (context->GetShiftChannelLeft()) {
            DAW::SetTrackSendPan(
                send_track,
                send_index,
                normalizedToPan(int14ToNormalized(msb, lsb)),
                is_hardware_out
            );
        } else {
            DAW::SetTrackSendVolume(
                send_track,
                send_index,
                int14ToVol(msb, lsb),
                is_hardware_out
            );
        }
    }
};

#endif
