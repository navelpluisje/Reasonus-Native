#include "csurf_reasonus_settings.hpp"
#include "../shared/csurf_utils.hpp"

ReaSonusSettings::ReaSonusSettings(std::string _device)
{
    device = _device;
    mINI::INIFile file(GetReaSonusIniPath(device));
    readAndCreateIni(settings, device);
};

void ReaSonusSettings::UpdateSettings()
{
    readAndCreateIni(settings, device);
}

void ReaSonusSettings::SetSetting(std::string group, std::string key, std::string value)
{
    settings[group][key] = value;
}

void ReaSonusSettings::SetSetting(std::string group, std::string key, bool value)
{
    settings[group][key] = value ? "1" : "0";
}

void ReaSonusSettings::SetSetting(std::string group, std::string key, int value)
{
    settings[group][key] = std::to_string(value);
}

void ReaSonusSettings::SetFunction(std::string index, std::string value, bool is_footswitch)
{
    std::string group = is_footswitch ? "footswitch" : "functions";
    SetSetting(group, index, value);
}

void ReaSonusSettings::SetAndSaveSetting(std::string group, std::string key, std::string value)
{
    SetSetting(group, key, value);
    StoreSettings();
}

void ReaSonusSettings::SetAndSaveFuntion(std::string key, std::string value, bool is_footswitch)
{
    SetFunction(key, value, is_footswitch);
    StoreSettings();
}

std::string ReaSonusSettings::GetSetting(std::string group, std::string key, std::string default_value)
{
    if (settings.has(group) && settings[group].has(key))
    {
        return settings[group][key];
    }
    else
    {
        return default_value;
    }
}

std::string ReaSonusSettings::GetFunction(std::string index, bool is_footswitch)
{
    std::string group = is_footswitch ? "footswitch" : "functions";

    return GetSetting(group, index, "0");
}

bool ReaSonusSettings::StoreSettings()
{
    mINI::INIFile file(GetReaSonusIniPath(device));
    if (!file.write(settings, true))
    {
        MB("Error while writing the settings file", "ReaSonus Settings Error", 0);
        return false;
    }

    return true;
};

/**
 * Settings for all devices
 */
bool ReaSonusSettings::GetMuteSoloMomentary()
{
    return settings["surface"]["mute-solo-momentary"] == "1";
};

int ReaSonusSettings::GetSurface()
{
    return stoi(settings["surface"]["surface"]);
}

int ReaSonusSettings::GetMidiInput()
{
    return stoi(settings["surface"]["midiin"]);
}

int ReaSonusSettings::GetMidiOutput()
{
    return stoi(settings["surface"]["midiout"]);
}

/**
 * Settings for the FaderPort 8 & 16
 */
bool ReaSonusSettings::GetDisablePluginControl()
{
    return settings["surface"]["disable-plugins"] == "1";
}

bool ReaSonusSettings::GetUntouchAfterLearn()
{
    return settings["surface"]["erase-last-param-after-learn"] == "1";
};

bool ReaSonusSettings::GetMasterFaderModeEnabled()
{
    return settings["surface"]["master-fader-mode"] == "1";
};

bool ReaSonusSettings::GetSwapShiftButtons()
{
    return settings["surface"]["swap-shift-buttons"] == "1";
};

bool ReaSonusSettings::GetFaderReset()
{
    return settings["surface"]["fader-reset"] == "1";
};

bool ReaSonusSettings::GetOverwriteTimeCode()
{
    return settings["surface"]["overwrite-time-code"] == "1";
};

int ReaSonusSettings::GetSurfaceTimeCode()
{
    return stoi(settings["surface"]["time-code"]);
}

bool ReaSonusSettings::GetDistractionFreeMode()
{
    return settings["surface"]["distraction-free"] == "1";
}

int ReaSonusSettings::GetTrackColorBrightness()
{
    return stoi(settings["surface"]["track-color-brightness"]);
}

double ReaSonusSettings::GetTrackColorBrightnessPercentage()
{
    return stoi(settings["surface"]["track-color-brightness"]) / 100.0;
}

bool ReaSonusSettings::GetLatchPreviewActionEnabled()
{
    return settings["surface"]["latch-preview-action"] == "1";
}

int ReaSonusSettings::GetLatchPreviewActionCode()
{
    return stoi(settings["surface"]["latch-preview-action-code"]);
}

int ReaSonusSettings::GetpluginStepSize()
{
    return stoi(settings["surface"]["plugin-step-size"]);
}

int ReaSonusSettings::GetTrackDisplay()
{
    return stoi(settings["displays"]["track"]);
}

/**
 * Settings for the FaderPort 2
 */
bool ReaSonusSettings::GetControlHiddenTracks()
{
    return settings["surface"]["control-hidden-tracks"] == "1";
};

bool ReaSonusSettings::GetCanDisableFader()
{
    return settings["surface"]["can-disable-fader"] == "1";
};

bool ReaSonusSettings::GetEndlessTrackScroll()
{
    return settings["surface"]["endless-track-scroll"] == "1";
}

/**
 * Filters related
 */
int ReaSonusSettings::GetNumberOfFilters()
{
    return stoi(settings["filters"]["nb-filters"]);
}

/**
 * @brief Create a new filter and return the index of the filter
 *
 * @return int The index of the newly created filter
 */
int ReaSonusSettings::AddNewFilter(std::string filter_name)
{
    std::string newKey = GenerateUniqueKey("filter_");
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

void ReaSonusSettings::UpdateFilter(std::string key, mINI::INIMap<std::string> filter)
{
    settings[key] = filter;
}

void ReaSonusSettings::UpdateFilterOrder(std::vector<std::string> keys)
{
    for (int i = 0; i < (int)keys.size(); i++)
    {
        settings["filters"][std::to_string(i)] = keys[i];
    }
    settings["filters"]["nb-filters"] = std::to_string(keys.size());
}

void ReaSonusSettings::Removefilter(std::string key)
{
    settings.remove(key);

    // Check if the key is in the vector and get its position
    // If we have a value, erase it from the vector
    std::vector<std::string> keys = GetFilterKeys();
    const auto it = std::find(keys.begin(), keys.end(), key);
    if (it != keys.end())
    {
        keys.erase(it);
    }

    // Clear all the filters and create the new list
    for (int i = 0; i < GetNumberOfFilters(); i++)
    {
        settings["filters"].remove(std::to_string(i));
    }

    UpdateFilterOrder(keys);
}

std::string ReaSonusSettings::GetFilterKeyByIndex(int index)
{
    return settings["filters"][std::to_string(index)];
}

std::vector<std::string> ReaSonusSettings::GetFilterKeys()
{
    std::vector<std::string> keys = {};

    for (int i = 0; i < GetNumberOfFilters(); i++)
    {
        keys.push_back(settings["filters"][std::to_string(i)]);
    }

    return keys;
}

std::vector<std::string> ReaSonusSettings::GetFilterNames()
{
    std::vector<std::string> names = {};

    for (int i = 0; i < GetNumberOfFilters(); i++)
    {
        names.push_back(settings[settings["filters"][std::to_string(i)]]["name"]);
    }

    return names;
}

mINI::INIMap<std::string> ReaSonusSettings::GetFilter(std::string key)
{
    return settings[key];
}

mINI::INIMap<std::string> ReaSonusSettings::GetFilter(int index)
{
    return GetFilter(GetFilterKeyByIndex(index));
}
