cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_library(pp
    src/graph_logic/x.cpp
)

target_include_directories(pp PUBLIC src/graph_logic)
target_compile_features(pp PRIVATE cxx_std_23)

target_link_libraries(pp PUBLIC SFML::Graphics)

add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE pp)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)