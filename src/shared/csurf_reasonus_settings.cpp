#include "csurf_reasonus_settings.hpp"

#include <utility>
#include "../shared/csurf_utils.hpp"

ReaSonusSettings::ReaSonusSettings(std::string _device) {
    device = std::move(_device);
    const mINI::INIFile file(GetReaSonusIniPath(device));
    ReadAndCreateIni(settings);
}

void ReaSonusSettings::UpdateSettings() {
    ReadAndCreateIni(settings);
}

void ReaSonusSettings::ReadAndCreateIni(mINI::INIStructure &data) const {
    const mINI::INIFile file(GetReaSonusIniPath(device));

    if (!file.read(data)) {
        RecursiveCreateDirectory(createPathName({std::string(GetResourcePath()), "ReaSonus", "Plugins"}).c_str(), 0);
        for (auto setting: shared_settings) {
            data[setting.at(0)][setting.at(1)] = setting.at(2);
        }

        if (device == FP_V2) {
            for (auto setting: fp_v2_settings) {
                data[setting.at(0)][setting.at(1)] = setting.at(2);
            }
        }
        if (device == FP_8) {
            for (auto setting: fp_8_settings) {
                data[setting.at(0)][setting.at(1)] = setting.at(2);
            }
        }
        if (!file.generate(data, true)) {
            MB("Error while creating the ini file. Please contact via the forum", "ReaSonus Error", 0);
        }
    } else {
        ValidateReaSonusIni(file, data);
    }
    file.read(data);
}

void ReaSonusSettings::ValidateReaSonusIni(const mINI::INIFile &file, mINI::INIStructure &data) const {
    bool has_missing_settings = false;

    for (auto setting: shared_settings) {
        if (!data[setting.at(0)].has(setting.at(1))) {
            data[setting.at(0)][setting.at(1)] = setting.at(2);
            has_missing_settings = true;
        }
    }

    if (device == FP_V2) {
        for (auto setting: fp_v2_settings) {
            if (!data[setting.at(0)].has(setting.at(1))) {
                data[setting.at(0)][setting.at(1)] = setting.at(2);
                has_missing_settings = true;
            }
        }
    }

    if (device == FP_8) {
        for (auto setting: fp_8_settings) {
            if (!data[setting.at(0)].has(setting.at(1))) {
                data[setting.at(0)][setting.at(1)] = setting.at(2);
                has_missing_settings = true;
            }
        }
    }

    if (has_missing_settings) {
        file.write(data, true);
    }
}

void ReaSonusSettings::SetSetting(const std::string &group, const std::string &key, const std::string &value) {
    settings[group][key] = value;
}

void ReaSonusSettings::SetSetting(const std::string &group, const std::string &key, const bool value) {
    settings[group][key] = value ? "1" : "0";
}

void ReaSonusSettings::SetSetting(const std::string &group, const std::string &key, const int value) {
    settings[group][key] = std::to_string(value);
}

void ReaSonusSettings::SetAndSaveSetting(const std::string &group, const std::string &key, const std::string &value) {
    SetSetting(group, key, value);
    StoreSettings();
}

void ReaSonusSettings::SetFunction(const std::string &index, const std::string &value, const bool is_footswitch) {
    const std::string group = is_footswitch ? "footswitch" : "functions";
    SetSetting(group, index, value);
}

void ReaSonusSettings::SetAndSaveFuntion(const std::string &key, const std::string &value, const bool is_footswitch) {
    SetFunction(key, value, is_footswitch);
    StoreSettings();
}

std::string ReaSonusSettings::GetSetting(const std::string &group, const std::string &key, std::string default_value) {
    if (settings.has(group) && settings[group].has(key)) {
        return settings[group][key];
    }

    return default_value;
}

std::string ReaSonusSettings::GetFunction(const std::string &key, const bool is_footswitch) {
    const std::string group = is_footswitch ? "footswitch" : "functions";

    return GetSetting(group, key, "0");
}

