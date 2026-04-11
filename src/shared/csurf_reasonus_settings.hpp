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

    int GetTrackValueBarType();

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
