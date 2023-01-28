cmake_minimum_required(VERSION 3.14)
project(calamus LANGUAGES CXX)

if (MSVC)
    message(FATAL_ERROR "Windows platform is currently unsupported")
endif ()

include(cmake/options.cmake)
include(cmake/packages.cmake)

# Is this enough??????
set(WARN_FLAGS -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -Wconversion -Wsign-conversion -Wimplicit-fallthrough)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MAIN_SOURCES
        src/file/resources.cpp
        src/file/toml-config.cpp
        src/graphics/image.cpp
        src/graphics/renderer.cpp
        src/graphics/texture.cpp
        src/graphics/window.cpp
        src/main.cpp
        src/resources/fonts.cpp
        src/resources/textures.cpp
        src/ui/ui_button.cpp
        src/ui/ui_label.cpp
        src/ui/ui_screen.cpp
        src/util/raylib/raylib_wrapper.cpp)

set(SCRIPT_SOURCES
        src/script/chunk.cpp
        src/script/compiler.cpp
        src/script/debug.cpp
        src/script/memory.cpp
        src/script/object.cpp
        src/script/scanner.cpp
        src/script/table.cpp
        src/script/value.cpp
        src/script/vm.cpp)

# SCRIPT_SOURCES is required by test/CMakeLists.txt, without turning these into absolute paths it won't find any files.
list(TRANSFORM SCRIPT_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${MAIN_SOURCES} ${SCRIPT_SOURCES})
target_link_libraries(${PROJECT_NAME} fmt::fmt raylib)

add_subdirectory(test)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif ()

set(COMPILE_OPTIONS ${WARN_FLAGS} ${ADDITIONAL_OPTIONS} -fdiagnostics-color=always)

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})
