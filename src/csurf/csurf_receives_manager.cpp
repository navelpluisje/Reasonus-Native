#ifndef CSURF_RECEIVES_MANAGER_C_
#define CSURF_RECEIVES_MANAGER_C_

#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_utils.hpp"
#include "csurf_channel_manager.hpp"
#include "csurf_navigator.hpp"
#include <vector>
#include "csurf_daw.hpp"

class CSurf_ReceivesManager : public CSurf_ChannelManager
{
protected:
    int nbReceives = 0;
    int nbTrackReceives[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int currentReceive = 0;
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

    void GetFaderValue(MediaTrack *media_track, int receiveIndex, int *faderValue, int *valueBarValue, int *_pan, string *panStr)
    {
        double volume, pan = 0.0;

        GetTrackReceiveUIVolPan(media_track, receiveIndex, &volume, &pan);
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
    CSurf_ReceivesManager(
        std::vector<CSurf_Track *> tracks,
        CSurf_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerType(Hui);
        UpdateTracks();
    }
    ~CSurf_ReceivesManager() {};

    void UpdateTracks() override
    {
        nbReceives = 0;
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();

        for (int i = 0; i < navigator->GetTrackCount(); i++)
        {
            MediaTrack *media_track = media_tracks.Get(i);
            int _nbTrackReceives = GetTrackNumSends(media_track, -1);
            nbTrackReceives[i] = _nbTrackReceives;

            if (_nbTrackReceives > nbReceives)
            {
                nbReceives = _nbTrackReceives;
            }
        }

        context->SetChannelManagerItemsCount(nbReceives);
        currentReceive = context->GetChannelManagerItemIndex();

        if (nbReceives < context->GetChannelManagerItemIndex())
        {
            currentReceive = nbReceives;
        }

        for (int i = 0; i < navigator->GetTrackCount(); i++)
        {
            int receiveIndex = nbTrackReceives[i] < currentReceive ? nbTrackReceives[i] : currentReceive;

            CSurf_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track);

            int pan, faderValue, valueBarValue = 0;
            string panStr;
            GetFaderValue(media_track, receiveIndex, &faderValue, &valueBarValue, &pan, &panStr);

            int flagsOut;
            const char *trackName = GetTrackState(media_track, &flagsOut);

            bool receiveMute = false;
            GetTrackReceiveUIMute(media_track, receiveIndex, &receiveMute);

            int autoMode = (int)GetTrackSendInfo_Value(media_track, -1, receiveIndex, "I_AUTOMODE");
            int sendMode = (int)GetTrackSendInfo_Value(media_track, -1, receiveIndex, "I_SENDMODE");
            bool sendPhase = (bool)GetTrackSendInfo_Value(media_track, -1, receiveIndex, "B_PHASE");
            bool sendMono = (bool)GetTrackSendInfo_Value(media_track, -1, receiveIndex, "B_MONO");

            const char *receiveName = "";
            MediaTrack *destTrack = (MediaTrack *)GetSetTrackSendInfo(media_track, -1, receiveIndex, "P_DESTTRACK", 0);
            if (destTrack)
            {
                receiveName = (const char *)GetSetMediaTrackInfo(destTrack, "P_NAME", NULL);
                // Because of the const an being a pointer we need to do this here
                track->SetDisplayLine(1, ALIGN_LEFT, receiveName, INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, GetSendModeString(sendMode).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, GetAutomationString(autoMode).c_str());
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Rcvs", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
            }

            track->SetTrackColor(colorActive, colorDim);
            track->SetSelectButtonValue(hasBit(flagsOut, 1) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue((context->GetShiftLeft() && receiveMute) ? BTN_VALUE_BLINK : receiveMute ? BTN_VALUE_ON
                                                                                                               : BTN_VALUE_OFF);
            track->SetSoloButtonValue(((context->GetShiftLeft() && sendMono) || (!context->GetShiftLeft() && sendPhase)) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetFaderValue(faderValue);
            track->SetValueBarMode(context->GetShiftLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(valueBarValue);

            track->SetDisplayMode(DISPLAY_MODE_2);
            track->SetDisplayLine(0, ALIGN_CENTER, trackName);
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
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        if (context->GetShiftLeft())
        {
            int sendMode = GetTrackSendInfo_Value(media_track, -1, currentReceive, "I_SENDMODE");
            SetTrackSendInfo_Value(media_track, 0, currentReceive, "I_SENDMODE", sendModes[(sendMode + 1) % 4]);
        }
        else
        {
            bool receiveMute = GetTrackSendInfo_Value(media_track, -1, currentReceive, "B_MUTE");
            SetTrackSendInfo_Value(media_track, 0, currentReceive, "B_MUTE", !receiveMute);
        }
    }

    void HandleSoloClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        if (context->GetShiftLeft())
        {
            bool sendMono = GetTrackSendInfo_Value(media_track, -1, currentReceive, "B_MONO");
            SetTrackSendInfo_Value(media_track, 0, currentReceive, "B_MONO", !sendMono);
        }
        else
        {
            bool receiveMute = GetTrackSendInfo_Value(media_track, -1, currentReceive, "B_PHASE");
            SetTrackSendInfo_Value(media_track, 0, currentReceive, "B_PHASE", !receiveMute);
        }
    }

    void HandleFaderTouch() override
    {
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(media_track, -1, currentReceive, "D_PAN", CSurf_OnRecvPanChange(media_track, currentReceive, normalizedToPan(int14ToNormalized(msb, lsb)), false));
        }
        else
        {
            SetTrackSendInfo_Value(media_track, -1, currentReceive, "D_VOL", CSurf_OnRecvVolumeChange(media_track, currentReceive, int14ToVol(msb, lsb), false));
        }
    }
};

#endif