#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QSlider;
class QPushButton;

class GLWidget;
class MainWindow;

class Widget : public QWidget
{
  Q_OBJECT

public:
  Widget(QWidget* parent = nullptr);

private:
  QSlider *createSlider();

  GLWidget *glWidget;
  QSlider *xSlider;
  QSlider *ySlider;
  QSlider *zSlider;
};

#endif
