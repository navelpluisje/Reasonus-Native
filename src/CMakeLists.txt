target_sources(${PROJECT_NAME}
    PRIVATE
    res.rc
    main.cpp
    # ../lib/WDL/WDL/win32_utf8.c
)

add_subdirectory(actions)
add_subdirectory(controls)
add_subdirectory(csurf_faderport_8)
add_subdirectory(csurf_faderport_v2)
add_subdirectory(shared)

