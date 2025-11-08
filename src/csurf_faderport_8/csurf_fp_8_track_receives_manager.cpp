#ifndef CSURF_FP_8_TRACK_RECEIVES_MANAGER_C_
#define CSURF_FP_8_TRACK_RECEIVES_MANAGER_C_

#include "../shared/csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_fp_8_track.hpp"
#include "../shared/csurf_utils.hpp"
#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_navigator.hpp"
#include <vector>
#include "../shared/csurf_daw.hpp"

class CSurf_FP_8_TrackReceivesManager : public CSurf_FP_8_ChannelManager
{
protected:
    void SetTrackColors(MediaTrack *media_track) override
    {
        if (!media_track)
        {
            color.SetColor(ButtonColorWhite);
            return;
        }

        int red = 0xff;
        int green = 0x00;
        int blue = 0x00;

        if (!context->GetArm())
        {
            int track_color = ::GetTrackColor(media_track);
            if (track_color == 0)
            {
                red = 0x7f;
                green = 0x7f;
                blue = 0x7f;
            }
            else
            {
                ColorFromNative(track_color, &red, &green, &blue);
            }
        }
        color.SetColor(red / 2, green / 2, blue / 2);
    }

    void GetFaderValue(MediaTrack *media_track, int receive_index, int *fader_value, int *value_bar_value, double *_pan, std::string *pan_str)
    {
        double volume, pan = 0.0;

        GetTrackReceiveUIVolPan(media_track, receive_index, &volume, &pan);
        *pan_str = GetPan1String(pan);
        *_pan = pan;

        if (context->GetShiftChannelLeft())
        {
            *fader_value = int(panToNormalized(pan) * 16383.0);
            *value_bar_value = int(volToNormalized(volume) * 127);
        }
        else
        {
            *fader_value = int(volToNormalized(volume) * 16383.0);
            *value_bar_value = int(panToNormalized(pan) * 127);
        }
    }

public:
    CSurf_FP_8_TrackReceivesManager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->ResetChannelManagerItemsCount();
        context->SetChannelManagerType(Track);

        UpdateTracks();
    }
    ~CSurf_FP_8_TrackReceivesManager() {};

    void UpdateTracks() override
    {
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *receives_track = GetSelectedTrack(0, 0);
        MediaTrack *add_receive_track;
        context->SetChannelManagerItemsCount(GetTrackNumSends(receives_track, -1));

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int receive_index = context->GetChannelManagerItemIndex() + i;
            bool add_receive_enabled = context->GetAddSendReceiveMode() == i;
            if (add_receive_enabled)
            {
                add_receive_track = ::GetTrack(0, context->GetCurrentSelectedSendReceive());
            }

            int fader_value, value_bar_value = 0;
            double pan = 0.0;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track);

            std::string pan_str;
            GetFaderValue(receives_track, receive_index, &fader_value, &value_bar_value, &pan, &pan_str);

            if (!media_track)
            {
                track->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT);
            }
            else
            {
                track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str(), receives_track == media_track ? INVERT : NON_INVERT);
            }

            if (DAW::HasTrackReceive(receives_track, receive_index))
            {
                if (add_receive_enabled)
                {
                    track->SetDisplayLine(1, ALIGN_LEFT, ("Trk: " + DAW::GetTrackIndex(add_receive_track)).c_str(), INVERT);
                    track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackName(add_receive_track).c_str(), INVERT);
                }
                else
                {
                    track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackReceiveSrcName(receives_track, receive_index).c_str(), INVERT);
                    track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackSurfaceReceiveMode(receives_track, receive_index).c_str());
                }
                track->SetDisplayLine(3, ALIGN_CENTER, DAW::GetTrackSurfaceReceiveAutoMode(receives_track, receive_index).c_str());
                track->SetFaderValue(fader_value);
                track->SetValueBarMode(context->GetShiftChannelLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
                track->SetValueBarValue(value_bar_value);
            }
            else
            {
                if (add_receive_enabled)
                {
                    track->SetDisplayLine(1, ALIGN_LEFT, ("Trk: " + DAW::GetTrackIndex(add_receive_track)).c_str(), INVERT);
                    track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackName(add_receive_track).c_str(), INVERT);
                }
                else
                {
                    track->SetDisplayLine(1, ALIGN_LEFT, "No Rcvs", INVERT);
                    track->SetDisplayLine(2, ALIGN_CENTER, "");
                }
                track->SetDisplayLine(3, ALIGN_CENTER, "");
                track->SetFaderValue(0);
                track->SetValueBarMode(VALUEBAR_MODE_FILL);
                track->SetValueBarValue(0);
            }

            track->SetTrackColor(color);
            track->SetSelectButtonValue(DAW::IsTrackSelected(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue(
                ButtonBlinkOnOff(
                    (context->GetShiftChannelLeft() && DAW::GetTrackReceiveMute(receives_track, receive_index)),
                    DAW::GetTrackReceiveMute(receives_track, receive_index),
                    context->GetDistractionFreeMode()));
            track->SetSoloButtonValue(((context->GetShiftChannelLeft() && DAW::GetTrackReceiveMono(receives_track, receive_index)) || (!context->GetShiftChannelLeft() && DAW::GetTrackReceivePhase(receives_track, receive_index)))
                                          ? BTN_VALUE_ON
                                          : BTN_VALUE_OFF);

            track->SetDisplayMode(DISPLAY_MODE_2);
        }
    }

    void HandleSelectClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        /**
         * Set add_send_receive_mode when left shift and select are engaged.
         * If add_send_receive_mode is set, regardless the shift keys, we will reset the add_send_receive_mode
         */
        if (context->GetShiftChannelLeft())
        {
            if (context->GetAddSendReceiveMode() == -1)
            {
                DAW::SetUniqueSelectedTrack(media_track);
                context->SetCurrentSelectedSendReceive(0);
                context->SetAddSendReceiveMode(index);
            }
            else
            {
                context->SetAddSendReceiveMode(-1);
            }
            return;
        }

        if (context->GetAddSendReceiveMode() == index)
        {
            context->SetAddSendReceiveMode(-1);
        }

        if (context->GetShiftChannelRight())
        {
            MediaTrack *selected_track = ::GetSelectedTrack(0, 0);
            ::RemoveTrackSend(selected_track, SEND_MODE_RECEIVE, context->GetChannelManagerItemIndex() + index);
            return;
        }

        DAW::SetUniqueSelectedTrack(media_track);
    }

    void HandleMuteClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *receives_track = ::GetSelectedTrack(0, 0);
        int receive_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft())
        {
            DAW::SetNextTrackReceiveMode(receives_track, receive_index);
        }
        else
        {
            DAW::ToggleTrackReceiveMute(receives_track, receive_index);
        }
    }

    void HandleSoloClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *receives_track = ::GetSelectedTrack(0, 0);
        int receive_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft())
        {
            DAW::ToggleTrackReceiveMono(receives_track, receive_index);
        }
        else
        {
            DAW::ToggleTrackReceivePhase(receives_track, receive_index);
        }
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *receives_track = ::GetSelectedTrack(0, 0);
        int receive_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft())
        {
            DAW::SetTrackReceivePan(receives_track, receive_index, normalizedToPan(int14ToNormalized(msb, lsb)));
        }
        else
        {
            DAW::SetTrackReceiveVolume(receives_track, receive_index, int14ToVol(msb, lsb));
        }
    }
};

#endif