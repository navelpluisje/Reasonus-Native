#ifndef CSURF_FP_8_PLUGINS_MANAGER_C_
#define CSURF_FP_8_PLUGINS_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"

class CSurf_FP_8_PluginsManager : public CSurf_FP_8_ChannelManager
{
protected:
    int nb_plugins = 0;
    int nb_track_plugins[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int current_plugin = 0;

    void GetFaderValue(MediaTrack *media_track, int *fader_value, int *value_bar_value)
    {
        int panMode = 0;
        double volume, pan1, pan2 = 0.0;

        ::GetTrackUIVolPan(media_track, &volume, &pan1);
        ::GetTrackUIPan(media_track, &pan1, &pan2, &panMode);

        *fader_value = int(volToNormalized(volume) * 16383.0);
        *value_bar_value = int(panToNormalized(pan1) * 127);
    }

    std::string GetBypassedText(bool bypassed)
    {
        return bypassed ? "Bypassed" : "Enabled";
    }

public:
    CSurf_FP_8_PluginsManager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerType(Hui);
        UpdateTracks();
    }
    ~CSurf_FP_8_PluginsManager() {};

    void UpdateTracks() override
    {
        nb_plugins = 0;
        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            MediaTrack *media_track;
            if (context->GetMasterFaderMode() && i == (context->GetNbChannels() - 1))
            {
                media_track = ::GetMasterTrack(0);
            }
            else
            {
                media_track = media_tracks.Get(i);
            }

            int _nb_track_plugins = ::TrackFX_GetCount(media_track);
            nb_track_plugins[i] = _nb_track_plugins;

            if (_nb_track_plugins > nb_plugins)
            {
                nb_plugins = _nb_track_plugins;
            }
        }

        context->SetChannelManagerItemsCount(nb_plugins);
        current_plugin = context->GetChannelManagerItemIndex();

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            MediaTrack *media_track;
            int fader_value = 0, value_bar_value = 0;
            int plugin_index = context->GetChannelManagerItemIndex(nb_track_plugins[i] - 1);

            CSurf_FP_8_Track *track = tracks.at(i);
            if (context->GetMasterFaderMode() && i == (context->GetNbChannels() - 1))
            {
                media_track = ::GetMasterTrack(0);
            }
            else
            {
                media_track = media_tracks.Get(i);
            }

            if (!media_track)
            {
                track->ClearTrack();
                continue;
            }

            SetTrackColors(media_track, DAW::IsTrackSelected(media_track));
            GetFaderValue(media_track, &fader_value, &value_bar_value);

            if (DAW::HasTrackFx(media_track, plugin_index))
            {
                track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str());
                track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackFxName(media_track, plugin_index).c_str(), INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackFxSurfceEnabled(media_track, plugin_index).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, Progress(plugin_index + 1, nb_track_plugins[i]).c_str());
                track->SetMuteButtonValue(
                    ButtonBlinkOnOff(
                        (context->GetShiftChannelLeft() && !DAW::GetTrackFxEnabled(media_track, plugin_index)),
                        !DAW::GetTrackFxEnabled(media_track, plugin_index),
                        settings->GetDistractionFreeMode()));
                track->SetSoloButtonValue(
                    ButtonBlinkOnOff(
                        DAW::GetTrackFxPanelOpen(media_track, plugin_index),
                        hasPluginConfigFile(media_track, plugin_index),
                        settings->GetDistractionFreeMode()));
            }
            else
            {
                track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str());
                track->SetDisplayLine(1, ALIGN_LEFT, "No Fx", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
                track->SetMuteButtonValue(BTN_VALUE_OFF);
                track->SetSoloButtonValue(BTN_VALUE_OFF);
            }

            track->SetTrackColor(color);
            track->SetSelectButtonValue(BTN_VALUE_ON);
            track->SetFaderValue(fader_value);
            track->SetValueBarMode(VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(value_bar_value);

            track->SetDisplayMode(DISPLAY_MODE_2);
        }
    }

    void HandleSelectClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }
        MediaTrack *media_track = navigator->GetTrackByIndex(index);

        if (context->GetArm())
        {
            ::CSurf_SetSurfaceRecArm(media_track, ::CSurf_OnRecArmChange(media_track, !DAW::IsTrackArmed(media_track)), NULL);
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
        int plugin_index = context->GetChannelManagerItemIndex(nb_track_plugins[index] - 1);

        if (context->GetShiftChannelLeft())
        {
            ::TrackFX_SetOffline(media_track, plugin_index, !DAW::GetTrackFxOffline(media_track, plugin_index));
        }
        else
        {
            ::TrackFX_SetEnabled(media_track, plugin_index, !DAW::GetTrackFxEnabled(media_track, plugin_index));
        }
    }

    void HandleSoloClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        int plugin_index = context->GetChannelManagerItemIndex(nb_track_plugins[index] - 1);

        // If the current plugin window is open, close it
        // Otherwise Close all other open windows and open the plugin window
        if (DAW::GetTrackFxPanelOpen(media_track, plugin_index))
        {
            ::TrackFX_Show(media_track, plugin_index, 0);
            ::TrackFX_Show(media_track, plugin_index, 2);
            context->SetPluginEditTrack(NULL);
            context->SetPluginEditPluginId(-1);
        }
        else
        {
            // First clean up all open fx windows and then open the plugin in a floating window
            Main_OnCommandStringEx("_REASONUS_CLOSE_ALL_FLOATING_FX_WINDOWS_COMMAND"); // SWS/S&M: Close all floating FX and chanin windows
            ::TrackFX_Show(media_track, plugin_index, 3);
            context->SetPluginEditTrack(media_track);
            context->SetPluginEditPluginId(plugin_index);
            // We need to check if the plugin is available and then set the correct manager
        }
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        MediaTrack *media_track = navigator->GetTrackByIndex(index);
        // Because it is the fx navigation, the fader will only change the channels volume
        ::CSurf_SetSurfaceVolume(media_track, ::CSurf_OnVolumeChange(media_track, int14ToVol(msb, lsb), false), NULL);
    }
};

#endif