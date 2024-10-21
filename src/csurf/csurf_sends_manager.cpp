#ifndef CSURF_SENDS_MANAGER_C_
#define CSURF_SENDS_MANAGER_C_

#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_channel_manager.hpp"
#include "csurf_navigator.hpp"
#include <vector>
#include "csurf_utils.hpp"
#include "csurf_daw.hpp"

class CSurf_SendsManager : public CSurf_ChannelManager
{
protected:
    int nbSends = 0;
    int nbTrackSends[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int currentSend = 0;

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
    CSurf_SendsManager(
        std::vector<CSurf_Track *> tracks,
        CSurf_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerType(Hui);
        UpdateTracks();
    }
    ~CSurf_SendsManager() {};

    void UpdateTracks() override
    {
        nbSends = 0;
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            MediaTrack *media_track = media_tracks.Get(i);
            int _nbTrackSends = GetTrackNumSends(media_track, 0);
            nbTrackSends[i] = _nbTrackSends;

            if (_nbTrackSends > nbSends)
            {
                nbSends = _nbTrackSends;
            }
        }

        context->SetChannelManagerItemsCount(nbSends);
        currentSend = context->GetChannelManagerItemIndex();

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int sendIndex = context->GetChannelManagerItemIndex(nbTrackSends[i] - 1);

            int pan, faderValue, valueBarValue = 0;
            string panStr;

            CSurf_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track);

            GetFaderValue(media_track, sendIndex, &faderValue, &valueBarValue, &pan, &panStr);

            if (DAW::HasTrackSend(media_track, sendIndex))
            {
                track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackSendDestName(media_track, sendIndex).c_str(), INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackSurfaceSendMode(media_track, sendIndex).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, Progress(sendIndex + 1, nbTrackSends[i]).c_str());
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Sends", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
            }

            track->SetTrackColor(colorActive, colorDim);
            track->SetSelectButtonValue(DAW::IsTrackSelected(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue(ButtonBlinkOnOff((context->GetShiftLeft() && DAW::GetTrackSendMute(media_track, sendIndex)), DAW::GetTrackSendMute(media_track, sendIndex)));
            track->SetSoloButtonValue(((context->GetShiftLeft() && DAW::GetTrackSendMono(media_track, sendIndex)) || (!context->GetShiftLeft() && DAW::GetTrackSendPhase(media_track, sendIndex)))
                                          ? BTN_VALUE_ON
                                          : BTN_VALUE_OFF);
            track->SetFaderValue(faderValue);
            track->SetValueBarMode(context->GetShiftLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(valueBarValue);

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

    void HandleMuteClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int sendIndex = context->GetChannelManagerItemIndex(nbTrackSends[index] - 1);

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(media_track, 0, sendIndex, "I_SENDMODE", DAW::GetNextTrackSendMode(media_track, sendIndex));
        }
        else
        {
            SetTrackSendInfo_Value(media_track, 0, currentSend, "B_MUTE", !DAW::GetTrackSendMute(media_track, sendIndex));
        }
    }

    void HandleSoloClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int sendIndex = context->GetChannelManagerItemIndex(nbTrackSends[index] - 1);

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(media_track, 0, sendIndex, "B_MONO", !DAW::GetTrackSendMono(media_track, sendIndex));
        }
        else
        {
            SetTrackSendInfo_Value(media_track, 0, sendIndex, "B_PHASE", !DAW::GetTrackSendPhase(media_track, sendIndex));
        }
    }

    void HandleFaderTouch(int index) override
    {
        (void)index;
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int sendIndex = context->GetChannelManagerItemIndex(nbTrackSends[index] - 1);

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(media_track, 0, sendIndex, "D_PAN", CSurf_OnSendPanChange(media_track, sendIndex, normalizedToPan(int14ToNormalized(msb, lsb)), false));
        }
        else
        {
            SetTrackSendInfo_Value(media_track, 0, sendIndex, "D_VOL", CSurf_OnSendVolumeChange(media_track, sendIndex, int14ToVol(msb, lsb), false));
        }
    }
};

#endif