#ifndef CSURF_SETTINGS_HPP_
#define CSURF_SETTINGS_HPP_

#include <string>
#include <vector>
#include <mini/ini.h>
#include "../shared/csurf_utils.hpp"

class ReaSonusSettings
{
private:
    // Private Constructor
    ReaSonusSettings(std::string device);

    std::string device;
    mINI::INIStructure settings;
    static ReaSonusSettings *instance8Ptr;
    static ReaSonusSettings *instanceV2Ptr;

public:
    ReaSonusSettings(const ReaSonusSettings &obj) = delete;

    static ReaSonusSettings *GetInstance(std::string device)
    {
        if (device == FP_V2)
        {
            if (instanceV2Ptr == nullptr)
            {
                instanceV2Ptr = new ReaSonusSettings(device);
            }
            return instanceV2Ptr;
        }

        if (instance8Ptr == nullptr)
        {
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
    void SetSetting(std::string group, std::string key, std::string value);
    void SetSetting(std::string group, std::string key, bool value);
    void SetSetting(std::string group, std::string key, int value);

    /**
     * @brief Set a Setting and save them to the ini file
     *
     * @param group The settings group to write to
     * @param key The key within the coven settings group to write to
     * @param value The actual value to write
     */
    void SetAndSaveSetting(std::string group, std::string key, std::string value);

    /**
     * @brief Set a Setting without saving to file
     *
     * @param key index of the function to store
     * @param value The actual value to write
     * @param is_footswitch Wether or not if the function is for the foot switch
     */
    void SetFunction(std::string index, std::string value, bool is_footswitch = false);

    /**
     * @brief Set a Setting and save them to the ini file
     *
     * @param key index of the function to store
     * @param value The actual value to write
     * @param is_footswitch Wether or not if the function is for the foot switch
     */
    void SetAndSaveFuntion(std::string key, std::string value, bool is_footswitch = false);

    /**
     * @brief Get the Setting vy its group and key
     *
     * @param group
     * @param key
     * @return std::string
     */
    std::string GetSetting(std::string group, std::string key, std::string default_value = "");

    /**
     * @brief Get the Setting vy its group and key
     *
     * @param value The actual value to write
     * @param is_footswitch Wether or not if the function is for the foot switch
     * @return std::string
     */
    std::string GetFunction(std::string key, bool is_footswitch = false);

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
    int GetTrackDisplay();
    int GetSurface();
    int GetMidiInput();
    int GetMidiOutput();

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
    int AddNewFilter(std::string filter_name);
    std::vector<std::string> GetFilterKeys();
    void UpdateFilter(std::string key, mINI::INIMap<std::string> filter);
    void UpdateFilterOrder(std::vector<std::string> keys);
    void Removefilter(std::string key);
    std::string GetFilterKeyByIndex(int index);
    std::vector<std::string> GetFilterNames();
    mINI::INIMap<std::string> GetFilter(std::string key);
    mINI::INIMap<std::string> GetFilter(int index);
};

#endif