#ifndef CSURF_CONTEXT_H_
#define CSURF_CONTEXT_H_

#include <array>
#include <map>
#include <string>
#include "csurf_context_resources.hpp"
#include "csurf_reasonus_settings.hpp"
#include "reaper_plugin.h"

class CSurf_Context {
    /**
     * @brief Number of channels
     */
    int nbChannels;
    std::string device;

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
    std::map<ChannelMode, int> channelManagerItemIndex;
    std::map<ChannelMode, int> channelManagerItemsCount;

    // Plugin Edit fields
    MediaTrack *pluginEditTrack;
    int pluginEditPluginId;
    int pluginEditParamId;

    // disable fader on v2
    bool fader_disabled;

    // Single Point Automation
    bool single_point_automation;

    ChannelMode channelMode;
    ChannelMode previousChannelMode = TrackMode;
    ChannelMode previousPluginChannelMode = TrackMode;
    ChannelManagerType channelManagerType{};

    ReaSonusSettings *settings;

public:
    explicit CSurf_Context(int nbChannels);

    ~CSurf_Context() = default;

    static int MOMENTARY_TIMEOUT;
    /**************************************************************************
     * Modifiers
     **************************************************************************/
    void SetShiftLeft(bool val);

    void SetShiftLeftLocked(bool val);

    [[nodiscard]] bool GetShiftChannelLeft() const;

    [[nodiscard]] bool GetShiftLeft() const;

    void SetShiftRight(bool val);

    void SetShiftRightLocked(bool val);

    [[nodiscard]] bool GetShiftChannelRight() const;

    [[nodiscard]] bool GetShiftRight() const;

    void SetArm(bool val);

    [[nodiscard]] bool GetArm() const;

    /**************************************************************************
     * Pan mode
     **************************************************************************/
    void SetPanEncoderMode(PanEncoderMode val);

    [[nodiscard]] PanEncoderMode GetPanEncoderMode() const;

    /**************************************************************************
     * Display the timecode
     **************************************************************************/
    void SetShowTimeCode(bool value);

    void ToggleShowTimeCode();

    [[nodiscard]] bool GetShowTimeCode() const;

    /**************************************************************************
     * Last touched Fx param control
     **************************************************************************/
    void ToggleLastTouchedFxMode();

    /**
     * @brief Set the Last Touched FX Mode. If value is true, reset master fader mode
     *
     * @param value
     */
    void SetLastTouchedFxMode(bool value);

    [[nodiscard]] bool GetLastTouchedFxMode() const;

    /**************************************************************************
     * Master Fader mode
     **************************************************************************/
    /**
     * @brief Toggle the master fader mode
     */
    void ToggleMasterFaderMode();

    /**
     * @brief Set the Master Fader Mode. If value is true, reset last touched Fx mode
     *
     * @param value
     */
    void SetMasterFaderMode(bool value);

    [[nodiscard]] bool GetMasterFaderMode() const;

    /**************************************************************************
     * Number of device channels
     **************************************************************************/
    /**
     * @brief Set the Nb Channels. These are the nb of faders on the FaderPort
     *
     * @param count The number of faders
     */
    void SetNbChannels(int count);

    /**
     * @brief Get the Nb Channels. These are the actual number if faders
     *
     * @return int The number of faders
     */
    [[nodiscard]] int GetNbChannels() const;

    /**
     * @brief Get the Nb Bank Channels. When lastTouchedFxMode is set, this will be 1 less the the actual fader count
     *
     * @return int The number of bank channels
     */
    [[nodiscard]] int GetNbBankChannels() const;

    void TogglePanPushMode();

    void ResetPanPushMode();

    [[nodiscard]] bool GetPanPushMode() const;

    /**************************************************************************
     * Channel manager Item
     **************************************************************************/
    void UpdateChannelManagerItemIndex(ChannelMode channel_mode, int val);

    void IncrementChannelmanagerItemIndex(ChannelMode channel_mode, int val);

    void DecrementChannelManagerItemIndex(ChannelMode channel_mode, int val);

    void ResetChannelManagerItemIndex(ChannelMode channel_mode);

    int GetChannelManagerItemIndex(ChannelMode channel_mode);

    int GetChannelManagerItemIndex(ChannelMode channel_mode, int max);

    void ResetChannelManagerItemIndex();

    [[nodiscard]] int GetChannelManagerItemIndex(ChannelMode channel_mode) const;

    [[nodiscard]] int GetChannelManagerItemIndex(ChannelMode channel_mode, int max) const;

    void SetChannelManagerItemsCount(ChannelMode channel_mode, int count);

    void ResetChannelManagerItemsCount();

    void ResetChannelManagerItemsCount(ChannelMode channel_mode);

    static int GetPluginMaxGroupCount();

    /**************************************************************************
     * Channel modes
     **************************************************************************/
    [[nodiscard]] bool IsChannelMode(ChannelMode _channelMode) const;

    void SetChannelMode(ChannelMode _channelMode);

    [[nodiscard]] ChannelMode GetChannelMode() const;

    [[nodiscard]] ChannelMode GetPreviousChannelMode() const;

    void SetPreviousPluginChannelMode(ChannelMode _channelMode);

    [[nodiscard]] ChannelMode GetPreviousPluginChannelMode() const;

    void SetPluginEditTrack(MediaTrack *track);

    [[nodiscard]] MediaTrack *GetPluginEditTrack() const;

    void SetPluginEditPluginId(int pluginId);

    [[nodiscard]] int GetPluginEditPluginId() const;

    void SetPluginEditParamId(int pluginId);

    [[nodiscard]] int GetPluginEditParamId() const;

    /**************************************************************************
     * Add Send mode
     **************************************************************************/
    void SetAddSendReceiveMode(int value);

    [[nodiscard]] int GetAddSendReceiveMode() const;

    void SetCurrentSelectedSendReceive(int value);

    void IncrementCurrentSelectedSendReceive();

    void DecrementCurrentSelectedSendReceive();

    [[nodiscard]] int GetCurrentSelectedSendReceive() const;

    /**************************************************************************
     * Channel manager type
     **************************************************************************/
    void SetChannelManagerType(ChannelManagerType type);

    void SetFaderDisabled(bool value);

    [[nodiscard]] bool GetFaderDisabled() const;

    /**************************************************************************
     * Single Point Automation
     **************************************************************************/
    void SetSinglePointAutomation(bool value);

    [[nodiscard]] bool IsSinglePointAutomationEnabled() const;

    /**************************************************************************
     * Settings
     **************************************************************************/
    [[nodiscard]] std::string GetSetting(const std::string &group, const std::string &key) const;

    [[nodiscard]] std::array<int, 4> GetDisplayLineValues() const;

    [[nodiscard]] std::array<int, 4> GetDisplayAlignValues() const;

    [[nodiscard]] std::array<int, 4> GetDisplayInvertValues() const;
};

#endif
