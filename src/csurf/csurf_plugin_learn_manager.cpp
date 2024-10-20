#ifndef CSURF_PLUGIN_LEARN_MANAGER_C_
#define CSURF_PLUGIN_LEARN_MANAGER_C_

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

class CSurf_PluginLearnManager : public CSurf_ChannelManager
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
        int pluginId = context->GetPluginEditPluginId();
        MediaTrack *media_track = context->GetPluginEditTrack();
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
    CSurf_PluginLearnManager(
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

    ~CSurf_PluginLearnManager() {};

    void UpdateTracks() override
    {
        string paramKey;
        int trackId, itemNumber, takeId, pluginId, paramId;
        if (GetTouchedOrFocusedFX(0, &trackId, &itemNumber, &takeId, &pluginId, &paramId))
        {
            context->SetPluginEditParamId(paramId);
        }

        if (updateCount > 100)
        {
            mINI::INIFile file(fileName);
            file.read(ini);
        }
        updateCount += 1;

        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            int paramIndex = context->GetChannelManagerItemIndex() + i;
            CSurf_Track *track = tracks.at(i);
            track->SetTrackColor(colorActive, colorDim);

            // If the track is armed always blink as an indication it is armed
            track->SetSelectButtonValue(BTN_VALUE_OFF, forceUpdate);
            track->SetMuteButtonValue(BTN_VALUE_OFF, forceUpdate);
            track->SetSoloButtonValue(BTN_VALUE_OFF, forceUpdate);
            track->SetFaderValue(0, forceUpdate);
            track->SetValueBarMode(VALUEBAR_MODE_NORMAL);
            track->SetValueBarValue(0);

            track->SetDisplayMode(DISPLAY_MODE_2, forceUpdate);

            paramKey = getParamKey("Select_", paramIndex);
            if (ini.has(paramKey))
            {
                track->SetDisplayLine(0, ALIGN_CENTER, ini[paramKey]["name"].c_str(), INVERT, forceUpdate);
            }
            else
            {
                track->SetDisplayLine(0, ALIGN_CENTER, "Free", INVERT, forceUpdate);
            }

            paramKey = getParamKey("Fader_", paramIndex);
            if (ini.has(paramKey))
            {
                track->SetDisplayLine(2, ALIGN_CENTER, ini[paramKey]["name"].c_str(), INVERT, forceUpdate);
            }
            else
            {
                track->SetDisplayLine(2, ALIGN_CENTER, "Free", INVERT, forceUpdate);
            }

            track->SetDisplayLine(1, ALIGN_CENTER, "", NON_INVERT, true);
            track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, true);
        }

        forceUpdate = false;
    }

    void HandleSelectClick(int index) override
    {
        int controlIndex = context->GetChannelManagerItemIndex() + index;
        int pluginId = context->GetPluginEditPluginId();
        int paramId = context->GetPluginEditParamId();

        string paramKey = getParamKey("Select_", controlIndex);

        if (context->GetShiftLeft())
        {
            // Show the edit screen
            if (!IsPluginEditDialogOpen())
            {
                ShowPluginEditDialog(fileName, controlIndex);
            }
        }
        else if (context->GetShiftRight())
        {
            if (ini.has(paramKey))
            {
                ini.remove(paramKey);
                SaveIniFile();
            }
        }
        else
        {
            MediaTrack *media_track = context->GetPluginEditTrack();
            string paramName = DAW::GetTrackFxParamName(media_track, pluginId, paramId);
            int nbSteps = DAW::GetTrackFxParamNbSteps(media_track, pluginId, paramId);

            if (!ini.has(paramKey))
            {
                ini[paramKey];
            }
            ini[paramKey]["origName"] = paramName;
            ini[paramKey]["name"] = paramName;
            ini[paramKey]["param"] = to_string(paramId);
            ini[paramKey]["steps"] = to_string(nbSteps);

            SaveIniFile();
        }
        // Open the dialog
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

    void HandleFaderTouch(int index) override
    {
        int controlIndex = context->GetChannelManagerItemIndex() + index;
        int pluginId = context->GetPluginEditPluginId();
        int paramId = context->GetPluginEditParamId();

        string paramKey = getParamKey("Fader_", controlIndex);

        if (context->GetShiftLeft())
        {
            // Show the edit screen
            if (!IsPluginEditDialogOpen())
            {
                ShowPluginEditDialog(fileName, controlIndex);
            }
        }
        else if (context->GetShiftRight())
        {
            ShowConsoleMsg("RightShift\n");
            if (ini.has(paramKey))
            {
                ini.remove(paramKey);
                SaveIniFile();
            }
        }
        else
        {
            MediaTrack *media_track = context->GetPluginEditTrack();
            string paramName = DAW::GetTrackFxParamName(media_track, pluginId, paramId);

            if (!ini.has(paramKey))
            {
                ini[paramKey];
            }
            ini[paramKey]["origName"] = paramName;
            ini[paramKey]["name"] = paramName;
            ini[paramKey]["param"] = to_string(paramId);

            SaveIniFile();
        }
    }
};

#endif