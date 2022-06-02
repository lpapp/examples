add_executable(HotkeyEditor
  borderLayout.cpp
  hotkeyEditorWidget.cpp
  keyboardWidget.cpp
  logo.cpp
  mainwindow.cpp
  main.cpp
  openglWidget.cpp
  preferencesDialog.cpp
)

target_link_libraries(HotkeyEditor Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)
