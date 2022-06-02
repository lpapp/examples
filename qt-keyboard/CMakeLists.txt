add_executable(Keyboard
  main.cpp
  mainwindow.cpp
  keyboardWidget.cpp
)

target_link_libraries(Keyboard Qt6::Core Qt6::Gui Qt6::Widgets)
