cmake_minimum_required(VERSION 3.15)

# ------------------------------------------------------------------------------
# Project Definition and Versioning
# ------------------------------------------------------------------------------

project(ReasonusNative VERSION 0.1.2 LANGUAGES C CXX)

find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_LATEST_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if(NOT "${GIT_LATEST_TAG}" STREQUAL "")
    set(GIT_ORIGINAL_VERSION ${GIT_LATEST_TAG})
    string(REGEX REPLACE "[^0-9.]" "" GIT_LATEST_TAG ${GIT_LATEST_TAG})
    if(NOT "${GIT_LATEST_TAG}" STREQUAL "")
      project(${PROJECT_NAME} VERSION ${GIT_LATEST_TAG} LANGUAGES C CXX)
    endif()
  endif()
endif()
message("Project version: ${PROJECT_VERSION}")

# ------------------------------------------------------------------------------
# Build Number and Commit Info (e.g., for CI)
# ------------------------------------------------------------------------------

if(DEFINED ENV{APPVEYOR})
    set(CMAKE_PROJECT_VERSION_TWEAK $ENV{APPVEYOR_BUILD_NUMBER})
    set(CMAKE_PROJECT_VERSION_COMMIT $ENV{APPVEYOR_REPO_COMMIT})
else()
    set(CMAKE_PROJECT_VERSION_TWEAK -1)
    set(CMAKE_PROJECT_VERSION_COMMIT a)
endif()
set(CMAKE_PROJECT_VERSION_COMMIT "\"${CMAKE_PROJECT_VERSION_COMMIT}\"")

# ------------------------------------------------------------------------------
# General Build Settings
# ------------------------------------------------------------------------------

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ------------------------------------------------------------------------------
# External Dependencies: FetchContent
# ------------------------------------------------------------------------------

include(FetchContent)
set(PROJECT_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

FetchContent_Declare(reaper-sdk
    GIT_REPOSITORY https://github.com/justinfrankel/reaper-sdk
    GIT_TAG        "origin/main" # or specify a tag or branch here
    SOURCE_DIR     "${PROJECT_LIB_DIR}/reaper-sdk"
    )

FetchContent_Declare(WDL
    GIT_REPOSITORY https://github.com/justinfrankel/WDL
    SOURCE_DIR     "${PROJECT_LIB_DIR}/WDL"
    GIT_TAG        "origin/main"
    )

FetchContent_Declare(GSL
    GIT_REPOSITORY "https://github.com/microsoft/GSL"
    GIT_TAG        "origin/main"
    SOURCE_DIR     "${PROJECT_LIB_DIR}/GSL"
)

FetchContent_Declare(mINI
    GIT_REPOSITORY "https://github.com/metayeti/mINI"
    GIT_TAG        "origin/master"
    SOURCE_DIR     "${PROJECT_LIB_DIR}/mINI"
)

FetchContent_MakeAvailable(WDL reaper-sdk GSL mINI)

# ------------------------------------------------------------------------------
# Symlink WDL into reaper-sdk
# ------------------------------------------------------------------------------

if (UNIX)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            # source
            "${PROJECT_LIB_DIR}/WDL/WDL"
            # target
            ${PROJECT_LIB_DIR}/reaper-sdk/WDL
)
else()
    file(TO_NATIVE_PATH "${PROJECT_LIB_DIR}/WDL/WDL" _dstDir)
    file(TO_NATIVE_PATH "${PROJECT_LIB_DIR}/reaper-sdk/WDL" _srcDir)
    execute_process(COMMAND cmd.exe /c mklink /J "${_dstDir}" "${_srcDir}")
endif()

# ------------------------------------------------------------------------------
# VCPKG Integration
# ------------------------------------------------------------------------------

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")
endif()

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif()

# ------------------------------------------------------------------------------
# Header Paths and Compiler Flags
# ------------------------------------------------------------------------------

set(header_paths 
    ${PROJECT_LIB_DIR}/reaper-sdk/sdk
    # add possible include directories for reaper plugin
    )

