#ifndef CSURF_TRACK_MANAGER_C_
#define CSURF_TRACK_MANAGER_C_

#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_channel_manager.hpp"
#include "csurf_navigator.cpp"
#include <vector>

class CSurf_TrackManager : public CSurf_ChannelManager
{
protected:
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

public:
    CSurf_TrackManager(std::vector<CSurf_Track *> tracks, CSurf_Navigator *navigator, CSurf_Context *context, midi_Output *m_midiout) : CSurf_ChannelManager(tracks, navigator, context, m_midiout)
    {
        UpdateTracks();
    }
    ~CSurf_TrackManager() {};

    void UpdateTracks() override
    {
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        int nb_tracks = tracks.size();

        for (int i = 0; i < nb_tracks; i++)
        {
            CSurf_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            int flagsOut = 0;
            int selectFlag = 1;
            double vol, pan = 0.0;
            GetTrackState(media_track, &flagsOut);
            SetTrackColors(media_track);
            if (context->GetArm())
            {
                selectFlag = 6;
            }
            GetTrackUIVolPan(media_track, &vol, &pan);

            track->SetTrackColor(colorActive, colorDim);
            track->SetSelectButtonValue(hasBit(flagsOut, selectFlag) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue(hasBit(flagsOut, 3) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetSoloButtonValue(hasBit(flagsOut, 4) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetFaderValue(vol);
        }
    }

    void HandleSelectClick(int index) override
    {
        int flagsOut = 0;
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        GetTrackState(media_track, &flagsOut);

        if (context->GetArm())
        {
            CSurf_SetSurfaceRecArm(media_track, CSurf_OnRecArmChange(media_track, !hasBit(flagsOut, 6)), NULL);
            return;
        }

        if (!context->GetShiftLeft())
        {
            SetTrackSelected(media_track, !(hasBit(flagsOut, 1) == 1));
            return;
        }

        for (int i = 0; i < 8; i++)
        {
            MediaTrack *media_track = navigator->GetTrackByIndex(i);
            SetTrackSelected(media_track, false);
        }
        SetTrackSelected(media_track, true);
    }

    void HandleMuteClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int flagsOut = 0;
        GetTrackState(media_track, &flagsOut);
        CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, !hasBit(flagsOut, 3)), NULL);
    }

    void HandleSoloClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int flagsOut = 0;
        GetTrackState(media_track, &flagsOut);
        CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, !hasBit(flagsOut, 4)), NULL);
    }

    void HandleFaderTouch() override {}

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), NULL);
    }
};

#endif