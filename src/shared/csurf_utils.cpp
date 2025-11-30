#include "../shared/csurf_utils.hpp"
#include "../shared/csurf.h"
#include "csurf_daw.hpp"
#include <WDL/db2val.h>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <string>
#include <vector>

#ifdef _WIN32
#include <ShlObj_core.h>
#endif

void Main_OnCommandStringEx(std::string action_name, int flag, ReaProject *proj)
{
    int actionId = NamedCommandLookup(action_name.c_str());
    Main_OnCommandEx(actionId, flag, proj);
}

void SetActionState(int actionId)
{
    int state = GetToggleCommandStateEx(0, actionId);
    if (state == 1)
    {
        Main_OnCommandEx(actionId, 0, 0);
    }
}

void SetActionState(std::string action_name)
{
    int actionId = NamedCommandLookup(action_name.c_str());
    SetActionState(actionId);
}

void SetActionState(int actionId, int new_state)
{
    SetToggleCommandState(0, actionId, new_state);
    RefreshToolbar2(0, actionId);
}

void SetActionState(std::string action_name, int new_state)
{
    int actionId = NamedCommandLookup(action_name.c_str());
    SetActionState(actionId, new_state);
}

bool GetToggleCommandStringState(std::string action_name)
{
    int actionId = NamedCommandLookup(action_name.c_str());
    return GetToggleCommandState(actionId);
}

bool hasBit(int val, int key)
{
    return val & (1 << key);
};

double volToNormalized(double vol)
{
    double d = (DB2SLIDER(VAL2DB(vol)) / 1000.0);
    if (d < 0.0)
    {
        return 0.0;
    }

    if (d > 1.0)
    {
        return 1.0;
    }

    return d;
}

double int14ToNormalized(unsigned char msb, unsigned char lsb)
{
    int val = lsb | (msb << 7);
    double normalizedVal = val / 16383.0;

    normalizedVal = normalizedVal < 0.0 ? 0.0 : normalizedVal;
    normalizedVal = normalizedVal > 1.0 ? 1.0 : normalizedVal;

    return normalizedVal;
}

double normalizedToPan(double val)
{
    return 2.0 * val - 1.0;
}

double panToNormalized(double val)
{
    return 0.5 * (val + 1.0);
}

double int14ToVol(unsigned char msb, unsigned char lsb)
{
    int val = lsb | (msb << 7);
    double pos = ((double)val * 1000.0) / 16383.0;
    pos = SLIDER2DB(pos);

    return DB2VAL(pos);
}

std::string StripPluginName(std::string pluginName)
{
    std::vector<std::string> pluginNameParts = split(StripPluginNamePrefixes(StripPluginChannelPostfix(pluginName.data()).data()), " (");
    if (pluginNameParts.size() > 1)
    {
        pluginNameParts.pop_back();
    }

    return join(pluginNameParts, " (");
}

std::string StripPluginDeveloper(std::string pluginName)
{
    std::vector<std::string> pluginNameParts = split(StripPluginNamePrefixes(StripPluginChannelPostfix(pluginName.data()).data()), " (");
    std::string developer = pluginNameParts.at(pluginNameParts.size() - 1);
    if (!developer.empty())
    {
        developer.pop_back();
    }

    return developer;
}

std::string StripPluginNamePrefixes(char *name)
{
    std::string s = std::string(name);
    std::string delimiter = ": ";
    int pos = (int)(s.find(delimiter) + size(delimiter));
    if (pos < 0)
    {
        return s;
    }
    s.erase(0, pos);
    return s;
}

std::string StripPluginChannelPostfix(char *name)
{
    std::string s = std::string(name);
    std::string delimiter = " (32 out)";
    int pos = (int)s.find(delimiter);
    if (pos < 0)
    {
        return s;
    }
    s.erase(s.find(delimiter), size(delimiter));
    return s;
}

bool IsPluginFX(std::string name)
{
    std::string delimiter = ": ";
    int pos = (int)name.find(delimiter);
    // If we can not find the delimiter, we can not determine the type of plugin, so asume it's an effect
    if (pos < 0)
    {
        return true;
    }
    name.erase(pos, name.length());
    name.erase(0, name.length() - 1);

    return name != "i";
}

std::string Progress(int current, int total)
{
    char buffer[127];
    snprintf(buffer, sizeof(buffer), "%d/%d", current, total);
    return std::string(buffer);
}

std::string GetSendModeString(int sendMode)
{
    switch (sendMode)
    {
    case 0:
        return "Post-Fdr";
    case 1:
        return "Pre-FX";
    case 2:
    case 3:
        return "Post-FX";
    default:
        return "Post-Fdr";
    };
}

