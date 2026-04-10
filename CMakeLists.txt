cmake_minimum_required(VERSION 3.16)
project(bubbleShooter)

option(USE_LOCAL_SDL "Use pre-compiled SDL libraries on this PC" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(USE_LOCAL_SDL)
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

  set(SDL3_LIB SDL3::SDL3)
  set(SDL3IMAGE_LIB SDL3_image::SDL3_image-shared)
  set(SDL3TTF_LIB SDL3_ttf::SDL3_ttf-shared)
  set(SDL3MIXER_LIB SDL3_mixer::SDL3_mixer-shared)

else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

  set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
  set(SDL3IMAGE_AVIF OFF CACHE BOOL "" FORCE)
  set(SDL3IMAGE_JXL OFF CACHE BOOL "" FORCE)
  set(SDL3IMAGE_WEBP OFF CACHE BOOL "" FORCE)
  set(SDL3IMAGE_TIF OFF CACHE BOOL "" FORCE)
  set(SDL3IMAGE_VENDORED ON CACHE BOOL "" FORCE)
  set(SDL3IMAGE_BACKEND_STB ON CACHE BOOL "" FORCE)

  set(SDL3TTF_VENDORED ON CACHE BOOL "" FORCE)
  set(SDL3TTF_HARFBUZZ OFF CACHE BOOL "" FORCE)

  include(FetchContent)

  FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
  )
  FetchContent_MakeAvailable(SDL3)

  FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG main
    GIT_SUBMODULES_RECURSE FALSE
  )
  FetchContent_MakeAvailable(SDL3_image)

  FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG main
  )
  FetchContent_MakeAvailable(SDL3_ttf)

  set(SDL3_LIB SDL3::SDL3)
  set(SDL3IMAGE_LIB SDL3_image::SDL3_image-shared)
  set(SDL3TTF_LIB SDL3_ttf::SDL3_ttf-shared)
  set(SDL3MIXER_LIB SDL3_mixer::SDL3_mixer-shared)
endif()


file(GLOB_RECURSE SOURCES
  "src/*.cpp"
  "src/*.h"
  "src/*.hpp"
)

# Create your game executable target as usual
add_executable(bubbleShooter ${SOURCES})

target_include_directories(bubbleShooter PRIVATE src libs)

target_link_libraries(bubbleShooter PRIVATE
  ${SDL3_LIB}
  ${SDL3IMAGE_LIB}
  ${SDL3TTF_LIB}
  ${SDL3MIXER_LIB}
)

add_custom_command(TARGET bubbleShooter POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3/lib/x86/SDL3.dll"
  "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3_image/lib/x86/SDL3_image.dll"
  "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3_ttf/lib/x86/SDL3_ttf.dll"
  "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3_mixer/lib/x86/SDL3_mixer.dll"
  "$<TARGET_FILE_DIR:bubbleShooter>"
)

add_custom_command(TARGET bubbleShooter POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/resources"
  "$<TARGET_FILE_DIR:bubbleShooter>/resources"
)
