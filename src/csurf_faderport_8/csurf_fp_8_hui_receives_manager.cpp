#ifndef CSURF_FP_8_RECEIVES_MANAGER_C_
#define CSURF_FP_8_RECEIVES_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"

class CSurf_FP_8_ReceivesManager : public CSurf_FP_8_ChannelManager {
protected:
    int nbReceives = 0;
    int nbTrackReceives[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int currentReceive = 0;
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
        nbReceives = 0;
        const WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *add_receive_track;

        if (has_last_touched_fx_enabled != context->GetLastTouchedFxMode()) {
            force_update = true;
        }

        for (int i = 0; i < context->GetNbChannels(); i++) {
            MediaTrack *media_track = media_tracks.Get(i);
            const int _nbTrackReceives = GetTrackNumSends(media_track, -1);
            nbTrackReceives[i] = _nbTrackReceives;

            if (_nbTrackReceives > nbReceives) {
                nbReceives = _nbTrackReceives;
            }
        }

        context->SetChannelManagerItemsCount(nbReceives);
        currentReceive = context->GetChannelManagerItemIndex();

        for (int i = 0; i < context->GetNbChannels(); i++) {
            const int receive_index = context->GetChannelManagerItemIndex(nbTrackReceives[i] - 1);
            const bool add_receive_enabled = context->GetAddSendReceiveMode() == i;

            if (add_receive_enabled) {
                add_receive_track = GetTrack(nullptr, context->GetCurrentSelectedSendReceive());
            }

            int faderValue = 0;
            int valueBarValue = 0;
            double pan = 0.0;
            std::string panStr;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);

            if (!media_track) {
                track->ClearTrack();
                continue;
            }

            SetTrackColors(media_track, DAW::IsTrackSelected(media_track));

            GetFaderValue(media_track, receive_index, &faderValue, &valueBarValue, &pan, &panStr);

            if (DAW::HasTrackReceive(media_track, receive_index)) {
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
                        DAW::GetTrackReceiveSrcName(media_track, receive_index).c_str(),
                        INVERT,
                        force_update
                    );
                    track->SetDisplayLine(
                        2,
                        ALIGN_CENTER,
                        DAW::GetTrackSurfaceReceiveMode(media_track, receive_index).c_str(),
                        NON_INVERT,
                        force_update
                    );
                }
                track->SetDisplayLine(
                    3,
                    ALIGN_CENTER,
                    Progress(receive_index + 1, nbTrackReceives[i]).c_str(),
                    NON_INVERT,
                    force_update
                );
                track->SetFaderValue(faderValue, force_update);
                track->SetValueBarMode(context->GetShiftChannelLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
                track->SetValueBarValue(valueBarValue);
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
                    context->GetShiftChannelLeft() && DAW::GetTrackReceiveMute(media_track, receive_index),
                    DAW::GetTrackReceiveMute(media_track, receive_index),
                    settings->GetDistractionFreeMode()),
                force_update);
            track->SetSoloButtonValue(
                (context->GetShiftChannelLeft() && DAW::GetTrackReceiveMono(media_track, receive_index))
                || (!context->GetShiftChannelLeft() && DAW::GetTrackReceivePhase(media_track, receive_index))
                    ? BTN_VALUE_ON
                    : BTN_VALUE_OFF,
                force_update);

            track->SetDisplayMode(DISPLAY_MODE_2, force_update);
            track->SetDisplayLine(0, ALIGN_CENTER, DAW::GetTrackName(media_track).c_str(), NON_INVERT, force_update);
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
        const int receive_index = context->GetChannelManagerItemIndex(nbTrackReceives[index] - 1);

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
        const int receive_index = context->GetChannelManagerItemIndex(nbTrackReceives[index] - 1);

        if (context->GetShiftChannelLeft()) {
            DAW::ToggleTrackReceiveMono(media_track, receive_index);
        } else {
            DAW::ToggleTrackReceivePhase(media_track, receive_index);
        }
    }

    void HandleFaderTouch(const int index, const int value) override {
        (void) index;
        (void) value;
    }

    void HandleFaderMove(const int index, const int msb, const int lsb) override {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        const int receive_index = context->GetChannelManagerItemIndex(nbTrackReceives[index] - 1);

        if (context->GetShiftChannelLeft()) {
            DAW::SetTrackReceivePan(media_track, receive_index, normalizedToPan(int14ToNormalized(msb, lsb)));
        } else {
            DAW::SetTrackReceiveVolume(media_track, receive_index, int14ToVol(msb, lsb));
        }
    }
};

#endif