bool ReaSonusSettings::StoreSettings() {
    const mINI::INIFile file(GetReaSonusIniPath(device));

    if (!file.write(settings, true)) {
        MB("Error while writing the settings file", "ReaSonus Settings Error", 0);
        return false;
    }

    return true;
}

/**
 * Settings for all devices
 */
bool ReaSonusSettings::GetMuteSoloMomentary() {
    return settings["surface"]["mute-solo-momentary"] == "1";
}

/**
 * Settings for the FaderPort 8 & 16
 */
bool ReaSonusSettings::GetDisablePluginControl() {
    return settings["surface"]["disable-plugins"] == "1";
}

bool ReaSonusSettings::GetUntouchAfterLearn() {
    return settings["surface"]["erase-last-param-after-learn"] == "1";
}

bool ReaSonusSettings::GetMasterFaderModeEnabled() {
    return settings["surface"]["master-fader-mode"] == "1";
}

bool ReaSonusSettings::GetSwapShiftButtons() {
    return settings["surface"]["swap-shift-buttons"] == "1";
}

bool ReaSonusSettings::GetFaderReset() {
    return settings["surface"]["fader-reset"] == "1";
}

bool ReaSonusSettings::GetOverwriteTimeCode() {
    return settings["surface"]["overwrite-time-code"] == "1";
}

int ReaSonusSettings::GetSurfaceTimeCode() {
    return stoi(settings["surface"]["time-code"]);
}

bool ReaSonusSettings::GetDistractionFreeMode() {
    return settings["surface"]["distraction-free"] == "1";
}

int ReaSonusSettings::GetTrackColorBrightness() {
    return stoi(settings["surface"]["track-color-brightness"]);
}

double ReaSonusSettings::GetTrackColorBrightnessPercentage() {
    return stoi(settings["surface"]["track-color-brightness"]) / 100.0;
}

bool ReaSonusSettings::GetLatchPreviewActionEnabled() {
    return settings["surface"]["latch-preview-action"] == "1";
}

int ReaSonusSettings::GetLatchPreviewActionCode() {
    return stoi(settings["surface"]["latch-preview-action-code"]);
}

int ReaSonusSettings::GetpluginStepSize() {
    return stoi(settings["surface"]["plugin-step-size"]);
}

int ReaSonusSettings::GetTrackDisplay() {
    return stoi(settings["displays"]["track"]);
}

int *ReaSonusSettings::GetTrackDisplayLineValues() {
    return ConvertDisplayValues(settings["displays"]["track-lines"]);
}

int *ReaSonusSettings::GetTrackDisplayAlignValues() {
    return ConvertDisplayValues(settings["displays"]["track-alignment"]);
}

int *ReaSonusSettings::GetTrackDisplayInvertValues() {
    return ConvertDisplayValues(settings["displays"]["track-invert"]);
}

int ReaSonusSettings::GetTrackValueBarMode() {
    return stoi(settings["displays"]["track-value-bar-mode"]);
}

int ReaSonusSettings::GetTrackValueBarValue() {
    return stoi(settings["displays"]["track-value-bar-value"]);
}

int ReaSonusSettings::GetSurface() {
    return stoi(settings["surface"]["surface"]);
}

int ReaSonusSettings::GetMidiInput() {
    return stoi(settings["surface"]["midiin"]);
}

int ReaSonusSettings::GetMidiOutput() {
    return stoi(settings["surface"]["midiout"]);
}

bool ReaSonusSettings::ShouldClearParamInput() {
    return stoi(settings["surface"]["plugin-map-param-clear"]) > 0;
}

/**
 * Settings for the FaderPort 2
 */
bool ReaSonusSettings::GetControlHiddenTracks() {
    return settings["surface"]["control-hidden-tracks"] == "1";
}

bool ReaSonusSettings::GetCanDisableFader() {
    return settings["surface"]["can-disable-fader"] == "1";
}

bool ReaSonusSettings::GetEndlessTrackScroll() {
    return settings["surface"]["endless-track-scroll"] == "1";
}

