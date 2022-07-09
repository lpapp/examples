add_executable(QtUsdView
  main.cpp
  scene.cpp
  view.cpp
)

target_link_libraries(QtUsdView PRIVATE
  tf gf vt ar sdf sdr ndr
  usd usdGeom usdShade usdLux usdImaging
  hd hdx hio
  Qt::Widgets)
