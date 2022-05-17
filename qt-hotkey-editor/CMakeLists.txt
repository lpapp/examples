cmake_minimum_required(VERSION 3.21.0)

project(HotkeyEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(HotkeyEditor
  hotkeyEditorWidget.cpp
  keyboardWidget.cpp
  mainwindow.cpp
  main.cpp
)

target_link_libraries(HotkeyEditor Qt::Widgets)
