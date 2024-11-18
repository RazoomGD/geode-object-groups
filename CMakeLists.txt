cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(ObjectGroups VERSION 1.0.0)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/fileUtils.cpp
    src/groups.cpp
    # Add any extra C++ source files here
)

if(APPLE)
    add_compile_options("-mmacosx-version-min=13.3")
    set(EXTRA_CFLAGS "${EXTRA_CFLAGS} -mmacosx-version-min=13.3")
    set(EXTRA_CXXFLAGS "${EXTRA_CXXFLAGS} -mmacosx-version-min=13.3")
    set(EXTRA_LDFLAGS "${EXTRA_LDFLAGS} -mmacosx-version-min=13.3")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 13.3)
endif()

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})

# lib: https://github.com/nbsdx/SimpleJSON
target_include_directories(${PROJECT_NAME} PRIVATE external/SimpleJSON)
