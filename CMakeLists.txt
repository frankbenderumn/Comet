cmake_minimum_required(VERSION 3.10)
project(comet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/include)
include_directories(/usr/local/comet/include)
link_directories(/usr/local/comet/lib)

# Add source files
set(SOURCES
    core/src/main.cc
)

# Add executable target
add_executable(comet ${SOURCES})
add_executable(pugixml-test core/test/pugixml-test.cc)
add_executable(picojson-test core/test/picojson-test.cc)
add_executable(rapidyaml-test core/test/rapidyaml-test.cc)
add_executable(openssl-test core/test/openssl-test.cc)
add_executable(psql-test core/test/psql-test.cc)
add_executable(mongo-test core/test/mongo-test.cc)

target_link_libraries(pugixml-test pugixml)
target_link_libraries(rapidyaml-test ryml)
target_link_libraries(rapidyaml-test c4core)
target_link_libraries(openssl-test crypto)
target_link_libraries(openssl-test ssl)
target_link_libraries(psql-test pq)
target_link_libraries(psql-test pqxx)

# Optional: set installation directory

# install
install(TARGETS comet DESTINATION bin)