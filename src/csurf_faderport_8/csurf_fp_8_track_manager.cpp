#ifndef CSURF_FP_8_TRACK_MANAGER_C_
#define CSURF_FP_8_TRACK_MANAGER_C_

#include <WDL/ptrlist.h>
#include "csurf_fp_8_track.hpp"
#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_navigator.hpp"
#include <vector>
#include "../shared/csurf_daw.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_context.cpp"

const int MOMENTARY_TIMEOUT = 500;
class CSurf_FP_8_TrackManager : public CSurf_FP_8_ChannelManager
{
    int mute_start[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int solo_start[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

protected:
    bool has_hast_touched_fx_enabled = false;

    void SetTrackColors(MediaTrack *media_track) override
    {
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

    void GetFaderValue(MediaTrack *media_track, int *faderValue, int *valueBarValue, std::string *_pan1, std::string *_pan2)
    {
        int pan_mode = 0;
        double volume, pan1, pan2 = 0.0;

        GetTrackUIVolPan(media_track, &volume, &pan1);
        GetTrackUIPan(media_track, &pan1, &pan2, &pan_mode);
        *_pan1 = GetPanString(pan1);
        *_pan2 = GetWidthString(pan2, pan_mode);

        if (context->GetShiftLeft())
        {
            *faderValue = int(panToNormalized(pan1) * 16383.0);
            *valueBarValue = int(volToNormalized(volume) * 127);
        }
        else if (context->GetShiftRight() && pan_mode > 4)
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
    CSurf_FP_8_TrackManager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout)
    {
        UpdateTracks();
    }
    ~CSurf_FP_8_TrackManager() {}

    void UpdateTracks() override
    {
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        std::vector<std::string> time_code;

        if (has_hast_touched_fx_enabled != context->GetLastTouchedFxMode() && !context->GetLastTouchedFxMode())
        {
            forceUpdate = true;
        }

        if (context->GetShowTimeCode())
        {
            time_code = DAW::GetProjectTime(context->GetOverwriteTimeCode(), context->GetSurfaceTimeCode());
        }

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int fader_value = 0, value_bar_value = 0;
            std::string strPan1, strPan2;

            CSurf_FP_8_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);

            if (!media_track || CountTracks(0) < i)
            {
                track->ClearTrack();
                continue;
            }
            SetTrackColors(media_track);

            bool isSelected = DAW::IsTrackSelected(media_track);
            bool isArmed = DAW::IsTrackArmed(media_track);

            GetFaderValue(media_track, &fader_value, &value_bar_value, &strPan1, &strPan2);
            Btn_Value selectValue = (context->GetArm() && isArmed) || (!context->GetArm() && isSelected) ? BTN_VALUE_ON
                                                                                                         : BTN_VALUE_OFF;

            track->SetTrackColor(color);
            // If the track is armed always blink as an indication it is armed
            track->SetSelectButtonValue((!context->GetArm() && isArmed) ? BTN_VALUE_BLINK : selectValue, forceUpdate);
            track->SetMuteButtonValue(DAW::IsTrackMuted(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
            track->SetSoloButtonValue(DAW::IsTrackSoloed(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF, forceUpdate);
            track->SetFaderValue(fader_value, forceUpdate);
            track->SetValueBarMode((context->GetShiftLeft() || context->GetArm()) ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(value_bar_value);

            if (context->GetShiftLeft() || context->GetShiftRight())
            {
                track->SetDisplayMode(DISPLAY_MODE_2, forceUpdate);
                track->SetDisplayLine(0, ALIGN_CENTER, DAW::GetTrackName(media_track).c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(1, ALIGN_CENTER, DAW::GetTrackIndex(media_track).c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(2, ALIGN_CENTER, strPan1.c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(3, ALIGN_CENTER, strPan2.c_str(), NON_INVERT, forceUpdate);
            }
            else if (context->GetArm() && isArmed)
            {
                track->SetDisplayMode(DISPLAY_MODE_2, forceUpdate);
                track->SetDisplayLine(0, ALIGN_CENTER, DAW::GetTrackName(media_track).c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(1, ALIGN_CENTER, DAW::GetTrackInputName(media_track).c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackMonitorMode(media_track).c_str(), NON_INVERT, forceUpdate);
                track->SetDisplayLine(3, ALIGN_CENTER, DAW::GetTrackRecordingMode(media_track).c_str(), NON_INVERT, forceUpdate);
            }
            else
            {
                track->SetVuMeterValue(DAW::GetTrackSurfacePeakInfo(media_track));
                int index = context->GetNbChannels() - (static_cast<int>(time_code.size()) + i);

                if (index < 1)
                {
                    track->SetDisplayMode(DISPLAY_MODE_0, forceUpdate);
                    track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str(), NON_INVERT, forceUpdate);
                    track->SetDisplayLine(1, ALIGN_CENTER, DAW::GetTrackIndex(media_track).c_str(), NON_INVERT, forceUpdate);
                    track->SetDisplayLine(2, ALIGN_CENTER, time_code.at(abs(index)).c_str(), INVERT, forceUpdate);
                }
                else
                {
                    track->SetDisplayMode(DISPLAY_MODE_8, forceUpdate);
                    track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str(), NON_INVERT, forceUpdate);
                    track->SetDisplayLine(1, ALIGN_CENTER, DAW::GetTrackIndex(media_track).c_str(), NON_INVERT, forceUpdate);
                    track->SetDisplayLine(2, ALIGN_CENTER, context->GetPanPushMode() ? strPan1.c_str() : strPan2.c_str(), NON_INVERT, forceUpdate);
                    track->SetDisplayLine(3, ALIGN_CENTER, std::string("").c_str(), NON_INVERT, forceUpdate);
                }
            }
        }

        has_hast_touched_fx_enabled = context->GetLastTouchedFxMode();
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

    void HandleMuteClick(int index, int value) override
    {
        int now = GetTickCount();
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        if (value == 0 && context->GetMuteSoloMomentary())
        {
            if (now - mute_start[index] > MOMENTARY_TIMEOUT)
            {
                CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, !DAW::IsTrackMuted(media_track)), NULL);
            }
            mute_start[index] = 0;
        }
        else if (value > 0)
        {
            mute_start[index] = now;
            CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, !DAW::IsTrackMuted(media_track)), NULL);
        }
    }

    void HandleSoloClick(int index, int value) override
    {
        int now = GetTickCount();
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        if (value == 0 && context->GetMuteSoloMomentary())
        {
            if (now - solo_start[index] > MOMENTARY_TIMEOUT)
            {
                CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, !DAW::IsTrackSoloed(media_track)), NULL);
            }
            solo_start[index] = 0;
        }
        else if (value > 0)
        {
            solo_start[index] = now;
            CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, !DAW::IsTrackSoloed(media_track)), NULL);
        }
    }

    void HandleFaderTouch(int index) override
    {
        (void)index;
    }

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