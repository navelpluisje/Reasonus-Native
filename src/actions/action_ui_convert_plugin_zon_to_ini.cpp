#include "action_ui_convert_plugin_zon_to_ini.hpp"
#include "../shared/csurf_utils.hpp"
#include "../ui/csurf_ui_plugin_mapping_converter.hpp"

#define STRINGIZE_DEF(x) #x
#define STRINGIZE(x) STRINGIZE_DEF(x)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// confine my plugin to namespace
namespace ACTION_UI_CONVERT_PLUGIN_ZON_TO_INI
{
    // some global non-const variables
    // the necessary 'evil'
    int command_id{0};
    constexpr auto command_name = "REASONUS_CONVERT_PLUGIN_ZON_TO_INI";
    constexpr auto action_name = "Reasonus: Convert the old zon mappings to the new ini mappings";
    custom_action_register_t action = {0, command_name, action_name, nullptr};

    // the main function of my plugin
    // gets called via callback or timer
    void MainFunctionOfMyPlugin()
    {
        if (ReaSonusPluginMappingConverter::converter_open)
        {
            ReaSonusPluginMappingConverter::Stop();
        }
        else
        {
            ReaSonusPluginMappingConverter::Start();
        }
    }

    // this gets called when my plugin action is run (e.g. from action list)
    bool OnAction(KbdSectionInfo *sec, int command, int val, int valhw, int relmode, HWND hwnd)
    {
        // treat unused variables 'pedantically'
        (void)sec;
        (void)val;
        (void)valhw;
        (void)relmode;
        (void)hwnd;

        // check command
        if (command != command_id)
        {
            return false;
        }

        MainFunctionOfMyPlugin();

        return true;
    }

    void GetVersion(int *majorOut, int *minorOut, int *patchOut, int *tweakOut, char *commitOut, int commitOut_sz)
    {
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
    void Register()
    {
        // register action name and get command_id
        command_id = plugin_register("custom_action", &action);

        // register run action/command
        plugin_register("hookcommand2", (void *)OnAction);
    }

    // shutdown, time to exit
    // modern C++11 syntax
    auto Unregister() -> void
    {
        plugin_register("-custom_action", &action);
        plugin_register("-hookcommand2", (void *)OnAction);
    }

} // namespace ACTION_UI_CONVERT_PLUGIN_ZON_TO_INI