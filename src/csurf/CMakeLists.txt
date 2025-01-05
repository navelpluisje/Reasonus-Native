add_subdirectory(controls)

target_sources(${PROJECT_NAME}

    PRIVATE
        csurf_channel_manager_resources.cpp
        csurf_daw.cpp
        csurf_fader_manager.cpp
        csurf_faderport.cpp
        csurf_faderport_ui_utils.cpp
        csurf_last_touched_fx_manager.cpp
        csurf_navigator.cpp
        csurf_navigator_filters.cpp
        csurf_track.cpp
        csurf_utils.cpp
        )