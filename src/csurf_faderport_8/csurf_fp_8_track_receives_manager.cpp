#ifndef CSURF_FP_8_TRACK_RECEIVES_MANAGER_C_
#define CSURF_FP_8_TRACK_RECEIVES_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"

class CSurf_FP_8_TrackReceivesManager : public CSurf_FP_8_ChannelManager {
protected:
    void GetFaderValue(
        MediaTrack *media_track,
        const int receive_index,
        int *fader_value,
        int *value_bar_value,
        double *_pan,
        std::string *pan_str
    ) const {
        double volume = 0.0;
        double pan = 0.0;

        GetTrackReceiveUIVolPan(media_track, receive_index, &volume, &pan);
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
    CSurf_FP_8_TrackReceivesManager(
        const std::vector<CSurf_FP_8_Track *> &tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout) {
        context->ResetChannelManagerItemIndex();
        context->ResetChannelManagerItemsCount();
        context->SetChannelManagerType(Track);

        CSurf_FP_8_TrackReceivesManager::UpdateTracks(true);
    }

    ~CSurf_FP_8_TrackReceivesManager() override {
    }

    void UpdateTracks(const bool force_update) override {
        const WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *receives_track = GetSelectedTrack(nullptr, 0);
        MediaTrack *add_receive_track;
        context->SetChannelManagerItemsCount(GetTrackNumSends(receives_track, -1));

        for (int i = 0; i < context->GetNbChannels(); i++) {
            const int receive_index = context->GetChannelManagerItemIndex() + i;
            const bool add_receive_enabled = context->GetAddSendReceiveMode() == i;
            if (add_receive_enabled) {
                add_receive_track = GetTrack(nullptr, context->GetCurrentSelectedSendReceive());
            }

            int fader_value, value_bar_value = 0;
            double pan = 0.0;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track, DAW::IsTrackSelected(media_track));

            std::string pan_str;
            GetFaderValue(receives_track, receive_index, &fader_value, &value_bar_value, &pan, &pan_str);

            if (!media_track) {
                track->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT, force_update);
            } else {
                track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str(),
                                      receives_track == media_track ? INVERT : NON_INVERT, force_update);
            }

            if (DAW::HasTrackReceive(receives_track, receive_index)) {
                if (add_receive_enabled) {
                    track->SetDisplayLine(
                        1,
                        ALIGN_LEFT,
                        ("Trk: " + DAW::GetTrackIndex(add_receive_track)).c_str(),
                        INVERT,
                        force_update
                    );
                    track->SetDisplayLine(
                        2,
                        ALIGN_CENTER,
                        DAW::GetTrackName(add_receive_track).c_str(),
                        INVERT,
                        force_update
                    );
                } else {
                    track->SetDisplayLine(
                        1,
                        ALIGN_LEFT,
                        DAW::GetTrackReceiveSrcName(receives_track, receive_index).c_str(),
                        INVERT,
                        force_update
                    );
                    track->SetDisplayLine(
                        2,
                        ALIGN_CENTER,
                        DAW::GetTrackSurfaceReceiveMode(receives_track, receive_index).c_str(),
                        NON_INVERT,
                        force_update
                    );
                }
                track->SetDisplayLine(
                    3,
                    ALIGN_CENTER,
                    DAW::GetTrackSurfaceReceiveAutoMode(receives_track, receive_index).c_str(),
                    NON_INVERT,
                    force_update
                );
                track->SetFaderValue(fader_value, force_update);
                track->SetValueBarMode(context->GetShiftChannelLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
                track->SetValueBarValue(value_bar_value);
            } else {
                if (add_receive_enabled) {
                    track->SetDisplayLine(
                        1,
                        ALIGN_LEFT,
                        ("Trk: " + DAW::GetTrackIndex(add_receive_track)).c_str(),
                        INVERT,
                        force_update
                    );
                    track->SetDisplayLine(
                        2,
                        ALIGN_CENTER,
                        DAW::GetTrackName(add_receive_track).c_str(),
                        INVERT,
                        force_update
                    );
                } else {
                    track->SetDisplayLine(1, ALIGN_LEFT, "No Rcvs", INVERT, force_update);
                    track->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, force_update);
                }
                track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, force_update);
                track->SetFaderValue(0, force_update);
                track->SetValueBarMode(VALUEBAR_MODE_FILL);
                track->SetValueBarValue(0);
            }

            track->SetTrackColor(color);
            track->SetSelectButtonValue(BTN_VALUE_ON, force_update);
            track->SetMuteButtonValue(
                ButtonBlinkOnOff(
                    context->GetShiftChannelLeft() && DAW::GetTrackReceiveMute(receives_track, receive_index),
                    DAW::GetTrackReceiveMute(receives_track, receive_index),
                    settings->GetDistractionFreeMode()),
                force_update);
            track->SetSoloButtonValue(
                (context->GetShiftChannelLeft() && DAW::GetTrackReceiveMono(receives_track, receive_index))
                || (!context->GetShiftChannelLeft() && DAW::GetTrackReceivePhase(receives_track, receive_index))
                    ? BTN_VALUE_ON
                    : BTN_VALUE_OFF,
                force_update);

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
            RemoveTrackSend(selected_track, SEND_MODE_RECEIVE, context->GetChannelManagerItemIndex() + index);
            return;
        }

        DAW::SetUniqueSelectedTrack(media_track);
    }

    void HandleMuteClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *receives_track = GetSelectedTrack(nullptr, 0);
        const int receive_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft()) {
            DAW::SetNextTrackReceiveMode(receives_track, receive_index);
        } else {
            DAW::ToggleTrackReceiveMute(receives_track, receive_index);
        }
    }

    void HandleSoloClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *receives_track = GetSelectedTrack(nullptr, 0);
        const int receive_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft()) {
            DAW::ToggleTrackReceiveMono(receives_track, receive_index);
        } else {
            DAW::ToggleTrackReceivePhase(receives_track, receive_index);
        }
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        MediaTrack *receives_track = GetSelectedTrack(nullptr, 0);
        const int receive_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft()) {
            DAW::SetTrackReceivePan(receives_track, receive_index, normalizedToPan(int14ToNormalized(msb, lsb)));
        } else {
            DAW::SetTrackReceiveVolume(receives_track, receive_index, int14ToVol(msb, lsb));
        }
    }
};

#endif
