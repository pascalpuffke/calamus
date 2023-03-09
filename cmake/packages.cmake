set(CPM_DOWNLOAD_VERSION 0.38.1)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if (NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake v${CPM_DOWNLOAD_VERSION}")
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif ()
include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(NAME fmt VERSION 9.1.0 GITHUB_REPOSITORY fmtlib/fmt GIT_TAG 9.1.0)
CPMAddPackage(NAME cxxopts VERSION 3.1.1 GITHUB_REPOSITORY jarro2783/cxxopts GIT_TAG v3.1.1)
CPMAddPackage(NAME tomlplusplus VERSION 3.3.0 GITHUB_REPOSITORY marzer/tomlplusplus GIT_TAG v3.3.0)
CPMAddPackage(NAME raylib VERSION 4.2.0 GITHUB_REPOSITORY raysan5/raylib GIT_TAG 4.2.0)
CPMAddPackage(NAME googletest VERSION 1.13.0 GITHUB_REPOSITORY google/googletest GIT_TAG v1.13.0)

# Include the CPM generated files. There probably is a better way to do this.
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/cxxopts-src/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/fmt-src/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/tomlplusplus-src/include)
