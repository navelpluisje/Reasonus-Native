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

        for (int i = 0; i < navigator->GetTrackCount(); i++)
        {
            int receiveIndex = context->GetChannelManagerItemIndex(nbTrackReceives[i] - 1);

            int pan, faderValue, valueBarValue = 0;
            string panStr;

            CSurf_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track);

            GetFaderValue(media_track, receiveIndex, &faderValue, &valueBarValue, &pan, &panStr);

            if (DAW::HasTrackReceive(media_track, receiveIndex))
            {
                track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackReceiveSrcName(media_track, receiveIndex).c_str(), INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackSurfaceReceiveMode(media_track, receiveIndex).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, Progress(receiveIndex + 1, nbTrackReceives[i]).c_str());
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Rcvs", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
            }

            track->SetTrackColor(colorActive, colorDim);
            track->SetSelectButtonValue(DAW::IsTrackSelected(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue((context->GetShiftLeft() && DAW::GetTrackReceiveMute(media_track, receiveIndex)) ? BTN_VALUE_BLINK
                                      : DAW::GetTrackReceiveMute(media_track, receiveIndex)                            ? BTN_VALUE_ON
                                                                                                                       : BTN_VALUE_OFF);
            track->SetSoloButtonValue(((context->GetShiftLeft() && DAW::GetTrackReceiveMono(media_track, receiveIndex)) || (!context->GetShiftLeft() && DAW::GetTrackReceivePhase(media_track, receiveIndex)))
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
        int flagsOut = 0;
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        GetTrackState(media_track, &flagsOut);

        if (context->GetShiftLeft())
        {
            SetTrackSelected(media_track, !(hasBit(flagsOut, 1) == 1));
            return;
        }

        DAW::UnSelectAllTracks();
        SetTrackSelected(media_track, true);
    }

    void HandleMuteClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int receiveIndex = context->GetChannelManagerItemIndex(nbTrackReceives[index] - 1);

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(media_track, -1, receiveIndex, "I_SENDMODE", DAW::GetNextTrackReceiveMode(media_track, receiveIndex));
        }
        else
        {
            SetTrackSendInfo_Value(media_track, -1, receiveIndex, "B_MUTE", !DAW::GetTrackReceiveMute(media_track, receiveIndex));
        }
    }

    void HandleSoloClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int receiveIndex = context->GetChannelManagerItemIndex(nbTrackReceives[index] - 1);

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(media_track, -1, receiveIndex, "B_MONO", !DAW::GetTrackReceiveMono(media_track, receiveIndex));
        }
        else
        {
            SetTrackSendInfo_Value(media_track, -1, receiveIndex, "B_PHASE", !DAW::GetTrackReceivePhase(media_track, receiveIndex));
        }
    }

    void HandleFaderTouch() override
    {
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int receiveIndex = context->GetChannelManagerItemIndex(nbTrackReceives[index] - 1);

        if (context->GetShiftLeft())
        {
            SetTrackSendInfo_Value(media_track, -1, receiveIndex, "D_PAN", CSurf_OnRecvPanChange(media_track, receiveIndex, normalizedToPan(int14ToNormalized(msb, lsb)), false));
        }
        else
        {
            SetTrackSendInfo_Value(media_track, -1, receiveIndex, "D_VOL", CSurf_OnRecvVolumeChange(media_track, receiveIndex, int14ToVol(msb, lsb), false));
        }
    }
};

#endif