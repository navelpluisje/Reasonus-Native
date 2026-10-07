#ifndef CSURF_FP_8_RECEIVES_MANAGER_C_
#define CSURF_FP_8_RECEIVES_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"

class CSurf_FP_8_ReceivesManager : public CSurf_FP_8_ChannelManager {
protected:
    int nb_receives = 0;
    int current_receive = 0;
    bool has_last_touched_fx_enabled = false;

    void GetFaderValue(
        MediaTrack *media_track,
        const int receive_index,
        int *faderValue,
        int *valueBarValue,
        double *_pan,
        std::string *panStr
    ) const {
        double volume = 0.0;
        double pan = 0.0;

        GetTrackReceiveUIVolPan(media_track, receive_index, &volume, &pan);
        *panStr = GetPan1String(pan);
        *_pan = pan;

        if (context->GetShiftChannelLeft()) {
            *faderValue = static_cast<int>(panToNormalized(pan) * 16383.0);
            *valueBarValue = static_cast<int>(volToNormalized(volume) * 127);
        } else {
            *faderValue = static_cast<int>(volToNormalized(volume) * 16383.0);
            *valueBarValue = static_cast<int>(panToNormalized(pan) * 127);
        }
    }

    std::string GetLine3Content(
        MediaTrack *media_track,
        const int receive_index
    ) const {
        const auto send_value = static_cast<DisplaySendValue>(settings->GetSendDisplayValueSend());

        switch (send_value) {
            case DISPLAY_SEND_VALUE_VOLUME:
                return GetVolumeString(GetTrackSendInfo_Value(
                    media_track,
                    SEND_MODE_RECEIVE,
                    receive_index,
                    "D_VOL"
                ));

            case DISPLAY_SEND_VALUE_PAN:
                return GetPan1String(GetTrackSendInfo_Value(
                    media_track,
                    SEND_MODE_RECEIVE,
                    receive_index,
                    "D_PAN"
                ));

            case DISPLAY_SEND_VALUE_MUTE:
                return DAW::GetTrackReceiveMute(media_track, receive_index)
                           ? "Muted"
                           : "";

            case DISPLAY_SEND_VALUE_PHASE:
                return DAW::GetTrackReceivePhase(media_track, receive_index)
                           ? "Phase Rev"
                           : "";

            case DISPLAY_SEND_VALUE_MONO:
                return DAW::GetTrackReceiveMono(media_track, receive_index)
                           ? "Mono"
                           : "Stereo";

            case DISPLAY_SEND_VALUE_SEND_MODE:
                return DAW::GetTrackSurfaceReceiveMode(media_track, receive_index);

            case DISPLAY_SEND_VALUE_AUTO_MODE:
                return DAW::GetTrackSurfaceReceiveAutoMode(media_track, receive_index);

            default:
                return "";
        }
    }

    std::string GetLine4Content(
        const int slot_index,
        const int display_index
    ) const {
        if (nb_track_items[display_index] == 0) {
            return "";
        }
        return Progress(slot_index + 1, nb_track_items[display_index]);
    }

public:
    CSurf_FP_8_ReceivesManager(
        const std::vector<CSurf_FP_8_Track *> &tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout) {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerType(Hui);
        CSurf_FP_8_ReceivesManager::UpdateTracks(true);
    }

    ~CSurf_FP_8_ReceivesManager() override {
    }

