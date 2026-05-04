#include "../shared/csurf_utils.hpp"
#include "../shared/csurf.h"
#include "csurf_daw.hpp"
#include <WDL/db2val.h>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <ShlObj_core.h>
#endif

void Main_OnCommandStringEx(const std::string &action_name, const int flag, ReaProject *proj) {
    const int actionId = NamedCommandLookup(action_name.c_str());
    Main_OnCommandEx(actionId, flag, proj);
}

void Main_OnCommandAsyncEx(const int action_id, const int flag, const ReaProject *proj) {
    (void) proj;
    if (!PostMessage(GetMainHwnd(), WM_COMMAND, action_id, flag)) {
        // TODO: Add LOG later.
    }
}

void SetActionState(const int actionId) {
    const int state = GetToggleCommandStateEx(0, actionId);
    if (state == 1) {
        Main_OnCommandAsyncEx(actionId, 0, nullptr);
    }
}

void SetActionState(const std::string &action_name) {
    const int actionId = NamedCommandLookup(action_name.c_str());
    SetActionState(actionId);
}

void SetActionState(const int actionId, const int new_state) {
    SetToggleCommandState(0, actionId, new_state);
    RefreshToolbar2(0, actionId);
}

void SetActionState(const std::string &action_name, const int new_state) {
    const int actionId = NamedCommandLookup(action_name.c_str());
    SetActionState(actionId, new_state);
}

bool GetToggleCommandIdState(const int action_id) {
    return GetToggleCommandState(action_id) != 0;
}

bool GetToggleCommandStringState(const std::string &action_name) {
    const int action_id = NamedCommandLookup(action_name.c_str());
    return GetToggleCommandState(action_id) != 0;
}

int GetIntConfigVar(const std::string &var_name) {
    return ConfigVar<int>(var_name);
}

bool SetIntConfigVar(const std::string &var_name, const int value) {
    ConfigVar<int> var(var_name);
    return var.SetValue(value);
}


bool hasBit(const int val, const int key) {
    return (val & 1 << key) != 0;
}

double volToNormalized(const double vol) {
    const double normalized_volume = DB2SLIDER(VAL2DB(vol)) / 1000.0;
    if (normalized_volume < 0.0) {
        return 0.0;
    }

    if (normalized_volume > 1.0) {
        return 1.0;
    }

    return normalized_volume;
}

double int14ToNormalized(const unsigned char msb, const unsigned char lsb) {
    const int val = lsb | msb << 7;
    double normalizedVal = val / 16383.0;

    normalizedVal = normalizedVal < 0.0 ? 0.0 : normalizedVal;
    normalizedVal = normalizedVal > 1.0 ? 1.0 : normalizedVal;

    return normalizedVal;
}

double normalizedToPan(const double val) {
    return 2.0 * val - 1.0;
}

double panToNormalized(const double val) {
    return 0.5 * (val + 1.0);
}

double int14ToVol(const unsigned char msb, const unsigned char lsb) {
    const int val = lsb | msb << 7;
    double pos = static_cast<double>(val) * 1000.0 / 16383.0;
    pos = SLIDER2DB(pos);

    return DB2VAL(pos);
}

std::string StripPluginName(const std::string &plugin_name) {
    std::vector<std::string> pluginNameParts = split(
        StripPluginNamePrefixes(StripPluginChannelPostfix(plugin_name.data()).data()), " ("
    );

    if (pluginNameParts.size() > 1) {
        pluginNameParts.pop_back();
    }

    return join(pluginNameParts, " (");
}

std::string StripPluginDeveloper(const std::string &plugin_name) {
    const std::vector<std::string> pluginNameParts = split(
        StripPluginNamePrefixes(StripPluginChannelPostfix(plugin_name.data()).data()), " ("
    );

    std::string developer = pluginNameParts.at(pluginNameParts.size() - 1);
    if (!developer.empty()) {
        developer.pop_back();
    }

    return developer;
}

std::string StripPluginNamePrefixes(char const *name) {
    std::vector<std::string> splitted_name = split(std::string(name), PREFIX_SEPARATOR);

    if (splitted_name.empty()) {
        return {name};
    }

    return splitted_name[splitted_name.size() - 1];
}

