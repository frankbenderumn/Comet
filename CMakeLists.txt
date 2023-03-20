cmake_minimum_required(VERSION 3.10)
project(comet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/include)

# Add source files
set(SOURCES
    core/src/main.cc
)

# Add executable target
add_executable(comet ${SOURCES})

# Optional: set installation directory

# install
install(TARGETS comet DESTINATION bin)