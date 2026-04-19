#ifndef CSURF_CONTEXT_H_
#define CSURF_CONTEXT_H_

#include "csurf_context_resources.hpp"
#include "csurf_utils.hpp"
#include "csurf_reasonus_settings.hpp"
#include "../controls/csurf_display_resources.hpp"

class CSurf_Context // NOLINT(*-use-internal-linkage)
{
    /**
     * @brief Number of channels
     */
    int nbChannels = 8;
    std::string device = FP_8;

    // Shift keys
    bool shift_left;
    bool shift_left_locked;
    bool shift_right;
    bool shift_right_locked;
    bool arm;

    // track mode show the time code in the displays
    bool show_time_code;

    // Last touched fx
    bool last_touched_fx_mode;

    // Make the fader control the master track for the FaderPort V2
    bool master_fader_mode;

    // Set the track id that has the add send mode. -1 is unselected
    int add_send_receive_mode = -1;
    int current_selected_send_receive = 0;

    // Pan encoder modes
    PanEncoderMode panEncoderMode = PanEncoderTrackPanMode;
    bool panPushModePan;

    // Channel manager
    int channelManagerItemIndex = 0;
    int channelManagerItemsCount = 0;

    // Plugin Edit fields
    MediaTrack *pluginEditTrack{nullptr};
    int pluginEditPluginId{0};
    int pluginEditParamId{0};

    // disable fader on v2
    bool fader_disabled{false};

    ChannelMode channelMode = TrackMode;
    ChannelMode previousChannelMode = TrackMode;
    ChannelMode previousPluginChannelMode = TrackMode;
    ChannelManagerType channelManagerType{};

