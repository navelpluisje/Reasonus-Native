add_subdirectory(actions)
add_subdirectory(csurf)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS ./*.c* ./*.h*)

target_sources(${PROJECT_NAME}

    PRIVATE
        main.cpp
        )