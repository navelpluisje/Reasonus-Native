#include "action_close_all_floating_fx_windows.hpp"

#define STRINGIZE_DEF(x) #x
#define STRINGIZE(x) STRINGIZE_DEF(x)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// confine my plugin to namespace
namespace ACTION_CLOSE_ALL_FLOATING_FX_WINDOWS
{
    // some global non-const variables
    // the necessary 'evil'
    int command_id{0};
    constexpr auto command_name = "REASONUS_CLOSE_ALL_FLOATING_FX_WINDOWS_COMMAND";
    constexpr auto action_name = "Reasonus: Close all floating fx windows";
    custom_action_register_t action = {0, command_name, action_name, nullptr};

    // the main function of my plugin
    // gets called via callback or timer
    void MainFunctionOfMyPlugin()
    {
        int nb_tracks = ::GetNumTracks();

        for (int i = 0; i < nb_tracks; i++)
        {
            MediaTrack *media_track = ::GetTrack(0, i);
            int nb_plugins = ::TrackFX_GetCount(media_track);

            for (int j = 0; j < nb_plugins; j++)
            {
                ::TrackFX_Show(media_track, j, 0);
                ::TrackFX_Show(media_track, j, 2);
            }
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

} // namespace ACTION_CLOSE_ALL_FLOATING_FX_WINDOWS