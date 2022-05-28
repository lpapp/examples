#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>

#include "glwidget.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QSurfaceFormat fmt;
  fmt.setDepthBufferSize(24);
  fmt.setSamples(4);
  fmt.setVersion(3, 2);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(fmt);

  MainWindow mainWindow;

  GLWidget::setTransparent(parser.isSet(transparentOption));
  if (GLWidget::isTransparent()) {
    mainWindow.setAttribute(Qt::WA_TranslucentBackground);
    mainWindow.setAttribute(Qt::WA_NoSystemBackground, false);
  }

  mainWindow.resize(mainWindow.sizeHint());
  mainWindow.show();

  return app.exec();
}
