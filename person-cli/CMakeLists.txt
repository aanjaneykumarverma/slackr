cmake_minimum_required(VERSION 3.14)
project(person_cli CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# jsoncpp is discovered via pkg-config; its headers live under a `jsoncpp/`
# prefix so that `#include <json/json.h>` resolves.
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# Core logic shared by the CLI and the tests.
add_library(person_core
    src/person.cpp
    src/person_registry.cpp)
target_include_directories(person_core PUBLIC include ${JSONCPP_INCLUDE_DIRS})
target_link_directories(person_core PUBLIC ${JSONCPP_LIBRARY_DIRS})
target_link_libraries(person_core PUBLIC ${JSONCPP_LIBRARIES})

add_executable(person_cli src/main.cpp)
target_link_libraries(person_cli PRIVATE person_core)

enable_testing()
add_subdirectory(test)
