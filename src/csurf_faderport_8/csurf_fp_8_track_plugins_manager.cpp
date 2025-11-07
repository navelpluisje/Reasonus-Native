#ifndef CSURF_FP_8_TRACK_PLUGINS_MANAGER_C_
#define CSURF_FP_8_TRACK_PLUGINS_MANAGER_C_

#include <reaper_plugin.h>
#include "../shared/csurf_context.cpp"
#include "csurf_fp_8_track.hpp"
#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_navigator.hpp"
#include <vector>
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_daw.hpp"

class CSurf_FP_8_TrackPluginsManager : public CSurf_FP_8_ChannelManager
{
protected:
    int current_plugin = 0;

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

    void GetFaderValue(MediaTrack *media_track, int *fader_value, int *value_bar_value)
    {
        int panMode = 0;
        double volume, pan1, pan2 = 0.0;

        GetTrackUIVolPan(media_track, &volume, &pan1);
        GetTrackUIPan(media_track, &pan1, &pan2, &panMode);

        *fader_value = int(volToNormalized(volume) * 16383.0);
        *value_bar_value = int(panToNormalized(pan1) * 127);
    }

    std::string GetBypassedText(bool bypassed)
    {
        return bypassed ? "Bypassed" : "Enabled";
    }

public:
    CSurf_FP_8_TrackPluginsManager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->ResetChannelManagerItemsCount();
        UpdateTracks();
    }
    ~CSurf_FP_8_TrackPluginsManager() {};

    void UpdateTracks() override
    {
        current_plugin = context->GetChannelManagerItemIndex();

        WDL_PtrList<MediaTrack> media_tracks = navigator->GetBankTracks();
        MediaTrack *plugin_track = GetSelectedTrack(0, 0);
        if (!plugin_track && context->GetMasterFaderMode() && DAW::IsTrackSelected(::GetMasterTrack(0)))
        {
            plugin_track = GetMasterTrack(0);
        }
        context->SetChannelManagerItemsCount(TrackFX_GetCount(plugin_track));

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            MediaTrack *media_track;
            int plugin_index = context->GetChannelManagerItemIndex() + i;
            int fader_value = 0, value_bar_value = 0;

            CSurf_FP_8_Track *track = tracks.at(i);
            if (context->GetMasterFaderMode() && i == (context->GetNbChannels() - 1))
            {
                media_track = ::GetMasterTrack(0);
            }
            else
            {
                media_track = media_tracks.Get(i);
            }

            SetTrackColors(media_track);
            GetFaderValue(media_track, &fader_value, &value_bar_value);

            if (!media_track)
            {
                track->SetDisplayLine(0, ALIGN_LEFT, "", NON_INVERT);
            }
            else
            {
                track->SetDisplayLine(0, ALIGN_LEFT, DAW::GetTrackName(media_track).c_str(), plugin_track == media_track ? INVERT : NON_INVERT);
            }

            if (DAW::HasTrackFx(plugin_track, plugin_index))
            {
                track->SetDisplayLine(1, ALIGN_LEFT, DAW::GetTrackFxName(plugin_track, plugin_index).c_str(), INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, DAW::GetTrackFxSurfceEnabled(plugin_track, plugin_index).c_str());
                track->SetDisplayLine(3, ALIGN_CENTER, "");
                track->SetMuteButtonValue(
                    ButtonBlinkOnOff(
                        (context->GetShiftChannelLeft() && !DAW::GetTrackFxEnabled(plugin_track, plugin_index)),
                        !DAW::GetTrackFxEnabled(plugin_track, plugin_index),
                        context->GetDistractionFreeMode()));
                track->SetSoloButtonValue(
                    ButtonBlinkOnOff(
                        DAW::GetTrackFxPanelOpen(plugin_track, plugin_index),
                        hasPluginConfigFile(plugin_track, plugin_index),
                        context->GetDistractionFreeMode()));
            }
            else
            {
                track->SetDisplayLine(1, ALIGN_LEFT, "No Fx", INVERT);
                track->SetDisplayLine(2, ALIGN_CENTER, "");
                track->SetDisplayLine(3, ALIGN_CENTER, "");
                track->SetMuteButtonValue(BTN_VALUE_OFF);
                track->SetSoloButtonValue(BTN_VALUE_OFF);
            }

            track->SetTrackColor(color);
            track->SetSelectButtonValue(DAW::IsTrackSelected(media_track) ? BTN_VALUE_ON : BTN_VALUE_OFF);
            track->SetFaderValue(fader_value);
            track->SetValueBarMode(VALUEBAR_MODE_BIPOLAR);
            track->SetValueBarValue(value_bar_value);

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

        DAW::SetUniqueSelectedTrack(media_track);
    }

    void HandleMuteClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = GetSelectedTrack(0, 0);
        if (!media_track && context->GetMasterFaderMode() && DAW::IsTrackSelected(::GetMasterTrack(0)))
        {
            media_track = GetMasterTrack(0);
        }

        int plugin_index = context->GetChannelManagerItemIndex() + index;

        if (context->GetShiftChannelLeft())
        {
            TrackFX_SetOffline(media_track, plugin_index, !DAW::GetTrackFxOffline(media_track, plugin_index));
        }
        else
        {
            TrackFX_SetEnabled(media_track, plugin_index, !DAW::GetTrackFxEnabled(media_track, plugin_index));
        }
    }

    void HandleSoloClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        MediaTrack *media_track = GetSelectedTrack(0, 0);
        if (!media_track && context->GetMasterFaderMode() && DAW::IsTrackSelected(::GetMasterTrack(0)))
        {
            media_track = GetMasterTrack(0);
        }

        int plugin_index = context->GetChannelManagerItemIndex() + index;

        if (DAW::GetTrackFxPanelOpen(media_track, plugin_index))
        {
            TrackFX_Show(media_track, plugin_index, 0);
            TrackFX_Show(media_track, plugin_index, 2);
            context->SetPluginEditTrack(NULL);
            context->SetPluginEditPluginId(-1);
        }
        else
        {
            // First clean up all open fx windows and then open the plugin in a floating window
            Main_OnCommandStringEx("_REASONUS_CLOSE_ALL_FLOATING_FX_WINDOWS_COMMAND"); // SWS/S&M: Close all floating FX and chain windows
            ::TrackFX_Show(media_track, plugin_index, 3);
            context->SetPluginEditTrack(media_track);
            context->SetPluginEditPluginId(plugin_index);
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