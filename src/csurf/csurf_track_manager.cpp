#ifndef CSURF_TRACK_MANAGER_C_
#define CSURF_TRACK_MANAGER_C_

#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_channel_manager.hpp"
#include "csurf_navigator.hpp"
#include <vector>
#include "csurf_daw.hpp"
#include "csurf_utils.hpp"

class CSurf_TrackManager : public CSurf_ChannelManager
{
protected:
    bool hasLastTouchedFxEnabled = false;

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

    void GetFaderValue(MediaTrack *media_track, int *faderValue, int *valueBarValue, string *_pan1, string *_pan2)
    {
        int panMode = 0;
        double volume, pan1, pan2 = 0.0;

        GetTrackUIVolPan(media_track, &volume, &pan1);
        GetTrackUIPan(media_track, &pan1, &pan2, &panMode);
        *_pan1 = GetPanString(pan1, panMode);
        *_pan2 = GetWidthString(pan2, panMode);

        if (context->GetShiftLeft())
        {
            *faderValue = int(panToNormalized(pan1) * 16383.0);
            *valueBarValue = int(volToNormalized(volume) * 127);
        }
        else if (context->GetShiftRight() && panMode > 4)
        {
            *faderValue = int(panToNormalized(pan2) * 16383.0);
            *valueBarValue = int(volToNormalized(volume) * 127);
        }
        else
        {
            *faderValue = int(volToNormalized(volume) * 16383.0);
            *valueBarValue = int(panToNormalized(pan1) * 127);
        }
    }

public:
    CSurf_TrackManager(
        std::vector<CSurf_Track *> tracks,
        CSurf_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_ChannelManager(tracks, navigator, context, m_midiout)
    {
        UpdateTracks();
    }
    ~CSurf_TrackManager() {};

    void UpdateTracks() override
    {
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();

        if (hasLastTouchedFxEnabled != context->GetLastTouchedFxMode() && !context->GetLastTouchedFxMode())
        {
            forceUpdate = true;
        }

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int faderValue = 0, valueBarValue = 0;
            string strPan1, strPan2;

            CSurf_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track);

            bool isSelected = DAW::IsTrackSelected(media_track);
            bool isArmed = DAW::IsTrackArmed(media_track);

            GetFaderValue(media_track, &faderValue, &valueBarValue, &strPan1, &strPan2);
            Btn_Value selectValue = (context->GetArm() && isArmed) || (!context->GetArm() && isSelected) ? BTN_VALUE_ON
                                                                                                         : BTN_VALUE_OFF;

            track->SetTrackColor(colorActive, colorDim);
            // If the track is armed always blink as an indication it is armed
            track->SetSelectButtonValue((!context->GetArm() && isArmed) ? BTN_VALUE_BLINK : selectValue, forceUpdate);
            track->SetMuteButtonValue(DAW::IsTrackMuted(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
            track->SetSoloButtonValue(DAW::IsTrackSoloed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
            track->SetFaderValue(faderValue, forceUpdate);
            track->SetValueBarMode(context->GetShiftLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(valueBarValue);
            track->SetVuMeterValue(DAW::GetTrackSurfacePeakInfo(media_track));

            if (!context->GetShiftLeft())
            {
                track->SetDisplayMode(DISPLAY_MODE_8, forceUpdate);
                track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(1, ALIGN_CENTER, DAW::GetTrackIndex(media_track).c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(2, ALIGN_CENTER, context->GetPanPushMode() ? strPan1.c_str() : strPan2.c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(3, ALIGN_CENTER, string("").c_str(), NON_INVERT, forceUpdate);
            }
            else if (media_track)
            {
                track->SetDisplayMode(DISPLAY_MODE_2, forceUpdate);
                track->SetDisplayLine(0, ALIGN_CENTER, DAW::GetTrackName(media_track).c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(1, ALIGN_CENTER, DAW::GetTrackIndex(media_track).c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(2, ALIGN_CENTER, strPan1.c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(3, ALIGN_CENTER, strPan2.c_str(), NON_INVERT, forceUpdate);
            }
            else
            {
                track->SetDisplayMode(DISPLAY_MODE_2, forceUpdate);
                track->SetDisplayLine(0, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
                track->SetDisplayLine(1, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
                track->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
                track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
            }
        }

        hasLastTouchedFxEnabled = context->GetLastTouchedFxMode();
        forceUpdate = false;
    }

    void HandleSelectClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetArm())
        {
            CSurf_SetSurfaceRecArm(media_track, CSurf_OnRecArmChange(media_track, !DAW::IsTrackArmed(media_track)), NULL);
            return;
        }

        if (context->GetShiftLeft())
        {
            SetTrackSelected(media_track, !IsTrackSelected(media_track));
            return;
        }

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            MediaTrack *media_track = navigator->GetTrackByIndex(i);
            SetTrackSelected(media_track, false);
        }
        SetTrackSelected(media_track, true);
    }

    void HandleMuteClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, !DAW::IsTrackMuted(media_track)), NULL);
    }

    void HandleSoloClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, !DAW::IsTrackSoloed(media_track)), NULL);
    }

    void HandleFaderTouch() override {}

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetShiftLeft())
        {
            CSurf_SetSurfacePan(media_track, CSurf_OnPanChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false), NULL);
        }
        else if (context->GetShiftRight())
        {
            SetMediaTrackInfo_Value(media_track, "D_WIDTH", CSurf_OnWidthChange(media_track, normalizedToPan(int14ToNormalized(msb, lsb)), false));
        }
        else
        {
            CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), NULL);
        }
    }
};

#endif