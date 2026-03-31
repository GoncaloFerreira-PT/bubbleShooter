cmake_minimum_required(VERSION 3.16)
project(miniclip_bubble)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

# Create your game executable target as usual
add_executable(miniclip_bubble WIN32 src/main.c)

# Link to the actual SDL3 library.
target_link_libraries(miniclip_bubble PRIVATE SDL3::SDL3)

# Add a custom target to run the game
add_custom_target(run
    COMMAND $<TARGET_FILE:miniclip_bubble>
    WORKING_DIRECTORY $<TARGET_FILE_DIR:miniclip_bubble>
    DEPENDS miniclip_bubble
    COMMENT "Running miniclip_bubble..."
)