std::string StripPluginChannelPostfix(char const *name) {
    std::vector<std::string> splitted_name = split(std::string(name), " (");
    splitted_name.pop_back();

    return join(splitted_name, " (");
}

bool IsPluginFX(std::string name) {
    const int pos = static_cast<int>(name.find(PREFIX_SEPARATOR));
    // If we can not find the delimiter, we can not determine the type of plugin, so asume it's an effect
    if (pos < 0) {
        return true;
    }
    name.erase(pos, name.length());
    name.erase(0, name.length() - 1);

    return name != "i";
}

std::string Progress(const int current, const int total) {
    char buffer[127];
    snprintf(buffer, sizeof(buffer), "%d/%d", current, total);
    return {buffer};
}

std::string GetSendModeString(const int sendMode) {
    switch (sendMode) {
        case 0:
            return "Post-Fdr";
        case 1:
            return "Pre-FX";
        case 2:
        case 3:
            return "Post-FX";
        default:
            return "Post-Fdr";
    }
}

std::string GetAutomationString(const int automationMode) {
    switch (automationMode) {
        case -1:
            return "Track";
        case 0:
            return "Trim";
        case 1:
            return "Read";
        case 2:
            return "Touch";
        case 3:
            return "Write";
        case 4:
            return "Latch";
        case 5:
            return "Preview";
        default:
            return "Trim";
    }
}

std::string GetReaSonusFolderPath() {
    const auto recource_path = std::string(GetResourcePath());
    return createPathName({recource_path, "ReaSonus"});
}

std::string GetReaSonusIniPath(const std::string &device) {
    return createPathName({GetReaSonusFolderPath(), device + ".ini"});
}

std::string GetReaSonusZonesPath() {
    return createPathName({std::string(GetResourcePath()), "CSI", "Zones", "ReasonusFaderPort", "_ReaSonusEffects"});
}

std::string GetReaSonusPluginPath(
    std::string developer,
    const std::string &plugin_name,
    const std::string &plugin_type,
    const bool create
) {
    const std::string path = createPathName({
        std::string(GetResourcePath()), "ReaSonus", "Plugins", std::move(developer)
    });

    if (create) {
        createPathIfNotExist(path);
    }
    return createPathName({path, plugin_name + "." + plugin_type + ".ini"});
}

std::string GetReaSonusLocalesFolderPath() {
    return createPathName({GetReaSonusFolderPath(), "Locales"});
}

std::string GetReaSonusLocalesPath(const std::string &language) {
    return createPathName({GetReaSonusLocalesFolderPath(), language + ".ini"});
}

std::string GetReaSonusLocalesRootFile() {
    return createPathName({std::string(GetResourcePath()), "UserPlugins", "ReaSonus", "en-US.ini"});
}

bool isInteger(const std::string &value) {
    char *p;
    strtol(value.c_str(), &p, 10);
    return *p == 0;
}

std::vector<std::string> split(const std::string &str, const std::string &delimiter) {
    std::vector<std::string> value;

    if (!str.empty()) {
        int start = 0;
        do {
            // Find the index of occurrence
            const int idx = static_cast<int>(str.find(delimiter, start));
            if (idx == static_cast<int>(std::string::npos)) {
                break;
            }

            // If found add the substring till that
            // occurrence in the vector
            const int length = idx - start;
            value.push_back(str.substr(start, length));
            start += static_cast<int>(length + delimiter.size());
        } while (true);
        value.push_back(str.substr(start));
    }

    return value;
}

std::vector<std::string> cutString(const std::string &str, const size_t size) {
    std::vector<std::string> result;
    std::string value = str;

    while (value.length() > size) {
        auto pos = result.begin();
        result.insert(pos, value.substr(value.length() - size, value.length() - 1));
        value = value.substr(0, value.length() - size);
    }

    const auto pos = result.begin();
    result.insert(pos, value);

    return result;
}

std::string join(const std::vector<std::string> &list, const std::string &delimiter) {
    std::string result;
    int count = 0;

    for (const std::string &key: list) {
        result += (count > 0 ? delimiter : "") + key;
        count++;
    }

    return result;
}

