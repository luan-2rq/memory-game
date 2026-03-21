cmake_minimum_required(VERSION 3.16)
project(MemoryGame CXX)

include(FetchContent)
include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(MEMORYGAME_BUILD_TESTS "Build unit and integration tests" ON)

find_package(SFML 2.5 COMPONENTS graphics window system QUIET)

if(NOT SFML_FOUND)
    message(STATUS "SFML not found locally — fetching SFML 2.6.2 from GitHub")
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.2
        GIT_SHALLOW    TRUE
    )
    set(SFML_BUILD_AUDIO   OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SFML)
endif()

if(TARGET SFML::Graphics)
    set(SFML_LINK_TARGETS SFML::Graphics SFML::Window SFML::System)
elseif(TARGET sfml-graphics)
    set(SFML_LINK_TARGETS sfml-graphics sfml-window sfml-system)
else()
    message(FATAL_ERROR "Could not locate SFML CMake targets.")
endif()

add_subdirectory(Engine)
add_subdirectory(MemoryGame)
add_subdirectory(E2EFramework)

if(MEMORYGAME_BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(Tests)
endif()
