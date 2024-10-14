#ifndef CSURF_PLUGIN_LEARN_MANAGER_C_
#define CSURF_PLUGIN_LEARN_MANAGER_C_

#include "csurf_context.cpp"
#include <WDL/ptrlist.h>
#include "csurf_track.hpp"
#include "csurf_channel_manager.hpp"
#include "csurf_navigator.hpp"
#include "extern/ini.hpp"
#include <vector>
#include "csurf_daw.hpp"
#include "csurf_utils.hpp"

struct Filter
{
    string name;
    string id;
};

class CSurf_PluginLearnManager : public CSurf_ChannelManager
{
protected:
    bool hasLastTouchedFxEnabled = false;
    mINI::INIStructure ini;
    string fileName;
    vector<Filter> filters;

    void SetTrackColors(MediaTrack *media_track) override
    {
        int red = 0xff;
        int green = 0x00;
        int blue = 0x00;

        if (!context->GetArm())
        {

            int trackColor = GetTrackColor(media_track);
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

    void GetCurrentPlugin()
    {
        int trackId, itemNumber, takeId, pluginId, paramId;

        GetTouchedOrFocusedFX(1, &trackId, &itemNumber, &takeId, &pluginId, &paramId);
        MediaTrack *media_track = GetTrack(0, trackId);
        string pluginName = DAW::GetTrackFxName(media_track, pluginId);
        fileName = GetReaSonusPluginPath(pluginName);

        mINI::INIFile file(fileName);
        if (!file.read(ini))
        {
            ini["Global"];
            ini["Global"]["origName"] = pluginName;
            ini["Global"]["name"] = pluginName;
            file.generate(ini);
        }
    }

    void GetFaderValue(MediaTrack *media_track, int *faderValue, int *valueBarValue, string *_pan1, string *_pan2)
    {
        int panMode = 0;
        double volume, pan1, pan2 = 0.0;

        GetTrackUIVolPan(media_track, &volume, &pan1);
        GetTrackUIPan(media_track, &pan1, &pan2, &panMode);
        *_pan1 = GetPanString(pan1, panMode);
        *_pan2 = GetWidthString(pan2, panMode);

        if (context->GetShiftLeft())
        {
            *faderValue = int(panToNormalized(pan1) * 16383.0);
            *valueBarValue = int(volToNormalized(volume) * 127);
        }
        else if (context->GetShiftRight() && panMode > 4)
        {
            *faderValue = int(panToNormalized(pan2) * 16383.0);
            *valueBarValue = int(volToNormalized(volume) * 127);
        }
        else
        {
            *faderValue = int(volToNormalized(volume) * 16383.0);
            *valueBarValue = int(panToNormalized(pan1) * 127);
        }
    }

public:
    CSurf_PluginLearnManager(
        std::vector<CSurf_Track *> tracks,
        CSurf_Navigator *navigator,
        CSurf_Context *context,
        midi_Output *m_midiout) : CSurf_ChannelManager(tracks, navigator, context, m_midiout)
    {
        context->ResetChannelManagerItemIndex();
        context->ResetChannelManagerItemsCount();
        GetCurrentPlugin();
        UpdateTracks();
    }
    ~CSurf_PluginLearnManager() {};

    void UpdateTracks() override
    {
        for (int i = 0; i < context->GetNbChannels(); i++)
        {
            string filterIndex = to_string(context->GetChannelManagerItemIndex() + i);
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

            // if (filterIndex < (int)filters.size())
            // {
            //     Filter f = filters.at(filterIndex);
            //     track->SetDisplayLine(1, ALIGN_CENTER, "Filter", INVERT, forceUpdate);
            //     track->SetDisplayLine(2, ALIGN_CENTER, filters.at(filterIndex).name.c_str(), NON_INVERT, forceUpdate);
            //     track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
            // }
            // else
            // {
            track->SetDisplayLine(0, ALIGN_CENTER, "Free", INVERT, forceUpdate);
            track->SetDisplayLine(1, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
            track->SetDisplayLine(2, ALIGN_CENTER, "Free", NON_INVERT, forceUpdate);
            track->SetDisplayLine(3, ALIGN_CENTER, "", NON_INVERT, forceUpdate);
            // }
        }

        forceUpdate = false;
    }

    void HandleSelectClick(int index) override
    {
        int trackId, itemNumber, takeId, pluginId, paramId;
        if (!GetTouchedOrFocusedFX(0, &trackId, &itemNumber, &takeId, &pluginId, &paramId))
        {
            return;
        }
        string controlIndex = to_string(context->GetChannelManagerItemIndex() + index);
        string paramKey = "Select_" + controlIndex;

        if (!context->GetShiftLeft())
        {
            MediaTrack *media_track = GetTrack(0, trackId);
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

            mINI::INIFile file(fileName);
            file.write(ini);
        }
        // Open the dialog
    }

    void HandleMuteClick(int index) override
    {
        // If in edit mode
        // Edit the current assignment
        // Else do nothing
    }

    void HandleSoloClick(int index) override {}

    void HandleFaderTouch(int index) override
    {
        int trackId, itemNumber, takeId, pluginId, paramId;
        if (!GetTouchedOrFocusedFX(0, &trackId, &itemNumber, &takeId, &pluginId, &paramId))
        {
            return;
        }

        if (!context->GetShiftLeft())
        {
            string controlIndex = to_string(context->GetChannelManagerItemIndex() + index);
            MediaTrack *media_track = GetTrack(0, trackId);
            string paramName = DAW::GetTrackFxParamName(media_track, pluginId, paramId);

            if (!ini.has("Fader_" + controlIndex))
            {
                ini["Fader_" + controlIndex];
            }
            ini["Fader_" + controlIndex]["origName"] = paramName;
            ini["Fader_" + controlIndex]["name"] = paramName;
            ini["Fader_" + controlIndex]["param"] = to_string(paramId);

            mINI::INIFile file(fileName);
            file.write(ini);
        }
        // Open the fader dialog
    }

    void HandleFaderMove(int index, int msb, int lsb) override {}
};

#endif