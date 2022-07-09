#include "view.h"

#include "scene.h"

#include <QMouseEvent>
#include <QOpenGLFunctions>

View::View(QWidget* parent)
  : QOpenGLWidget(parent)
{ 
  setMouseTracking(true);
}

View::~View()
{
  if (_scene) {
    delete(_scene);
  }
}

void View::initializeGL()
{
  // Set up the rendering context, load shaders and other resources, etc.:
  QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
  f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  _elapsed.start();

  // init usd after we've been initialized 
  _scene = new Scene;
}

inline void View::paintGL()
{
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);

  _scene->prepare(_elapsed.elapsed() / 100.0f);
  _scene->draw(width(), height());
  _elapsed.restart();

  update();    
}

void View::mouseMoveEvent(QMouseEvent* event)
{
  _scene->cursor(event->x() / static_cast<float>(width()), event->y() / static_cast<float>(height()));
}

void View::mousePressEvent(QMouseEvent* /*event*/)
{
  _scene->click();
}
