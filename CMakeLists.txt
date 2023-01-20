# This file is a horrible mess. Please don't kill me.
# I don't get CMake.

cmake_minimum_required(VERSION 3.14)
project(calamus LANGUAGES CXX)

if (MSVC)
    message(FATAL_ERROR "Windows platform is currently unsupported")
endif ()

option(NATIVE_OPTS "Enable processor-native optimizations" ON)
option(LTO "Enable link-time optimizations" ON)
option(USE_MOLD "Force using the mold linker" OFF)
if (${USE_MOLD})
    add_link_options(-fuse-ld=mold)
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -DNDEBUG")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -DNDEBUG")
endif ()

# Is this enough??????
set(WARN_FLAGS -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -Wconversion -Wsign-conversion -Wimplicit-fallthrough)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPM_DOWNLOAD_VERSION 0.36.0)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if (NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake v${CPM_DOWNLOAD_VERSION}")
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif ()
include(${CPM_DOWNLOAD_LOCATION})

set(MAIN_SOURCES
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

set(TEST_SOURCES
        test/script/test_basic.cpp
        test/test_basiccolor.cpp
        test/test_position.cpp
        test/test_rect.cpp
        test/test_result.cpp
        test/test_size.cpp)

CPMAddPackage(NAME fmt VERSION 9.1.0 GITHUB_REPOSITORY fmtlib/fmt GIT_TAG 9.1.0)
CPMAddPackage(NAME cxxopts VERSION 3.0.0 GITHUB_REPOSITORY jarro2783/cxxopts GIT_TAG v3.0.0)
CPMAddPackage(NAME tomlplusplus VERSION 3.2.0 GITHUB_REPOSITORY marzer/tomlplusplus GIT_TAG v3.2.0)
CPMAddPackage(NAME raylib VERSION 4.2.0 GITHUB_REPOSITORY raysan5/raylib GIT_TAG 4.2.0)
CPMAddPackage(NAME googletest VERSION 1.13.0 GITHUB_REPOSITORY google/googletest GIT_TAG v1.13.0)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib)

# Include the CPM generated files. There probably is a better way to do this.
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/cxxopts-src/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/fmt-src/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/tomlplusplus-src/include)

add_executable(${PROJECT_NAME} ${MAIN_SOURCES} ${SCRIPT_SOURCES})
add_executable(${PROJECT_NAME}_test ${TEST_SOURCES} ${SCRIPT_SOURCES})
target_link_libraries(${PROJECT_NAME} fmt::fmt raylib)
target_link_libraries(${PROJECT_NAME}_test fmt::fmt raylib GTest::gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif ()

set(COMPILE_OPTIONS ${WARN_FLAGS} -fdiagnostics-color=always)

if (${NATIVE_OPTS})
    set(COMPILE_OPTIONS ${COMPILE_OPTIONS} -march=native)
endif ()

if (${LTO})
    set(COMPILE_OPTIONS ${COMPILE_OPTIONS} -flto)
endif ()

message(STATUS "Native optimizations: ${NATIVE_OPTS}")
message(STATUS "Link-time optimizations: ${LTO}")
message(STATUS "Uses mold: ${USE_MOLD}")
message(STATUS "Compile options: ${COMPILE_OPTIONS}")

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})
