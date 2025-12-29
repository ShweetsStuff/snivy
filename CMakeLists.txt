cmake_minimum_required(VERSION 3.27)
project(snivy LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "" FORCE)
endif()

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
if (NOT WIN32)
    set(SDL_ALSA ON CACHE BOOL "" FORCE)
    set(SDL_ALSA_SHARED ON CACHE BOOL "" FORCE)
    set(SDL_JACK ON CACHE BOOL "" FORCE)
    set(SDL_JACK_SHARED ON CACHE BOOL "" FORCE)
    set(SDL_PIPEWIRE ON CACHE BOOL "" FORCE)
    set(SDL_PIPEWIRE_SHARED ON CACHE BOOL "" FORCE)
    set(SDL_SNDIO ON CACHE BOOL "" FORCE)
    set(SDL_SNDIO_SHARED ON CACHE BOOL "" FORCE)
endif ()
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_TRAY OFF CACHE BOOL "" FORCE)
set(SDL_VULKAN OFF CACHE BOOL "" FORCE)
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

set(SDLMIXER_DEPS_SHARED OFF CACHE BOOL "" FORCE)
set(SDLMIXER_AIFF OFF CACHE BOOL "" FORCE)
set(SDLMIXER_AU OFF CACHE BOOL "" FORCE)
set(SDLMIXER_FLAC_LIBFLAC OFF CACHE BOOL "" FORCE)
set(SDLMIXER_FLAC_DRFLAC OFF CACHE BOOL "" FORCE)
set(SDLMIXER_GME OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MOD_XMP OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MP3_DRMP3 OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MP3_MPG123 OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MIDI_FLUIDSYNTH OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MIDI_TIMIDITY OFF CACHE BOOL "" FORCE)
set(SDLMIXER_OPUS ON CACHE BOOL "" FORCE)
set(SDLMIXER_VOC OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VORBIS_STB ON CACHE BOOL "" FORCE)
set(SDLMIXER_VORBIS_VORBISFILE OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VORBIS_TREMOR OFF CACHE BOOL "" FORCE)
set(SDLMIXER_WAVE ON CACHE BOOL "" FORCE)
set(SDLMIXER_WAVPACK OFF CACHE BOOL "" FORCE)
set(SDLMIXER_TEST OFF CACHE BOOL "" FORCE)
set(SDLMIXER_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/SDL_mixer EXCLUDE_FROM_ALL)




set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(TINYXML2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyxml2)
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/glm)

set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
)

set (TINYXML2_SOURCES ${TINYXML2_DIR}/tinyxml2.cpp)

file(GLOB PROJECT_SRC CONFIGURE_DEPENDS
        include/*.cpp
        src/*.cpp
        src/resource/*.cpp
        src/window/*.cpp
        src/util/*.cpp
        src/util/*.h
)

add_executable(${PROJECT_NAME}
        ${PROJECT_SRC}
        ${IMGUI_SOURCES}
        ${TINYXML2_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_mixer/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external # tinyxml2 headers are included as <tinyxml2/tinyxml2.h>
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${GLM_DIR}
        ${TINYXML2_DIR}
        include/glad
        include/KHR
        include
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static SDL3_mixer::SDL3_mixer-static)

set(PROJECT_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
set(PROJECT_RESOURCES_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
if(EXISTS "${PROJECT_RESOURCES_DIR}")
    file(GLOB_RECURSE PROJECT_RESOURCE_FILES CONFIGURE_DEPENDS
            "${PROJECT_RESOURCES_DIR}/*")
    add_custom_target(copy_resources ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_RESOURCES_DIR}" "${PROJECT_RESOURCES_BINARY_DIR}"
            DEPENDS ${PROJECT_RESOURCE_FILES}
            COMMENT "Copying resources directory")
    add_dependencies(${PROJECT_NAME} copy_resources)
    set(HAS_PROJECT_RESOURCES TRUE)
else()
    set(HAS_PROJECT_RESOURCES FALSE)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    target_link_options(${PROJECT_NAME} PRIVATE
            "-sMIN_WEBGL_VERSION=2"
            "-sMAX_WEBGL_VERSION=2"
            "-sFULL_ES2=1"
            "-sUSE_OGG=1"
            "-sUSE_VORBIS=1"
            "-sALLOW_MEMORY_GROWTH=1"
    )
    if(HAS_PROJECT_RESOURCES)
        target_link_options(${PROJECT_NAME} PRIVATE
                "--preload-file"
                "${PROJECT_RESOURCES_BINARY_DIR}@resources"
        )
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES
            OUTPUT_NAME "index"
            SUFFIX ".js")
else()
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
endif()

message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
