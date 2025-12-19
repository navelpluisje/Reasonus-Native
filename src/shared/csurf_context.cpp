#ifndef CSURF_CONTEXT_H_
#define CSURF_CONTEXT_H_

#include "csurf_context_resources.hpp"
#include <cstdlib>
#include "csurf_utils.hpp"
#include "csurf_reasonus_settings.hpp"

class CSurf_Context
{
    /**
     * @brief Number of channels
     */
    int nbChannels = 8;
    std::string device = FP_8;

    // Shift keys
    bool shift_left = false;
    bool shift_left_locked = false;
    bool shift_right = false;
    bool shift_right_locked = false;
    bool arm = false;

    // track mode show the time code in the displays
    bool show_time_code = false;

    // Last touched fx
    bool last_touched_fx_mode = false;

    // Make the fader control the master track for the FaderPort V2
    bool master_fader_mode = false;

    // Set the track id that has the add send mode. -1 is unselected
    int add_send_receive_mode = -1;
    int current_selected_send_receive = 0;

    // Pan encoder modes
    PanEncoderMode panEncoderMode = PanEncoderTrackPanMode;
    bool panPushModePan = true;

    // Channel manager
    int channelManagerItemIndex = 0;
    int channelManagerItemsCount = 0;

    // Plugin Edit fields
    MediaTrack *pluginEditTrack;
    int pluginEditPluginId;
    int pluginEditParamId;

    // disable fader on v2
    bool fader_disabled;

    ChannelMode channelMode = TrackMode;
    ChannelMode previousChannelMode = TrackMode;
    ChannelMode previousPluginChannelMode = TrackMode;
    ChannelManagerType channelManagerType;

    ReaSonusSettings *settings;

public:
    CSurf_Context(int nbChannels) : nbChannels(nbChannels)
    {
        if (nbChannels == 1)
        {
            device = FP_V2;
        }
        settings = ReaSonusSettings::GetInstance(device);
    }
    ~CSurf_Context() {}

    /**************************************************************************
     * Modifiers
     **************************************************************************/
    void
    SetShiftLeft(bool val)
    {
        if (settings->GetSwapShiftButtons())
        {
            shift_right = val;
        }
        else
        {
            shift_left = val;
        }
    }

    void SetShiftLeftLocked(bool val)
    {
        if (settings->GetSwapShiftButtons())
        {
            shift_right_locked = val;
        }
        else
        {
            shift_left_locked = val;
        }
    }

    bool GetShiftChannelLeft()
    {
        if (!shift_left_locked)
        {
            return shift_left;
        }
        else
        {
            return !shift_left;
        }
    }

    bool GetShiftLeft()
    {
        return shift_left;
    }

    void SetShiftRight(bool val)
    {
        if (settings->GetSwapShiftButtons())
        {
            shift_left = val;
        }
        else
        {
            shift_right = val;
        }
    }

    void SetShiftRightLocked(bool val)
    {
        if (settings->GetSwapShiftButtons())
        {
            shift_left_locked = val;
        }
        else
        {
            shift_right_locked = val;
        }
    }

    bool GetShiftChannelRight()
    {
        if (!shift_right_locked)
        {
            return shift_right;
        }
        else
        {
            return !shift_right;
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

    /**************************************************************************
     * Pan mode
     **************************************************************************/
    void SetPanEncoderMode(PanEncoderMode val)
    {
        panEncoderMode = val;
    }

    PanEncoderMode GetPanEncoderMode()
    {
        return panEncoderMode;
    }

    /**************************************************************************
     * Display the timecode
     **************************************************************************/
    void SetShowTimeCode(bool value)
    {
        show_time_code = value;
    }

    void ToggleShowTimeCode()
    {
        show_time_code = !show_time_code;
    }

    bool GetShowTimeCode()
    {
        return show_time_code;
    }

    /**************************************************************************
     * Last touched Fx param control
     **************************************************************************/
    void ToggleLastTouchedFxMode()
    {
        SetLastTouchedFxMode(!last_touched_fx_mode);
    }

    /**
     * @brief Set the Last Touched FX Mode. If value is true, reset master fader mode
     *
     * @param value
     */
    void SetLastTouchedFxMode(bool value)
    {
        if (value)
        {
            master_fader_mode = false;
        }
        last_touched_fx_mode = value;
    }

    bool GetLastTouchedFxMode()
    {
        return last_touched_fx_mode;
    }

    /**************************************************************************
     * Master Fader mode
     **************************************************************************/
    /**
     * @brief Toggle the master fader mode
     */
    void ToggleMasterFaderMode()
    {
        SetMasterFaderMode(!master_fader_mode);
    }

    /**
     * @brief Set the Master Fader Mode. If value is true, reset last touched Fx mode
     *
     * @param value
     */
    void SetMasterFaderMode(bool value)
    {
        if (value)
        {
            last_touched_fx_mode = false;
        }
        master_fader_mode = value;
    }

    bool GetMasterFaderMode()
    {
        if (nbChannels > 1)
        {
            return master_fader_mode && settings->GetMasterFaderModeEnabled();
        }
        else
        {
            return master_fader_mode;
        }
    }

    /**************************************************************************
     * Number of device channels
     **************************************************************************/
    /**
     * @brief Set the Nb Channels. These are the nb of faders on the FaderPort
     *
     * @param count The number of faders
     */
    void SetNbChannels(int count)
    {
        nbChannels = count;
    }

    /**
     * @brief Get the Nb Channels. These are the actual number if faders
     *
     * @return int The number of faders
     */
    int GetNbChannels()
    {
        // TODO: Should be only nbChannels
        return last_touched_fx_mode ? nbChannels - 1 : nbChannels;
    }

    /**
     * @brief Get the Nb Bank Channels. When lastTouchedFxMode is set, this will be 1 less the the actual fader count
     *
     * @return int The number of bank channels
     */
    int GetNbBankChannels()
    {
        return (last_touched_fx_mode || master_fader_mode) ? nbChannels - 1 : nbChannels;
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

    /**************************************************************************
     * Channel manager Item
     **************************************************************************/
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

    /**************************************************************************
     * Channel modes
     **************************************************************************/
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

    void SetPreviousPluginChannelMode(ChannelMode _channelMode)
    {
        previousPluginChannelMode = _channelMode;
    }

    ChannelMode GetPreviousPluginChannelMode()
    {
        return previousPluginChannelMode;
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

    /**************************************************************************
     * Add Send mode
     **************************************************************************/
    void SetAddSendReceiveMode(int value)
    {
        add_send_receive_mode = value;
    };

    int GetAddSendReceiveMode()
    {
        return add_send_receive_mode;
    };

    void SetCurrentSelectedSendReceive(int value)
    {
        current_selected_send_receive = minmax(0, value, ::CountTracks(0) - 1);
    }

    void IncrementCurrentSelectedSendReceive()
    {
        SetCurrentSelectedSendReceive(current_selected_send_receive + 1);
    }

    void DecrementCurrentSelectedSendReceive()
    {
        SetCurrentSelectedSendReceive(current_selected_send_receive - 1);
    }

    int GetCurrentSelectedSendReceive()
    {
        return current_selected_send_receive;
    }

    /**************************************************************************
     * Channel manager type
     **************************************************************************/
    void SetChannelManagerType(ChannelManagerType type)
    {
        channelManagerType = type;
    }

    void SetFaderDisabled(bool value)
    {
        fader_disabled = value;
    };

    int GetFaderDisabled()
    {
        return fader_disabled;
    }
};

#endif
