#ifndef CSURF_FP_8_SENDS_MANAGER_C_
#define CSURF_FP_8_SENDS_MANAGER_C_

#include "../shared/csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_fp_8_track.hpp"
#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_navigator.hpp"
#include <vector>
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"

class CSurf_FP_8_SendsManager : public CSurf_FP_8_ChannelManager
{
protected:
    int nb_sends = 0;
    int nb_track_sends[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int current_send = 0;

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
            int trackColor = ::GetTrackColor(media_track);
            if (trackColor == 0)
            {
                red = 0x7f;
                green = 0x7f;
                blue = 0x7f;
            }
            else
            {
                ColorFromNative(trackColor, &red, &green, &blue);
            }
        }
        color.SetColor(red / 2, green / 2, blue / 2);
    }

    void GetFaderValue(MediaTrack *media_track, int send_index, int *fader_value, int *value_bar_value, int *_pan, std::string *panStr)
    {
        double volume, pan = 0.0;

        GetTrackSendUIVolPan(media_track, send_index, &volume, &pan);
        *panStr = GetPanString(pan);
        *_pan = (int)pan;

        if (context->GetShiftLeft())
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
    CSurf_FP_8_SendsManager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerType(Hui);
        UpdateTracks();
    }
    ~CSurf_FP_8_SendsManager() {};

    void UpdateTracks() override
    {
        nb_sends = 0;
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            MediaTrack *media_track = media_tracks.Get(i);

            int _nb_track_sends = GetTrackNumSends(media_track, 0);
            nb_track_sends[i] = _nb_track_sends;

            if (_nb_track_sends > nb_sends)
            {
                nb_sends = _nb_track_sends;
            }
        }

        context->SetChannelManagerItemsCount(nb_sends);
        current_send = context->GetChannelManagerItemIndex();

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int send_index = context->GetChannelManagerItemIndex(nb_track_sends[i] - 1);

            int pan, fader_value, value_bar_value = 0;
            std::string panStr;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);

            if (!media_track)
            {
                track->ClearTrack();
                continue;
            }

            SetTrackColors(media_track);

            GetFaderValue(media_track, send_index, &fader_value, &value_bar_value, &pan, &panStr);

            if (DAW::HasTrackSend(media_track, send_index))
            {
                track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackSendDestName(media_track, send_index).c_str(), INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackSurfaceSendMode(media_track, send_index).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, Progress(send_index + 1, nb_track_sends[i]).c_str());
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Sends", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
            }

            track->SetTrackColor(color);
            track->SetSelectButtonValue(DAW::IsTrackSelected(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue(ButtonBlinkOnOff((context->GetShiftLeft() && DAW::GetTrackSendMute(media_track, send_index)), DAW::GetTrackSendMute(media_track, send_index)));
            track->SetSoloButtonValue(((context->GetShiftLeft() && DAW::GetTrackSendMono(media_track, send_index)) || (!context->GetShiftLeft() && DAW::GetTrackSendPhase(media_track, send_index)))
                                          ? BTN_VALUE_ON
                                          : BTN_VALUE_OFF);
            track->SetFaderValue(fader_value);
            track->SetValueBarMode(context->GetShiftLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(value_bar_value);

            track->SetDisplayMode(DISPLAY_MODE_2);
            track->SetDisplayLine(0, ALIGN_CENTER, DAW::GetTrackName(media_track).c_str());
        }
    }

    void HandleSelectClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetShiftLeft())
        {
            SetTrackSelected(media_track, !DAW::IsTrackSelected(media_track));
            return;
        }

        DAW::UnSelectAllTracks();
        SetTrackSelected(media_track, true);
    }

    void HandleMuteClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int send_index = context->GetChannelManagerItemIndex(nb_track_sends[index] - 1);

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(media_track, 0, send_index, "I_SENDMODE", DAW::GetNextTrackSendMode(media_track, send_index));
        }
        else
        {
            SetTrackSendInfo_Value(media_track, 0, current_send, "B_MUTE", !DAW::GetTrackSendMute(media_track, send_index));
        }
    }

    void HandleSoloClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int send_index = context->GetChannelManagerItemIndex(nb_track_sends[index] - 1);

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(media_track, 0, send_index, "B_MONO", !DAW::GetTrackSendMono(media_track, send_index));
        }
        else
        {
            SetTrackSendInfo_Value(media_track, 0, send_index, "B_PHASE", !DAW::GetTrackSendPhase(media_track, send_index));
        }
    }

    void HandleFaderTouch(int index) override
    {
        (void)index;
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int send_index = context->GetChannelManagerItemIndex(nb_track_sends[index] - 1);

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(media_track, 0, send_index, "D_PAN", CSurf_OnSendPanChange(media_track, send_index, normalizedToPan(int14ToNormalized(msb, lsb)), false));
        }
        else
        {
            SetTrackSendInfo_Value(media_track, 0, send_index, "D_VOL", CSurf_OnSendVolumeChange(media_track, send_index, int14ToVol(msb, lsb), false));
        }
    }
};

#endif