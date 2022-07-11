cmake_minimum_required(VERSION 3.21)

project(Examples VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets OpenGL OpenGLWidgets Test)
if (NOT Qt6_FOUND)
  find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets OpenGL Test REQUIRED)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "(Clang|GNU)")
  add_compile_options(-Wall -Wpedantic -Wextra -Werror)
  add_compile_options(-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set_property(DIRECTORY APPEND PROPERTY COMPILE_OPTIONS "/w")
endif()

add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0xFFFFFF)

add_subdirectory(qt)
add_subdirectory(qt-button-dnd)
add_subdirectory(qt-keyboard)
add_subdirectory(qt-keypad)
add_subdirectory(qt-keysequenceedit-tab)
add_subdirectory(qt-opengl)
add_subdirectory(qt-save-dialog)
add_subdirectory(qt-shortcut-editor)
add_subdirectory(qt-simple-hotkey-editor)
add_subdirectory(qt-simple-shortcut-editor)

find_package(QtSingleApplication)
if (QtSingleApplication_FOUND)
  add_subdirectory(qt-single-application)
endif()

add_subdirectory(qt-tab-navigation)
add_subdirectory(qt-test-keysequence)
add_subdirectory(qt-treeview)

find_package(pxr)
if (pxr_FOUND)
  # add_subdirectory(usd-tf-windows)
  add_subdirectory(qt-usd-view)
endif()