/**
 * Filters related
 */
int ReaSonusSettings::GetNumberOfFilters() {
    return stoi(settings["filters"]["nb-filters"]);
}

/**
 * @brief Create a new filter and return the index of the filter
 *
 * @return int The index of the newly created filter
 */
int ReaSonusSettings::AddNewFilter(const std::string &filter_name) {
    const std::string newKey = GenerateUniqueKey("filter_");

    settings["filters"][settings["filters"]["nb-filters"]] = newKey;
    settings["filters"]["nb-filters"] = std::to_string(GetNumberOfFilters() + 1);

    settings[newKey];
    settings[newKey]["name"] = filter_name;
    settings[newKey]["text"] = "";
    settings[newKey]["case-insensitive"] = "0";
    settings[newKey]["sibblings"] = "0";
    settings[newKey]["parents"] = "0";
    settings[newKey]["children"] = "0";
    settings[newKey]["top-level"] = "0";
    settings[newKey]["match-multiple"] = "0";

    return GetNumberOfFilters() - 1;
}

std::vector<std::string> ReaSonusSettings::GetFilterKeys() {
    std::vector<std::string> keys = {};
    keys.reserve(GetNumberOfFilters());

    for (int i = 0; i < GetNumberOfFilters(); i++) {
        keys.push_back(settings["filters"][std::to_string(i)]);
    }

    return keys;
}

void ReaSonusSettings::UpdateFilter(const std::string &key, const mINI::INIMap<std::string> &filter) {
    settings[key] = filter;
}

void ReaSonusSettings::UpdateFilterOrder(const std::vector<std::string> &keys) {
    for (int i = 0; i < static_cast<int>(keys.size()); i++) {
        settings["filters"][std::to_string(i)] = keys[i];
    }
    settings["filters"]["nb-filters"] = std::to_string(keys.size());
}

void ReaSonusSettings::Removefilter(const std::string &key) {
    settings.remove(key);

    // Check if the key is in the vector and get its position
    // If we have a value, erase it from the vector
    std::vector<std::string> keys = GetFilterKeys();
    const auto itterator = std::find(keys.begin(), keys.end(), key);
    if (itterator != keys.end()) {
        keys.erase(itterator);
    }

    // Clear all the filters and create the new list
    for (int i = 0; i < GetNumberOfFilters(); i++) {
        settings["filters"].remove(std::to_string(i));
    }

    UpdateFilterOrder(keys);
}

std::string ReaSonusSettings::GetFilterKeyByIndex(const int index) {
    return settings["filters"][std::to_string(index)];
}

std::vector<std::string> ReaSonusSettings::GetFilterNames() {
    std::vector<std::string> names = {};
    names.reserve(GetNumberOfFilters());

    for (int i = 0; i < GetNumberOfFilters(); i++) {
        names.push_back(settings[settings["filters"][std::to_string(i)]]["name"]);
    }

    return names;
}

mINI::INIMap<std::string> ReaSonusSettings::GetFilter(const std::string &key) {
    return settings[key];
}

mINI::INIMap<std::string> ReaSonusSettings::GetFilter(const int index) {
    return GetFilter(GetFilterKeyByIndex(index));
}

int *ReaSonusSettings::ConvertDisplayValues(const std::string &settings_value) {
    const std::string delimiter = ",";
    int value[4] = {};

    if (!settings_value.empty()) {
        int start = 0;
        int index = 0;
        do {
            // Find the index of occurrence
            const int idx = static_cast<int>(settings_value.find(delimiter, start));
            if (idx == static_cast<int>(std::string::npos)) {
                break;
            }

            // If found add the substring till that
            // occurrence in the vector
            const int length = idx - start;
            value[index] = stoi(settings_value.substr(start, length));

            start += static_cast<int>(length + delimiter.size());
            index++;
        } while (true);

        value[index] = stoi(settings_value.substr(start));
    }

    return value;
}
