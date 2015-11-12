cmake_minimum_required(VERSION 3.2)
project(EssenGetummelBot)

add_subdirectory(tgbot-cpp)
include_directories(tgbot-cpp/include/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")

set(SOURCE_FILES main.cpp)
add_executable(EssenGetummelBot ${SOURCE_FILES})

target_link_libraries(EssenGetummelBot TgBot)