bool hasPluginConfigFile(MediaTrack *media_track, const int pluginId) {
    const std::string plugin_name = DAW::GetTrackFxName(media_track, pluginId, false);
    const std::string developer_name = DAW::GetTrackFxDeveloper(media_track, pluginId);
    const std::string plugin_type = DAW::GetTrackFxType(media_track, pluginId);

    return file_exists(GetReaSonusPluginPath(developer_name, plugin_name, plugin_type, false).c_str());
}

void logInteger(const char *key, const int value) {
    char buffer[250];
    snprintf(buffer, sizeof(buffer), "%s: %d\n", key, value);
    ShowConsoleMsg(buffer);
}

void logDouble(const char *key, const double value) {
    char buffer[250];
    snprintf(buffer, sizeof(buffer), "%s: %f\n", key, value);
    ShowConsoleMsg(buffer);
}

std::string GenerateUniqueKey(std::string prefix) {
    srand(std::time(nullptr));
    const int now = GetTickCount();

    constexpr char characters[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < 24; i++) {
        const char rnd = characters[(rand() + 100) % 36];
        prefix += rnd;
    }
    prefix += std::to_string(now);

    return prefix;
}

std::vector<std::string> unwanted_param_names = {
    "MIDI CC", // Decomposer, Arturia
    "reserved", // Decomposer, Valhalla
    // Blue Cat
    "MIDI Program Change",
    "MIDI Controller",
    // Arturia
    "unassigned",
    "VST_ProgramChange_",
    "HardwareDisplayControl",
    "MPE_",
    // SPITFIRE
    "general purpose",
    // global
    "undefined",
};

bool IsWantedParam(const std::string &param_name) {
    bool result = true;

    for (std::string const &unwanted_name: unwanted_param_names) {
        const int res = param_name.find(unwanted_name);

        // We found the string. Set result to false and break;
        if (res != static_cast<int>(std::string::npos)) {
            result = false;
            break;
        }
    }

    return result;
}

int minmax(const int min, const int value, const int max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

double minmax(const double min, const double value, const double max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

int min(const int value_1, const int value_2) {
    return value_1 < value_2
               ? value_1
               : value_2;
}

double min(const double value_1, const double value_2) {
    return value_1 < value_2
               ? value_1
               : value_2;
}

int max(const int value_1, const int value_2) {
    return value_1 > value_2
               ? value_1
               : value_2;
}

double max(const double value_1, const double value_2) {
    return value_1 > value_2
               ? value_1
               : value_2;
}

HWND findWindowChildByID(const HWND parentHWND, const int window_child_id) {
#ifdef _WDL_SWELL
    if (!ValidatePtr(parentHWND, "HWND")) {
        return nullptr;
    }
#endif
    return GetDlgItem(parentHWND, window_child_id);
}

// TODO: check if we can remove this and replace with
bool getWindowScrollInfo(
    void *windowHWND,
    const char *scrollbar,
    int *positionOut,
    int *pageSizeOut,
    int *minOut,
    int *maxOut,
    int *trackPosOut
) {
    if (!ValidatePtr(windowHWND, "HWND")) {
        return false;
    }

    SCROLLINFO scroll_info = {
        sizeof(SCROLLINFO),
        SIF_ALL,
    };

    const int nBar = strchr(scrollbar, 'v') || strchr(scrollbar, 'V') ? SB_VERT : SB_HORZ;
    // Match strings such as "SB_VERT", "VERT" or "v".

    const bool isOK = !!CoolSB_GetScrollInfo(static_cast<HWND>(windowHWND), nBar, &scroll_info);

    *minOut = scroll_info.nMin;
    *maxOut = scroll_info.nMax;
    *pageSizeOut = scroll_info.nPage;
    *positionOut = scroll_info.nPos;
    *trackPosOut = scroll_info.nTrackPos;

    return isOK;
}

bool setWindowScrollPos(void *windowHWND, const char *scrollbar, const int position) {
    bool isOK = false;

    if (ValidatePtr(windowHWND, "HWND")) {
        if (strchr(scrollbar, 'v') || strchr(scrollbar, 'V')) {
            isOK = !!CoolSB_SetScrollPos(static_cast<HWND>(windowHWND), SB_VERT, position, TRUE);

            if (!isOK) {
                isOK = !!CoolSB_SetScrollPos(static_cast<HWND>(windowHWND), SB_VERT, position, TRUE);
            }

            if (isOK) {
                SendMessage(static_cast<HWND>(windowHWND), WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, position), 0);
            }
        } else {
            isOK = !!CoolSB_SetScrollPos(static_cast<HWND>(windowHWND), SB_HORZ, position, TRUE);

            if (!isOK) {
                isOK = !!CoolSB_SetScrollPos(static_cast<HWND>(windowHWND), SB_HORZ, position, TRUE);
            }

            if (isOK) {
                SendMessage(static_cast<HWND>(windowHWND), WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, position), 0);
            }
        }
    }
    return isOK;
}

