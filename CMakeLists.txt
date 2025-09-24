cmake_minimum_required(VERSION 3.21)

project(bjmgr
    VERSION 1.1.0
    DESCRIPTION "Local source code manager for Baekjoon Online Judge with solved.ac"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(APP_NAME "bjmgr")

add_compile_definitions(APP_NAME="${APP_NAME}")
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

if(NOT DISABLE_ANSI)
add_compile_definitions(ANSI_ENABLED=1)
endif()

add_compile_options(-Wall -std=c++17)

include_directories(./include)

file(GLOB_RECURSE SRCS "./src/**.cpp")
add_executable(${APP_NAME} ${SRCS})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(${APP_NAME} ${CURL_LIBRARIES})

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(${APP_NAME} nlohmann_json::nlohmann_json)