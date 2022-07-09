add_executable(SimpleShortcutEditor
  actionmanager.cpp
  shortcuteditorwidget.cpp
  mainwindow.cpp
  main.cpp
)

target_link_libraries(SimpleShortcutEditor Qt::Widgets)
