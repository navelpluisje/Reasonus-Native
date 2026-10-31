#include "csurf_context.hpp"
#include <array>
#include "csurf_context_resources.hpp"
#include "csurf_reasonus_settings.hpp"
#include "csurf_utils.hpp"
#include "reaper_plugin.h"
#include "reaper_plugin_functions.h"

CSurf_Context::CSurf_Context(const int nbChannels) : nbChannels(nbChannels) {
    if (nbChannels == 1) {
        device = FP_V2;
    } else {
        device = FP_8;
    }
    settings = ReaSonusSettings::GetInstance(device);
    shift_left = false;
    last_touched_fx_mode = false;
    shift_left_locked = false;
    shift_right = false;
    shift_right_locked = false;
    arm = false;
    show_time_code = false;
    master_fader_mode = false;
    panPushModePan = true;
    fader_disabled = false;
    pluginEditPluginId = 0;
    pluginEditParamId = 0;
    pluginEditTrack = nullptr;

    channelMode = TrackMode;
    single_point_automation = false;
}

/**************************************************************************
 * Modifiers
 **************************************************************************/
void CSurf_Context::SetShiftLeft(const bool val) {
    if (settings->GetSwapShiftButtons()) {
        shift_right = val;
    } else {
        shift_left = val;
    }
}

void CSurf_Context::SetShiftLeftLocked(const bool val) {
    if (settings->GetSwapShiftButtons()) {
        shift_right_locked = val;
    } else {
        shift_left_locked = val;
    }
}

bool CSurf_Context::GetShiftChannelLeft() const {
    if (!shift_left_locked) {
        return shift_left;
    }

    return !shift_left;
}

bool CSurf_Context::GetShiftLeft() const {
    return shift_left;
}

void CSurf_Context::SetShiftRight(const bool val) {
    if (settings->GetSwapShiftButtons()) {
        shift_left = val;
    } else {
        shift_right = val;
    }
}

void CSurf_Context::SetShiftRightLocked(const bool val) {
    if (settings->GetSwapShiftButtons()) {
        shift_left_locked = val;
    } else {
        shift_right_locked = val;
    }
}

bool CSurf_Context::GetShiftChannelRight() const {
    if (!shift_right_locked) {
        return shift_right;
    }

    return !shift_right;
}

bool CSurf_Context::GetShiftRight() const {
    return shift_right;
}

void CSurf_Context::SetArm(const bool val) {
    arm = val;
}

bool CSurf_Context::GetArm() const {
    return arm;
}

/**************************************************************************
 * Pan mode
 **************************************************************************/
void CSurf_Context::SetPanEncoderMode(const PanEncoderMode val) {
    panEncoderMode = val;
}

PanEncoderMode CSurf_Context::GetPanEncoderMode() const {
    return panEncoderMode;
}

/**************************************************************************
 * Display the timecode
 **************************************************************************/
void CSurf_Context::SetShowTimeCode(const bool value) {
    show_time_code = value;
}

void CSurf_Context::ToggleShowTimeCode() {
    show_time_code = !show_time_code;
}

bool CSurf_Context::GetShowTimeCode() const {
    return show_time_code;
}

/**************************************************************************
 * Last touched Fx param control
 **************************************************************************/
void CSurf_Context::ToggleLastTouchedFxMode() {
    SetLastTouchedFxMode(!last_touched_fx_mode);
}

void CSurf_Context::SetLastTouchedFxMode(const bool value) {
    if (value) {
        master_fader_mode = false;
    }
    last_touched_fx_mode = value;
}

bool CSurf_Context::GetLastTouchedFxMode() const {
    return last_touched_fx_mode;
}

/**************************************************************************
 * Master Fader mode
 **************************************************************************/
void CSurf_Context::ToggleMasterFaderMode() {
    SetMasterFaderMode(!master_fader_mode);
}

void CSurf_Context::SetMasterFaderMode(const bool value) {
    if (value) {
        last_touched_fx_mode = false;
    }
    master_fader_mode = value;
}

bool CSurf_Context::GetMasterFaderMode() const {
    if (nbChannels > 1) {
        return master_fader_mode && settings->GetMasterFaderModeEnabled();
    }

    return master_fader_mode;
}

/**************************************************************************
 * Number of device channels
 **************************************************************************/
void CSurf_Context::SetNbChannels(const int count) {
    nbChannels = count;
}

int CSurf_Context::GetNbChannels() const {
    // TODO: Should be only nbChannels
    return last_touched_fx_mode ? nbChannels - 1 : nbChannels;
}

int CSurf_Context::GetNbBankChannels() const {
    return last_touched_fx_mode || master_fader_mode ? nbChannels - 1 : nbChannels;
}

void CSurf_Context::TogglePanPushMode() {
    panPushModePan = !panPushModePan;
}

void CSurf_Context::ResetPanPushMode() {
    panPushModePan = true;
}

bool CSurf_Context::GetPanPushMode() const {
    return panPushModePan;
}

