add_subdirectory(controls)

target_sources(${PROJECT_NAME}

    PRIVATE
        csurf_track.cpp
        csurf_faderport.cpp
        csurf_last_touched_fx_manager.cpp
        csurf_navigator.cpp
        )