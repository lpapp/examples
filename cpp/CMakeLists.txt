cmake_minimum_required(VERSION 3.21)

project(xxx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libfoo SHARED libfoo.cpp libfoo.h)
target_include_directories(libfoo PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})

add_library(libbar SHARED libbar.cpp libbar.h)
target_link_libraries(libbar PRIVATE libfoo)

add_library(libFooTime SHARED footime.cpp footime.h)
target_include_directories(libFooTime PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})

# add_library(libGeomOp SHARED GeomOp.cpp GeomOp.h)
# target_include_directories(libGeomOp PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})

add_executable(main main.cpp)
target_link_libraries(main PRIVATE libbar)
