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
    int sendModes[3] = {0, 1, 3};

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

    void GetFaderValue(MediaTrack *media_track, int receiveIndex, int *faderValue, int *valueBarValue, double *_pan, std::string *panStr)
    {
        double volume, pan = 0.0;

        GetTrackReceiveUIVolPan(media_track, receiveIndex, &volume, &pan);
        *panStr = GetPanString(pan);
        *_pan = pan;

        if (context->GetShiftLeft())
        {
            *faderValue = int(panToNormalized(pan) * 16383.0);
            *valueBarValue = int(volToNormalized(volume) * 127);
        }
        else
        {
            *faderValue = int(volToNormalized(volume) * 16383.0);
            *valueBarValue = int(panToNormalized(pan) * 127);
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
        context->SetChannelManagerItemsCount(GetTrackNumSends(receives_track, -1));

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int receiveIndex = context->GetChannelManagerItemIndex() + i;
            int faderValue, valueBarValue = 0;
            double pan = 0.0;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track);

            std::string panStr;
            GetFaderValue(receives_track, receiveIndex, &faderValue, &valueBarValue, &pan, &panStr);

            if (!media_track)
            {
                track->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT);
            }
            else
            {
                track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str(), receives_track == media_track ? INVERT : NON_INVERT);
            }

            if (DAW::HasTrackReceive(receives_track, receiveIndex))
            {
                track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackReceiveSrcName(receives_track, receiveIndex).c_str(), INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackSurfaceReceiveMode(receives_track, receiveIndex).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, DAW::GetTrackSurfaceReceiveAutoMode(receives_track, receiveIndex).c_str());
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Rcvs", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
            }

            track->SetTrackColor(color);
            track->SetSelectButtonValue(DAW::IsTrackSelected(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue(ButtonBlinkOnOff((context->GetShiftLeft() && DAW::GetTrackReceiveMute(receives_track, receiveIndex)), DAW::GetTrackReceiveMute(receives_track, receiveIndex)));
            track->SetSoloButtonValue(((context->GetShiftLeft() && DAW::GetTrackReceiveMono(receives_track, receiveIndex)) || (!context->GetShiftLeft() && DAW::GetTrackReceivePhase(receives_track, receiveIndex)))
                                          ? BTN_VALUE_ON
                                          : BTN_VALUE_OFF);
            track->SetFaderValue(faderValue);
            track->SetValueBarMode(context->GetShiftLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(valueBarValue);

            track->SetDisplayMode(DISPLAY_MODE_2);
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

        MediaTrack *receives_track = GetSelectedTrack(0, 0);
        int receiveIndex = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(receives_track, -1, receiveIndex, "I_SENDMODE", DAW::GetNextTrackReceiveMode(receives_track, receiveIndex));
        }
        else
        {
            SetTrackSendInfo_Value(receives_track, -1, receiveIndex, "B_MUTE", !DAW::GetTrackReceiveMute(receives_track, receiveIndex));
        }
    }

    void HandleSoloClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *receives_track = GetSelectedTrack(0, 0);
        int receiveIndex = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(receives_track, -1, receiveIndex, "B_MONO", !DAW::GetTrackReceiveMono(receives_track, receiveIndex));
        }
        else
        {
            SetTrackSendInfo_Value(receives_track, -1, receiveIndex, "B_PHASE", !DAW::GetTrackReceivePhase(receives_track, receiveIndex));
        }
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *receives_track = GetSelectedTrack(0, 0);
        int receiveIndex = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(receives_track, -1, receiveIndex, "D_PAN", CSurf_OnRecvPanChange(receives_track, receiveIndex, normalizedToPan(int14ToNormalized(msb, lsb)), false));
        }
        else
        {
            SetTrackSendInfo_Value(receives_track, -1, receiveIndex, "D_VOL", CSurf_OnRecvVolumeChange(receives_track, receiveIndex, int14ToVol(msb, lsb), false));
        }
    }
};

#endif