cmake_minimum_required(VERSION 3.22.0)

project(Keyboard VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(Keyboard
  main.cpp
  mainwindow.cpp
  keyboardWidget.cpp
)

target_link_libraries(Keyboard Qt::Core Qt::Gui Qt::Widgets)
