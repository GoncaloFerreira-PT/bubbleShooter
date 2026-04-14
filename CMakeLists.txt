cmake_minimum_required(VERSION 3.16)
project(bubbleShooter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3"
  "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3_image"
  "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3_ttf"
  "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3_mixer"
)

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_package(SDL3_mixer REQUIRED CONFIG)

file(GLOB_RECURSE SOURCES
  "src/*.cpp"
  "src/*.h"
  "src/*.hpp"
)

add_executable(bubbleShooter ${SOURCES})

target_include_directories(bubbleShooter PRIVATE src libs)

target_link_libraries(bubbleShooter PRIVATE
  SDL3::SDL3
  SDL3_image::SDL3_image-shared
  SDL3_ttf::SDL3_ttf-shared
  SDL3_mixer::SDL3_mixer-shared
)

if(WIN32)
  add_custom_command(TARGET bubbleShooter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3/lib/x86/SDL3.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3_image/lib/x86/SDL3_image.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3_ttf/lib/x86/SDL3_ttf.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3_mixer/lib/x86/SDL3_mixer.dll"
    "$<TARGET_FILE_DIR:bubbleShooter>"
  )
endif()

add_custom_command(TARGET bubbleShooter POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/resources"
  "$<TARGET_FILE_DIR:bubbleShooter>/resources"
)