#ifndef CSURF_UTILS_H_
#define CSURF_UTILS_H_

#include <string>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <mini/ini.h>

const int TOGGLE_SPEED = 150;
const int DOUBLE_CLICK_SPEED = 750;

const std::string FP_V2 = "FP_V2";
const std::string FP_8 = "FP";

const int AUTOMATION_OFF = -1;
const int AUTOMATION_TRIM = 0;
const int AUTOMATION_READ = 1;
const int AUTOMATION_TOUCH = 2;
const int AUTOMATION_LATCH = 4;
const int AUTOMATION_PREVIEW = 5;
const int AUTOMATION_WRITE = 3;

const int time_code_indexes[6] = {0, 2, 3, 4, 5, 8};
const std::string time_code_names[6] = {"Time", "Beats", "Seconds", "Samples", "Hr:Min:Sec:Fr", "Abs. Frames"};

struct ShiftState
{
    bool active = false;
    int start = false;
    bool invert = false;

    void SetValue(bool value)
    {
        int time = GetTickCount();
        active = value;

        if (start == 0)
        {
            start = time;
        }
        else
        {
            if (time - start < TOGGLE_SPEED)
            {
                ToggleInvert();
            }
            start = 0;
        }
    }

    void ToggleInvert()
    {
        invert = !invert;
    }

    bool IsActive()
    {
        return invert ? !active : active;
    }

    bool IsLocked()
    {
        return invert;
    }
};

struct DoubleClickState
{
    bool active = false;
    int start = 0;
    int clicks = 0;

    void SetValue(bool value)
    {
        int time = GetTickCount();

        if (value && start > 0 && clicks == 1)
        {
            if (time - start < DOUBLE_CLICK_SPEED)
            {
                active = true;
                start = 0;
                clicks = 0;
            }
            else
            {
                active = false;
                start = time;
                clicks = 1;
            }
            return;
        }

        if (!value && start > 0 && clicks == 1 && active)
        {
            active = false;
            start = 0;
            return;
        }

        if (start == 0 && value && clicks == 0)
        {
            clicks += 1;
            start = time;
            active = false;
            return;
        }
    }

    bool IsActive()
    {
        return active;
    }
};

void Main_OnCommandStringEx(std::string action_name, int flag = 0, ReaProject *proj = 0);

void SetActionState(int actionId);

void SetActionState(std::string action_name);

bool GetToggleCommandStringState(std::string action_name);

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

/**
 * @brief Strip the type from the plugin name
 *
 * @param name The full plugin name
 * @return std::string
 */
std::string StripPluginNamePrefixes(char *name);

/**
 * @brief Strip the final out info when applicable
 *
 * @param name
 * @return std::string
 */
std::string StripPluginChannelPostfix(char *name);

/**
 * @brief Check if the prefix of the fx tells it is actuielly an fx and not an instrument
 *
 * @param name
 * @return bool
 */
bool IsPluginFX(std::string name);

std::string Progress(int current, int total);

/**
 * @brief Get a std::string representation of the send mode
 *
 * @param sendMode The send mode
 * @return std::string
 */
std::string GetSendModeString(int sendMode);

/**
 * @brief Get a std::string representation of the automtion mode
 *
 * @param automationMode The automation mode
 * @return std::string
 */
std::string GetAutomationString(int automationMode);

std::string GetReaSonusIniPath(std::string device);

std::string GetReaSonusPluginPath(std::string developer, std::string pluginName, bool create = false);

bool isInteger(std::string value);

std::vector<std::string> split(std::string str, std::string delimiter);

std::vector<std::string> cutString(std::string str, size_t size);

std::string join(std::vector<std::string> list, std::string delimiter);

bool hasPluginConfigFile(MediaTrack *media_track, int pluginId);

void logInteger(const char *key, int value);

void logDouble(const char *key, double value);

/**
 * @brief Check if we have the FP.ini available. If not, we create it
 *
 * @param data The data object to write the ini data to
 * @param device Wether its a Faderport 8/16 or V2
 */
void readAndCreateIni(mINI::INIStructure &data, std::string device);

/**
 * @brief Validate if the current ini still is valid. If not create a backup and update the current
 *
 * @param data The data object to write the ini data to
 * @param device Wether its a Faderport 8/16 or V2
 */
void validateReaSonusIni(mINI::INIFile file, mINI::INIStructure &data, std::string device);

/**
 * @brief Create a unique key
 *
 * @param prefix The prefix used for the key
 */
std::string GenerateUniqueKey(std::string prefix);

int minmax(int min, int value, int max);
double minmax(double min, double value, double max);

#endif // CSURF_UTILS_H_