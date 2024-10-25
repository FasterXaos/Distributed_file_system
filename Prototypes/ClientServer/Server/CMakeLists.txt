cmake_minimum_required(VERSION 3.14)

project(Server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

set(PROJECT_SOURCES
    server_main.cpp
    server.cpp
    server.hpp
)

add_executable(Server ${PROJECT_SOURCES})
target_link_libraries(Server PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
