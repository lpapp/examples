add_executable(ShortcutEditor
  actionManager.cpp
  borderLayout.cpp
  shortcutEditorWidget.cpp
  keyboardWidget.cpp
  logo.cpp
  mainwindow.cpp
  main.cpp
  openglWidget.cpp
  preferencesDialog.cpp
)

target_link_libraries(ShortcutEditor Qt::Widgets Qt::OpenGL $<$<TARGET_EXISTS:Qt::OpenGLWidgets>:Qt::OpenGLWidgets>)
