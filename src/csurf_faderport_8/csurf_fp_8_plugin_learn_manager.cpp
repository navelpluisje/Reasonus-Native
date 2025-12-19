#ifndef CSURF_FP_8_PLUGIN_LEARN_MANAGER_C_
#define CSURF_FP_8_PLUGIN_LEARN_MANAGER_C_

#include "csurf_fp_8_channel_manager.hpp"
#include "csurf_fp_8_ui_control_panel.hpp"
#include "../shared/csurf_faderport_ui_imgui_utils.hpp"

class CSurf_FP_8_PluginLearnManager : public CSurf_FP_8_ChannelManager
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
        int pluginId = context->GetPluginEditPluginId();
        MediaTrack *media_track = context->GetPluginEditTrack();
        std::string fullName = DAW::GetTrackFxName(media_track, pluginId, true);
        std::string pluginName = DAW::GetTrackFxName(media_track, pluginId);
        std::string developerName = DAW::GetTrackFxDeveloper(media_track, pluginId);
        fileName = GetReaSonusPluginPath(developerName, pluginName, true);

        mINI::INIFile file(fileName);
        if (!file.read(ini))
        {
            ini["Global"];
            ini["Global"]["origName"] = fullName;
            ini["Global"]["name"] = pluginName;
            ini["Global"]["developer"] = pluginName;
            (void)file.generate(ini, true);
        }
    }

    void SaveIniFile()
    {
        mINI::INIFile file(fileName);
        file.write(ini, true);
    }

public:
    CSurf_FP_8_PluginLearnManager(
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

    ~CSurf_FP_8_PluginLearnManager() {};

    void UpdateTracks() override
    {
        std::string paramKey;
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
            CSurf_FP_8_Track *track = tracks.at(i);
            track->SetTrackColor(color);

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

    void OpenMappingUi(int plugin_id, int channel)
    {
        MediaTrack *media_track = context->GetPluginEditTrack();
        std::string plugin_name = DAW::GetTrackFxName(media_track, plugin_id);
        std::string developer_name = DAW::GetTrackFxDeveloper(media_track, plugin_id);

        if (!ReaSonus8ControlPanel::control_panel_open)
        {
            ToggleFP8ControlPanel(ReaSonus8ControlPanel::MAPPING_PAGE);
        }
        else if (ReaSonus8ControlPanel::current_page != ReaSonus8ControlPanel::MAPPING_PAGE)
        {
            ReaSonus8ControlPanel::SetCurrentPage(ReaSonus8ControlPanel::MAPPING_PAGE);
        }
        else if (
            ReaSonus8ControlPanel::GetPageStringProperty(0).compare(developer_name) == 0 &&
            ReaSonus8ControlPanel::GetPageStringProperty(1).compare(plugin_name + ".ini") == 0 &&
            ReaSonus8ControlPanel::GetPageProperty(0) == channel)
        {
            ToggleFP8ControlPanel(ReaSonus8ControlPanel::MAPPING_PAGE);
        }

        if (ReaSonus8ControlPanel::control_panel_open)
        {
            ReaSonus8ControlPanel::SetPageStringProperty(0, developer_name);
            ReaSonus8ControlPanel::SetPageStringProperty(1, plugin_name + ".ini");
            ReaSonus8ControlPanel::SetPageProperty(0, channel);
        }
    }

    void HandleSelectClick(int index, int value) override
    {
        if (value == 0)
        {
            return;
        }
        int controlIndex = context->GetChannelManagerItemIndex() + index;
        int pluginId = context->GetPluginEditPluginId();
        int trackId, itemNumber, takeId, _pluginId, paramId;

        std::string paramKey = getParamKey("Select_", controlIndex);

        if (context->GetShiftLeft())
        {
            OpenMappingUi(pluginId, controlIndex);
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
            if (!GetTouchedOrFocusedFX(0, &trackId, &itemNumber, &takeId, &_pluginId, &paramId))
            {
                return;
            }

            context->SetPluginEditParamId(paramId);

            MediaTrack *media_track = context->GetPluginEditTrack();
            std::string paramName = DAW::GetTrackFxParamName(media_track, pluginId, paramId);
            int nbSteps = DAW::GetTrackFxParamNbSteps(media_track, pluginId, paramId);

            if (!ini.has(paramKey))
            {
                ini[paramKey];
            }
            ini[paramKey]["origName"] = paramName;
            ini[paramKey]["name"] = paramName;
            ini[paramKey]["param"] = std::to_string(paramId);
            ini[paramKey]["steps"] = std::to_string(nbSteps);

            SaveIniFile();
            if (settings->GetUntouchAfterLearn())
            {
                DAW::SetTrackFXParamUntouched(media_track, pluginId);
            }
        }
        // Open the dialog
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
        (void)value;
        int controlIndex = context->GetChannelManagerItemIndex() + index;
        int pluginId = context->GetPluginEditPluginId();
        int trackId, itemNumber, takeId, _pluginId, paramId;

        std::string paramKey = getParamKey("Fader_", controlIndex);

        if (context->GetShiftLeft())
        {
            OpenMappingUi(pluginId, controlIndex);
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
            if (!GetTouchedOrFocusedFX(0, &trackId, &itemNumber, &takeId, &_pluginId, &paramId))
            {
                return;
            }

            context->SetPluginEditParamId(paramId);

            MediaTrack *media_track = context->GetPluginEditTrack();
            std::string paramName = DAW::GetTrackFxParamName(media_track, pluginId, paramId);

            if (!ini.has(paramKey))
            {
                ini[paramKey];
            }

            ini[paramKey]["origName"] = paramName;
            ini[paramKey]["name"] = paramName;
            ini[paramKey]["param"] = std::to_string(paramId);

            SaveIniFile();
            if (settings->GetUntouchAfterLearn())
            {
                DAW::SetTrackFXParamUntouched(media_track, pluginId);
            }
        }
    }
};

#endif