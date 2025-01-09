#include "csurf_utils.hpp"
#include "csurf_daw.hpp"
#include <db2val.h>
#include <reaper_plugin_functions.h>
#include <string>
#include <vector>

using namespace std;

void Main_OnCommandStringEx(string action_name, int flag, ReaProject *proj)
{
    int actionId = NamedCommandLookup(action_name.c_str());
    Main_OnCommandEx(actionId, flag, proj);
}

bool GetToggleCommandStringState(string action_name)
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

string StripPluginNamePrefixes(char *name)
{
    string s = string(name);
    string delimiter = ": ";
    int pos = s.find(delimiter) + size(delimiter);
    if (pos < 0)
    {
        return s;
    }
    s.erase(0, pos);
    return s;
}

string StripPluginChannelPostfix(char *name)
{
    string s = string(name);
    string delimiter = " (32 out)";
    int pos = s.find(delimiter);
    if (pos < 0)
    {
        return s;
    }
    s.erase(s.find(delimiter), size(delimiter));
    return s;
}

bool IsPluginFX(string name)
{
    string delimiter = ": ";
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

string Progress(int current, int total)
{
    char buffer[127];
    snprintf(buffer, sizeof(buffer), "%d/%d", current, total);
    return string(buffer);
}

string GetSendModeString(int sendMode)
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

string GetAutomationString(int automationMode)
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

string GetReaSonusIniPath() { return string(GetResourcePath()) + "/ReaSonus/FP.ini"; }

string GetReaSonusPluginPath(string developer, string pluginName, bool create)
{
    if (create)
    {
        RecursiveCreateDirectory((string(GetResourcePath()) + "/ReaSonus/Plugins/" + developer).c_str(), 0);
    }
    return string(GetResourcePath()) + "/ReaSonus/Plugins/" + developer + "/" + pluginName + ".ini";
}

bool isInteger(string value)
{
    char *p;
    strtol(value.c_str(), &p, 10);
    return *p == 0;
}

vector<string> split(string str, string delimiter)
{
    vector<string> v;

    if (!str.empty())
    {
        int start = 0;
        do
        {
            // Find the index of occurrence
            int idx = str.find(delimiter, start);
            if (idx == static_cast<int>(string::npos))
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

string join(vector<string> list, string delimiter)
{
    string result = "";
    int count = 0;
    for (const string &key : list)
    {
        result += (count > 0 ? delimiter : "") + key;
        count++;
    }
    return result;
}

bool hasPluginConfigFile(MediaTrack *media_track, int pluginId)
{
    string pluginName = DAW::GetTrackFxName(media_track, pluginId);
    string developerName = DAW::GetTrackFxDeveloper(media_track, pluginId);

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
