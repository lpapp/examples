add_executable(OpenGLWindow
  openglwidget.cpp
  logo.cpp
  main.cpp
  widget.cpp
)

target_link_libraries(OpenGLWindow Qt::Widgets Qt::OpenGL Qt::OpenGLWidgets)
