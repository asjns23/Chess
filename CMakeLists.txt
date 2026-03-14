cmake_minimum_required(VERSION 3.20)
project(Chess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(Chess ${SRC_FILES})

target_include_directories(Chess PRIVATE src)

target_link_libraries(Chess
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

target_compile_definitions(Chess PRIVATE
    PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
    ASSET_DIR="${CMAKE_SOURCE_DIR}/assets"
    VERSION="1.0"
)