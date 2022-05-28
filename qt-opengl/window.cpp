#include "window.h"

#include "glwidget.h"
#include "mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSlider>
#include <QVBoxLayout>

Window::Window(MainWindow *mw)
  : mainWindow(mw)
{
  glWidget = new GLWidget;

  xSlider = createSlider();
  ySlider = createSlider();
  zSlider = createSlider();

  connect(xSlider, &QSlider::valueChanged, glWidget, &GLWidget::setXRotation);
  connect(glWidget, &GLWidget::xRotationChanged, xSlider, &QSlider::setValue);
  connect(ySlider, &QSlider::valueChanged, glWidget, &GLWidget::setYRotation);
  connect(glWidget, &GLWidget::yRotationChanged, ySlider, &QSlider::setValue);
  connect(zSlider, &QSlider::valueChanged, glWidget, &GLWidget::setZRotation);
  connect(glWidget, &GLWidget::zRotationChanged, zSlider, &QSlider::setValue);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *container = new QHBoxLayout;
  container->addWidget(glWidget);
  container->addWidget(xSlider);
  container->addWidget(ySlider);
  container->addWidget(zSlider);

  QWidget *w = new QWidget;
  w->setLayout(container);
  mainLayout->addWidget(w);

  setLayout(mainLayout);

  xSlider->setValue(15 * 16);
  ySlider->setValue(345 * 16);
  zSlider->setValue(0 * 16);
}

QSlider *Window::createSlider()
{
  QSlider *slider = new QSlider(Qt::Vertical);
  slider->setRange(0, 360 * 16);
  slider->setSingleStep(16);
  slider->setPageStep(15 * 16);
  slider->setTickInterval(15 * 16);
  slider->setTickPosition(QSlider::TicksRight);
  return slider;
}

void Window::keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Escape) {
    close();
  }
  else {
    QWidget::keyPressEvent(e);
  }
}