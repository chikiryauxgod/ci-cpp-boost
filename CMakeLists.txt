cmake_minimum_required(VERSION 3.20)

project(ci_boost_server
    VERSION 1.0.0
    LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(project_warnings INTERFACE)

target_compile_options(project_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang>:
        -Wall -Wextra -Wpedantic
        -Wconversion -Wsign-conversion
        -Wshadow -Wnon-virtual-dtor
    >
)

find_package(Boost 1.74 REQUIRED COMPONENTS system json)

add_library(core
    src/core/Router.cpp
)

target_include_directories(core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(core
    PUBLIC
        Boost::system
        Boost::json
        project_warnings
)

add_library(services
    src/services/HashService.cpp
    src/services/PrimeService.cpp
)

target_link_libraries(services
    PUBLIC
        project_warnings
)

add_library(http
    src/http/HttpSession.cpp
    src/http/HttpServer.cpp
)

target_link_libraries(http
    PUBLIC
        core
        Boost::system
        project_warnings
)

add_executable(server
    src/main.cpp
)

target_link_libraries(server
    PRIVATE
        http
        services
)

find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)

install(TARGETS server
    RUNTIME DESTINATION bin
)