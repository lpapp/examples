add_executable(Keyboard
  main.cpp
  mainwindow.cpp
  keyboardWidget.cpp
)

target_link_libraries(Keyboard Qt::Core Qt::Gui Qt::Widgets)
