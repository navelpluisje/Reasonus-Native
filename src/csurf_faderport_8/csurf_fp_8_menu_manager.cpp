#ifndef CSURF_FP_8_MENU_MANAGER_C_
#define CSURF_FP_8_MENU_MANAGER_C_

#include "../shared/csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_fp_8_track.hpp"
#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_navigator.hpp"
#include <vector>
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"

class CSurf_FP_8_Menu_Manager : public CSurf_FP_8_ChannelManager
{
protected:
    int nbSends = 0;
    int nbTrackSends[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int currentSend = 0;

    void SetTrackColors(MediaTrack *media_track) override
    {
        if (!media_track)
        {
            color.SetColor(ButtonColorWhite);
            return;
        }

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

    void GetFaderValue(MediaTrack *media_track, int sendIndex, int *faderValue, int *valueBarValue, int *_pan, std::string *panStr)
    {
        double volume, pan = 0.0;

        GetTrackSendUIVolPan(media_track, sendIndex, &volume, &pan);
        *panStr = GetPanString(pan);
        *_pan = (int)pan;

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
    CSurf_FP_8_Menu_Manager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerType(Hui);
        UpdateTracks();
    }
    ~CSurf_FP_8_Menu_Manager() {};

    void UpdateTracks() override
    {
        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            CSurf_FP_8_Track *track = tracks.at(i);
            track->SetDisplayMode(DISPLAY_MODE_9);
            track->SetDisplayLine(0, ALIGN_LEFT, "Menu", INVERT);
            track->SetDisplayLine(1, ALIGN_LEFT, "Line 2");
            track->SetDisplayLine(2, ALIGN_LEFT, "Line 3");
            track->SetDisplayLine(3, ALIGN_LEFT, "Line 4");
            track->SetDisplayLine(4, ALIGN_LEFT, "Line 5");
            track->SetDisplayLine(5, ALIGN_LEFT, "Line 6");
        }
    }

    void HandleSelectClick(int index) override
    {
        (void)index;
    }

    void HandleMuteClick(int index, int value) override
    {
        (void)index;
        (void)value;
    }

    void HandleSoloClick(int index, int value) override
    {
        (void)index;
        (void)value;
    }

    void HandleFaderTouch(int index) override
    {
        (void)index;
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        (void)index;
        (void)msb;
        (void)lsb;
    }
};

#endif