std::string GetAutomationString(int automationMode)
{
    switch (automationMode)
    {
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
    };
}

std::string GetReaSonusFolderPath() { return std::string(GetResourcePath()) + pathSeparator + "ReaSonus"; }

std::string GetReaSonusIniPath(std::string device) { return GetReaSonusFolderPath() + pathSeparator + device + ".ini"; }

std::string GetReaSonusZonesPath() { return std::string(GetResourcePath()) + pathSeparator + "CSI" + pathSeparator + "Zones" + pathSeparator + "ReasonusFaderPort" + pathSeparator + "_ReaSonusEffects"; }

std::string GetReaSonusPluginPath(std::string developer, std::string pluginName, bool create)
{
    std::string path = GetReaSonusFolderPath() + pathSeparator + "Plugins" + pathSeparator + developer;

    if (create)
    {
#ifdef _WIN32
        SHCreateDirectoryEx(NULL, path.c_str(), NULL);
#else
        RecursiveCreateDirectory(path.c_str(), 0);
#endif
    }
    return path + pathSeparator + pluginName + ".ini";
}

std::string GetReaSonusLocalesFolderPath()
{
    return GetReaSonusFolderPath() + pathSeparator + "Locales";
}

std::string GetReaSonusLocalesPath(std::string language)
{
    return GetReaSonusLocalesFolderPath() + pathSeparator + language + ".ini";
}

std::string GetReaSonusLocalesRootFile()
{
    return std::string(GetResourcePath()) + pathSeparator + "UserPlugins" + pathSeparator + "ReaSonus" + pathSeparator + "en-US.ini";
}

bool isInteger(std::string value)
{
    char *p;
    strtol(value.c_str(), &p, 10);
    return *p == 0;
}

std::vector<std::string> split(std::string str, std::string delimiter)
{
    std::vector<std::string> value;

    if (!str.empty())
    {
        int start = 0;
        do
        {
            // Find the index of occurrence
            int idx = (int)str.find(delimiter, start);
            if (idx == static_cast<int>(std::string::npos))
            {
                break;
            }

            // If found add the substring till that
            // occurrence in the vector
            int length = idx - start;
            value.push_back(str.substr(start, length));
            start += (int)(length + delimiter.size());
        } while (true);
        value.push_back(str.substr(start));
    }

    return value;
}

std::vector<std::string> cutString(std::string str, size_t size)
{
    std::vector<std::string> v;
    std::string value = str;

    while (value.length() > size)
    {
        auto pos = v.begin();
        v.insert(pos, value.substr(value.length() - size, value.length() - 1));
        value = value.substr(0, value.length() - size);
    }

    auto pos = v.begin();
    v.insert(pos, value);

    return v;
}

std::string join(std::vector<std::string> list, std::string delimiter)
{
    std::string result = "";
    int count = 0;
    for (const std::string &key : list)
    {
        result += (count > 0 ? delimiter : "") + key;
        count++;
    }
    return result;
}

bool hasPluginConfigFile(MediaTrack *media_track, int pluginId)
{
    std::string pluginName = DAW::GetTrackFxName(media_track, pluginId);
    std::string developerName = DAW::GetTrackFxDeveloper(media_track, pluginId);

    return file_exists(GetReaSonusPluginPath(developerName, pluginName).c_str());
}

void logInteger(const char *key, int value)
{
    char buffer[250];
    snprintf(buffer, sizeof(buffer), "%s: %d\n", key, value);
    ShowConsoleMsg(buffer);
}

void logDouble(const char *key, double value)
{
    char buffer[250];
    snprintf(buffer, sizeof(buffer), "%s: %f\n", key, value);
    ShowConsoleMsg(buffer);
}

