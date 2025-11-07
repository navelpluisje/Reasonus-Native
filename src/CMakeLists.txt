if(WIN32)
    target_sources(${PROJECT_NAME}
        PRIVATE
        res.rc
        main.cpp
        ../lib/WDL/WDL/win32_utf8.c
    )
else()
    target_sources(${PROJECT_NAME}
        PRIVATE
        res.rc
        main.cpp
    )
endif()

add_subdirectory(actions)
add_subdirectory(controls)
add_subdirectory(csurf_faderport_8)
add_subdirectory(csurf_faderport_v2)
add_subdirectory(shared)
add_subdirectory(i18n)
add_subdirectory(ui)

