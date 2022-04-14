cmake_minimum_required(VERSION 3.21.0)

project(helloworld VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(PySide2 REQUIRED)
find_package(Shiboken2 REQUIRED)
find_package(Python CONFIG REQUIRED COMPONENTS Interpreter Development)

add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0xFFFFFF)

add_executable(helloworld
    main.cpp
)

target_link_libraries(helloworld
  PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    PySide2::pyside2
    Shiboken2::libshiboken
    Python::Python 
)
