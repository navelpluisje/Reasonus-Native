#ifndef CSURF_FP_8_TRACK_SENDS_MANAGER_C_
#define CSURF_FP_8_TRACK_SENDS_MANAGER_C_

#include "../shared/csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_fp_8_track.hpp"
#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_navigator.hpp"
#include <vector>
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"

class CSurf_FP_8_TrackSendsManager : public CSurf_FP_8_ChannelManager
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

    void GetFaderValue(MediaTrack *media_track, int send_index, int *fader_value, int *value_bar_value, double *_pan, std::string *pan_str)
    {
        double volume, pan = 0.0;

        GetTrackSendUIVolPan(media_track, send_index, &volume, &pan);
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
    CSurf_FP_8_TrackSendsManager(
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
    ~CSurf_FP_8_TrackSendsManager() {};

    void UpdateTracks() override
    {
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *sends_track = GetSelectedTrack(0, 0);
        context->SetChannelManagerItemsCount(GetTrackNumSends(sends_track, 0));

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int send_index = context->GetChannelManagerItemIndex() + i;
            int fader_value, value_bar_value = 0;
            double pan = 0.0;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track);

            std::string pan_str;
            GetFaderValue(sends_track, send_index, &fader_value, &value_bar_value, &pan, &pan_str);

            if (!media_track)
            {
                track->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT);
            }
            else
            {
                track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str(), sends_track == media_track ? INVERT : NON_INVERT);
            }

            if (DAW::HasTrackSend(sends_track, send_index))
            {
                track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackSendDestName(sends_track, send_index).c_str(), INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackSurfaceSendMode(sends_track, send_index).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, DAW::GetTrackSurfaceSendAutoMode(sends_track, send_index).c_str());
                track->SetFaderValue(fader_value);
                track->SetValueBarMode(context->GetShiftChannelLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
                track->SetValueBarValue(value_bar_value);
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Sends", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
                track->SetFaderValue(0);
                track->SetValueBarMode(VALUEBAR_MODE_FILL);
                track->SetValueBarValue(0);
            }

            track->SetTrackColor(color);
            track->SetSelectButtonValue(DAW::IsTrackSelected(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue(
                ButtonBlinkOnOff(
                    (context->GetShiftChannelLeft() && DAW::GetTrackSendMute(sends_track, send_index)),
                    DAW::GetTrackSendMute(sends_track, send_index),
                    context->GetDistractionFreeMode()));
            track->SetSoloButtonValue(((context->GetShiftChannelLeft() && DAW::GetTrackSendMono(sends_track, send_index)) || (!context->GetShiftChannelLeft() && DAW::GetTrackSendPhase(sends_track, send_index)))
                                          ? BTN_VALUE_ON
                                          : BTN_VALUE_OFF);

            track->SetDisplayMode(DISPLAY_MODE_2);
        }
    }

    void HandleSelectClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetShiftChannelLeft())
        {
            DAW::ToggleSelectedTrack(media_track);
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

        MediaTrack *send_track = GetSelectedTrack(0, 0);
        int send_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft())
        {
            DAW::SetNextTrackSendMode(send_track, send_index);
        }
        else
        {
            DAW::ToggleTrackSendMute(send_track, send_index);
        }
    }

    void HandleSoloClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *send_track = GetSelectedTrack(0, 0);
        int send_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft())
        {
            DAW::ToggleTrackSendMono(send_track, send_index);
        }
        else
        {
            DAW::ToggleTrackSendPhase(send_track, send_index);
        }
    }

    void HandleFaderTouch(int index, int value) override
    {
        (void)index;
        (void)value;
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *send_track = GetSelectedTrack(0, 0);
        int send_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft())
        {
            DAW::SetTrackSendPan(send_track, send_index, normalizedToPan(int14ToNormalized(msb, lsb)));
        }
        else
        {
            DAW::SetTrackSendVolume(send_track, send_index, int14ToVol(msb, lsb));
        }
    }
};

#endif