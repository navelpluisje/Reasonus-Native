#ifndef CSURF_SETTINGS_HPP_
#define CSURF_SETTINGS_HPP_

#include <string>
#include <vector>
#include <mini/ini.h>
#include "../shared/csurf_utils.hpp"

class ReaSonusSettings {
    // Private Constructor
    explicit ReaSonusSettings(std::string _device);

    std::string device;
    mINI::INIStructure settings;
    static ReaSonusSettings *instance8Ptr;
    static ReaSonusSettings *instanceV2Ptr;

    const std::vector<std::vector<std::string> > shared_settings = {
        {"surface", "midiin", "0"},
        {"surface", "midiout", "0"},
        {"surface", "mute-solo-momentary", "0"},
        {"surface", "latch-preview-action", "0"},
        {"surface", "latch-preview-action-code", "42013"},
        {"functions", "1", "0"},
        {"functions", "2", "0"},
        {"functions", "3", "0"},
        {"functions", "4", "0"},
        {"footswitch", "1", "0"},
        {"footswitch", "2", "0"},
        {"footswitch", "3", "0"},
    };

    const std::vector<std::vector<std::string> > fp_v2_settings = {
        {"surface", "control-hidden-tracks", "0"},
        {"surface", "can-disable-fader", "0"},
        {"surface", "endless-track-scroll", "0"},
    };

    const std::vector<std::vector<std::string> > fp_8_settings = {
        {"surface", "surface", "0"},
        {"surface", "disable-plugins", "0"},
        {"surface", "distraction-free", "0"},
        {"surface", "erase-last-param-after-learn", "0"},
        {"surface", "master-fader-mode", "0"},
        {"surface", "swap-shift-buttons", "0"},
        {"surface", "fader-reset", "0"},
        {"surface", "overwrite-time-code", "1"},
        {"surface", "time-code", "2"},
        {"surface", "track-color-brightness", "25"},
        {"surface", "plugin-step-size", "1"},
        {"surface", "plugin-map-param-clear", "0"},
        {"displays", "track", "8"},
        {"displays", "track-lines", "0,4,1,2"},
        {"displays", "track-alignment", "1,0,0,0"},
        {"displays", "track-invert", "0,0,0,0"},
        {"displays", "track-value-bar-mode", "1"},
        {"displays", "track-value-bar-value", "0"},
        {"functions", "5", "0"},
        {"functions", "6", "0"},
        {"functions", "7", "0"},
        {"functions", "8", "0"},
        {"functions", "9", "0"},
        {"functions", "10", "0"},
        {"functions", "11", "0"},
        {"functions", "12", "0"},
        {"functions", "13", "0"},
        {"functions", "14", "0"},
        {"functions", "15", "0"},
        {"functions", "16", "0"},
        {"filters", "nb-filters", "0"},
    };

public:
    ReaSonusSettings(const ReaSonusSettings &obj) = delete;

    static ReaSonusSettings *GetInstance(const std::string &device) {
        if (device == FP_V2) {
            if (instanceV2Ptr == nullptr) {
                instanceV2Ptr = new ReaSonusSettings(device);
            }
            return instanceV2Ptr;
        }

        if (instance8Ptr == nullptr) {
            instance8Ptr = new ReaSonusSettings(device);
        }
        return instance8Ptr;
    }

    /**
     * @brief Read the ini file
     *
     */
    void UpdateSettings();

    void ReadAndCreateIni(mINI::INIStructure &data) const;

    void ValidateReaSonusIni(const mINI::INIFile &file, mINI::INIStructure &data) const;

    /**
     * @brief Set a Setting without saving to file
     *
     * @param group The settings group to write to
     * @param key The key within the coven settings group to write to
     * @param value The actual value to write
     */
    void SetSetting(const std::string &group, const std::string &key, const std::string &value);

    void SetSetting(const std::string &group, const std::string &key, bool value);

    void SetSetting(const std::string &group, const std::string &key, int value);

    /**
     * @brief Set a Setting and save them to the ini file
     *
     * @param group The settings group to write to
     * @param key The key within the coven settings group to write to
     * @param value The actual value to write
     */
    void SetAndSaveSetting(const std::string &group, const std::string &key, const std::string &value);

    /**
     * @brief Set a Setting without saving to file
     *
     * @param index index of the function to store
     * @param value The actual value to write
     * @param is_footswitch Wether or not if the function is for the foot switch
     */
    void SetFunction(const std::string &index, const std::string &value, bool is_footswitch = false);

    /**
     * @brief Set a Setting and save them to the ini file
     *
     * @param key index of the function to store
     * @param value The actual value to write
     * @param is_footswitch Wether or not if the function is for the foot switch
     */
    void SetAndSaveFuntion(const std::string &key, const std::string &value, bool is_footswitch = false);

    /**
     * @brief Get the Setting vy its group and key
     *
     * @param group
     * @param key
     * @return std::string
     */
    std::string GetSetting(const std::string &group, const std::string &key, std::string default_value = "");

    /**
     * @brief Get the Setting vy its group and key
     *
     * @param key The actual value to write
     * @param is_footswitch Wether or not if the function is for the foot switch
     * @return std::string
     */
    std::string GetFunction(const std::string &key, bool is_footswitch = false);

    /**
     * @brief Save the current settng sobeject to the file
     */
    bool StoreSettings();

    /**
     * Settings for all devices
     */
    bool GetMuteSoloMomentary();

    /**
     * Settings for the FaderPort 8 & 16
     */
    bool GetDisablePluginControl();

    bool GetUntouchAfterLearn();

    bool GetMasterFaderModeEnabled();

    bool GetSwapShiftButtons();

    bool GetFaderReset();

    bool GetOverwriteTimeCode();

    int GetSurfaceTimeCode();

    bool GetDistractionFreeMode();

    int GetTrackColorBrightness();

    double GetTrackColorBrightnessPercentage();

    bool GetLatchPreviewActionEnabled();

    int GetLatchPreviewActionCode();

    int GetpluginStepSize();

    int GetTrackDisplay();

    std::array<int, 4> GetTrackDisplayLineValues();

    std::array<int, 4> GetTrackDisplayAlignValues();

    std::array<int, 4> GetTrackDisplayInvertValues();

    int GetTrackValueBarMode();

    int GetTrackValueBarValue();

    int GetSurface();

    int GetMidiInput();

    int GetMidiOutput();

    bool ShouldClearParamInput();

    /**
     * Settings for the FaderPort 2
     */
    bool GetControlHiddenTracks();

    bool GetCanDisableFader();

    bool GetEndlessTrackScroll();

    /**
     * Filters related
     */
    int GetNumberOfFilters();

    int AddNewFilter(const std::string &filter_name);

    std::vector<std::string> GetFilterKeys();

    void UpdateFilter(const std::string &key, const mINI::INIMap<std::string> &filter);

    void UpdateFilterOrder(const std::vector<std::string> &keys);

    void Removefilter(const std::string &key);

    std::string GetFilterKeyByIndex(int index);

    std::vector<std::string> GetFilterNames();

    mINI::INIMap<std::string> GetFilter(const std::string &key);

    mINI::INIMap<std::string> GetFilter(int index);

    static std::array<int, 4> ConvertDisplayValues(const std::string &settings_value);
};

#endif
