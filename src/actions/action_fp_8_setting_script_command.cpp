#include "action_fp_8_setting_script_command.hpp"
#include <mini/ini.h>

#include "../shared/csurf_reasonus_settings.hpp"
#include "../shared/csurf_utils.hpp"

#define STRINGIZE_DEF(x) #x
#define STRINGIZE(x) STRINGIZE_DEF(x)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// confine my plugin to namespace
namespace ACTION_FP_8_SETTING_SCRIPT_COMMAND {
    // some global non-const variables
    // the necessary 'evil'
    int command_id{0};
    constexpr auto command_name = "REASONUS_FP8_SCRIPT_COMMAND";
    constexpr auto action_name = "Reasonus: Custom command triggered by script";
    custom_action_register_t action = {0, command_name, action_name, nullptr};

    std::string ReadIniValue(std::string group, std::string item) {
        return ReaSonusSettings::GetInstance(FP_8)->GetSetting(group, item);
    }

    void WriteIniValue(std::string group, std::string item, std::string value) {
        ReaSonusSettings::GetInstance(FP_8)->SetAndSaveSetting(group, item, value);
    }

    // this gets called when my plugin action is run (e.g. from action list)
	bool runCommand(int command, int flag) {
        // check command
        if (command != command_id) {
            return false;
        }
		
		WriteIniValue("surface", "script-command-id", std::to_string(flag));
		
		return true;
	}
	
    void GetVersion(int *majorOut, int *minorOut, int *patchOut, int *tweakOut, char *commitOut, int commitOut_sz) {
        *majorOut = PROJECT_VERSION_MAJOR;
        *minorOut = PROJECT_VERSION_MINOR;
        *patchOut = PROJECT_VERSION_PATCH;
        *tweakOut = PROJECT_VERSION_TWEAK;
        const char *commit = STRINGIZE(PROJECT_VERSION_COMMIT);
        std::copy(commit, commit + min(commitOut_sz - 1, (int)strlen(commit)), commitOut);
        commitOut[min(commitOut_sz - 1, (int)strlen(commit))] = '\0'; // Ensure null termination
    }

    // when my plugin gets loaded
    // function to register my plugins 'stuff' with REAPER
    void Register() {
        // register action name and get command_id
        command_id = plugin_register("custom_action", &action);

        // register run action/command
		plugin_register("hookcommand", (void *) runCommand);
    }

    // shutdown, time to exit
    // modern C++11 syntax
    auto Unregister() -> void {
        plugin_register("-custom_action", &action);
		plugin_register("-hookcommand", (void *) runCommand);
    }
} // namespace ACTION_FP_8_SETTING_SCRIPT_COMMAND
