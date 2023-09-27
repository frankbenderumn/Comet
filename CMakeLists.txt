cmake_minimum_required(VERSION 3.10)
project(comet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CNAKE_BUILD_TYPE Debug)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext)
include_directories(/usr/local/comet/include)
link_directories(/usr/local/comet/lib)

# Add source files
add_library(comet_lib SHARED
    core/src/main.cc
    core/src/util/color.cc
    core/src/util/io.cc
    core/src/util/helpers.cc
    core/src/command/install.cc
    core/src/command/show.cc
    core/src/command/token.cc
    core/src/command/env.cc
    core/src/command/load.cc
    core/src/index.cc
    core/src/manifest.cc
    core/src/package.cc
    core/src/comet_manager.cc
    core/src/semver.cc
)

add_executable(comet core/src/main.cc)
target_link_libraries(comet comet_lib)

# install
install(TARGETS comet DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(TARGETS comet_lib DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/core/include/comet
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.h")