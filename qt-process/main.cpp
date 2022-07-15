#include <QCoreApplication>
#include <QProcess>

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  QProcess* myProcess = new QProcess();
  myProcess->start("echo", {"test1"});
  myProcess->waitForFinished();

  myProcess->setStandardOutputFile("my_file_path");
  myProcess->start("echo", {"test2"});
  myProcess->waitForFinished();

  myProcess->setStandardOutputFile({});
  myProcess->start("echo", {"test3"});

  return app.exec();
}
