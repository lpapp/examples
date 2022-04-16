cmake_minimum_required(VERSION 3.21.0)

project(helloworld VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GLEW REQUIRED)                                                     
find_package(OpenGL REQUIRED)

add_executable(helloworld
    main.cpp
)

target_link_libraries(helloworld
  PRIVATE 
    OpenGL::GL OpenGL::GLU 
  PUBLIC
    GLEW::GLEW
)