file(GLOB_RECURSE headers CONFIGURE_DEPENDS ${PROJECT_LIB_DIR}/*.h*)
foreach(header ${headers})
    if(WIN32)
        set_source_files_properties(${header} PROPERTIES COMPILE_FLAGS "/W0")
    else()
        set_source_files_properties(${header} PROPERTIES COMPILE_FLAGS "-w")
    endif()
endforeach()

# ------------------------------------------------------------------------------
# Setup Interface Library for reaper-sdk
# ------------------------------------------------------------------------------

add_library(reaper-sdk INTERFACE)
target_include_directories(reaper-sdk   
    INTERFACE 
    ${header_paths}
    )
target_link_libraries(reaper-sdk INTERFACE GSL)
target_include_directories(reaper-sdk INTERFACE ${CMAKE_BINARY_DIR})

target_link_libraries(reaper-sdk INTERFACE mINI)

# ------------------------------------------------------------------------------
# Find and Link WDL + SWELL
# ------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(WDL REQUIRED)

if(NOT WIN32)
  find_package(SWELL REQUIRED)
endif()

target_link_libraries(reaper-sdk INTERFACE WDL::WDL)

if(SWELL_FOUND)
  target_link_libraries(reaper-sdk INTERFACE SWELL::swell)
endif()

# ------------------------------------------------------------------------------
# Interprocedural Optimization (LTO)
# ------------------------------------------------------------------------------

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL     ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE        ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)

if(VCPKG_TOOLCHAIN)
  set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release)
  set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)
endif()

add_library(${PROJECT_NAME} SHARED)
add_subdirectory(src)
target_link_libraries(${PROJECT_NAME} PRIVATE reaper-sdk)

configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

if(WIN32)
  foreach(arg
    CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE
    CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_C_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_MINSIZEREL
  )
    # Embed debug information in the object files
    string(REGEX REPLACE "/Z[iI]" "/Z7" ${arg} "${${arg}}")
  endforeach()

  add_compile_options(
    # Exception handling model
    /EHsc

    # Enhance optimized debugging
    $<$<NOT:$<CONFIG:Debug>>:/Zo>

    # Eliminate duplicate strings
    $<$<NOT:$<CONFIG:Debug>>:/GF>

    # Enable function-level linking
    $<$<NOT:$<CONFIG:Debug>>:/Gy>

    # Remove unreferenced COMDAT
    $<$<NOT:$<CONFIG:Debug>>:/Zc:inline>
  )

  if(NOT CMAKE_PROJECT_VERSION_TWEAK EQUAL "1")
    add_link_options(
      # Remove unreferenced code
      $<$<NOT:$<CONFIG:Debug>>:/OPT:REF>

      # Remove duplicate sections
      $<$<NOT:$<CONFIG:Debug>>:/OPT:ICF>

      # Use relative path to the PDB file to avoid exposing the full path
      /PDBALTPATH:%_PDB%
    )
  endif()
else()
  set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES 
    COMPILE_OPTIONS 
    -fno-unsigned-char 
    -fstack-protector-strong 
    -fdiagnostics-color
    )
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

if(WIN32)
    target_compile_options(
        ${PROJECT_NAME} 
        PRIVATE 
        /W3
        /wd4996
        )
    target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)
else()
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

# ------------------------------------------------------------------------------
# Installer Defaults and CPack Configuration
# ------------------------------------------------------------------------------

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if(WIN32)
    set(USER_CONFIG_DIR "$ENV{APPDATA}")
  elseif(APPLE)
    set(USER_CONFIG_DIR "~/Library/Application Support")
  else()
    set(USER_CONFIG_DIR "~/.config")
  endif()

  set(CMAKE_INSTALL_PREFIX "${USER_CONFIG_DIR}/REAPER" CACHE PATH "REAPER resource path where to install ReaPack" FORCE)
endif()

if(CMAKE_OSX_ARCHITECTURES)
  list(JOIN CMAKE_OSX_ARCHITECTURES "-" ARCH_NAME)
elseif(MSVC_CXX_ARCHITECTURE_ID)
  set(ARCH_NAME ${MSVC_CXX_ARCHITECTURE_ID})
else()
  set(ARCH_NAME ${CMAKE_SYSTEM_PROCESSOR})
endif()

STRING(TOLOWER "${ARCH_NAME}" ARCH_NAME)

# Set output name (no "lib" prefix) and architecture
set_target_properties(${PROJECT_NAME} PROPERTIES
  PREFIX "" # disable the "lib" prefix
  OUTPUT_NAME "reaper_${PROJECT_NAME}-${ARCH_NAME}"
)

set(REAPER_USER_PLUGINS "UserPlugins")
install(TARGETS ${PROJECT_NAME}
  COMPONENT ${PROJECT_NAME}
  RUNTIME DESTINATION "${REAPER_USER_PLUGINS}" # Windows .dll
  LIBRARY DESTINATION "${REAPER_USER_PLUGINS}" # Linux .so/macOS .dylib
)

# Set the component as required
set(CPACK_COMPONENT_${PROJECT_NAME}_REQUIRED ON)
if(WIN32)
install(
    FILES $<TARGET_PDB_FILE:${PROJECT_NAME}>
    COMPONENT ${PROJECT_NAME}
    DESTINATION "${REAPER_USER_PLUGINS}"
)
endif()

if(APPLE)
  set(CPACK_PACKAGING_INSTALL_PREFIX "/Library/Application Support/REAPER")
  set(CPACK_COMPONENTS_ALL ${PROJECT_NAME})
  set(CPACK_GENERATOR productbuild)
elseif(WIN32)
  set(CPACK_GENERATOR NSIS)
else()
  set(CPACK_GENERATOR TXZ)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL "8")
  set(CPUBITS "64")
else(CMAKE_SIZEOF_VOID_P EQUAL "4")
  set(CPUBITS "32")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CPACK_PACKAGE_FILE_NAME 
    "${PROJECT_NAME}-${PROJECT_VERSION}-macOS"
    )
endif()

# if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
#   execute_process(
#     COMMAND ${CMAKE_COMMAND} -E create_symlink
#             # source
#             "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
#             # target
#             "${CMAKE_CURRENT_BINARY_DIR}/link_readme.txt")
#   set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_BINARY_DIR}/link_readme.txt")
# endif()

# if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
#   execute_process(
#     COMMAND ${CMAKE_COMMAND} -E create_symlink
#             # source
#             "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
#             # target
#             "${CMAKE_CURRENT_BINARY_DIR}/link_license.txt")
#   set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/link_license.txt")
# endif()

set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_NSIS_CPUBITS ${CPUBITS})

include(CPack)
