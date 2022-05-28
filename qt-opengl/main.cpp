#include <QApplication>
#include <QSurfaceFormat>

#include "widget.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QSurfaceFormat fmt;
  fmt.setDepthBufferSize(24);
  fmt.setSamples(4);
  fmt.setVersion(3, 2);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(fmt);

  Widget widget;
  // widget.setAttribute(Qt::WA_TranslucentBackground);
  widget.setAttribute(Qt::WA_NoSystemBackground, false);
  widget.show();

  return app.exec();
}
