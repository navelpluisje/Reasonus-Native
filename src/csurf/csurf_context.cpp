#ifndef CSURF_CONTEXT_H_
#define CSURF_CONTEXT_H_

#include "csurf_context_resources.hpp"
#include <cstdlib>

class CSurf_Context
{
    bool shift_left;
    bool shift_right;
    bool arm;
    bool lastTouchedFxMode;
    int nbTracks = 8;
    PanEncoderMode panEncoderMode = PanEncoderPanMode;
    bool panPushModePan = true;
    int sendIndex = 0;
    int receiveIndex = 0;
    int pluginIndex = 0;
    int pluginCount = 0;
    int channelManagerItemIndex = 0;
    int channelManagerItemsCount = 0;

public:
    CSurf_Context()
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

    void SetNbTracks(int count) { nbTracks = count; }
    int GetNbTracks() { return nbTracks; }

    void TogglePanPushMode() { panPushModePan = !panPushModePan; }
    void ResetPanPushMode() { panPushModePan = true; }
    bool GetPanPushMode() { return panPushModePan; }

    void IncrementTrackSendIndex(int val)
    {
        if (sendIndex + val >= 0)
        {
            sendIndex += val;
        }
    }
    void ResetTrackSendIndex() { sendIndex = 0; }
    int GetSendIndex() { return sendIndex; }

    void IncrementTrackReceiveIndex(int val)
    {
        if (receiveIndex + val >= 0)
        {
            receiveIndex += val;
        }
    }
    void ResetTrackReceiveIndex() { receiveIndex = 0; }
    int GetReceiveIndex() { return receiveIndex; }

    void UpdateChannelManagerItemIndex(int val)
    {
        val < 0 ? DecrementChannelManagerItemIndex(abs(val))
                : IncrementChannelmanagerItemIndex(val);
    }
    void IncrementChannelmanagerItemIndex(int val)
    {
        if ((pluginIndex + val + nbTracks) < pluginCount)
        {
            pluginIndex += val;
        }
    }
    void DecrementChannelManagerItemIndex(int val)
    {
        if (pluginIndex - val > 0)
        {
            pluginIndex -= val;
        }
    }
    void ResetChannelManagerItemIndex() { channelManagerItemIndex = 0; }
    int GetChannelManagerItemIndex() { return channelManagerItemIndex; }
    int GetChannelManagerItemIndex(int max) { return max < channelManagerItemIndex ? max : channelManagerItemIndex; }
    void SetChannelManagerItemsCount(int count) { channelManagerItemsCount = count; }
    void ResetChannelManagerItemsCount() { channelManagerItemsCount = 0; }
};

#endif
