#include "hotkeyEditorWidget.h"

#include <QAction>
#include <QApplication>
#include <QPalette>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  HotkeyEditorWidget* hotkeyEditorWidget = new HotkeyEditorWidget; 
  hotkeyEditorWidget->show();
  return app.exec();
}
