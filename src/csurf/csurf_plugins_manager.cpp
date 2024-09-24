#ifndef CSURF_PLUGINS_MANAGER_C_
#define CSURF_PLUGINS_MANAGER_C_

#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_channel_manager.hpp"
#include "csurf_navigator.hpp"
#include <vector>
#include "csurf_utils.hpp"

class CSurf_PluginsManager : public CSurf_ChannelManager
{
protected:
    int nbPlugins = 0;
    int nbTrackPlugins[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int currentPlugin = 0;
    int pluginModes[3] = {0, 1, 3};

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

    void GetFaderValue(MediaTrack *media_track, int *faderValue, int *valueBarValue)
    {
        int panMode = 0;
        double volume, pan1, pan2 = 0.0;

        GetTrackUIVolPan(media_track, &volume, &pan1);
        GetTrackUIPan(media_track, &pan1, &pan2, &panMode);

        *faderValue = int(volToNormalized(volume) * 16383.0);
        *valueBarValue = int(panToNormalized(pan1) * 127);
    }

    string GetBypassedText(bool bypassed)
    {
        return bypassed ? "Bypassed" : "Enabled";
    }

public:
    CSurf_PluginsManager(
        std::vector<CSurf_Track *> tracks,
        CSurf_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetTrackPluginIndex();
        UpdateTracks();
    }
    ~CSurf_PluginsManager() {};

    void UpdateTracks() override
    {
        nbPlugins = 0;
        currentPlugin = context->GetPluginIndex();

        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();

        for (int i = 0; i < navigator->GetTrackCount(); i++)
        {
            MediaTrack *media_track = media_tracks.Get(i);
            int _nbTrackPlugins = TrackFX_GetCount(media_track);
            nbTrackPlugins[i] = _nbTrackPlugins;

            if (_nbTrackPlugins > nbPlugins)
            {
                nbPlugins = _nbTrackPlugins;
            }
        }

        if (nbPlugins < context->GetPluginIndex())
        {
            currentPlugin = nbPlugins;
        }

        for (int i = 0; i < navigator->GetTrackCount(); i++)
        {
            int pluginIndex = nbTrackPlugins[i] < currentPlugin ? nbTrackPlugins[i] : currentPlugin;
            int flagsOut, faderValue = 0, valueBarValue = 0;

            CSurf_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            SetTrackColors(media_track);

            GetFaderValue(media_track, &faderValue, &valueBarValue);

            const char *trackName = GetTrackState(media_track, &flagsOut);

            bool enabled = false;

            char pluginName[256] = "";
            if (TrackFX_GetFXName(media_track, pluginIndex, pluginName, size(pluginName)))
            {
                track->SetDisplayLine(1, ALIGN_LEFT, StripFxType(pluginName).c_str(), INVERT);
                enabled = TrackFX_GetEnabled(media_track, pluginIndex);
                track->SetDisplayLine(2, ALIGN_CENTER, GetBypassedText(!enabled).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, "");
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Fx", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
            }

            track->SetTrackColor(colorActive, colorDim);
            track->SetSelectButtonValue(hasBit(flagsOut, 1) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetMuteButtonValue((context->GetShiftLeft() && enabled) ? BTN_VALUE_BLINK : enabled ? BTN_VALUE_ON
                                                                                                       : BTN_VALUE_OFF);
            track->SetSoloButtonValue(BTN_VALUE_OFF);
            track->SetFaderValue(faderValue);
            track->SetValueBarMode(VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(valueBarValue);

            track->SetDisplayMode(DISPLAY_MODE_2);
            track->SetDisplayLine(0, ALIGN_CENTER, trackName);
        }
    }

    void
    HandleSelectClick(int index) override
    {
        int flagsOut = 0;
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        GetTrackState(media_track, &flagsOut);

        if (context->GetArm())
        {
            CSurf_SetSurfaceRecArm(media_track, CSurf_OnRecArmChange(media_track, !hasBit(flagsOut, 6)), NULL);
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

        if (context->GetShiftLeft())
        {
            bool offline = TrackFX_GetOffline(media_track, currentPlugin);
            TrackFX_SetOffline(media_track, currentPlugin, !offline);
        }
        else
        {
            bool enabled = TrackFX_GetEnabled(media_track, currentPlugin);
            TrackFX_SetEnabled(media_track, currentPlugin, !enabled);
        }
    }

    void HandleSoloClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        // First clean up all open fx windows and then open the plugin in a floating window
        Main_OnCommandStringEx("_S&M_WNCLS3"); // SWS/S&M: Close all floating FX windows
        Main_OnCommandStringEx("_S&M_WNCLS4"); // SWS/S&M: Close all FX chain windows
        TrackFX_Show(media_track, currentPlugin, 3);
    }

    void HandleFaderTouch() override
    {
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        // Because it is the fx navigation, the fader will only change the channels volume
        CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), NULL);
    }
};

#endif