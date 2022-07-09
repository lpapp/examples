add_executable(QtUsdView
  main.cpp
  scene.cpp
  view.cpp
)

target_link_libraries(QtUsdView Qt::Widgets)
