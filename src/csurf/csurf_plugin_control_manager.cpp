#ifndef CSURF_PLUGIN_CONTROL_MANAGER_C_
#define CSURF_PLUGIN_CONTROL_MANAGER_C_

#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_channel_manager.hpp"
#include "csurf_faderport_ui_plugin_edit.hpp"
#include "csurf_navigator.hpp"
#include "extern/ini.hpp"
#include <vector>
#include "csurf_daw.hpp"
#include "csurf_utils.hpp"

using namespace CSURF_FADERPORT_UI_PLUGIN_EDIT;

class CSurf_PluginControlManager : public CSurf_ChannelManager
{
protected:
    mINI::INIStructure ini;
    string fileName;
    int updateCount;

    string getParamKey(string prefix, int index)
    {
        string controlIndex = to_string(context->GetChannelManagerItemIndex() + index);
        return prefix + controlIndex;
    }

    void GetCurrentPlugin()
    {
        MediaTrack *media_track = context->GetPluginEditTrack();
        int pluginId = context->GetPluginEditPluginId();
        string pluginName = DAW::GetTrackFxName(media_track, pluginId);
        fileName = GetReaSonusPluginPath(pluginName);

        mINI::INIFile file(fileName);
        if (!file.read(ini))
        {
            ini["Global"];
            ini["Global"]["origName"] = pluginName;
            ini["Global"]["name"] = pluginName;
            file.generate(ini, true);
        }
    }

    void SaveIniFile()
    {
        mINI::INIFile file(fileName);
        file.write(ini, true);
    }

public:
    CSurf_PluginControlManager(
        std::vector<CSurf_Track *> tracks,
        CSurf_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->SetChannelManagerItemsCount(127);
        GetCurrentPlugin();
        UpdateTracks();
        colorActive = ButtonColorWhiteDim;
        colorDim = ButtonColorWhiteDim;
    }

    ~CSurf_PluginControlManager() {};

    void UpdateTracks() override
    {
        string paramKey;
        int trackId, itemNumber, takeId, pluginId, paramId;
        double min, max = 0.0;

        if (GetTouchedOrFocusedFX(1, &trackId, &itemNumber, &takeId, &pluginId, &paramId))
        {
            context->SetPluginEditParamId(paramId);
        }

        MediaTrack *media_track = GetTrack(0, trackId);

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            char buffer[255];
            int filterIndex = context->GetChannelManagerItemIndex() + i;
            CSurf_Track *track = tracks.at(i);
            track->SetTrackColor(colorActive, colorDim);

            // If the track is armed always blink as an indication it is armed
            track->SetSelectButtonValue(BTN_VALUE_OFF, forceUpdate);
            track->SetMuteButtonValue(BTN_VALUE_OFF, forceUpdate);
            track->SetSoloButtonValue(BTN_VALUE_OFF, forceUpdate);
            track->SetValueBarMode(VALUEBAR_MODE_NORMAL);
            track->SetValueBarValue(0);

            track->SetDisplayMode(DISPLAY_MODE_2, forceUpdate);

            paramKey = getParamKey("Select_", filterIndex);
            if (ini.has(paramKey))
            {
                track->SetDisplayLine(0, ALIGN_CENTER, ini[paramKey]["name"].c_str(), INVERT, forceUpdate);
                TrackFX_GetFormattedParamValue(media_track, pluginId, stoi(ini[paramKey]["param"]), buffer, sizeof(buffer));
                track->SetDisplayLine(1, ALIGN_CENTER, buffer, NON_INVERT, forceUpdate);
            }
            else
            {
                track->SetDisplayLine(0, ALIGN_CENTER, "Free", INVERT, forceUpdate);
                track->SetDisplayLine(1, ALIGN_CENTER, "", NON_INVERT, true);
            }

            paramKey = getParamKey("Fader_", filterIndex);
            if (ini.has(paramKey))
            {
                track->SetDisplayLine(2, ALIGN_CENTER, ini[paramKey]["name"].c_str(), INVERT, forceUpdate);
                TrackFX_GetFormattedParamValue(media_track, pluginId, stoi(ini[paramKey]["param"]), buffer, sizeof(buffer));
                track->SetDisplayLine(3, ALIGN_CENTER, buffer, NON_INVERT, forceUpdate);
                double value = TrackFX_GetParam(media_track, pluginId, stoi(ini[paramKey]["param"]), &min, &max);
                track->SetFaderValue((int)(value * 16383.0), forceUpdate);
            }
            else
            {
                track->SetDisplayLine(2, ALIGN_CENTER, "Free", INVERT, forceUpdate);
                track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, true);
                track->SetFaderValue(0, forceUpdate);
            }
        }

        forceUpdate = false;
    }

    void HandleSelectClick(int index) override
    {
        double min, max = 0.0;
        int controlIndex = context->GetChannelManagerItemIndex() + index;
        MediaTrack *media_track = context->GetPluginEditTrack();
        int pluginId = context->GetPluginEditPluginId();
        string paramKey = getParamKey("Select_", controlIndex);

        if (ini.has(paramKey))
        {
            int paramId = stoi(ini[paramKey]["param"]);

            int nbSteps = stoi(ini[paramKey]["steps"]) - 1;
            double value = TrackFX_GetParam(media_track, pluginId, paramId, &min, &max);
            double newValue = ((int)(value * nbSteps + 1) % (nbSteps + 1)) / nbSteps;
            TrackFX_SetParam(media_track, pluginId, paramId, newValue);
        }
    }

    void HandleMuteClick(int index) override
    {
        (void)index;
        // If in edit mode
        // Edit the current assignment
        // Else do nothing
    }

    void HandleSoloClick(int index) override
    {
        (void)index;
    }

    void HandleFaderMove(int index, int msb, int lsb) override
    {
        int controlIndex = context->GetChannelManagerItemIndex() + index;
        int pluginId = context->GetPluginEditPluginId();

        string paramKey = getParamKey("Fader_", controlIndex);

        if (ini.has(paramKey))
        {
            MediaTrack *media_track = context->GetPluginEditTrack();
            double value = int14ToNormalized(msb, lsb);

            TrackFX_SetParam(media_track, pluginId, stoi(ini[paramKey]["param"]), value);
        }
    }
};

#endif