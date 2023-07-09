cmake_minimum_required(VERSION 3.10)
project(comet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CNAKE_BUILD_TYPE Debug)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/include)
include_directories(/usr/local/comet/include)
link_directories(/usr/local/comet/lib)

# Add source files
add_library(comet_lib SHARED
    core/src/main.cc
    core/src/index.cc
    core/src/io.cc
    core/src/package.cc
    core/src/color.cc
    core/src/setup.cc
    core/src/command/install.cc
    core/src/command/command.cc
    core/src/manifest.cc
)

# Add executable target
add_executable(pugixml-test core/test/pugixml-test.cc)
add_executable(picojson-test core/test/picojson-test.cc)
add_executable(rapidyaml-test core/test/rapidyaml-test.cc)
add_executable(openssl-test core/test/openssl-test.cc)
add_executable(psql-test core/test/psql-test.cc)
add_executable(mongo-test core/test/mongo-test.cc)
# add_executable(install-test core/test/install-test.cc)
add_executable(setup-test core/test/setup-test.cc)
add_executable(comet core/src/main.cc)

# target_link_libraries(pugixml-test pugixml)
target_link_libraries(rapidyaml-test ryml)
target_link_libraries(rapidyaml-test c4core)
target_link_libraries(openssl-test crypto)
target_link_libraries(openssl-test ssl)
target_link_libraries(psql-test pq)
target_link_libraries(psql-test pqxx)
# target_link_libraries(install-test comet_lib)
target_link_libraries(setup-test comet_lib)
target_link_libraries(comet comet_lib)

#tests
add_executable(manifest core/debug/manifest.cc)
add_executable(get core/debug/get.cc)
add_executable(install_test core/debug/install.cc)
add_executable(update core/debug/update.cc)
add_executable(uninstall core/debug/uninstall.cc)
add_executable(purge core/debug/purge.cc)
add_executable(index_io core/debug/index_io.cc)

target_link_libraries(manifest comet_lib)
target_link_libraries(get comet_lib)
target_link_libraries(install_test comet_lib)
target_link_libraries(update comet_lib)
target_link_libraries(uninstall comet_lib)
target_link_libraries(purge comet_lib)
target_link_libraries(index_io comet_lib)

# Optional: set installation directory

# install
install(TARGETS comet DESTINATION bin)