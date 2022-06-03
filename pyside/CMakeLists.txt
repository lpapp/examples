find_package(PySide2 REQUIRED)
find_package(Shiboken2 REQUIRED)
find_package(Python CONFIG REQUIRED COMPONENTS Interpreter Development)

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
