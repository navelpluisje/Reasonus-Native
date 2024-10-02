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
            ColorFromNative(trackColor, &red, &green, &blue);
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
            int flagsOut, pan, faderValue, valueBarValue = 0;

            CSurf_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track);

            string panStr;
            GetFaderValue(sends_track, sendIndex, &faderValue, &valueBarValue, &pan, &panStr);

            const char *trackName = GetTrackState(media_track, &flagsOut);

            bool sendMute = false;
            GetTrackSendUIMute(sends_track, sendIndex, &sendMute);

            int autoMode = (int)GetTrackSendInfo_Value(sends_track, 0, sendIndex, "I_AUTOMODE");
            int sendMode = (int)GetTrackSendInfo_Value(sends_track, 0, sendIndex, "I_SENDMODE");
            bool sendPhase = (bool)GetTrackSendInfo_Value(sends_track, 0, sendIndex, "B_PHASE");
            bool sendMono = (bool)GetTrackSendInfo_Value(sends_track, 0, sendIndex, "B_MONO");

            track->SetDisplayLine(0, ALIGN_LEFT, trackName, sends_track == media_track ? INVERT : NON_INVERT);

            const char *sendName = "";
            MediaTrack *destTrack = (MediaTrack *)GetSetTrackSendInfo(sends_track, 0, sendIndex, "P_DESTTRACK", 0);
            if (destTrack)
            {
                sendName = (const char *)GetSetMediaTrackInfo(destTrack, "P_NAME", NULL);
                // Because of the const an being a pointer we need to do this here
                track->SetDisplayLine(1, ALIGN_LEFT, sendName, INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, GetSendModeString(sendMode).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, GetAutomationString(autoMode).c_str());
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Sends", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
            }

            track->SetTrackColor(colorActive, colorDim);
            track->SetSelectButtonValue(hasBit(flagsOut, 1) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue((context->GetShiftLeft() && sendMute) ? BTN_VALUE_BLINK : sendMute ? BTN_VALUE_ON
                                                                                                         : BTN_VALUE_OFF);
            track->SetSoloButtonValue(((context->GetShiftLeft() && sendMono) || (!context->GetShiftLeft() && sendPhase)) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetFaderValue(faderValue);
            track->SetValueBarMode(context->GetShiftLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(valueBarValue);

            track->SetDisplayMode(DISPLAY_MODE_2);
        }
    }

    void HandleSelectClick(int index) override
    {
        int flagsOut = 0;
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        GetTrackState(media_track, &flagsOut);

        if (context->GetShiftLeft())
        {
            SetTrackSelected(media_track, !(hasBit(flagsOut, 1) == 1));
            return;
        }

        unSelectAllTracks();
        SetTrackSelected(media_track, true);
    }

    void HandleMuteClick(int index) override
    {
        MediaTrack *send_track = GetSelectedTrack(0, 0);
        int sendIndex = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftLeft())
        {
            int sendMode = GetTrackSendInfo_Value(send_track, 0, sendIndex, "I_SENDMODE");
            SetTrackSendInfo_Value(send_track, 0, sendIndex, "I_SENDMODE", sendModes[(sendMode + 1) % 4]);
        }
        else
        {
            bool sendMute = GetTrackSendInfo_Value(send_track, 0, sendIndex, "B_MUTE");
            SetTrackSendInfo_Value(send_track, 0, sendIndex, "B_MUTE", !sendMute);
        }
    }

    void HandleSoloClick(int index) override
    {
        MediaTrack *send_track = GetSelectedTrack(0, 0);
        int sendIndex = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftLeft())
        {
            bool sendMono = GetTrackSendInfo_Value(send_track, 0, sendIndex, "B_MONO");
            SetTrackSendInfo_Value(send_track, 0, sendIndex, "B_MONO", !sendMono);
        }
        else
        {
            bool sendMute = GetTrackSendInfo_Value(send_track, 0, sendIndex, "B_PHASE");
            SetTrackSendInfo_Value(send_track, 0, sendIndex, "B_PHASE", !sendMute);
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