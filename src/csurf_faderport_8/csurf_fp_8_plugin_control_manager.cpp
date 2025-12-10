#ifndef CSURF_FP_8_PLUGIN_CONTROL_MANAGER_C_
#define CSURF_FP_8_PLUGIN_CONTROL_MANAGER_C_

#include "../shared/csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_fp_8_track.hpp"
#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_navigator.hpp"
#include <mini/ini.h>
#include <vector>
#include "../shared/csurf_daw.hpp"
#include "../shared/csurf_utils.hpp"

class CSurf_FP_8_PluginControlManager : public CSurf_FP_8_ChannelManager
{
protected:
    mINI::INIStructure ini;
    std::string fileName;
    int updateCount;

    std::string getParamKey(std::string prefix, int index)
    {
        std::string controlIndex = std::to_string(context->GetChannelManagerItemIndex() + index);
        return prefix + controlIndex;
    }

    void GetCurrentPlugin()
    {
        MediaTrack *media_track = context->GetPluginEditTrack();
        int pluginId = context->GetPluginEditPluginId();
        std::string pluginName = DAW::GetTrackFxName(media_track, pluginId);
        std::string developerName = DAW::GetTrackFxDeveloper(media_track, pluginId);
        fileName = GetReaSonusPluginPath(developerName, pluginName);

        mINI::INIFile file(fileName);
        if (!file.read(ini))
        {
            ini["Global"];
            ini["Global"]["origName"] = pluginName;
            ini["Global"]["name"] = pluginName;
            ini["Global"]["developer"] = developerName;
            (void)file.generate(ini, true);
        }
    }

    void SaveIniFile()
    {
        mINI::INIFile file(fileName);
        file.write(ini, true);
    }

public:
    CSurf_FP_8_PluginControlManager(
        std::vector<CSurf_FP_8_Track *> tracks,
        CSurf_FP_8_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_FP_8_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerItemsCount(127);
        GetCurrentPlugin();
        UpdateTracks();
        color = ButtonColorWhiteDim;
    }

    ~CSurf_FP_8_PluginControlManager() {};

    void UpdateTracks() override
    {
        std::string paramKey;
        double min, max = 0.0;

        MediaTrack *media_track = context->GetPluginEditTrack();
        int pluginId = context->GetPluginEditPluginId();

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            char buffer[255];
            int filterIndex = context->GetChannelManagerItemIndex() + i;
            CSurf_FP_8_Track *track = tracks.at(i);
            track->SetTrackColor(color);

            // If the track is armed always blink as an indication it is armed
            track->SetSelectButtonValue(BTN_VALUE_OFF, forceUpdate);
            track->SetMuteButtonValue(BTN_VALUE_OFF, forceUpdate);
            track->SetSoloButtonValue(BTN_VALUE_OFF, forceUpdate);
            track->SetValueBarMode(VALUEBAR_MODE_FILL);

            track->SetDisplayMode(DISPLAY_MODE_2, forceUpdate);

            paramKey = getParamKey("Select_", filterIndex);
            if (ini.has(paramKey) && ini[paramKey]["param"] != "")
            {
                track->SetDisplayLine(0, ALIGN_CENTER, ini[paramKey]["name"].c_str(), INVERT, forceUpdate);
                TrackFX_GetFormattedParamValue(media_track, pluginId, stoi(ini[paramKey]["param"]), buffer, sizeof(buffer));
                track->SetDisplayLine(1, ALIGN_CENTER, buffer, NON_INVERT, forceUpdate);
            }
            else
            {
                track->SetDisplayLine(0, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
                track->SetDisplayLine(1, ALIGN_CENTER, "", NON_INVERT, true);
            }

            paramKey = getParamKey("Fader_", filterIndex);
            if (ini.has(paramKey) && ini[paramKey]["param"] != "")
            {
                track->SetDisplayLine(2, ALIGN_CENTER, ini[paramKey]["name"].c_str(), INVERT, forceUpdate);
                TrackFX_GetFormattedParamValue(media_track, pluginId, stoi(ini[paramKey]["param"]), buffer, sizeof(buffer));
                track->SetDisplayLine(3, ALIGN_CENTER, buffer, NON_INVERT, forceUpdate);
                double value = TrackFX_GetParam(media_track, pluginId, stoi(ini[paramKey]["param"]), &min, &max);
                track->SetFaderValue((int)(value * 16383.0), forceUpdate);
                track->SetValueBarValue((int)(value * 127.0));
            }
            else
            {
                track->SetDisplayLine(2, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
                track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, true);
                track->SetFaderValue(0, forceUpdate);
                track->SetValueBarMode(VALUEBAR_MODE_OFF);
                track->SetValueBarValue(0);
            }
        }

        forceUpdate = false;
    }

    void HandleSelectClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }

        double min, max = 0.0;
        int controlIndex = context->GetChannelManagerItemIndex() + index;
        MediaTrack *media_track = context->GetPluginEditTrack();
        int pluginId = context->GetPluginEditPluginId();
        std::string paramKey = getParamKey("Select_", controlIndex);

        if (ini.has(paramKey) && ini[paramKey]["param"] != "")
        {
            int paramId = stoi(ini[paramKey]["param"]);

            int nbSteps = stoi(ini[paramKey]["steps"]);
            if (nbSteps == 1)
            {
                nbSteps = 2;
            }
            double value = TrackFX_GetParam(media_track, pluginId, paramId, &min, &max);
            double stepSize = max / (nbSteps - 1);
            double newValue = int((value + stepSize) * 100);
            if (newValue > 100)
            {
                newValue = 0;
            }

            TrackFX_SetParam(media_track, pluginId, paramId, newValue / 100);
        }
    }

    void HandleMuteClick(int index, int value) override
    {
        (void)index;
        (void)value;
        // If in edit mode
        // Edit the current assignment
        // Else do nothing
    }

    void HandleSoloClick(int index, int value) override
    {
        (void)index;
        (void)value;
    }

    void HandleFaderTouch(int index, int value) override
    {
        int controlIndex = context->GetChannelManagerItemIndex() + index;
        int pluginId = context->GetPluginEditPluginId();

        std::string paramKey = getParamKey("Fader_", controlIndex);

        if (ini.has(paramKey))
        {
            MediaTrack *media_track = context->GetPluginEditTrack();
            if (!media_track || value > 0)
            {
                return;
            }

            TrackFX_EndParamEdit(media_track, pluginId, stoi(ini[paramKey]["param"]));
        }
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        int controlIndex = context->GetChannelManagerItemIndex() + index;
        int pluginId = context->GetPluginEditPluginId();

        std::string paramKey = getParamKey("Fader_", controlIndex);

        if (ini.has(paramKey))
        {
            MediaTrack *media_track = context->GetPluginEditTrack();
            double value = int14ToNormalized(msb, lsb);

            TrackFX_SetParam(media_track, pluginId, stoi(ini[paramKey]["param"]), value);
        }
    }
};

#endif