#ifndef CSURF_FP_8_TRACK_SENDS_MANAGER_C_
#define CSURF_FP_8_TRACK_SENDS_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"

class CSurf_FP_8_TrackSendsManager : public CSurf_FP_8_ChannelManager {
protected:
    void GetFaderValue(
        MediaTrack *media_track,
        const int send_index,
        int *fader_value,
        int *value_bar_value,
        double *_pan,
        std::string *pan_str
    ) const {
        double volume = 0.0;
        double pan = 0.0;

        GetTrackSendUIVolPan(media_track, send_index, &volume, &pan);
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

    ~CSurf_FP_8_TrackSendsManager() override {
    }

    void UpdateTracks(const bool force_update) override {
        const WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *sends_track = GetSelectedTrack(nullptr, 0);
        MediaTrack *add_send_track;
        context->SetChannelManagerItemsCount(GetTrackNumSends(sends_track, 0));

        for (int i = 0; i < context->GetNbChannels(); i++) {
            const int send_index = context->GetChannelManagerItemIndex() + i;
            const bool add_send_enabled = context->GetAddSendReceiveMode() == i;
            if (add_send_enabled) {
                add_send_track = GetTrack(nullptr, context->GetCurrentSelectedSendReceive());
            }

            int fader_value = 0;
            int value_bar_value = 0;
            double pan = 0.0;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track, DAW::IsTrackSelected(media_track));

            std::string pan_str;
            GetFaderValue(sends_track, send_index, &fader_value, &value_bar_value, &pan, &pan_str);

            if (!media_track) {
                track->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT, force_update);
            } else {
                track->SetDisplayLine(
                    0,
                    ALIGN_LEFT,
                    DAW::GetTrackName(media_track).c_str(),
                    sends_track == media_track ? INVERT : NON_INVERT,
                    force_update
                );
            }

            if (DAW::HasTrackSend(sends_track, send_index)) {
                if (add_send_enabled) {
                    track->SetDisplayLine(
                        1,
                        ALIGN_LEFT,
                        ("Trk: " + DAW::GetTrackIndex(add_send_track)).c_str(),
                        INVERT,
                        force_update
                    );
                    track->SetDisplayLine(
                        2,
                        ALIGN_CENTER,
                        DAW::GetTrackName(add_send_track).c_str(),
                        INVERT,
                        force_update
                    );
                } else {
                    track->SetDisplayLine(
                        1,
                        ALIGN_LEFT,
                        DAW::GetTrackSendDestName(sends_track, send_index).c_str(),
                        INVERT,
                        force_update
                    );
                    track->SetDisplayLine(
                        2,
                        ALIGN_CENTER,
                        DAW::GetTrackSurfaceSendMode(sends_track, send_index).c_str(),
                        NON_INVERT,
                        force_update
                    );
                }
                track->SetDisplayLine(
                    3,
                    ALIGN_CENTER,
                    DAW::GetTrackSurfaceSendAutoMode(sends_track, send_index).c_str(),
                    NON_INVERT,
                    force_update
                );
                track->SetFaderValue(fader_value, force_update);
                track->SetValueBarMode(context->GetShiftChannelLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
                track->SetValueBarValue(value_bar_value);
            } else {
                if (add_send_enabled) {
                    track->SetDisplayLine(
                        1,
                        ALIGN_LEFT,
                        ("Trk: " + DAW::GetTrackIndex(add_send_track)).c_str(),
                        INVERT,
                        force_update
                    );
                    track->SetDisplayLine(
                        2,
                        ALIGN_CENTER,
                        DAW::GetTrackName(add_send_track).c_str(),
                        INVERT,
                        force_update
                    );
                } else {
                    track->SetDisplayLine(1, ALIGN_LEFT, "No Sends", INVERT, force_update);
                    track->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, force_update);
                }
                track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, force_update);
                track->SetFaderValue(0, force_update);
                track->SetValueBarMode(VALUEBAR_MODE_FILL);
                track->SetValueBarValue(0);
            }

            track->SetTrackColor(color, force_update);
            track->SetSelectButtonValue(BTN_VALUE_ON, force_update);
            track->SetMuteButtonValue(
                ButtonBlinkOnOff(
                    context->GetShiftChannelLeft() && DAW::GetTrackSendMute(sends_track, send_index),
                    DAW::GetTrackSendMute(sends_track, send_index),
                    settings->GetDistractionFreeMode()),
                force_update);
            track->SetSoloButtonValue(
                (context->GetShiftChannelLeft() && DAW::GetTrackSendMono(sends_track, send_index))
                || (!context->GetShiftChannelLeft() && DAW::GetTrackSendPhase(sends_track, send_index))
                    ? BTN_VALUE_ON
                    : BTN_VALUE_OFF,
                force_update
            );

            track->SetDisplayMode(DISPLAY_MODE_2, force_update);
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
            MediaTrack *selected_track = GetSelectedTrack(nullptr, 0);
            RemoveTrackSend(selected_track, SEND_MODE_SEND, context->GetChannelManagerItemIndex() + index);
            return;
        }

        DAW::SetUniqueSelectedTrack(media_track);
    }

    void HandleMuteClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *send_track = GetSelectedTrack(nullptr, 0);
        const int send_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft()) {
            DAW::SetNextTrackSendMode(send_track, send_index);
        } else {
            DAW::ToggleTrackSendMute(send_track, send_index);
        }
    }

    void HandleSoloClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *send_track = GetSelectedTrack(nullptr, 0);
        const int send_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft()) {
            DAW::ToggleTrackSendMono(send_track, send_index);
        } else {
            DAW::ToggleTrackSendPhase(send_track, send_index);
        }
    }

    void HandleFaderTouch(const int index, const int value) override {
        (void) index;
        (void) value;
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        MediaTrack *send_track = GetSelectedTrack(nullptr, 0);
        const int send_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft()) {
            DAW::SetTrackSendPan(send_track, send_index, normalizedToPan(int14ToNormalized(msb, lsb)));
        } else {
            DAW::SetTrackSendVolume(send_track, send_index, int14ToVol(msb, lsb));
        }
    }
};

#endif
