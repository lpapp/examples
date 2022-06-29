#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QString>

#include <iostream>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QString filters("XML (*.xml);;All files (*.*)");
  QFileDialog fileDialog(nullptr, "Save events file", QDir::homePath(), filters);  
  fileDialog.setAcceptMode(QFileDialog::AcceptSave);
  fileDialog.setWindowModality(Qt::WindowModal);
  if (fileDialog.exec()) {
    std::cout << "TEST FILE SAVED" << std::endl;
  }

  return app.exec();
}
