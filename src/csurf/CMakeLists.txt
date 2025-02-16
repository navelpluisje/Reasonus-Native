add_subdirectory(controls)

target_sources(${PROJECT_NAME}
    PRIVATE
        csurf_automation_manager.cpp
        csurf_channel_manager_resources.cpp
        csurf_context.cpp
        csurf_daw.cpp
        csurf_fader_manager.cpp
        csurf_faderport_ui_utils.cpp
        csurf_faderport.cpp
        csurf_filter_manager.cpp
        csurf_general_control_manager.cpp
        csurf_last_touched_fx_manager.cpp
        csurf_main.cpp
        csurf_mix_manager.cpp
        csurf_navigator_filters.cpp
        csurf_navigator.cpp
        csurf_plugin_control_manager.cpp
        csurf_plugins_manager.cpp
        csurf_receives_manager.cpp
        csurf_sends_manager.cpp
        csurf_faderport_ui_plugin_edit.cpp
        csurf_faderport_ui_functions.cpp

        csurf_track.cpp
        csurf_utils.cpp
        )