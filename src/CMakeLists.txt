target_sources(${PROJECT_NAME}
    PRIVATE
    res.rc
        main.cpp
        ../third_party/WDL/WDL/win32_utf8.c
        )

add_subdirectory(actions)
add_subdirectory(csurf_faderport)
add_subdirectory(csurf_faderport_v2)
add_subdirectory(controls)
add_subdirectory(shared)

