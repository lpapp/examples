cmake_minimum_required(VERSION 3.22.0)

project(helloworld VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0xFFFFFF)

add_executable(BorderLayout
  main.cpp
  window.cpp
  borderlayout.cpp
)

target_link_libraries(BorderLayout Qt::Widgets)
