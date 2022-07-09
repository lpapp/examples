#include "scene.h"

#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/references.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>
#include "pxr/imaging/glf/simpleLightingContext.h"

#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

Scene::Scene()
  : _width(0)
  , _height(0)
  , _won(false)
{
  _stage = UsdStage::Open("board.usda");
  UsdStageRefPtr switchStage = UsdStage::Open("switch.usda");
  
  _board = _stage->GetPrimAtPath(SdfPath("/board1"));
  auto cameraPrim = _stage->GetPrimAtPath(SdfPath("/camera1"));
  _camera = UsdGeomCamera(cameraPrim).GetCamera(UsdTimeCode::Default());
  
  // Static USD produces warning that the visibility attribute doesn't exist.
  {
      UsdGeomImageable i_board(_board);
      i_board.CreateVisibilityAttr();
  }

  UsdPrimRange range(switchStage->GetPrimAtPath(SdfPath::AbsoluteRootPath()));
  // Iterate everything except root.
  for (auto it = ++range.begin(); it != range.end(); it++)
  {
      const auto& prim = *it;
      UsdGeomImageable imageable(prim);
      imageable.CreateVisibilityAttr();
  }
  
  srand(time(nullptr));
  
  char name[32];
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      sprintf(name, "/board1/xf%ix%i", i, j);
      auto xfPrim = _stage->DefinePrim(SdfPath(name), TfToken("Xform"));
  
      // Static USD produces warning that the visibility attribute doesn't
      // exist.
      UsdGeomImageable imageable(xfPrim);
      imageable.CreateVisibilityAttr();
  
      UsdGeomXformCommonAPI(xfPrim).SetTranslate(
              GfVec3f(105.0f * i, 0.0f, -105.0f * j));
  
      // Set attributes
      // Number of turns
      auto turnsAttr = xfPrim.CreateAttribute(
              TfToken("turns"),
              SdfValueTypeNames->Int,
              true);
      turnsAttr.Set(rand() % 2);
  
      // Index
      auto indexIAttr = xfPrim.CreateAttribute(
              TfToken("indexI"),
              SdfValueTypeNames->Int,
              true);
      indexIAttr.Set(i);
      auto indexJAttr = xfPrim.CreateAttribute(
              TfToken("indexJ"),
              SdfValueTypeNames->Int,
              true);
      indexJAttr.Set(j);
  
      // Add an object
      sprintf(name, "%s/switch", name);
      auto instPrim = _stage->DefinePrim(SdfPath(name));
      instPrim.GetReferences().AddReference(
              switchStage->GetRootLayer()->GetIdentifier(),
              SdfPath("/switch1"));
    }
  }
  
  GLint major = 0;
  GLint minor = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  std::cout << "OpenGL version is " << major << "." << minor << std::endl;

  if (UsdImagingGLEngine::IsHydraEnabled()) {
    std::cout << "--> " << _renderer.GetRendererDisplayName(_renderer.GetCurrentRendererId()) << std::endl;
  }

  _params.frame = 1.0;
  _params.complexity = 1.1f;
  _params.enableLighting = true;
}

void Scene::prepare(float seconds)
{
  bool rotated = false;
  
  for (const auto& prim : _board.GetChildren()) {
    auto turnsAttr = prim.GetAttribute(TfToken("turns"));
    if (!turnsAttr.IsValid()) {
        continue;
    }

    UsdGeomXformCommonAPI xf(prim);

    GfVec3d translation;
    GfVec3f rotation;
    GfVec3f scale;
    GfVec3f pivot;
    UsdGeomXformCommonAPI::RotationOrder rotOrder;

    xf.GetXformVectors(&translation, &rotation, &scale, &pivot, &rotOrder, UsdTimeCode::Default());

    int turns;
    turnsAttr.Get(&turns);

    float target = std::min(90.0f * turns, rotation[1] + 300.0f * seconds);
    if (target > rotation[1]) {
      rotation[1] = target;
      xf.SetRotate(rotation);
      rotated = true;
    }

    if (_current.HasPrefix(prim.GetPath())) {
      float s = std::min(scale[0] + seconds, 1.1f);
      if (scale[0] < s) {
        xf.SetScale(GfVec3f(s, s, s));
      }
    }
    else {
      float s = std::max(scale[0] - seconds, 1.0f);
      if (scale[0] > s) {
        xf.SetScale(GfVec3f(s, s, s));
      }
    }

    if (_won > 1)
    {
      double t = std::max(translation[1] - 100.0 * seconds, -200.0);
      if (translation[1] > t) {
        translation[1] = t;
        xf.SetTranslate(translation);
      }
    }
  }

  if (_won && !rotated) {
    _won = 2;
  }
}

