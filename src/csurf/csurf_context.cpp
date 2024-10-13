#ifndef CSURF_CONTEXT_H_
#define CSURF_CONTEXT_H_

#include "csurf_context_resources.hpp"
#include <cstdlib>
#include <reaper_plugin_functions.h>
#include "csurf_utils.hpp"

enum ChannelManagerType
{
    Track,
    Hui,
};

class CSurf_Context
{
    bool shift_left;
    bool shift_right;
    bool arm;
    bool lastTouchedFxMode;
    int nbChannels = 8;
    PanEncoderMode panEncoderMode = PanEncoderPanMode;
    bool panPushModePan = true;
    int channelManagerItemIndex = 0;
    int channelManagerItemsCount = 0;
    ChannelManagerType channelManagerType;

public:
    CSurf_Context(int nbChannels) : nbChannels(nbChannels)
    {
    }
    ~CSurf_Context() {}

    void SetShiftLeft(bool val) { shift_left = val; }
    bool GetShiftLeft() { return shift_left; }

    void SetShiftRight(bool val) { shift_right = val; }
    bool GetShiftRight() { return shift_right; }

    void SetArm(bool val) { arm = val; }
    bool GetArm() { return arm; }

    void SetPanEncoderMode(PanEncoderMode val) { panEncoderMode = val; }
    PanEncoderMode GetPanEncoderMode() { return panEncoderMode; }

    void ToggleLastTouchedFxMode() { lastTouchedFxMode = !lastTouchedFxMode; }
    bool GetLastTouchedFxMode() { return lastTouchedFxMode; }

    void SetNbChannels(int count) { nbChannels = count; }
    int GetNbChannels()
    {
        return lastTouchedFxMode ? nbChannels - 1 : nbChannels;
    }

    void TogglePanPushMode() { panPushModePan = !panPushModePan; }
    void ResetPanPushMode() { panPushModePan = true; }
    bool GetPanPushMode() { return panPushModePan; }

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
    void ResetChannelManagerItemIndex() { channelManagerItemIndex = 0; }

    int GetChannelManagerItemIndex() { return channelManagerItemIndex; }

    int GetChannelManagerItemIndex(int max) { return max < channelManagerItemIndex ? max : channelManagerItemIndex; }

    void SetChannelManagerItemsCount(int count) { channelManagerItemsCount = count; }

    void ResetChannelManagerItemsCount() { channelManagerItemsCount = 0; }

    void SetChannelManagerType(ChannelManagerType type) { channelManagerType = type; }
};

#endif
