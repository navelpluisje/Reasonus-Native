#ifndef CSURF_UTILS_H_
#define CSURF_UTILS_H_

#include <string>
#include <reaper_plugin_functions.h>

using namespace std;

const int TOGGLE_SPEED = 150;

const int AUTOMATION_OFF = -1;
const int AUTOMATION_TRIM = 0;
const int AUTOMATION_READ = 1;
const int AUTOMATION_TOUCH = 2;
const int AUTOMATION_LATCH = 4;
const int AUTOMATION_PREVIEW = 5;
const int AUTOMATION_WRITE = 3;

struct ShiftState
{
    bool active = false;
    int start = false;
    bool invert = false;

    void ToggleInvert()
    {
        invert = !invert;
    }
};

void Main_OnCommandStringEx(string action_name, int flag = 0, ReaProject *proj = 0);

bool GetToggleCommandStringState(string action_name);

/**
 * @brief Check if the bit with index key is set in val
 *
 * @param val The value to check
 * @param key The key to search for
 * @return true
 * @return false
 */
bool hasBit(int val, int key);

/**
 * @brief Get the normalized value of the volume to send to the device faders
 *
 * @param vol The volume to normalize
 * @return double Normalized value between 0.0 - 1.0
 */
double volToNormalized(double vol);

/**
 * @brief Get the normalized valiue of the 14 bit midi input
 *
 * @param msb 7 bits of data
 * @param lsb another 7 bots of data
 * @return double Normalized value between 0.0 - 1.0
 */
double int14ToNormalized(unsigned char msb, unsigned char lsb);

/**
 * @brief Normalize the Pan value
 *
 * @param val Normalized value between 0.0 - 1.0
 * @return double The pan value. Is between -1.0 - 1.0
 */
double normalizedToPan(double val);

/**
 * @brief Normalize the Pan value
 *
 * @param val The pan value to normalize. Is between -1.0 - 1.0
 * @return double Normalized value between 0.0 - 1.0
 */
double panToNormalized(double val);

/**
 * @brief get the midi messages of the fader and translate it to volume
 *
 * @param msb Midi message 2
 * @param lsb Midi message 1
 * @return double
 */
double int14ToVol(unsigned char msb, unsigned char lsb);

void logInteger(const char *key, int value);

void logDouble(const char *key, double value);

#endif // CSURF_UTILS_H_