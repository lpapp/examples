#ifndef SCENE_H
#define SCENE_H

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usdImaging/usdImagingGL/engine.h>
#include <pxr/base/gf/camera.h>

class Scene
{
public:
  Scene();
  void prepare(float seconds);
  void draw(int width, int height);
  void click();
  void cursor(float x, float y);

private:
  pxr::UsdStageRefPtr _stage;
  pxr::SdfPathVector _excludePaths;
  pxr::UsdImagingGLEngine _renderer;
  pxr::UsdImagingGLRenderParams _params;
  
  pxr::UsdPrim _board;
  pxr::GfCamera _camera;
  pxr::SdfPath _current;

  int _width;
  int _height;
  int _won;
};

#endif
