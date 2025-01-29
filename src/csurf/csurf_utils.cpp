#include "csurf_utils.hpp"
#include "csurf_daw.hpp"
#include <WDL/db2val.h>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <string>
#include <vector>

void Main_OnCommandStringEx(std::string action_name, int flag, ReaProject *proj)
{
    int actionId = NamedCommandLookup(action_name.c_str());
    Main_OnCommandEx(actionId, flag, proj);
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

std::string StripPluginNamePrefixes(char *name)
{
    std::string s = std::string(name);
    std::string delimiter = ": ";
    int pos = s.find(delimiter) + size(delimiter);
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
    int pos = s.find(delimiter);
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
    int pos = name.find(delimiter);
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
        return "Post-fdr";
    case 1:
        return "Pre-FX";
    case 2:
    case 3:
        return "Post-FX";
    default:
        return "Post-fdr";
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

std::string GetReaSonusIniPath() { return std::string(GetResourcePath()) + "/ReaSonus/FP.ini"; }

std::string GetReaSonusPluginPath(std::string developer, std::string pluginName, bool create)
{
    if (create)
    {
        RecursiveCreateDirectory((std::string(GetResourcePath()) + "/ReaSonus/Plugins/" + developer).c_str(), 0);
    }
    return std::string(GetResourcePath()) + "/ReaSonus/Plugins/" + developer + "/" + pluginName + ".ini";
}

bool isInteger(std::string value)
{
    char *p;
    strtol(value.c_str(), &p, 10);
    return *p == 0;
}

std::vector<std::string> split(std::string str, std::string delimiter)
{
    std::vector<std::string> v;

    if (!str.empty())
    {
        int start = 0;
        do
        {
            // Find the index of occurrence
            int idx = str.find(delimiter, start);
            if (idx == static_cast<int>(std::string::npos))
            {
                break;
            }

            // If found add the substring till that
            // occurrence in the vector
            int length = idx - start;
            v.push_back(str.substr(start, length));
            start += (length + delimiter.size());
        } while (true);
        v.push_back(str.substr(start));
    }

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

void readAndCreateIni(mINI::INIStructure &data)
{
    mINI::INIFile file(GetReaSonusIniPath());
    if (!file.read(data))
    {
        RecursiveCreateDirectory((std::string(GetResourcePath()) + "/ReaSonus/Plugins").c_str(), 0);
        data["Surface"]["MidiIn"] = "0";
        data["Surface"]["MidiOut"] = "0";
        data["Surface"]["Surface"] = "0";
        data["Surface"]["Disable-Plugins"] = "0";
        data["Surface"]["Swap-Shift-Buttons"] = "0";
        data["Functions"]["1"] = "0";
        data["Functions"]["2"] = "0";
        data["Functions"]["3"] = "0";
        data["Functions"]["4"] = "0";
        data["Functions"]["5"] = "0";
        data["Functions"]["6"] = "0";
        data["Functions"]["7"] = "0";
        data["Functions"]["8"] = "0";
        data["Filters"]["Nb-Filters"] = "0";
        file.generate(data, true);
    };
    file.read(data);
}

std::string GenerateUniqueKey(std::string prefix)
{
    srand(std::time(0));
    char a[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < 24; i++)
    {
        char rnd = a[(rand() + 100) % 36];
        prefix += rnd;
    }
    return prefix;
};
