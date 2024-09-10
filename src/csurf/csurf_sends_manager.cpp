#ifndef CSURF_SENDS_MANAGER_C_
#define CSURF_SENDS_MANAGER_C_

#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_channel_manager.hpp"
#include "csurf_navigator.cpp"
#include <vector>
#include "csurf_channel_manager_resources.hpp"

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
    CSurf_SendsManager(std::vector<CSurf_Track *> tracks, CSurf_Navigator *navigator, CSurf_Context *context, midi_Output *m_midiout) : CSurf_ChannelManager(tracks, navigator, context, m_midiout)
    {
        UpdateTracks();
    }
    ~CSurf_SendsManager() {};

    void UpdateTracks() override
    {
        nbSends = 0;
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        int nb_tracks = tracks.size();

        for (int i = 0; i < nb_tracks; i++)
        {
            // MediaTrack *media_track = media_tracks.Get(i);
            // int _nbTrackSends = GetTrackNumSends(media_track, 0);
            // nbTrackSends[i] = _nbTrackSends;

            // if (_nbTrackSends > nbSends)
            // {
            //     nbSends = _nbTrackSends;
            // }
        }

        // if (nbSends < currentSend)
        // {
        //     currentSend = nbSends;
        // }

        for (int i = 0; i < nb_tracks; i++)
        {
            // int sendIndex = nbTrackSends[i] < currentSend ? nbTrackSends[i] : currentSend;

            // CSurf_Track *track = tracks.at(i);
            // MediaTrack *media_track = media_tracks.Get(i);
            // SetTrackColors(media_track);

            // int pan, volume, valueBarValue = 0;
            // string panStr;
            // GetFaderValue(media_track, sendIndex, &volume, &valueBarValue, &pan, &panStr);

            // const char *sendName;
            // MediaTrack *destTrack = (MediaTrack *)GetSetTrackSendInfo(media_track, 0, sendIndex, "P_DESTTRACK", 0);
            // if (destTrack)
            // {
            //     sendName = (const char *)GetSetMediaTrackInfo(destTrack, "P_NAME", NULL);
            // }

            // track->SetTrackColor(colorActive, colorDim);
            // track->SetSelectButtonValue(BTN_VALUE_OFF);
            // track->SetMuteButtonValue(BTN_VALUE_OFF);
            // track->SetSoloButtonValue(BTN_VALUE_OFF);
            // track->SetFaderValue(context->GetShiftLeft() ? pan : volume);
            // track->SetValueBarMode(context->GetShiftLeft() ? VALUEBAR_MODE_FILL : VALUEBAR_MODE_BIPOLAR);
            // track->SetValueBarValue(valueBarValue);

            // track->SetDisplayMode(DISPLAY_MODE_2);
            // ShowConsoleMsg(sendName);
            // track->SetDisplayLine(0, ALIGN_LEFT, sendName);
        }
    }

    void HandleSelectClick(int index) override
    {
        (void)index;
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