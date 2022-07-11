#include <QtSingleApplication/qtsingleapplication.h>

int main(int argc, char **argv)
{
  QtSingleApplication app(argc, argv);
  return app.exec();
}