    ReaSonusSettings *settings;

public:
    explicit CSurf_Context(const int nbChannels) : nbChannels(nbChannels) {
        if (nbChannels == 1) {
            device = FP_V2;
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
    }

    ~CSurf_Context() = default;

    /**************************************************************************
     * Modifiers
     **************************************************************************/
    void SetShiftLeft(const bool val) {
        if (settings->GetSwapShiftButtons()) {
            shift_right = val;
        } else {
            shift_left = val;
        }
    }

    void SetShiftLeftLocked(const bool val) {
        if (settings->GetSwapShiftButtons()) {
            shift_right_locked = val;
        } else {
            shift_left_locked = val;
        }
    }

    [[nodiscard]] bool GetShiftChannelLeft() const {
        if (!shift_left_locked) {
            return shift_left;
        }

        return !shift_left;
    }

    [[nodiscard]] bool GetShiftLeft() const {
        return shift_left;
    }

    void SetShiftRight(const bool val) {
        if (settings->GetSwapShiftButtons()) {
            shift_left = val;
        } else {
            shift_right = val;
        }
    }

    void SetShiftRightLocked(const bool val) {
        if (settings->GetSwapShiftButtons()) {
            shift_left_locked = val;
        } else {
            shift_right_locked = val;
        }
    }

    [[nodiscard]] bool GetShiftChannelRight() const {
        if (!shift_right_locked) {
            return shift_right;
        }

        return !shift_right;
    }

    [[nodiscard]] bool GetShiftRight() const {
        return shift_right;
    }

    void SetArm(const bool val) {
        arm = val;
    }

    [[nodiscard]] bool GetArm() const {
        return arm;
    }

    /**************************************************************************
     * Pan mode
     **************************************************************************/
    void SetPanEncoderMode(const PanEncoderMode val) {
        panEncoderMode = val;
    }

    [[nodiscard]] PanEncoderMode GetPanEncoderMode() const {
        return panEncoderMode;
    }

    /**************************************************************************
     * Display the timecode
     **************************************************************************/
    void SetShowTimeCode(const bool value) {
        show_time_code = value;
    }

    void ToggleShowTimeCode() {
        show_time_code = !show_time_code;
    }

    [[nodiscard]] bool GetShowTimeCode() const {
        return show_time_code;
    }

    /**************************************************************************
     * Last touched Fx param control
     **************************************************************************/
    void ToggleLastTouchedFxMode() {
        SetLastTouchedFxMode(!last_touched_fx_mode);
    }

    /**
     * @brief Set the Last Touched FX Mode. If value is true, reset master fader mode
     *
     * @param value
     */
    void SetLastTouchedFxMode(const bool value) {
        if (value) {
            master_fader_mode = false;
        }
        last_touched_fx_mode = value;
    }

    [[nodiscard]] bool GetLastTouchedFxMode() const {
        return last_touched_fx_mode;
    }

    /**************************************************************************
     * Master Fader mode
     **************************************************************************/
    /**
     * @brief Toggle the master fader mode
     */
    void ToggleMasterFaderMode() {
        SetMasterFaderMode(!master_fader_mode);
    }

    /**
     * @brief Set the Master Fader Mode. If value is true, reset last touched Fx mode
     *
     * @param value
     */
    void SetMasterFaderMode(const bool value) {
        if (value) {
            last_touched_fx_mode = false;
        }
        master_fader_mode = value;
    }

    [[nodiscard]] bool GetMasterFaderMode() const {
        if (nbChannels > 1) {
            return master_fader_mode && settings->GetMasterFaderModeEnabled();
        }

        return master_fader_mode;
    }

    /**************************************************************************
     * Number of device channels
     **************************************************************************/
    /**
     * @brief Set the Nb Channels. These are the nb of faders on the FaderPort
     *
     * @param count The number of faders
     */
    void SetNbChannels(const int count) {
        nbChannels = count;
    }

    /**
     * @brief Get the Nb Channels. These are the actual number if faders
     *
     * @return int The number of faders
     */
    [[nodiscard]] int GetNbChannels() const {
        // TODO: Should be only nbChannels
        return last_touched_fx_mode ? nbChannels - 1 : nbChannels;
    }

    /**
     * @brief Get the Nb Bank Channels. When lastTouchedFxMode is set, this will be 1 less the the actual fader count
     *
     * @return int The number of bank channels
     */
    [[nodiscard]] int GetNbBankChannels() const {
        return last_touched_fx_mode || master_fader_mode ? nbChannels - 1 : nbChannels;
    }

    void TogglePanPushMode() {
        panPushModePan = !panPushModePan;
    }

    void ResetPanPushMode() {
        panPushModePan = true;
    }

    [[nodiscard]] bool GetPanPushMode() const {
        return panPushModePan;
    }

    /**************************************************************************
     * Channel manager Item
     **************************************************************************/
    void UpdateChannelManagerItemIndex(const int val) {
        val < 0
            ? DecrementChannelManagerItemIndex(abs(val))
            : IncrementChannelmanagerItemIndex(val);
    }

    void IncrementChannelmanagerItemIndex(const int val) {
        if (channelManagerItemIndex + val + (channelManagerType == Hui ? 0 : nbChannels) < channelManagerItemsCount) {
            channelManagerItemIndex += val;
        }
    }

    void DecrementChannelManagerItemIndex(const int val) {
        if (channelManagerItemIndex - val >= 0) {
            channelManagerItemIndex -= val;
        }
    }

    void ResetChannelManagerItemIndex() {
        channelManagerItemIndex = 0;
    }

    [[nodiscard]] int GetChannelManagerItemIndex() const {
        return channelManagerItemIndex;
    }

    [[nodiscard]] int GetChannelManagerItemIndex(const int max) const {
        return max < channelManagerItemIndex ? max : channelManagerItemIndex;
    }

    void SetChannelManagerItemsCount(const int count) {
        channelManagerItemsCount = count;
    }

    void ResetChannelManagerItemsCount() {
        channelManagerItemsCount = 0;
    }

    static int GetPluginMaxGroupCount() {
        return 1024;
    }

    /**************************************************************************
     * Channel modes
     **************************************************************************/
    [[nodiscard]] bool IsChannelMode(const ChannelMode _channelMode) const {
        return channelMode == _channelMode;
    }

    void SetChannelMode(const ChannelMode _channelMode) {
        previousChannelMode = channelMode;
        channelMode = _channelMode;
    }

    [[nodiscard]] ChannelMode GetChannelMode() const {
        return channelMode;
    }

    [[nodiscard]] ChannelMode GetPreviousChannelMode() const {
        return previousChannelMode;
    }

    void SetPreviousPluginChannelMode(const ChannelMode _channelMode) {
        previousPluginChannelMode = _channelMode;
    }

    [[nodiscard]] ChannelMode GetPreviousPluginChannelMode() const {
        return previousPluginChannelMode;
    }

    void SetPluginEditTrack(MediaTrack *track) {
        pluginEditTrack = track;
    }

    [[nodiscard]] MediaTrack *GetPluginEditTrack() const {
        return pluginEditTrack;
    }

    void SetPluginEditPluginId(const int pluginId) {
        pluginEditPluginId = pluginId;
    }

    [[nodiscard]] int GetPluginEditPluginId() const {
        return pluginEditPluginId;
    }

    void SetPluginEditParamId(const int pluginId) {
        pluginEditParamId = pluginId;
    }

    [[nodiscard]] int GetPluginEditParamId() const {
        return pluginEditParamId;
    }

    /**************************************************************************
     * Add Send mode
     **************************************************************************/
    void SetAddSendReceiveMode(const int value) {
        add_send_receive_mode = value;
    }

    [[nodiscard]] int GetAddSendReceiveMode() const {
        return add_send_receive_mode;
    }

    void SetCurrentSelectedSendReceive(const int value) {
        current_selected_send_receive = minmax(0, value, CountTracks(nullptr) - 1);
    }

    void IncrementCurrentSelectedSendReceive() {
        SetCurrentSelectedSendReceive(current_selected_send_receive + 1);
    }

    void DecrementCurrentSelectedSendReceive() {
        SetCurrentSelectedSendReceive(current_selected_send_receive - 1);
    }

    [[nodiscard]] int GetCurrentSelectedSendReceive() const {
        return current_selected_send_receive;
    }

    /**************************************************************************
     * Channel manager type
     **************************************************************************/
    void SetChannelManagerType(const ChannelManagerType type) {
        channelManagerType = type;
    }

    void SetFaderDisabled(const bool value) {
        fader_disabled = value;
    }

    [[nodiscard]] bool GetFaderDisabled() const {
        return fader_disabled;
    }

    /**************************************************************************
     * Settings
     **************************************************************************/
    [[nodiscard]] std::string GetSetting(const std::string &group, const std::string &key) const {
        return settings->GetSetting(group, key);
    }

    [[nodiscard]] auto GetDisplayLineValues() const {
        return settings->GetTrackDisplayLineValues();
    }

    [[nodiscard]] auto GetDisplayAlignValues() const {
        return settings->GetTrackDisplayAlignValues();
    }

    [[nodiscard]] auto GetDisplayInvertValues() const {
        return settings->GetTrackDisplayInvertValues();
    }
};

#endif
