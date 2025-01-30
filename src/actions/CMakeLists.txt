file(GLOB_RECURSE sources CONFIGURE_DEPENDS ./*.c*)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${sources}
        )