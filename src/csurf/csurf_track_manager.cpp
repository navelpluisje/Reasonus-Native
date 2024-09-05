#ifndef CSURF_TRACK_MANAGER_C_
#define CSURF_TRACK_MANAGER_C_

#include "csurf_button.hpp"
#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_navigator.cpp"
#include <vector>

bool hasBit(int val, int key)
{
    return val & (1 << key);
};

class CSurf_TrackManager
{
protected:
    CSurf_Context *context;
    CSurf_Navigator *navigator;

    ButtonColor colorActive;
    ButtonColor colorDim;

    std::vector<CSurf_Track *> tracks;

    void SetTrackColors(MediaTrack *media_track)
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
    CSurf_TrackManager(std::vector<CSurf_Track *> _tracks, CSurf_Navigator *_navigator, CSurf_Context *_context, midi_Output *m_midiout)
    {
        context = _context;
        tracks = _tracks;
        navigator = _navigator;

        UpdateTracks(m_midiout);
    }
    ~CSurf_TrackManager();

    void UpdateTracks(midi_Output *m_midiout)
    {
        (void)m_midiout;
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        int nb_tracks = tracks.size();

        for (int i = 0; i < nb_tracks; i++)
        {
            CSurf_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            int flagsOut = 0;
            int selectFlag = 1;
            GetTrackState(media_track, &flagsOut);
            SetTrackColors(media_track);
            if (context->GetArm())
            {
                selectFlag = 6;
            }

            track->SetTrackColor(colorActive, colorDim, m_midiout);
            track->SetSelectButtonValue(hasBit(flagsOut, selectFlag) ? BTN_VALUE_ON : BTN_VALUE_OFF, m_midiout);
            track->SetMuteButtonValue(hasBit(flagsOut, 3) ? BTN_VALUE_ON : BTN_VALUE_OFF, m_midiout);
            track->SetSoloButtonValue(hasBit(flagsOut, 4) ? BTN_VALUE_ON : BTN_VALUE_OFF, m_midiout);
        }
    }

    void HandleSelectClick(int index)
    {
        int flagsOut = 0;
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        GetTrackState(media_track, &flagsOut);

        if (context->GetArm())
        {
            CSurf_SetSurfaceRecArm(media_track, CSurf_OnRecArmChange(media_track, !hasBit(flagsOut, 6)), NULL);
            return;
        }

        if (!context->GetSfiftLeft())
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

    void HandleMuteClick(int index)
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int flagsOut = 0;
        GetTrackState(media_track, &flagsOut);
        CSurf_SetSurfaceMute(media_track, CSurf_OnMuteChange(media_track, !hasBit(flagsOut, 3)), NULL);
    }

    void HandleSoloClick(int index)
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int flagsOut = 0;
        GetTrackState(media_track, &flagsOut);
        CSurf_SetSurfaceSolo(media_track, CSurf_OnSoloChange(media_track, !hasBit(flagsOut, 4)), NULL);
    }
};

#endif