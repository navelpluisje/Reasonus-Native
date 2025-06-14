#ifndef CSURF_CONTEXT_H_
#define CSURF_CONTEXT_H_

#include "csurf_context_resources.hpp"
#include <cstdlib>
#include "csurf_utils.hpp"

class CSurf_Context
{
    // Global settings
    bool plugin_control;
    bool swap_shift_buttons;
    bool mute_solo_momentary;
    bool overwrite_time_code;
    int surface_time_code;
    int nbChannels = 8;

    // Shift keys
    bool shift_left = false;
    bool shift_right = false;
    bool arm = false;

    // track mode show the time code in the displays
    bool showTimeCode = false;

    // Last touched fx
    bool lastTouchedFxMode = false;

    // Make the fader control the master track for the FaderPort V2
    bool masterFaderMode = false;

    // Pan encoder modes
    PanEncoderMode panEncoderMode = PanEncoderPanMode;
    bool panPushModePan = true;

    // Channel manager
    int channelManagerItemIndex = 0;
    int channelManagerItemsCount = 0;

    // Plugin Edit fields
    MediaTrack *pluginEditTrack;
    int pluginEditPluginId;
    int pluginEditParamId;

    ChannelMode channelMode = TrackMode;
    ChannelMode previousChannelMode = TrackMode;
    ChannelManagerType channelManagerType;

public:
    CSurf_Context(int nbChannels) : nbChannels(nbChannels) {}
    ~CSurf_Context() {}

    void SetPluginControl(bool enabled)
    {
        plugin_control = enabled;
    }

    bool GetPluginControl()
    {
        return plugin_control;
    }

    void SetSwapShiftButtons(bool enabled)
    {
        swap_shift_buttons = enabled;
    }

    bool GetSwapShiftButtons()
    {
        return swap_shift_buttons;
    }

    void SetMuteSoloMomentary(bool enabled)
    {
        mute_solo_momentary = enabled;
    }

    bool GetMuteSoloMomentary()
    {
        return mute_solo_momentary;
    }

    void SetOverwriteTimeCode(bool enabled)
    {
        overwrite_time_code = enabled;
    }

    bool GetOverwriteTimeCode()
    {
        return overwrite_time_code;
    }

    void SetSurfaceTimeCode(int value)
    {
        surface_time_code = value;
    }

    bool GetSurfaceTimeCode()
    {
        return surface_time_code;
    }

    void SetShiftLeft(bool val)
    {
        if (swap_shift_buttons)
        {
            shift_right = val;
        }
        else
        {
            shift_left = val;
        }
    }

    bool GetShiftLeft()
    {
        return shift_left;
    }

    void SetShiftRight(bool val)
    {
        if (swap_shift_buttons)
        {
            shift_left = val;
        }
        else
        {
            shift_right = val;
        }
    }
    bool GetShiftRight()
    {
        return shift_right;
    }

    void SetArm(bool val)
    {
        arm = val;
    }

    bool GetArm()
    {
        return arm;
    }

    void SetPanEncoderMode(PanEncoderMode val)
    {
        panEncoderMode = val;
    }

    PanEncoderMode GetPanEncoderMode()
    {
        return panEncoderMode;
    }

    void SetShowTimeCode(bool value)
    {
        showTimeCode = value;
    }

    void ToggleShowTimeCode()
    {
        showTimeCode = !showTimeCode;
    }

    bool GetShowTimeCode()
    {
        return showTimeCode;
    }

    void ToggleLastTouchedFxMode()
    {
        lastTouchedFxMode = !lastTouchedFxMode;
    }

    void SetLastTouchedFxMode(bool value)
    {
        lastTouchedFxMode = value;
    }

    bool GetLastTouchedFxMode()
    {
        return lastTouchedFxMode;
    }

    void SetMasterFaderMode(bool value)
    {
        masterFaderMode = value;
    }

    bool GetMasterFaderMode()
    {
        return masterFaderMode;
    }

    void SetNbChannels(int count)
    {
        nbChannels = count;
    }

    int GetNbChannels()
    {
        return lastTouchedFxMode ? nbChannels - 1 : nbChannels;
    }

    void TogglePanPushMode()
    {
        panPushModePan = !panPushModePan;
    }

    void ResetPanPushMode()
    {
        panPushModePan = true;
    }

    bool GetPanPushMode()
    {
        return panPushModePan;
    }

    void UpdateChannelManagerItemIndex(int val)
    {
        val < 0 ? DecrementChannelManagerItemIndex(abs(val))
                : IncrementChannelmanagerItemIndex(val);
    }

    void IncrementChannelmanagerItemIndex(int val)
    {
        if ((channelManagerItemIndex + val + (channelManagerType == Hui ? 0 : nbChannels)) < channelManagerItemsCount)
        {
            channelManagerItemIndex += val;
        }
    }

    void DecrementChannelManagerItemIndex(int val)
    {
        if (channelManagerItemIndex - val >= 0)
        {
            channelManagerItemIndex -= val;
        }
    }

    void ResetChannelManagerItemIndex()
    {
        channelManagerItemIndex = 0;
    }

    int GetChannelManagerItemIndex()
    {
        return channelManagerItemIndex;
    }

    int GetChannelManagerItemIndex(int max)
    {
        return max < channelManagerItemIndex ? max : channelManagerItemIndex;
    }

    void SetChannelManagerItemsCount(int count)
    {
        channelManagerItemsCount = count;
    }

    void ResetChannelManagerItemsCount()
    {
        channelManagerItemsCount = 0;
    }

    bool IsChannelMode(ChannelMode _channelMode)
    {
        return channelMode == _channelMode;
    }

    void SetChannelMode(ChannelMode _channelMode)
    {
        previousChannelMode = channelMode;
        channelMode = _channelMode;
    }

    ChannelMode GetChannelMode()
    {
        return channelMode;
    }

    ChannelMode GetPreviousChannelMode()
    {
        return previousChannelMode;
    }

    void SetPluginEditTrack(MediaTrack *track)
    {
        pluginEditTrack = track;
    };

    MediaTrack *GetPluginEditTrack()
    {
        return pluginEditTrack;
    };

    void SetPluginEditPluginId(int pluginId)
    {
        pluginEditPluginId = pluginId;
    };

    int GetPluginEditPluginId()
    {
        return pluginEditPluginId;
    };

    void SetPluginEditParamId(int pluginId)
    {
        pluginEditParamId = pluginId;
    };

    int GetPluginEditParamId()
    {
        return pluginEditParamId;
    };

    void SetChannelManagerType(ChannelManagerType type)
    {
        channelManagerType = type;
    }
};

#endif
