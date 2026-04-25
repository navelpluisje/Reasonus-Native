#ifndef CSURF_UTILS_H_
#define CSURF_UTILS_H_

#include <string>
// ReSharper disable once CppUnusedIncludeDirective
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <mini/ini.h>

constexpr char PATH_SEPARATOR =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

const std::string PREFIX_SEPARATOR = ": ";

constexpr int TOGGLE_SPEED = 150;
constexpr int DOUBLE_CLICK_SPEED = 750;

const std::string FP_V2 = "FP_V2";
const std::string FP_8 = "FP";

constexpr int AUTOMATION_OFF = -1;
constexpr int AUTOMATION_TRIM = 0;
constexpr int AUTOMATION_READ = 1;
constexpr int AUTOMATION_TOUCH = 2;
constexpr int AUTOMATION_LATCH = 4;
constexpr int AUTOMATION_PREVIEW = 5;
constexpr int AUTOMATION_WRITE = 3;

struct ShiftState {
    bool active = false; // NOLINT(*-non-private-member-variables-in-classes)
    int start = 0; // NOLINT(*-non-private-member-variables-in-classes)
    bool invert = false; // NOLINT(*-non-private-member-variables-in-classes)

    void SetValue(const bool value) {
        const int time = GetTickCount();
        active = value;

        if (start == 0) {
            start = time;
        } else {
            if (time - start < TOGGLE_SPEED) {
                ToggleInvert();
            }
            start = 0;
        }
    }

    void ToggleInvert() {
        invert = !invert;
    }

    [[nodiscard]] bool IsActive() const {
        return invert ? !active : active;
    }

    [[nodiscard]] bool IsLocked() const {
        return invert;
    }
};

struct DoubleClickState {
    bool active = false; // NOLINT(*-non-private-member-variables-in-classes)
    int start = 0; // NOLINT(*-non-private-member-variables-in-classes)
    int clicks = 0; // NOLINT(*-non-private-member-variables-in-classes)

    void SetValue(const bool value) {
        const int time = GetTickCount();

        if (value && start > 0 && clicks == 1) {
            if (time - start < DOUBLE_CLICK_SPEED) {
                active = true;
                start = 0;
                clicks = 0;
            } else {
                active = false;
                start = time;
                clicks = 1;
            }
            return;
        }

        if (!value && start > 0 && clicks == 1 && active) {
            active = false;
            start = 0;
            return;
        }

        if (start == 0 && value && clicks == 0) {
            clicks += 1;
            start = time;
            active = false;
        }
    }

    [[nodiscard]] bool IsActive() const {
        return active;
    }
};

void Main_OnCommandStringEx(const std::string &action_name, int flag, ReaProject *proj);

void Main_OnCommandAsyncEx(int action_id, int flag, const ReaProject *proj);

void SetActionState(int actionId);

void SetActionState(const std::string &action_name);

void SetActionState(int actionId, int new_state);

void SetActionState(const std::string &action_name, int new_state);

bool GetToggleCommandStringState(const std::string &action_name);

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
 * @brief Strip the plugin name from its pre- and post-fixes
 *
 * @param plugin_name The full plugin name
 * @return The stripped down name
 */
std::string StripPluginName(std::string plugin_name);

/**
 * @brief Strip the plugin developer from its pre- and post-fixes
 *
 * @param plugin_name The full plugin name
 * @return The stripped down name
 */
std::string StripPluginDeveloper(std::string plugin_name);

/**
 * @brief Strip the type from the plugin name
 *
 * @param name The full plugin name
 * @return std::string
 */
std::string StripPluginNamePrefixes(char const *name);

/**
 * @brief Strip the final out info when applicable
 *
 * @param name
 * @return std::string
 */
std::string StripPluginChannelPostfix(char *name);

/**
 * @brief Check if the prefix of the fx tells it is actually a fx and not an instrument
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

std::string GetReaSonusIniPath(const std::string &device);

std::string GetReaSonusZonesPath();

std::string GetReaSonusPluginPath(
    std::string developer,
    const std::string &plugin_name,
    const std::string &plugin_type,
    bool create
);

std::string GetReaSonusLocalesFolderPath();

std::string GetReaSonusLocalesPath(const std::string &language);

std::string GetReaSonusLocalesRootFile();

bool isInteger(const std::string &value);

std::vector<std::string> split(std::string str, const std::string &delimiter);

std::vector<std::string> cutString(const std::string &str, size_t size);

std::string join(const std::vector<std::string> &list, std::string delimiter);

bool hasPluginConfigFile(MediaTrack *media_track, int pluginId);

void logInteger(const char *key, int value);

void logDouble(const char *key, double value);

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

/**
 * Return the value when it is between min or max.
 * Return the min value as val is smaller than min
 * Return the max value as val is larger than max
 * @param min The minmal value to return
 * @param value The value to chack against
 * @param max The maximaum value to return
 * @return
 */
int minmax(int min, int value, int max);

/**
 * Return the value when it is between min or max.
 * Return the min value as val is smaller than min
 * Return the max value as val is larger than max
 * @param min The minmal value to return
 * @param value The value to chack against
 * @param max The maximaum value to return
 * @return
 */
double minmax(double min, double value, double max);

/**
 * Get the lowest value of the 2 given values
 * @param value_1
 * @param value_2
 * @return
 */
int min(int value_1, int value_2);

/**
 * Get the lowest value of the 2 given values
 * @param value_1
 * @param value_2
 * @return
 */
double min(double value_1, double value_2);

/**
 * Get the highest value of the 2 given values
 * @param value_1
 * @param value_2
 * @return
 */
int max(int value_1, int value_2);

/**
 * Get the highest value of the 2 given values
 * @param value_1
 * @param value_2
 * @return
 */
double max(double value_1, double value_2);

/**
 * @brief borrowed from https://github.com/juliansader/ReaExtensions
 *
 * @param parentHWND
 * @param window_child_id
 * @return HWND
 */
HWND findWindowChildByID(HWND parentHWND, int window_child_id);

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
bool getWindowScrollInfo(void *windowHWND, const char *scrollbar, int *positionOut, int *pageSizeOut, int *minOut,
                         int *maxOut, int *trackPosOut);

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

/**
 * Chack if a value is between the min and max value given
 * @param min The lowest value for val
 * @param val The value that has to be between min and max
 * @param max The max value for val
 * @return Wether the value is between min and max
 */
bool between(int min, int val, int max);

bool createPathIfNotExist(const std::string &path);

std::string createPathName(const std::vector<std::string> &path_elements);

/**
 * Convert a string to all lowercase
 * @param value Value to convert
 * @return
 */
std::string toLowerCase(std::string value);

/**
 * Convert a boolean value to a double
 * @param value Value to convert
 * @return
 */
double boolToDouble(bool value);

/**
 * Convert a double value to a boolean
 * @param value value to convert
 * @return
 */
bool doubleToBool(double value);

#endif // CSURF_UTILS_H_