void readAndCreateIni(mINI::INIStructure &data, std::string device)
{
    mINI::INIFile file(GetReaSonusIniPath(device));
    if (!file.read(data))
    {
        RecursiveCreateDirectory((std::string(GetResourcePath()) + pathSeparator + "ReaSonus" + pathSeparator + "Plugins").c_str(), 0);
        data["surface"]["midiin"] = "0";
        data["surface"]["midiout"] = "0";
        data["surface"]["mute-solo-momentary"] = "0";
        data["functions"]["1"] = "0";
        data["functions"]["2"] = "0";
        data["functions"]["3"] = "0";
        data["functions"]["4"] = "0";
        if (device == FP_V2)
        {
            data["surface"]["control-hidden-tracks"] = "0";
            data["surface"]["can-disable-fader"] = "0";
            data["surface"]["endless-track-scroll"] = "0";
        }
        if (device == FP_8)
        {
            data["surface"]["surface"] = "0";
            data["surface"]["disable-plugins"] = "0";
            data["surface"]["distraction-free"] = "0";
            data["surface"]["erase-last-param-after-learn"] = "0";
            data["surface"]["master-fader-mode"] = "0";
            data["surface"]["swap-shift-buttons"] = "0";
            data["surface"]["fader-reset"] = "0";
            data["surface"]["overwrite-time-code"] = "1";
            data["surface"]["time-code"] = "2";
            data["surface"]["track-color-brightness"] = "25";
            data["displays"]["track"] = "8";
            data["functions"]["5"] = "0";
            data["functions"]["6"] = "0";
            data["functions"]["7"] = "0";
            data["functions"]["8"] = "0";
            data["functions"]["9"] = "0";
            data["functions"]["10"] = "0";
            data["functions"]["11"] = "0";
            data["functions"]["12"] = "0";
            data["functions"]["13"] = "0";
            data["functions"]["14"] = "0";
            data["functions"]["15"] = "0";
            data["functions"]["16"] = "0";
            data["filters"]["nb-filters"] = "0";
        }
        file.generate(data, true);
    }
    else
    {
        validateReaSonusIni(file, data, device);
    };
    file.read(data);
}

void validateReaSonusIni(mINI::INIFile file, mINI::INIStructure &data, std::string device)
{
    data["surface"]["midiin"] = data["surface"].has("midiin") ? data["surface"]["midiin"] : "0";
    data["surface"]["midiout"] = data["surface"].has("midiout") ? data["surface"]["midiout"] : "0";
    data["surface"]["mute-solo-momentary"] = data["surface"].has("mute-solo-momentary") ? data["surface"]["mute-solo-momentary"] : "0";

    data["functions"]["1"] = data["functions"].has("1") ? data["functions"]["1"] : "0";
    data["functions"]["2"] = data["functions"].has("2") ? data["functions"]["2"] : "0";
    data["functions"]["3"] = data["functions"].has("3") ? data["functions"]["3"] : "0";
    data["functions"]["4"] = data["functions"].has("4") ? data["functions"]["4"] : "0";

    if (device == FP_V2)
    {
        data["surface"]["control-hidden-tracks"] = data["surface"].has("control-hidden-tracks") ? data["surface"]["control-hidden-tracks"] : "0";
        data["surface"]["can-disable-fader"] = data["surface"].has("can-disable-fader") ? data["surface"]["can-disable-fader"] : "0";
        data["surface"]["endless-track-scroll"] = data["surface"].has("endless-track-scroll") ? data["surface"]["endless-track-scroll"] : "0";
        ;
    }

    if (device == FP_8)
    {
        data["surface"]["surface"] = data["surface"].has("surface") ? data["surface"]["surface"] : "0";
        data["surface"]["disable-plugins"] = data["surface"].has("disable-plugins") ? data["surface"]["disable-plugins"] : "0";
        data["surface"]["distraction-free"] = data["surface"].has("distraction-free") ? data["surface"]["distraction-free"] : "0";
        data["surface"]["erase-last-param-after-learn"] = data["surface"].has("erase-last-param-after-learn") ? data["surface"]["erase-last-param-after-learn"] : "0";
        data["surface"]["master-fader-mode"] = data["surface"].has("master-fader-mode") ? data["surface"]["master-fader-mode"] : "0";
        data["surface"]["swap-shift-buttons"] = data["surface"].has("swap-shift-buttons") ? data["surface"]["swap-shift-buttons"] : "0";
        data["surface"]["fader-reset"] = data["surface"].has("fader-reset") ? data["surface"]["fader-reset"] : "0";
        data["surface"]["overwrite-time-code"] = data["surface"].has("overwrite-time-code") ? data["surface"]["overwrite-time-code"] : "1";
        data["surface"]["time-code"] = data["surface"].has("time-code") ? data["surface"]["time-code"] : "2";
        data["surface"]["track-color-brightness"] = data["surface"].has("track-color-brightness") ? data["surface"]["track-color-brightness"] : "25";
        data["displays"]["track"] = data["displays"].has("track") ? data["displays"]["track"] : "8";
        data["functions"]["5"] = data["functions"].has("5") ? data["functions"]["5"] : "0";
        data["functions"]["6"] = data["functions"].has("6") ? data["functions"]["6"] : "0";
        data["functions"]["7"] = data["functions"].has("7") ? data["functions"]["7"] : "0";
        data["functions"]["8"] = data["functions"].has("8") ? data["functions"]["8"] : "0";
        data["functions"]["9"] = data["functions"].has("9") ? data["functions"]["9"] : "0";
        data["functions"]["10"] = data["functions"].has("10") ? data["functions"]["10"] : "0";
        data["functions"]["11"] = data["functions"].has("11") ? data["functions"]["11"] : "0";
        data["functions"]["12"] = data["functions"].has("12") ? data["functions"]["12"] : "0";
        data["functions"]["13"] = data["functions"].has("13") ? data["functions"]["13"] : "0";
        data["functions"]["14"] = data["functions"].has("14") ? data["functions"]["14"] : "0";
        data["functions"]["15"] = data["functions"].has("15") ? data["functions"]["15"] : "0";
        data["functions"]["16"] = data["functions"].has("16") ? data["functions"]["16"] : "0";
        data["filters"]["nb-filters"] = data["filters"].has("nb-filters") ? data["filters"]["nb-filters"] : "0";
    }

    file.write(data, true);
}

