file(GLOB_RECURSE sources CONFIGURE_DEPENDS ./*.c* ./*.h*)

target_sources(${PROJECT_NAME}

    PRIVATE
        main.cpp
        ../third_party/WDL/WDL/win32_utf8.c
        )

add_subdirectory(actions)
add_subdirectory(csurf)

