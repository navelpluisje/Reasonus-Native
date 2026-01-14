#ifndef CSURF_UTILS_H_
#define CSURF_UTILS_H_

#include <string>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <mini/ini.h>

const char pathSeparator =
#ifdef _WIN32
    '\\';
#else
    '/';
#endif

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

const std::vector<std::vector<std::string>> shared_settings = {
    {"surface", "midiin", "0"},
    {"surface", "midiout", "0"},
    {"surface", "mute-solo-momentary", "0"},
    {"surface", "latch-preview-action", "0"},
    {"surface", "latch-preview-action-code", "42013"},
    {"functions", "1", "0"},
    {"functions", "2", "0"},
    {"functions", "3", "0"},
    {"functions", "4", "0"},
    {"footswitch", "1", "0"},
    {"footswitch", "2", "0"},
    {"footswitch", "3", "0"},
};

const std::vector<std::vector<std::string>> fp_v2_settings = {
    {"surface", "control-hidden-tracks", "0"},
    {"surface", "can-disable-fader", "0"},
    {"surface", "endless-track-scroll", "0"},
};

const std::vector<std::vector<std::string>> fp_8_settings = {
    {"surface", "surface", "0"},
    {"surface", "disable-plugins", "0"},
    {"surface", "distraction-free", "0"},
    {"surface", "erase-last-param-after-learn", "0"},
    {"surface", "master-fader-mode", "0"},
    {"surface", "swap-shift-buttons", "0"},
    {"surface", "fader-reset", "0"},
    {"surface", "overwrite-time-code", "1"},
    {"surface", "time-code", "2"},
    {"surface", "track-color-brightness", "25"},
    {"surface", "plugin-step-size", "1"},
    {"surface", "plugin-map-param-clear", "0"},
    {"displays", "track", "8"},
    {"functions", "5", "0"},
    {"functions", "6", "0"},
    {"functions", "7", "0"},
    {"functions", "8", "0"},
    {"functions", "9", "0"},
    {"functions", "10", "0"},
    {"functions", "11", "0"},
    {"functions", "12", "0"},
    {"functions", "13", "0"},
    {"functions", "14", "0"},
    {"functions", "15", "0"},
    {"functions", "16", "0"},
    {"filters", "nb-filters", "0"},
};

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

void SetActionState(int actionId, int state);

void SetActionState(std::string action_name, int state);

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
 * @brief Strip the plugin name from its pre and post fixes
 *
 * @param name The full plugin name
 * @return The stripped down name
 */
std::string StripPluginName(std::string pluginName);

/**
 * @brief Strip the plugin developer from its pre and post fixes
 *
 * @param name The full plugin name
 * @return The stripped down name
 */
std::string StripPluginDeveloper(std::string pluginName);

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

std::string GetReaSonusFolderPath();
std::string GetReaSonusIniPath(std::string device);

std::string GetReaSonusZonesPath();

std::string GetReaSonusPluginPath(std::string developer, std::string pluginName, bool create = false);

std::string GetReaSonusLocalesFolderPath();
std::string GetReaSonusLocalesPath(std::string language);

std::string GetReaSonusLocalesRootFile();

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

/**
 * @brief Chack if a plugin param name is wanted. therre is a list of values that shoould NOT be in a param name
 *
 * @param param_name
 * @return true
 * @return false
 */
bool IsWantedParam(std::string param_name);

int minmax(int min, int value, int max);
double minmax(double min, double value, double max);

int min(int value_1, int value_2);
double min(double value_1, double value_2);

int max(int value_1, int value_2);
double max(double value_1, double value_2);

/**
 * @brief borrowed from https://github.com/juliansader/ReaExtensions
 *
 * @param parentHWND
 * @param ID
 * @return HWND
 */
HWND findWindowChildByID(HWND parentHWND, int ID);

/**
 * @brief Get the Window Scroll Info object
 *
 * borrowed from https://github.com/juliansader/ReaExtensions
 *
 * @param windowHWND
 * @param scrollbar
 * @param positionOut
 * @param pageSizeOut
 * @param minOut
 * @param maxOut
 * @param trackPosOut
 * @return true
 * @return false
 */
bool getWindowScrollInfo(void *windowHWND, const char *scrollbar, int *positionOut, int *pageSizeOut, int *minOut, int *maxOut, int *trackPosOut);

/**
 * @brief Set the window scroll position
 *
 * borrowed from https://github.com/juliansader/ReaExtensions
 *
 * @param windowHWND
 * @param scrollbar
 * @param position
 * @return true
 * @return false
 */
bool setWindowScrollPos(void *windowHWND, const char *scrollbar, int position);

/**
 * @brief Get a list with all the avaiulable ini translation files
 */
void GetLanguages(std::vector<std::string> &language_names);

double between(int min, int val, int max);

#endif // CSURF_UTILS_H_