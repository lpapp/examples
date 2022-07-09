#include "view.h"

#include <QDir>
#include <QApplication>

int main(int argc, char **argv)
{
  qputenv("PATH", qPrintable(QDir::currentPath()));
  qputenv("QT_PLUGIN_PATH", qPrintable(QDir::currentPath()));
  QApplication app(argc, argv);
  View view;
  view.show();
  return app.exec();
}