std::string GenerateUniqueKey(std::string prefix)
{
    srand(std::time(0));
    int now = GetTickCount();

    char a[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < 24; i++)
    {
        char rnd = a[(rand() + 100) % 36];
        prefix += rnd;
    }
    prefix += std::to_string(now);

    return prefix;
}

std::vector<std::string> unwanted_param_names = {
    "MIDI CC",  // Decomposer, Arturia
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

bool IsWantedParam(std::string param_name)
{
    bool result = true;

    for (std::string unwanted_name : unwanted_param_names)
    {
        int res = param_name.find(unwanted_name);
        // We found the string. Set result to false and break;
        if (res != (int)std::string::npos)
        {
            result = false;
            break;
        }
    }

    return result;
}

int minmax(int min, int value, int max)
{
    return value < min
               ? min
           : value > max
               ? max
               : value;
}

double minmax(double min, double value, double max)
{
    return value < min
               ? min
           : value > max
               ? max
               : value;
}

HWND findWindowChildByID(HWND parentHWND, int ID)
{
#ifdef _WDL_SWELL
    if (!ValidatePtr(parentHWND, "HWND"))
        return nullptr;
#endif
    return GetDlgItem(parentHWND, ID);
}

bool getWindowScrollInfo(void *windowHWND, const char *scrollbar, int *positionOut, int *pageSizeOut, int *minOut, int *maxOut, int *trackPosOut)
{
    if (!ValidatePtr(windowHWND, "HWND"))
    {
        return false;
    }

    SCROLLINFO si = {
        sizeof(SCROLLINFO),
        SIF_ALL,
    };

    int nBar = ((strchr(scrollbar, 'v') || strchr(scrollbar, 'V')) ? SB_VERT : SB_HORZ); // Match strings such as "SB_VERT", "VERT" or "v".

    bool isOK = !!CoolSB_GetScrollInfo((HWND)windowHWND, nBar, &si);

    *pageSizeOut = si.nPage;
    *positionOut = si.nPos;
    *minOut = si.nMin;
    *maxOut = si.nMax;
    *trackPosOut = si.nTrackPos;

    return isOK;
}

bool setWindowScrollPos(void *windowHWND, const char *scrollbar, int position)
{
    bool isOK = false;
    if (ValidatePtr(windowHWND, "HWND"))
    {
        if (strchr(scrollbar, 'v') || strchr(scrollbar, 'V'))
        {
            isOK = !!CoolSB_SetScrollPos((HWND)windowHWND, SB_VERT, position, TRUE);
            if (!isOK)
            {
                isOK = !!CoolSB_SetScrollPos((HWND)windowHWND, SB_VERT, position, TRUE);
            }
            if (isOK)
            {
                SendMessage((HWND)windowHWND, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, position), 0);
            }
        }
        else
        {
            isOK = !!CoolSB_SetScrollPos((HWND)windowHWND, SB_HORZ, position, TRUE);
            if (!isOK)
            {
                isOK = !!CoolSB_SetScrollPos((HWND)windowHWND, SB_HORZ, position, TRUE);
            }
            if (isOK)
            {
                SendMessage((HWND)windowHWND, WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, position), 0);
            }
        }
    }
    return isOK;
}

void GetLanguages(std::vector<std::string> &language_names)
{
    std::string locale_path = GetReaSonusLocalesFolderPath();
    language_names.clear();

    for (const auto &entry : std::filesystem::recursive_directory_iterator(locale_path))
    {
        if (entry.is_regular_file() && !entry.is_symlink())
        {
            std::filesystem::path path(entry.path());
            if (path.has_extension() && path.extension() == ".ini")
            {
                language_names.push_back((split(path.filename().u8string(), ".").at(0)));
            }
        }
    }
}
