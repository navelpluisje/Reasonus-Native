#ifndef CSURF_TRACK_SENDS_MANAGER_C_
#define CSURF_TRACK_SENDS_MANAGER_C_

#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_channel_manager.hpp"
#include "csurf_navigator.hpp"
#include <vector>
#include "csurf_utils.hpp"
#include "csurf_daw.hpp"

class CSurf_TrackSendsManager : public CSurf_ChannelManager
{
protected:
    int sendModes[3] = {0, 1, 3};

    void SetTrackColors(MediaTrack *media_track) override
    {
        int red = 0xff;
        int green = 0x00;
        int blue = 0x00;

        if (!context->GetArm())
        {
            int trackColor = GetTrackColor(media_track);
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
        colorActive.SetColor(red / 2, green / 2, blue / 2);
        colorDim.SetColor(red / 4, green / 4, blue / 4);
    }

    void GetFaderValue(MediaTrack *media_track, int sendIndex, int *faderValue, int *valueBarValue, int *_pan, string *panStr)
    {
        double volume, pan = 0.0;

        GetTrackSendUIVolPan(media_track, sendIndex, &volume, &pan);
        *panStr = GetPanString(pan, 0);
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
    CSurf_TrackSendsManager(
        std::vector<CSurf_Track *> tracks,
        CSurf_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->ResetChannelManagerItemsCount();
        context->SetChannelManagerType(Track);

        UpdateTracks();
    }
    ~CSurf_TrackSendsManager() {};

    void UpdateTracks() override
    {
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *sends_track = GetSelectedTrack(0, 0);
        context->SetChannelManagerItemsCount(GetTrackNumSends(sends_track, 0));

        for (int i = 0; i < navigator->GetTrackCount(); i++)
        {
            int sendIndex = context->GetChannelManagerItemIndex() + i;
            int pan, faderValue, valueBarValue = 0;

            CSurf_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track);

            string panStr;
            GetFaderValue(sends_track, sendIndex, &faderValue, &valueBarValue, &pan, &panStr);

            track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str(), sends_track == media_track ? INVERT : NON_INVERT);

            if (DAW::HasTrackSend(sends_track, sendIndex))
            {
                track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackSendDestName(sends_track, sendIndex).c_str(), INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackSurfaceSendMode(sends_track, sendIndex).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, DAW::GetTrackSurfaceSendAutoMode(sends_track, sendIndex).c_str());
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Sends", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
            }

            track->SetTrackColor(colorActive, colorDim);
            track->SetSelectButtonValue(DAW::IsTrackSelected(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue((context->GetShiftLeft() && DAW::GetTrackSendMute(sends_track, sendIndex))
                                          ? BTN_VALUE_BLINK
                                      : DAW::GetTrackSendMute(sends_track, sendIndex) ? BTN_VALUE_ON
                                                                                      : BTN_VALUE_OFF);
            track->SetSoloButtonValue(((context->GetShiftLeft() && DAW::GetTrackSendMono(sends_track, sendIndex)) || (!context->GetShiftLeft() && DAW::GetTrackSendPhase(sends_track, sendIndex)))
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

    void HandleMuteClick(int index) override
    {
        MediaTrack *send_track = GetSelectedTrack(0, 0);
        int sendIndex = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(send_track, 0, sendIndex, "I_SENDMODE", DAW::GetNextTrackSendMode(send_track, sendIndex));
        }
        else
        {
            SetTrackSendInfo_Value(send_track, 0, sendIndex, "B_MUTE", !DAW::GetTrackSendMute(send_track, sendIndex));
        }
    }

    void HandleSoloClick(int index) override
    {
        MediaTrack *send_track = GetSelectedTrack(0, 0);
        int sendIndex = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(send_track, 0, sendIndex, "B_MONO", !DAW::GetTrackSendMono(send_track, sendIndex));
        }
        else
        {
            SetTrackSendInfo_Value(send_track, 0, sendIndex, "B_PHASE", !DAW::GetTrackSendPhase(send_track, sendIndex));
        }
    }

    void HandleFaderTouch() override
    {
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *send_track = GetSelectedTrack(0, 0);
        int sendIndex = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(send_track, 0, sendIndex, "D_PAN", CSurf_OnSendPanChange(send_track, sendIndex, normalizedToPan(int14ToNormalized(msb, lsb)), false));
        }
        else
        {
            SetTrackSendInfo_Value(send_track, 0, sendIndex, "D_VOL", CSurf_OnSendVolumeChange(send_track, sendIndex, int14ToVol(msb, lsb), false));
        }
    }
};

#endif