/**************************************************************************
 * Channel manager Item
 **************************************************************************/
void CSurf_Context::UpdateChannelManagerItemIndex(const int val) {
    val < 0
        ? DecrementChannelManagerItemIndex(abs(val))
        : IncrementChannelmanagerItemIndex(val);
}

void CSurf_Context::IncrementChannelmanagerItemIndex(const int val) {
    if (channelManagerItemIndex + val + (channelManagerType == Hui ? 0 : nbChannels) < channelManagerItemsCount) {
        channelManagerItemIndex += val;
    }
}

void CSurf_Context::DecrementChannelManagerItemIndex(const int val) {
    if (channelManagerItemIndex - val >= 0) {
        channelManagerItemIndex -= val;
    }
}

void CSurf_Context::ResetChannelManagerItemIndex() {
    channelManagerItemIndex = 0;
}

int CSurf_Context::GetChannelManagerItemIndex() const {
    return channelManagerItemIndex;
}

int CSurf_Context::GetChannelManagerItemIndex(const int max) const {
    return max < channelManagerItemIndex ? max : channelManagerItemIndex;
}

void CSurf_Context::SetChannelManagerItemsCount(const int count) {
    channelManagerItemsCount = count;
}

void CSurf_Context::ResetChannelManagerItemsCount() {
    channelManagerItemsCount = 0;
}

int CSurf_Context::GetPluginMaxGroupCount() {
    return 1024;
}

/**************************************************************************
 * Channel modes
 **************************************************************************/
bool CSurf_Context::IsChannelMode(const ChannelMode _channelMode) const {
    return channelMode == _channelMode;
}

void CSurf_Context::SetChannelMode(const ChannelMode _channelMode) {
    previousChannelMode = channelMode;
    channelMode = _channelMode;
}

ChannelMode CSurf_Context::GetChannelMode() const {
    return channelMode;
}

ChannelMode CSurf_Context::GetPreviousChannelMode() const {
    return previousChannelMode;
}

void CSurf_Context::SetPreviousPluginChannelMode(const ChannelMode _channelMode) {
    previousPluginChannelMode = _channelMode;
}

ChannelMode CSurf_Context::GetPreviousPluginChannelMode() const {
    return previousPluginChannelMode;
}

void CSurf_Context::SetPluginEditTrack(MediaTrack *track) {
    pluginEditTrack = track;
}

MediaTrack *CSurf_Context::GetPluginEditTrack() const {
    return pluginEditTrack;
}

void CSurf_Context::SetPluginEditPluginId(const int pluginId) {
    pluginEditPluginId = pluginId;
}

int CSurf_Context::GetPluginEditPluginId() const {
    return pluginEditPluginId;
}

void CSurf_Context::SetPluginEditParamId(const int pluginId) {
    pluginEditParamId = pluginId;
}

int CSurf_Context::GetPluginEditParamId() const {
    return pluginEditParamId;
}

/**************************************************************************
 * Add Send mode
 **************************************************************************/
void CSurf_Context::SetAddSendReceiveMode(const int value) {
    add_send_receive_mode = value;
}

int CSurf_Context::GetAddSendReceiveMode() const {
    return add_send_receive_mode;
}

void CSurf_Context::SetCurrentSelectedSendReceive(const int value) {
    current_selected_send_receive = minmax(0, value, CountTracks(nullptr) - 1);
}

void CSurf_Context::IncrementCurrentSelectedSendReceive() {
    SetCurrentSelectedSendReceive(current_selected_send_receive + 1);
}

void CSurf_Context::DecrementCurrentSelectedSendReceive() {
    SetCurrentSelectedSendReceive(current_selected_send_receive - 1);
}

int CSurf_Context::GetCurrentSelectedSendReceive() const {
    return current_selected_send_receive;
}

/**************************************************************************
 * Channel manager type
 **************************************************************************/
void CSurf_Context::SetChannelManagerType(const ChannelManagerType type) {
    channelManagerType = type;
}

void CSurf_Context::SetFaderDisabled(const bool value) {
    fader_disabled = value;
}

bool CSurf_Context::GetFaderDisabled() const {
    return fader_disabled;
}

/**************************************************************************
 * Single Point Automation
 **************************************************************************/
void CSurf_Context::SetSinglePointAutomation(const bool value) {
    single_point_automation = value;
}

bool CSurf_Context::IsSinglePointAutomationEnabled() const {
    return single_point_automation;
}

/**************************************************************************
 * Settings
 **************************************************************************/
std::string CSurf_Context::GetSetting(const std::string &group, const std::string &key) const {
    return settings->GetSetting(group, key);
}

std::array<int, 4> CSurf_Context::GetDisplayLineValues() const {
    return settings->GetTrackDisplayLineValues();
}

std::array<int, 4> CSurf_Context::GetDisplayAlignValues() const {
    return settings->GetTrackDisplayAlignValues();
}

std::array<int, 4> CSurf_Context::GetDisplayInvertValues() const {
    return settings->GetTrackDisplayInvertValues();
}
