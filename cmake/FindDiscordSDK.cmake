# Locate Discord Social SDK library and headers.
# 
# Copyright (c) 2025 hat_kid
# https://github.com/thehatkid/DiscordSocialSDKExample
# 
# Usage of this module as follows:
#   find_package(DiscordSDK)
# 
# Variables defined by this module:
#   DISCORDSDK_FOUND            Whether was found library and headers.
#   DISCORDSDK_INCLUDE_DIR      SDK include path.
#   DISCORDSDK_LIBRARY          SDK shared library path.
#   DISCORDSDK_IMPLIB           Win32: SDK object library path to link.

# Set SDK root directory path (You can change it to different path)
set(DISCORDSDK_ROOT_DIR "${CMAKE_SOURCE_DIR}/ext/discord_social_sdk" CACHE PATH "Discord Social SDK root path")

# Set SDK library build variant
set(DISCORDSDK_VARIANT "release" CACHE STRING "Discord Social SDK library variant")

set_property(CACHE DISCORDSDK_VARIANT PROPERTY STRINGS "release" "debug")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DISCORDSDK_VARIANT "debug")
endif()

# Find SDK include directory path
find_path(
    DISCORDSDK_INCLUDE_DIR
    NAMES cdiscord.h discordpp.h
    PATHS ${DISCORDSDK_ROOT_DIR}/include
    DOC "Discord Social SDK include directory"
    NO_DEFAULT_PATH
)

# Find SDK library path
if(WIN32)
    find_file(
        DISCORDSDK_LIBRARY
        NAMES discord_partner_sdk.dll
        PATHS ${DISCORDSDK_ROOT_DIR}/bin/${DISCORDSDK_VARIANT}
        DOC "Discord Social SDK Windows Dynamic Link Library (.dll)"
        NO_DEFAULT_PATH
    )
    find_file(
        DISCORDSDK_IMPLIB
        NAMES discord_partner_sdk.lib
        PATHS ${DISCORDSDK_ROOT_DIR}/lib/${DISCORDSDK_VARIANT}
        DOC "Discord Social SDK Windows Object Library (.lib)"
        NO_DEFAULT_PATH
    )
else()
    find_library(
        DISCORDSDK_LIBRARY
        NAMES libdiscord_partner_sdk discord_partner_sdk
        PATHS ${DISCORDSDK_ROOT_DIR}/lib/${DISCORDSDK_VARIANT}
        DOC "Discord Social SDK shared library"
        NO_DEFAULT_PATH
    )
endif()

mark_as_advanced(
    DISCORDSDK_ROOT_DIR
    DISCORDSDK_VARIANT
    DISCORDSDK_INCLUDE_DIR
    DISCORDSDK_LIBRARY
)

include(FindPackageHandleStandardArgs)

if(WIN32)
    find_package_handle_standard_args(
        DiscordSDK
        REQUIRED_VARS DISCORDSDK_IMPLIB DISCORDSDK_LIBRARY DISCORDSDK_INCLUDE_DIR
    )
    mark_as_advanced(DISCORDSDK_IMPLIB)
else()
    find_package_handle_standard_args(
        DiscordSDK
        REQUIRED_VARS DISCORDSDK_LIBRARY DISCORDSDK_INCLUDE_DIR
    )
endif()

if(NOT DiscordSDK_FOUND)
    message(FATAL_ERROR "Could NOT find Discord Social SDK redistributable! Please check for SDK files in ${DISCORDSDK_ROOT_DIR}")
endif()

# Add imported shared library as DiscordSDK::DiscordSDK
add_library(DiscordSDK::DiscordSDK SHARED IMPORTED)

set_target_properties(
    DiscordSDK::DiscordSDK PROPERTIES
    IMPORTED_LOCATION ${DISCORDSDK_LIBRARY}
    INTERFACE_COMPILE_DEFINITIONS DISCORDPP_IMPLEMENTATION
    INTERFACE_INCLUDE_DIRECTORIES ${DISCORDSDK_INCLUDE_DIR}
)

if(WIN32)
    set_target_properties(
        DiscordSDK::DiscordSDK PROPERTIES
        IMPORTED_IMPLIB ${DISCORDSDK_IMPLIB}
    )
endif()
