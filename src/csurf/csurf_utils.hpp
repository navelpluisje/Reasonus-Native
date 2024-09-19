#ifndef CSURF_UTILS_H_
#define CSURF_UTILS_H_

#include <string>
#include <db2val.h>
#include <reaper_plugin_functions.h>

const int AUTOMATION_OFF = -1;
const int AUTOMATION_TRIM = 0;
const int AUTOMATION_READ = 1;
const int AUTOMATION_TOUCH = 2;
const int AUTOMATION_LATCH = 4;
const int AUTOMATION_PREVIEW = 5;
const int AUTOMATION_WRITE = 3;

static void Main_OnCommandStringEx(string action_name, int flag = 0, ReaProject *proj = 0)
{
    int actionId = NamedCommandLookup(action_name.c_str());
    Main_OnCommandEx(actionId, flag, proj);
}

static bool GetToggleCommandStringState(string action_name)
{
    int actionId = NamedCommandLookup(action_name.c_str());
    return GetToggleCommandState(actionId);
}

/**
 * @brief Check if the bit with index key is set in val
 *
 * @param val The value to check
 * @param key The key to search for
 * @return true
 * @return false
 */
static bool hasBit(int val, int key)
{
    return val & (1 << key);
};

/**
 * @brief Get the normalized value of the volume to send to the device faders
 *
 * @param vol The volume to normalize
 * @return double Normalized value between 0.0 - 1.0
 */
static double volToNormalized(double vol)
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

/**
 * @brief Get the normalized valiue of the 14 bit midi input
 *
 * @param msb 7 bits of data
 * @param lsb another 7 bots of data
 * @return double Normalized value between 0.0 - 1.0
 */
static double int14ToNormalized(unsigned char msb, unsigned char lsb)
{
    int val = lsb | (msb << 7);
    double normalizedVal = val / 16383.0;

    normalizedVal = normalizedVal < 0.0 ? 0.0 : normalizedVal;
    normalizedVal = normalizedVal > 1.0 ? 1.0 : normalizedVal;

    return normalizedVal;
}

/**
 * @brief Normalize the Pan value
 *
 * @param val Normalized value between 0.0 - 1.0
 * @return double The pan value. Is between -1.0 - 1.0
 */
static double normalizedToPan(double val)
{
    return 2.0 * val - 1.0;
}

/**
 * @brief Normalize the Pan value
 *
 * @param val The pan value to normalize. Is between -1.0 - 1.0
 * @return double Normalized value between 0.0 - 1.0
 */
static double panToNormalized(double val)
{
    return 0.5 * (val + 1.0);
}

/**
 * @brief get the midi messages of the fader and translate it to volume
 *
 * @param msb Midi message 2
 * @param lsb Midi message 1
 * @return double
 */
static double int14ToVol(unsigned char msb, unsigned char lsb)
{
    int val = lsb | (msb << 7);
    double pos = ((double)val * 1000.0) / 16383.0;
    pos = SLIDER2DB(pos);

    return DB2VAL(pos);
}

#endif // CSURF_UTILS_H_