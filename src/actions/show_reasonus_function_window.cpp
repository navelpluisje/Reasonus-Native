#include "show_reasonus_function_window.hpp"
#include "../csurf/csurf_faderport_ui_functions.hpp"

#define STRINGIZE_DEF(x) #x
#define STRINGIZE(x) STRINGIZE_DEF(x)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// confine my plugin to namespace
namespace SHOW_REASONUS_FUNCTION_WINDOW
{
    // some global non-const variables
    // the necessary 'evil'
    int command_id{0};
    bool toggle_action_state{false};
    constexpr auto command_name = "REASONUS_SHOW_REASONUS_FUNCTION_WINDOW";
    constexpr auto action_name = "Reasonus: Show the ReaSonus Functions window";
    custom_action_register_t action = {0, command_name, action_name, nullptr};

    // the main function of my plugin
    // gets called via callback or timer
    void MainFunctionOfMyPlugin()
    {
        if (toggle_action_state) {
            CSURF_FADERPORT_UI_FUNCTIONS::ShowFunctionsDialog();
        } else {
            CSURF_FADERPORT_UI_FUNCTIONS::HideFunctionsDialog();
        }
    }

    // c++11 trailing return type syntax
    // REAPER calls this to check this action it's toggle state
    auto ToggleActionCallback(int command) -> int
    {
        if (command != command_id)
        {
            // not quite our command_id
            return -1;
        }
        if (toggle_action_state) // if toggle_action_state == true
        {
            return 1;
        }
        return 0;
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
        
        // flip state on/off
        toggle_action_state = !toggle_action_state;
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

    void Register()
    {
        command_id = plugin_register("custom_action", &action);
        plugin_register("toggleaction", (void *)ToggleActionCallback);
        plugin_register("hookcommand2", (void *)OnAction);
    }

    auto Unregister() -> void
    {
        plugin_register("-custom_action", &action);
        plugin_register("-toggleaction", (void *)ToggleActionCallback);
        plugin_register("-hookcommand2", (void *)OnAction);
    }

} // namespace SHOW_REASONUS_FUNCTION_WINDOW