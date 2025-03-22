find_package(DiscordSDK)

set(EXAMPLE_TARGET "discord-social-example" CACHE STRING "Set target and executable name")

set(EXAMPLE_SOURCES
    main.cpp
)

add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SOURCES})

target_link_libraries(${EXAMPLE_TARGET} PRIVATE DiscordSDK::DiscordSDK)

add_custom_command(
    TARGET ${EXAMPLE_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DISCORDSDK_LIBRARY} $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
    COMMENT "Copying Discord Social SDK shared library to build directory"
)