void Scene::draw(int width, int height)
{
  _width = width;
  _height = height;
  
  auto frustum = _camera.GetFrustum();
  GfMatrix4d viewMat = frustum.ComputeViewMatrix();
  GfMatrix4d projMat = frustum.ComputeProjectionMatrix();
  
  const GfVec4d viewport(0, 0, width, height);
  _renderer.SetRenderViewport(viewport);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  float position[4] = { 0,0.5,2,0 };
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  
  _renderer.SetLightingStateFromOpenGL();
  _renderer.SetCameraState(viewMat, projMat);
  
  // USD render.
  _renderer.Render(_board, _params);
  
  // Clear OpenGL errors. Because UsdImagingGL::TestIntersection prints them.
  while (glGetError() != GL_NO_ERROR) {
    std::cout << "GL ERROR" << std::endl;
  }
}

void Scene::click()
{
  if (_current.IsEmpty()) {
      return;
  }
  
  auto clickedPrim = _stage->GetPrimAtPath(_current);
  while (clickedPrim.IsValid() && clickedPrim.GetTypeName() != "Xform") {
    clickedPrim = clickedPrim.GetParent();
  }
  
  if (!clickedPrim.IsValid()) {
      return;
  }
  
  // Get index
  int clickedI;
  int clickedJ;
  auto indexIAttr = clickedPrim.GetAttribute(TfToken("indexI"));
  auto indexJAttr = clickedPrim.GetAttribute(TfToken("indexJ"));
  indexIAttr.Get(&clickedI);
  indexJAttr.Get(&clickedJ);
  
  _won = 1;
  
  // Turn the switches of the same row and column
  for (const auto& prim : _board.GetChildren()) {
    auto turnsAttr = prim.GetAttribute(TfToken("turns"));
    if (!turnsAttr.IsValid()) {
      continue;
    }
  
    indexIAttr = prim.GetAttribute(TfToken("indexI"));
    indexJAttr = prim.GetAttribute(TfToken("indexJ"));
    int i;
    int j;
    indexIAttr.Get(&i);
    indexJAttr.Get(&j);
  
    int turns;
    turnsAttr.Get(&turns);
  
    if (clickedI == i || clickedJ == j) {
      turnsAttr.Set(++turns);
    }
  
    _won = _won & (turns % 2);
  }
}

void Scene::cursor(float x, float y)
{
  GfVec2d size(1.0 / _width, 1.0 / _height);
  
  // Compute pick frustum.
  auto cameraFrustum = _camera.GetFrustum();
  auto frustum = cameraFrustum.ComputeNarrowedFrustum(GfVec2d(2.0 * x - 1.0, 2.0 * (1.0 - y) - 1.0), size);
  
  GfVec3d outHitPoint;
  GfVec3d outHitNormal;
  SdfPath outHitPrimPath;
  
  if (_renderer.TestIntersection(frustum.ComputeViewMatrix(), frustum.ComputeProjectionMatrix(),
        _board, _params, &outHitPoint, &outHitNormal, &outHitPrimPath)) {
    if (outHitPrimPath != _current) {
      _current = outHitPrimPath;
      std::cout << "hit " << _current.GetPrimPath().GetString() << std::endl;
    }
  }
  else {
    _current = SdfPath();
  }
}