    void UpdateTracks(bool force_update) override {
        nb_receives = 0;
        const WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *add_receive_track;

        if (has_last_touched_fx_enabled != context->GetLastTouchedFxMode()) {
            force_update = true;
        }

        for (int i = 0; i < context->GetNbChannels(); i++) {
            MediaTrack *media_track = media_tracks.Get(i);
            const int _nbTrackReceives = GetTrackNumSends(media_track, -1);
            nb_track_items[i] = _nbTrackReceives;

            if (_nbTrackReceives > nb_receives) {
                nb_receives = _nbTrackReceives;
            }
        }

        context->SetChannelManagerItemsCount(nb_receives);
        current_receive = context->GetChannelManagerItemIndex();

        for (int i = 0; i < context->GetNbChannels(); i++) {
            const int receive_index = context->GetChannelManagerItemIndex(nb_track_items[i] - 1);
            const bool add_receive_enabled = context->GetAddSendReceiveMode() == i;

            if (add_receive_enabled) {
                add_receive_track = GetTrack(nullptr, context->GetCurrentSelectedSendReceive());
            }

            int faderValue = 0;
            int valueBarValue = 0;
            double pan = 0.0;
            std::string panStr;

            const CSurf_FP_8_Track *faderport_channel = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);

            if (media_track == nullptr) {
                faderport_channel->ClearTrack(true, force_update);
                continue;
            }

            SetTrackColors(media_track, DAW::IsTrackSelected(media_track), false);

            GetFaderValue(media_track, receive_index, &faderValue, &valueBarValue, &pan, &panStr);

            // Handle the displays
            faderport_channel->SetDisplayMode(DISPLAY_MODE_2, force_update);
            faderport_channel->SetDisplayLine(
                DISPLAY_LINE_1,
                ALIGN_CENTER,
                DAW::GetTrackName(media_track).c_str(),
                NON_INVERT,
                force_update
            );

            if (add_receive_enabled) {
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_2,
                    ALIGN_LEFT,
                    ("Trk: " + DAW::GetTrackIndex(add_receive_track)).c_str(),
                    INVERT,
                    force_update
                );
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_3,
                    ALIGN_CENTER,
                    DAW::GetTrackName(add_receive_track).c_str(),
                    INVERT,
                    force_update
                );
            } else if (DAW::HasTrackReceive(media_track, receive_index)) {
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_2,
                    ALIGN_LEFT,
                    DAW::GetTrackReceiveSrcName(media_track, receive_index).c_str(),
                    INVERT,
                    force_update
                );
                faderport_channel->SetDisplayLine(
                    DISPLAY_LINE_3,
                    ALIGN_CENTER,
                    GetLine3Content(media_track, receive_index).c_str(),
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
                GetLine4Content(receive_index, i).c_str(),
                NON_INVERT,
                force_update
            );

            if (DAW::HasTrackReceive(media_track, receive_index)) {
                faderport_channel->SetFaderValue(faderValue, force_update);
                faderport_channel->SetValueBarMode(
                    context->GetShiftChannelLeft()
                        ? VALUEBAR_MODE_FILL
                        : VALUEBAR_MODE_BIPOLAR
                );
                faderport_channel->SetValueBarValue(valueBarValue);
            } else {
                faderport_channel->SetFaderValue(0, force_update);
                faderport_channel->SetValueBarMode(VALUEBAR_MODE_FILL);
                faderport_channel->SetValueBarValue(0);
            }

            faderport_channel->SetTrackColor(color, force_update);
            faderport_channel->SetSelectButtonValue(BTN_VALUE_ON, force_update);
            faderport_channel->SetMuteButtonValue(
                ButtonBlinkOnOff(
                    context->GetShiftChannelLeft() && DAW::GetTrackReceiveMute(media_track, receive_index),
                    DAW::GetTrackReceiveMute(media_track, receive_index),
                    settings->GetDistractionFreeMode()),
                force_update);
            faderport_channel->SetSoloButtonValue(
                (context->GetShiftChannelLeft() && DAW::GetTrackReceiveMono(media_track, receive_index))
                || (!context->GetShiftChannelLeft() && DAW::GetTrackReceivePhase(media_track, receive_index))
                    ? BTN_VALUE_ON
                    : BTN_VALUE_OFF,
                force_update);
        }

        has_last_touched_fx_enabled = context->GetLastTouchedFxMode();
    }

    void HandleSelectClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        /**
         * Set add_send_mode when right shift and select are engaged.
         * If add_send_mode is set, regardless the shift keys, we will reset the add_send_mode
         *
         */
        if (context->GetShiftChannelLeft()) {
            if (context->GetAddSendReceiveMode() == -1) {
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
            const int receive_index = context->GetChannelManagerItemIndex(
                GetTrackNumSends(media_track, SEND_MODE_RECEIVE) - 1
            );
            RemoveTrackSend(media_track, SEND_MODE_RECEIVE, receive_index);
            return;
        }

        DAW::SetUniqueSelectedTrack(media_track);
    }

    void HandleMuteClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        const int receive_index = context->GetChannelManagerItemIndex(nb_track_items[index] - 1);

        if (context->GetShiftChannelLeft()) {
            DAW::SetNextTrackReceiveMode(media_track, receive_index);
        } else {
            DAW::ToggleTrackReceiveMute(media_track, receive_index);
        }
    }

    void HandleSoloClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        const int receive_index = context->GetChannelManagerItemIndex(nb_track_items[index] - 1);

        if (context->GetShiftChannelLeft()) {
            DAW::ToggleTrackReceiveMono(media_track, receive_index);
        } else {
            DAW::ToggleTrackReceivePhase(media_track, receive_index);
        }
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        const int receive_index = context->GetChannelManagerItemIndex(nb_track_items[index] - 1);

        if (context->GetShiftChannelLeft()) {
            DAW::SetTrackReceivePan(media_track, receive_index, normalizedToPan(int14ToNormalized(msb, lsb)));
        } else {
            DAW::SetTrackReceiveVolume(media_track, receive_index, int14ToVol(msb, lsb));
        }
    }
};

#endif
