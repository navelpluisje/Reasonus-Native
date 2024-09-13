#ifndef CSURF_CHANNEL_MANAGER_RESOURCES_H_
#define CSURF_CHANNEL_MANAGER_RESOURCES_H_

#include "csurf_context.cpp"
#include "csurf_navigator.cpp"
#include "csurf_button.hpp"
#include "csurf_track.hpp"
#include <vector>
#include <db2val.h>

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

/**
 * @brief Get a string representation of the pan value and the panMode. If panMode is 5, it will return the left pan representation
 *
 * @param pan The pan value
 * @param panMode The pan mode
 * @return string
 */
static string GetPanString(double pan, int panMode)
{
    int panInt = (int)(pan * 100.0);
    string strVal = to_string(abs(panInt));
    if (panMode == 5)
    {
        panInt = (panInt + 100) / 2;
        return to_string(panInt) + "L";
    }
    if (pan < 0)
    {
        return "L" + strVal;
    }
    if (pan > 0)
    {
        return "R" + strVal;
    }
    return "<C>";
}

/**
 * @brief Get a string representation of the width value and the panMode. If panMode is 5, it will return the right pan representation
 *
 * @param width The pan value
 * @param panMode The pan mode
 * @return string
 */
static string GetWidthString(double width, int panMode)
{
    int panInt = ((int)(width * 100.0) + 100) / 2;
    panInt = (panInt + 100) / 2;
    string strVal = to_string(abs(panInt));
    if (panMode == 5)
    {
        return to_string(panInt) + "R";
    }
    return "W" + strVal;
}

/**
 * @brief Get a string representation of the automtion mode
 *
 * @param automationMode The automation mode
 * @return string
 */
static string GetAutomationString(int automationMode)
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

/**
 * @brief Get a string representation of the send mode
 *
 * @param sendMode The send mode
 * @return string
 */
static string GetSendModeString(int sendMode)
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

enum ChannelMode
{
    TrackMode,
    PluginMode,
    SendMode,
    PanMode,
};

#endif