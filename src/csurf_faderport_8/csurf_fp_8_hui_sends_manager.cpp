#ifndef CSURF_FP_8_SENDS_MANAGER_C_
#define CSURF_FP_8_SENDS_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"

class CSurf_FP_8_SendsManager : public CSurf_FP_8_ChannelManager {
protected:
    int nb_sends = 0;
    int nb_track_sends[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int current_send = 0;

    void GetFaderValue(
        MediaTrack *media_track,
        const int send_index,
        int *fader_value,
        int *value_bar_value,
        int *_pan,
        std::string *pan_str
    ) const {
        double volume = 0.0;
        double pan = 0.0;

        GetTrackSendUIVolPan(media_track, send_index, &volume, &pan);
        *pan_str = GetPan1String(pan);
        *_pan = static_cast<int>(pan);

        if (context->GetShiftChannelLeft()) {
            *fader_value = static_cast<int>(panToNormalized(pan) * 16383.0);
            *value_bar_value = static_cast<int>(volToNormalized(volume) * 127);
        } else {
            *fader_value = static_cast<int>(volToNormalized(volume) * 16383.0);
            *value_bar_value = static_cast<int>(panToNormalized(pan) * 127);
        }
    }

public:
    CSurf_FP_8_SendsManager(
        const std::vector<CSurf_FP_8_Track *> &tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout) {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerType(Hui);
        context->SetAddSendReceiveMode(-1);
        CSurf_FP_8_SendsManager::UpdateTracks(true);
    }

    ~CSurf_FP_8_SendsManager() override {
    }

    void UpdateTracks(const bool force_update) override {
        nb_sends = 0;
        const WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *add_send_track;

        for (int i = 0; i < context->GetNbChannels(); i++) {
            MediaTrack *media_track = media_tracks.Get(i);
            const int _nb_track_sends = GetTrackNumSends(media_track, 0);
            nb_track_sends[i] = _nb_track_sends;

            if (_nb_track_sends > nb_sends) {
                nb_sends = _nb_track_sends;
            }
        }

        context->SetChannelManagerItemsCount(nb_sends);
        current_send = context->GetChannelManagerItemIndex();

        for (int i = 0; i < context->GetNbChannels(); i++) {
            const int send_index = context->GetChannelManagerItemIndex(nb_track_sends[i] - 1);
            const bool add_send_enabled = context->GetAddSendReceiveMode() == i;

            if (add_send_enabled) {
                add_send_track = GetTrack(nullptr, context->GetCurrentSelectedSendReceive());
            }

            int pan, fader_value, value_bar_value = 0;
            std::string pan_str;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);

            if (!media_track) {
                track->ClearTrack();
                continue;
            }

            SetTrackColors(media_track, DAW::IsTrackSelected(media_track));

            GetFaderValue(media_track, send_index, &fader_value, &value_bar_value, &pan, &pan_str);

            if (DAW::HasTrackSend(media_track, send_index)) {
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
                        DAW::GetTrackSendDestName(media_track, send_index).c_str(),
                        INVERT,
                        force_update
                    );
                    track->SetDisplayLine(
                        2,
                        ALIGN_CENTER,
                        DAW::GetTrackSurfaceSendMode(media_track, send_index).c_str(),
                        NON_INVERT,
                        force_update
                    );
                }
                track->SetDisplayLine(
                    3,
                    ALIGN_CENTER,
                    Progress(send_index + 1, nb_track_sends[i]).c_str(),
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

            track->SetTrackColor(color);
            track->SetSelectButtonValue(BTN_VALUE_ON, force_update);
            track->SetMuteButtonValue(
                ButtonBlinkOnOff(
                    context->GetShiftChannelLeft() && DAW::GetTrackSendMute(media_track, send_index),
                    DAW::GetTrackSendMute(media_track, send_index),
                    settings->GetDistractionFreeMode()),
                force_update);
            track->SetSoloButtonValue(
                (context->GetShiftChannelLeft() && DAW::GetTrackSendMono(media_track, send_index))
                || (!context->GetShiftChannelLeft() && DAW::GetTrackSendPhase(media_track, send_index))
                    ? BTN_VALUE_ON
                    : BTN_VALUE_OFF,
                force_update);

            track->SetDisplayMode(DISPLAY_MODE_2, force_update);
            track->SetDisplayLine(0, ALIGN_CENTER, DAW::GetTrackName(media_track).c_str(), NON_INVERT, force_update);
        }
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
            const int send_index = context->GetChannelManagerItemIndex(
                GetTrackNumSends(media_track, SEND_MODE_SEND) - 1
            );
            RemoveTrackSend(media_track, SEND_MODE_SEND, send_index);
            return;
        }

        DAW::SetUniqueSelectedTrack(media_track);
    }

    void HandleMuteClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        const int send_index = context->GetChannelManagerItemIndex(nb_track_sends[index] - 1);

        if (context->GetShiftChannelLeft()) {
            DAW::SetNextTrackSendMode(media_track, send_index);
        } else {
            DAW::ToggleTrackSendMute(media_track, send_index);
        }
    }

    void HandleSoloClick(const int index, const int value) override {
        if (value == 0) {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        const int send_index = context->GetChannelManagerItemIndex(nb_track_sends[index] - 1);

        if (context->GetShiftChannelLeft()) {
            DAW::ToggleTrackSendMono(media_track, send_index);
        } else {
            DAW::ToggleTrackSendPhase(media_track, send_index);
        }
    }

    void HandleFaderTouch(const int index, const int value) override {
        (void) index;
        (void) value;
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        const int send_index = context->GetChannelManagerItemIndex(nb_track_sends[index] - 1);

        if (context->GetShiftChannelLeft()) {
            DAW::SetTrackSendPan(media_track, send_index, normalizedToPan(int14ToNormalized(msb, lsb)));
        } else {
            DAW::SetTrackSendVolume(media_track, send_index, int14ToVol(msb, lsb));
        }
    }
};

#endif
