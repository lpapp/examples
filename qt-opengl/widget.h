#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QSlider;

class OpenGLWidget;

class Widget : public QWidget
{
  Q_OBJECT

public:
  Widget(QWidget* parent = nullptr);

private:
  QSlider *createSlider();

  OpenGLWidget *openGLWidget;
  QSlider *xSlider;
  QSlider *ySlider;
  QSlider *zSlider;
};

#endif
