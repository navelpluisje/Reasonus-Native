#ifndef CSURF_PLUGINS_MANAGER_C_
#define CSURF_PLUGINS_MANAGER_C_

#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_channel_manager.hpp"
#include "csurf_navigator.hpp"
#include <vector>
#include "csurf_utils.hpp"
#include "csurf_daw.hpp"

class CSurf_PluginsManager : public CSurf_ChannelManager
{
protected:
    int nbPlugins = 0;
    int nbTrackPlugins[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int currentPlugin = 0;

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
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerType(Hui);
        UpdateTracks();
    }
    ~CSurf_PluginsManager() {};

    void UpdateTracks() override
    {
        nbPlugins = 0;
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            MediaTrack *media_track = media_tracks.Get(i);
            int _nbTrackPlugins = TrackFX_GetCount(media_track);
            nbTrackPlugins[i] = _nbTrackPlugins;

            if (_nbTrackPlugins > nbPlugins)
            {
                nbPlugins = _nbTrackPlugins;
            }
        }

        context->SetChannelManagerItemsCount(nbPlugins);
        currentPlugin = context->GetChannelManagerItemIndex();

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int faderValue = 0, valueBarValue = 0;
            int pluginIndex = context->GetChannelManagerItemIndex(nbTrackPlugins[i] - 1);

            CSurf_Track *track = tracks.at(i);
            MediaTrack *media_track = media_tracks.Get(i);
            if (!media_track)
            {
                track->ClearTrack();
                continue;
            }
            SetTrackColors(media_track);
            GetFaderValue(media_track, &faderValue, &valueBarValue);

            if (DAW::HasTrackFx(media_track, pluginIndex))
            {
                track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str());
                track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackFxName(media_track, pluginIndex).c_str(), INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackFxSurfceEnabled(media_track, pluginIndex).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, Progress(pluginIndex + 1, nbTrackPlugins[i]).c_str());
                track->SetMuteButtonValue(ButtonBlinkOnOff((context->GetShiftLeft() && !DAW::GetTrackFxEnabled(media_track, pluginIndex)), !DAW::GetTrackFxEnabled(media_track, pluginIndex)));
            }
            else
            {
                track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str());
                track->SetDisplayLine(1, ALIGN_LEFT, "No Fx", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
                track->SetMuteButtonValue(BTN_VALUE_OFF);
            }

            track->SetTrackColor(colorActive, colorDim);
            track->SetSelectButtonValue(DAW::IsTrackSelected(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetSoloButtonValue(ButtonBlinkOnOff(DAW::GetTrackFxPanelOpen(media_track, pluginIndex), hasPluginConfigFile(media_track, pluginIndex)));
            track->SetFaderValue(faderValue);
            track->SetValueBarMode(VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(valueBarValue);

            track->SetDisplayMode(DISPLAY_MODE_2);
        }
    }

    void HandleSelectClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetArm())
        {
            CSurf_SetSurfaceRecArm(media_track, CSurf_OnRecArmChange(media_track, !DAW::IsTrackArmed(media_track)), NULL);
            return;
        }

        DAW::UnSelectAllTracks();
        SetTrackSelected(media_track, true);
    }

    void HandleMuteClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int pluginIndex = context->GetChannelManagerItemIndex(nbTrackPlugins[index] - 1);

        if (context->GetShiftLeft())
        {
            TrackFX_SetOffline(media_track, pluginIndex, !DAW::GetTrackFxOffline(media_track, pluginIndex));
        }
        else
        {
            TrackFX_SetEnabled(media_track, pluginIndex, !DAW::GetTrackFxEnabled(media_track, pluginIndex));
        }
    }

    void HandleSoloClick(int index) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int pluginIndex = context->GetChannelManagerItemIndex(nbTrackPlugins[index] - 1);

        // If the current plugin window is open, close it
        // Otherwise Close all other open windows and open the plugin window
        if (DAW::GetTrackFxPanelOpen(media_track, pluginIndex))
        {
            TrackFX_Show(media_track, pluginIndex, 0);
            TrackFX_Show(media_track, pluginIndex, 2);
            context->SetPluginEditTrack(NULL);
            context->SetPluginEditPluginId(-1);
        }
        else
        {
            // First clean up all open fx windows and then open the plugin in a floating window
            Main_OnCommandStringEx("_S&M_WNCLS3"); // SWS/S&M: Close all floating FX windows
            Main_OnCommandStringEx("_S&M_WNCLS4"); // SWS/S&M: Close all FX chain windows
            TrackFX_Show(media_track, pluginIndex, 3);
            context->SetPluginEditTrack(media_track);
            context->SetPluginEditPluginId(pluginIndex);
            // We need to check if the plugin is available and then set the correct manager
        }
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        // Because it is the fx navigation, the fader will only change the channels volume
        CSurf_SetSurfaceVolume(media_track, CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), NULL);
    }
};

#endif