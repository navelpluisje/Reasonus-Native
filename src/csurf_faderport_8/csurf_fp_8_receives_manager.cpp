#ifndef CSURF_FP_8_RECEIVES_MANAGER_C_
#define CSURF_FP_8_RECEIVES_MANAGER_C_

#include "../shared/csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_fp_8_track.hpp"
#include "../shared/csurf_utils.hpp"
#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_navigator.hpp"
#include <vector>
#include "../shared/csurf_daw.hpp"

class CSurf_FP_8_ReceivesManager : public CSurf_FP_8_ChannelManager
{
protected:
    int nbReceives = 0;
    int nbTrackReceives[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int currentReceive = 0;

    void SetTrackColors(MediaTrack *media_track) override
    {
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

    void GetFaderValue(MediaTrack *media_track, int receive_index, int *faderValue, int *valueBarValue, double *_pan, std::string *panStr)
    {
        double volume, pan = 0.0;

        GetTrackReceiveUIVolPan(media_track, receive_index, &volume, &pan);
        *panStr = GetPan1String(pan);
        *_pan = pan;

        if (context->GetShiftChannelLeft())
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
    CSurf_FP_8_ReceivesManager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerType(Hui);
        UpdateTracks();
    }
    ~CSurf_FP_8_ReceivesManager() {};

    void UpdateTracks() override
    {
        nbReceives = 0;
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();

        for (int i = 0; i < context->GetNbChannels(); i++)
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

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int receive_index = context->GetChannelManagerItemIndex(nbTrackReceives[i] - 1);

            int faderValue, valueBarValue = 0;
            double pan = 0.0;
            std::string panStr;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            if (!media_track)
            {
                track->ClearTrack();
                continue;
            }
            SetTrackColors(media_track);

            GetFaderValue(media_track, receive_index, &faderValue, &valueBarValue, &pan, &panStr);

            if (DAW::HasTrackReceive(media_track, receive_index))
            {
                track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackReceiveSrcName(media_track, receive_index).c_str(), INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackSurfaceReceiveMode(media_track, receive_index).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, Progress(receive_index + 1, nbTrackReceives[i]).c_str());
                track->SetFaderValue(faderValue);
                track->SetValueBarMode(context->GetShiftChannelLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
                track->SetValueBarValue(valueBarValue);
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Rcvs", INVERT);
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
                    (context->GetShiftChannelLeft() && DAW::GetTrackReceiveMute(media_track, receive_index)),
                    DAW::GetTrackReceiveMute(media_track, receive_index),
                    context->GetDistractionFreeMode()));
            track->SetSoloButtonValue(((context->GetShiftChannelLeft() && DAW::GetTrackReceiveMono(media_track, receive_index)) || (!context->GetShiftChannelLeft() && DAW::GetTrackReceivePhase(media_track, receive_index)))
                                          ? BTN_VALUE_ON
                                          : BTN_VALUE_OFF);

            track->SetDisplayMode(DISPLAY_MODE_2);
            track->SetDisplayLine(0, ALIGN_CENTER, DAW::GetTrackName(media_track).c_str());
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

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int receive_index = context->GetChannelManagerItemIndex(nbTrackReceives[index] - 1);

        if (context->GetShiftChannelLeft())
        {
            DAW::SetNextTrackReceiveMode(media_track, receive_index);
        }
        else
        {
            DAW::ToggleTrackReceiveMute(media_track, receive_index);
        }
    }

    void HandleSoloClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int receive_index = context->GetChannelManagerItemIndex(nbTrackReceives[index] - 1);

        if (context->GetShiftChannelLeft())
        {
            DAW::ToggleTrackReceiveMono(media_track, receive_index);
        }
        else
        {
            DAW::ToggleTrackReceivePhase(media_track, receive_index);
        }
    }

    void HandleFaderTouch(int index, int value) override
    {
        (void)index;
        (void)value;
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int receive_index = context->GetChannelManagerItemIndex(nbTrackReceives[index] - 1);

        if (context->GetShiftChannelLeft())
        {
            DAW::SetTrackReceivePan(media_track, receive_index, normalizedToPan(int14ToNormalized(msb, lsb)));
        }
        else
        {
            DAW::SetTrackReceiveVolume(media_track, receive_index, int14ToVol(msb, lsb));
        }
    }
};

#endif