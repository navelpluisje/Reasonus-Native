#include "csurf_utils.hpp"
#include <db2val.h>
#include <reaper_plugin_functions.h>
#include <string>

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

string StripFxType(char *name)
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
