add_subdirectory(controls)
add_subdirectory(extern)

target_sources(${PROJECT_NAME}

    PRIVATE
        csurf_track.cpp
        csurf_faderport.cpp
        csurf_last_touched_fx_manager.cpp
        csurf_channel_manager_resources.cpp
        csurf_channel_context_manager.cpp
        csurf_faderport_ui_utils.cpp
        csurf_navigator_filters.cpp
        csurf_navigator.cpp
        csurf_utils.cpp
        csurf_daw.cpp
        )