void GetLanguages(std::vector<std::string> &language_names) {
    const std::string locale_path = GetReaSonusLocalesFolderPath();
    language_names.clear();

    for (const auto &entry: std::filesystem::recursive_directory_iterator(locale_path)) {
        if (entry.is_regular_file() && !entry.is_symlink()) {
            const std::filesystem::path &path(entry.path());
            if (path.has_extension() && path.extension() == ".ini") {
                language_names.push_back(split(path.filename().u8string(), ".").at(0));
            }
        }
    }
}

/**
 * Check if the value is betweemn the min and max value
 * @property min The min value to check againt. The value should be larger than this value
 * @property val The value to test against
 * @property max The max value to check againt. The value should be smaller than this value
 *
 * @return boolean value
 */
bool between(const int min, const int val, const int max) {
    const double diff = max - min;
    return diff > 0 && diff < val;
}

/**
 * @brief Create a Path If Not Exist object
 *
 * @param path
 * @return true when path exists
 * @return false when path not exists
 */
bool createPathIfNotExist(const std::string &path) {
    if (!std::filesystem::exists(path)) {
#ifdef _WIN32
        SHCreateDirectoryEx(NULL, path.c_str(), NULL);
#else
        RecursiveCreateDirectory(path.c_str(), 0);
#endif
    }

    return std::filesystem::exists(path);
}

std::string createPathName(const std::vector<std::string> &path_elements) {
    return join(path_elements, std::string(1, PATH_SEPARATOR));
}

std::string toLowerCase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), tolower);
    return value;
}

std::string toUpperCase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), ::toupper);
    return value;
}

double boolToDouble(const bool value) {
    return value ? 1.0 : 0.0;
}

bool doubleToBool(const double value) {
    return value > 0.0;
}

std::vector<std::string> GetPluginTypes() {
    std::vector<std::string> plugin_types = {
        "vst",
        "vsti",
        "vst3",
        "vst3i",
        "au",
        "aui",
        "clap",
        "clapi",
        "lv2",
        "lv2i"
    };
    return plugin_types;
}

std::string FormatPluginType(std::string value) {
    std::string plugin_type = toUpperCase(value);

    if (value.back() == 'i') {
        plugin_type.pop_back();
        plugin_type.push_back('i');
    }

    return plugin_type;
}

std::string GetPluginRequestString(const std::string &plugin_origname, std::string plugin_type) {
    std::vector<std::string> allowed_plugin_types = GetPluginTypes();
    const auto it_type = std::find(allowed_plugin_types.begin(), allowed_plugin_types.end(), plugin_type);

    if (it_type == allowed_plugin_types.end()) {
        // Not found, the plugin type has either not been set or is invalid!
        return "";
    }

    // ------------------------------------------------------------------------
    // VST actually means version 1 of the VST spec. Most plugins nowadays are
    // using version 2.x of the VST spec. VST2 is what needs to be used during
    // plugin insertion, otherwise no plugin will be found unless there is an
    // alternate version also installed.
    // ------------------------------------------------------------------------
    if (plugin_type == "vst" || plugin_type == "vsti") {
        plugin_type.replace(0, 3, "VST2");
    }

    // Make sure the properly formatted type is included when adding a plugin!
    std::string plugin_name_with_type = FormatPluginType(plugin_type) + ": " + plugin_origname;

    return plugin_name_with_type;
}
