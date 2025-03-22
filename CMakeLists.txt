cmake_minimum_required(VERSION 3.12)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds are not allowed. Please choose a separate build directory.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

project(
    DiscordSocialSDKExamples
    DESCRIPTION "Discord Social SDK C++ examples demonstrating its features"
    HOMEPAGE_URL "https://github.com/thehatkid/DiscordSocialSDKExamples"
    LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

set(CMAKE_COLOR_DIAGNOSTICS ON)

# Discord Social SDK requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set global compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "/W3 /EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "/MTd /DEBUG /Od /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /O2 /Zi")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas")
    set(CMAKE_CXX_FLAGS_DEBUG "-Og -ggdb3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -ggdb")
endif()

if(MINGW)
    # MinGW: Static linking libstdc++/libc++, libgcc_s_seh to target executables
    set(CMAKE_EXE_LINKER_FLAGS "-static ${CMAKE_EXE_LINKER_FLAGS}")
endif()

if(UNIX)
    # Set RPATH for Linux and macOS
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")

    if(APPLE)
        set(CMAKE_INSTALL_RPATH "@executable_path")
    endif()
endif()

# Include CMake examples
add_subdirectory(examples/01_simple examples_01_simple)
